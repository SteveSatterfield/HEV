
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dtk.h>
#include <iris.h>

#include "hev-segmentSelection.h"
#include "hssOps.h"
#include "hssDBHL.h"

bool Active = false;

static int DebugIter = 0;




///////////////////////////////////////////////////////////////////


/*


Thinking about the program as a finite state machine:

Events:

   These events are derived directly from the devices
   (actually from the shared memory segs that represent
   the devices) :

      BD  - left button goes from up to down
      BU  - left button goes from down to up
      BRD - right button goes from up to down
      BRU - right button goes from down to up

      move      - cursor changes position


   These events are derived from the device data in
   conjunction with the data that describes the current
   segments and their hot spots:

      move near - cursor pos moves into hot spot 
      move far  - cursor pos moves out of current hot spot 


  

States:

  Far     - not near and end pt; no highlight; none pickable;
                     ---> initial state

  Create-Drag - user setting first point of segment

  Drag    - editing position of an end pt

  Near    - near an end pt; highlight; pt is pickable

  Near-Delete  - near an edit point, after right button 

  Delete  - near an edit point, waiting for right button confirmation



Far is the initial state.


Transitions:

  begin state     event       action                         end state

  Far             LBD         start new seg: attach first    Create-Drag
                              end marker to wand

  Create-Drag     move        drag 1st end marker            Create-Drag

  Create-Drag     LBU         set position of first          Drag
                              end marker, add line and
                              2nd end marker attached
                              to wand; 2nd end pt is
                              current

  Drag            LBD         no action                      Drag


  Drag            LBU         set new pos of current obj     Far

  Drag            move        drag current obj               Drag

  Near            move near   redo highlights if nec.        Near

  Near            move far    unhighlight                    Far

  Near            LBD         set current obj to drag        Drag

  Near            RBD         highlight curr seg to delete   Near-Delete

  Near-Delete     move far    unhighlight                    Far

  Near-Delete     RBU         set current seg to delete      Delete

  Far             move near   redo highlights                Near

  Delete          RBD         no action                      Delete

  Delete          RBU         delete current segment         Far

  Delete          move far    unhighlight                    Far





States:

  BU-Far     - not near and end pt; no highlight; none pickable;
                     possible initial state

  BD-Create-Drag - user setting first point of segment

  BU-Drag        - user setting second point of segment; only
                     way to get here is during segment creation.


  BD-Startup - button down at startup; possible initial state

  BD-Drag    - editing position of an end pt

  BU-Near    - near an end pt; highlight; pt is pickable

  BU_Delete  - near an edit point, after right button select 


BU-Far or BD-Startup are the possible initial states of the program.

Transitions:

  begin state     event      action                         end state

  BU-Far          BD         start new seg: attach first    BD-Create-Drag
                             end marker to wand

  BD-Create-Drag  move       drag 1st end marker            BD-Create-Drag

  BD-Create-Drag  BU         set position of first          BU-Drag
                             end marker, add line and
                             2nd end marker attached
                             to wand; 2nd end pt is
                             current

  BU-Drag        BD          no action                      BD-Drag

  BU-Drag        move        drag current end pt            BU-Drag

  BD-Startup     BU          no action                      BU-Far





  BD-Drag        BU          set new pos of current obj     BU-Near

  BD-Drag        move        drag current obj               BD-Drag

  BU-Near        move near   redo highlights if nec.        BU-Near

  BU-Near        move far    unhighlight                    BU-Far

  BU-Near        BD          set current obj to drag        BD-Drag

  BU-Near        BRU         highlight curr seg to delete   BU-Near

  BU-Near        BRU         set current seg to delete      BU-Delete

  BU-Far         move near   redo highlights                BU-Near

  BU-Far         move far    no action                      BU-Far

  BU-Delete      RBD         no action                      BU-Delete

  BU-Delete      RBU         delete current segment         BU-Far

  BU-Delete      move far    unhighlight                    BU-Far









Note that the commands could be another source of events.  I think
that the only one that could change state is RESET.  When RESET
is received, all states transition to either BU-Far or BD-Startup 
depending on current state of button.


*/




static FILE *sgeFP = stdout;

void
sendSgeMsg (const char *format, ...)
	{

	va_list ap;
	va_start(ap, format);
	vfprintf (sgeFP, format, ap);
	va_end(ap);

	if (Debug)
		{
		fprintf (stderr, "%s: Sending sge command:\n        ",  ProgName);
		va_start(ap, format);
		vfprintf (stderr, format, ap);
		va_end(ap);
		}

	int len = strlen (format);
	if (format[len-1] != '\n')
		{
		fprintf (sgeFP, "\n");
		if (Debug)
			{
			fprintf (stderr, "\n");
			}
		}

	fflush (sgeFP);
	}  // end of sendSgeMsg




void
executeCmd (const char *format, ...)
	{
	char cmd[1000];

	va_list ap;
	va_start(ap, format);
	vsnprintf (cmd, sizeof(cmd), format, ap);
	va_end(ap);

	if (Debug)
		{
		fprintf (stderr, "%s: Executing command \" %s \".\n", ProgName, cmd);
		}

	system (cmd);
	}  // end of executeCmd




///////////////////////////////////////////////////////////////////
// shared memory stuff


dtkSharedMem *CreateButtonShm = NULL;
dtkSharedMem *DeleteButtonShm = NULL;




dtkSharedMem *SegmentShm = NULL;
char OutCoordsNodeName[1000];

dtkSharedMem *WandishShm = NULL;
bool WandishIsCoord = true;

static char *CursorNodeName = const_cast<char*>("hevssEndPt");
static char *CursorDCSName = const_cast<char*>("hevssCursorDCS");



void
highlightCursor ()
	{
	// This is a bit of a kludge.  Note that we are using the
	// end pt drag highlight object as the highlighted cursor.
	// This makes perfect sense for the use within this program.
	sendSgeMsg ("REMOVECHILD %s %s\n", CursorNodeName, CursorDCSName);
	sendSgeMsg ("ADDCHILD %s %s\n", EndPtNodeName[1], CursorDCSName);
	}  // end of highlightCursor


void
unhighlightCursor ()
	{
	sendSgeMsg ("ADDCHILD %s %s\n", CursorNodeName, CursorDCSName);
	sendSgeMsg ("REMOVECHILD %s %s\n", EndPtNodeName[1], CursorDCSName);
	}  // end of unhighlightCursor

//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////



int setupButtons (
  char *createButtonShmName, 
  char *deleteButtonShmName )
	{
	if (Debug)
		{
		fprintf (stderr, "setupButtons called\n");
		}


	CreateButtonShm = 
	    new dtkSharedMem (sizeof(unsigned char), createButtonShmName);
	if (CreateButtonShm->isInvalid())
		{
		fprintf (stderr, "%s: Unable to open shared memory %s.\n",
				ProgName, createButtonShmName);
		return -1;
		}
        // fprintf (stderr, 
        // "(CreateButton) Successfully opened shm named \"%s\".\n", 
        // createButtonShmName);


	DeleteButtonShm = 
	    new dtkSharedMem (sizeof(unsigned char), deleteButtonShmName);
	if (DeleteButtonShm->isInvalid())
		{
		fprintf (stderr, "%s: Unable to open shared memory %s.\n",
				ProgName, deleteButtonShmName);
		return -1;
		}
        // fprintf (stderr, 
        // "(DeleteButton) Successfully opened shm named \"%s\".\n", 
        // deleteButtonShmName);


	return 0;
	}  // end of setupButtons


static dtkSharedMem *
openTransformShm (
  char *shmName,
  bool mustExist,
  bool createCoord,
  bool & existingIsCoord
  )
        {
        existingIsCoord = true;

        // try to open existing coord file
        dtkSharedMem * shm = NULL;

        shm = new dtkSharedMem (shmName);
        if ( ! shm->isInvalid () )
                {
                int sz = shm->getSize();
                if (sz == 6*sizeof(float))
                        {
                        existingIsCoord = true;
                        return shm;
                        }
                else if (sz == 16*sizeof(double))
                        {
                        existingIsCoord = false;
                        return shm;
                        }
                delete shm;
                }


        // file doesn't exist

        existingIsCoord = false;

        if (mustExist)
                {
                fprintf (stderr,
                        "%s: Unable to open shared memory transformation %s.\n"
                        "     Note that existing files must hold either "
                        " 6 floats (XYZHPR) or 16 doubles (matrix).\n",
                        ProgName, shmName);
                return NULL;
                }


        if (createCoord)
                {
                shm = new dtkSharedMem (6*sizeof(float), shmName);
                }
        else
                {
                shm = new dtkSharedMem (16*sizeof(double), shmName);
                }

        if ( shm->isInvalid () )
                {
                fprintf (stderr,
                        "%s: Unable to open shared memory transformation %s.\n"
                        "     Note that existing files must hold either "
                        " 6 floats (XYZHPR) or 16 doubles (matrix).\n",
                        ProgName, shmName);
                delete shm;
                shm = NULL;
                }

        return shm;
        } // end of openTransformShm


int setupInShm (char *inCoordShmName)
	{
	// fprintf (stderr, "setupCoords called\n");

	WandishShm = 
            openTransformShm (inCoordShmName, true, false, WandishIsCoord);
        if (WandishShm == NULL)
                {
                return -1;
                }

	return 0;
	}  // end of setupCoords

//////////////////////////////////////////////////////////
//
// The following code accesses the point shared memory file
//
//
// The file has two sections.  The first section is a header
// that is currently defined as eight ints.  The second section
// is an array of segment structures.
//
// The structure of the header is:
//
//    int nSeg;               // total number of segments
//                            //     for which there is space
//    int segBeingEdited;     // index of seg; -1 for none
//    int lastSegChanged;     // index of seg; -1 for none
//    int changeCounter;      // updated each time data is changed
//    int futureUse[4];
//
//  NOTE: these fields might not be properly updated: 
//            segBeingEdited
//            lastSegChanged
//            changeCounter
//
// This is followed by nSeg segment structures.  Each segment 
// structure looks like this:
//
//        long int inUse;    // Says whether this segment is in use;
//                           // in other words, does it contain valid data.
//                           // note that we use a long int rather
//                           // something shorter to reduce the
//                           // possibility of mis-alignment problems.
//        float pt[2][3];    // the two end points of the segment
//

// #define PTSHM_MAX_NUM_SEGS          (200)
#define PTSHM_NSEG_POS              (0)
#define PTSHM_EDIT_SEG_POS          (PTSHM_NSEG+sizeof(int))
#define PTSHM_LAST_SEG_CHANGED_POS  (PTSHM_EDIT_SEG+sizeof(int))
#define PTSHM_CHANGE_COUNTER_POS    (PTSHM_LAST_SEG_CHANGED+sizeof(int))
#define PTSHM_HDR_SIZE              (8*sizeof(int))

#define PTSHM_SEG_DATA_START        (PTSHM_HDR_SIZE)
#define PTSHM_SEG_LENGTH            (sizeof(long int)+(6*sizeof(float)))
#define PTSHM_TOTAL_LENGTH          (PTSHM_HDR_SIZE+(PTSHM_MAX_NUM_SEGS*PTSHM_SEG_LENGTH))

#define PTSHM_SEG_POS(n)            (PTSHM_SEG_DATA_START+(n*PTSHM_SEG_LENGTH))

#if 0
typedef struct
	{
	long int inUse;
	float pt[2][3];
	}  PtShmSegment;
#endif



// The following little array contains :
//    int segBeingEdited;     // index of seg; -1 for none
//    int lastSegChanged;     // index of seg; -1 for none
//    int changeCounter;      // updated each time data is changed
int SegInShmStatus[3] = {-1, -1, 0};

int
writeSegShm (int iSeg, float seg[2][3], double coordScale)
	{
        float lseg[2][3];

        memcpy (lseg, seg, 6*sizeof(float));

        lseg[0][0] *= coordScale;
        lseg[0][1] *= coordScale;
        lseg[0][2] *= coordScale;
        lseg[1][0] *= coordScale;
        lseg[1][1] *= coordScale;
        lseg[1][2] *= coordScale;





	if (SegmentShm->write ( lseg,
				6*sizeof (float) , 
				PTSHM_SEG_POS(iSeg)+sizeof(long int)))
		{
		fprintf (stderr, 
          	   "%s: Error writing segment shared memory"
		   " - segment coords %d.\n",
		   	  ProgName, iSeg);
		return -1;
		}

	return 0;
	}  // end of writeSegShm (int iSeg, float seg[2][3], double coordScale)


int
writeSegShm (int iSeg, int whichPt, float pt[3], double coordScale)
	{
        float ipt[3];
        ipt[0] = pt[0] * coordScale;
        ipt[1] = pt[1] * coordScale;
        ipt[2] = pt[2] * coordScale;

	if (SegmentShm->write 
		( 
		ipt,
		3*sizeof (float) , 
		PTSHM_SEG_POS(iSeg)+sizeof(long int)+(whichPt*3*sizeof(float))
		) 
	   )
		{
		fprintf (stderr, 
          	   "%s: Error writing segment shared memory"
		   " - pt coords %d %d.\n",
		   	  ProgName, iSeg, whichPt);
		return -1;
		}

	return 0;
	}  // end of writeSegShm (int , int , float pt[3], double )




int
writeSegShm (int iSeg, PtShmSegment *seg, double coordScale)
	{
        PtShmSegment lseg;
        memcpy (&lseg, seg, sizeof (PtShmSegment));
        lseg.pt[0][0] *= coordScale;
        lseg.pt[0][1] *= coordScale;
        lseg.pt[0][2] *= coordScale;
        lseg.pt[1][0] *= coordScale;
        lseg.pt[1][1] *= coordScale;
        lseg.pt[1][2] *= coordScale;

	if (SegmentShm->write (&lseg, PTSHM_SEG_LENGTH, PTSHM_SEG_POS(iSeg)))
		{
		fprintf (stderr, 
	          "%s: Error writing segment shared memory - segment %d.\n",
		   	  ProgName, iSeg);
		return -1;
		}

	return 0;
	}  // end of writeSegShm (int iSeg, PtShmSegment *seg, double )

int
writeSegShmValid (int iSeg, bool valid)
	{

	// fprintf (stderr, "writeSegShmValid: iSeg = %d  valid = %d\n",
	//                       iSeg, valid);

	long int validFlag = valid ? 1 : 0;

	if (SegmentShm->write (&validFlag, 
				sizeof (long int) , PTSHM_SEG_POS(iSeg)))
		{
		fprintf (stderr, 
          	   "%s: Error writing segment shared memory"
		   " - segment flag %d.\n",
		   	  ProgName, iSeg);
		return -1;
		}

	return 0;
	}  // end of writeSegShmValid (int iSeg, bool valid)


static int 
wipeSegShm ()
	{
	int hdr[8] = {200, -1, -1, 0, 0, 0, 0, 0};
	PtShmSegment seg = {0, {{0,0,0},{0,0,0}}};
		
	if (SegmentShm->write ( hdr, PTSHM_HDR_SIZE, 0) )
		{
		fprintf (stderr, 
		   "%s: Error writing segment shared memory header.\n",
		   ProgName);
		return -1;
		}

	for (int i = 0; i < PTSHM_MAX_NUM_SEGS; i++)
		{
		if (SegmentShm->write (&seg, PTSHM_SEG_LENGTH, PTSHM_SEG_POS(i)))
			{
			fprintf (stderr, 
		          "%s: Error writing segment shared memory - segment %d.\n",
		   	  ProgName, i);
			return -1;
			}
		}

	return 0;
	}  //  end of wipeSegShm


int setupPtsShm (char *ptsShmName, char *ptsCoordNode)
	{
	// fprintf (stderr, "setupPtsShm called\n");

        strcpy (OutCoordsNodeName, ptsCoordNode);	

	if (sizeof(PtShmSegment) != (sizeof(long int) + 6*sizeof(float)))
		{
		fprintf (stderr, 
			"%s: Fatal internal error: Bad structure padding.\n",
			ProgName);
		return -1;
		}

	// Here's the point at which we should check if we want to
	// initialize from the existing shared memory file.

	if (Verbose)
		{
		fprintf (stderr,
		   "%s: Opening shared memory file \"%s\" of size %d.\n",
			ProgName, ptsShmName, PTSHM_TOTAL_LENGTH);
		}

	SegmentShm = new dtkSharedMem (PTSHM_TOTAL_LENGTH, ptsShmName);
	if (SegmentShm->isInvalid())
		{
		fprintf (stderr, "%s: Unable to open shared memory %s.\n",
				ProgName, ptsShmName);
		return -1;
		}


	// If we don't want to initialize from the existing file, we
	// want to wipe out anything that is currently in the file.
	wipeSegShm ();


	// strcpy (OutCoordsNodeName, "scene");



	initSegs ();

	return 0;
	}  // end of setupPtsShm






//////////////////////////////////////////////////////////

// static dtkSharedMem *InCtrlShm = NULL;
// static char *MyCtrlID = NULL;
static iris::ShmString *InCtrlShm = NULL;
static std::string MyCtrlIDStr = "";

// static char *CtrlIDBuffer = NULL;
// static int CtrlIDSize = 0;
// #define CTRL_ID_SHM_SIZE 200


int setupCtrlShm (char *inCtrlShmName, char *ctrlID)
        {


#if 1
        InCtrlShm = new iris::ShmString (inCtrlShmName);
        if (InCtrlShm->isInvalid())
                {
                fprintf (stderr, "%s: Unable to open shared memory %s.\n",
                                ProgName, inCtrlShmName);
                return -1;
                }

        MyCtrlIDStr = ctrlID;
#else

        if (inCtrlShmName[0])
            {
            // first see if it exists:
            InCtrlShm = new dtkSharedMem (inCtrlShmName);
            bool setShm = false;
            if (InCtrlShm->isInvalid())
                {
                InCtrlShm =
                    new dtkSharedMem (CTRL_ID_SHM_SIZE, inCtrlShmName);
                setShm = true;
                if (InCtrlShm->isInvalid())
                    {
                    fprintf (stderr, "%s: Unable to open shared memory %s.\n",
                                ProgName, inCtrlShmName);
                    return -1;
                    }
                }


            CtrlIDSize = InCtrlShm->getSize();

            CtrlIDBuffer = (char *) malloc ( (CtrlIDSize+1) * sizeof (char));
            if (CtrlIDBuffer == NULL)
                {
                fprintf (stderr, "%s: Unable to allocate memory (A).\n",
                                ProgName);
                return -1;
                }

            MyCtrlID = (char *) malloc ( (CtrlIDSize+1) * sizeof (char));
            if (MyCtrlID == NULL)
                {
                fprintf (stderr, "%s: Unable to allocate memory (B).\n",
                                ProgName);
                return -1;
                }

            if (strlen(ctrlID) >= CtrlIDSize)
                {
                fprintf (stderr,
                        "%s: Conflict in selector string sizes: \n"
                        "    Specified selector string (%s) is "
                                                  "longer than size of \n"
                        "    selector shared memory file %s.\n"
                        "    Length of string is %d; size of file is %d.\n",
                        ProgName,
                        ctrlID,
                        inCtrlShmName,
                        strlen(ctrlID),
                        CtrlIDSize );
                return -1;
                }

            memset (CtrlIDBuffer, 0, CtrlIDSize+1);
            memset (MyCtrlID, 0, CtrlIDSize+1);

            if (setShm)
                {
                InCtrlShm->write (CtrlIDBuffer);
                }

            strcpy (MyCtrlID, ctrlID);

            }  // end of section for setting up control ID shm
#endif

        return 0; // success
        } // end of setupCtrlShm



int activate ()
	{
	// fprintf (stderr, "activate called\n");


        if (Active)
                {
                return 0;
                }

	// DebugIter = 0;



	// There seems to be some sort of problem getting the contents
	// of the wand and outCoord transformations, but it only seems
	// to happen occasionally the first time that the tracking is 
	// activated.
	// I don't really know, but I suspect that it is some sort of
	// race condition.  I'm putting a .1 second delay here in the first
	// activation so that the node transformation can catch up.  
	static bool firstTime = true;
	if (firstTime)
		{
		usleep (100000);
		firstTime = false;
		}

	sendSgeMsg ("ADDCHILD %s %s\n", CursorDCSName, OutCoordsNodeName);
	trackInCoords ();



	Active = true;
	return 0;
	}  // end of activate

int deactivate ()
	{
	// fprintf (stderr, "deactivate called\n");
        if ( ! Active )
                {
                return 0;
                }



	sendSgeMsg ("REMOVECHILD %s %s\n", CursorDCSName, OutCoordsNodeName);



	Active = false;
	return 0;
	}  // end of deactivate

bool active ()
        {

        if (InCtrlShm)
            {
            // Because we have two ways of activating and deactivating, we
            // have the problem of what to do when both are in use and
            // they give contradictory instructions.
            //
            // At present, the shm method will override.  So the fifo
            // method will only really work when the control shm is not
            // specified.  For an alternative, see how it's implemented
            // in hev-relativeMove.


            // we only do something if the id match has changed

#if 1
            bool newActive = (InCtrlShm->getString() == MyCtrlIDStr);
#else
            if (InCtrlShm->read (CtrlIDBuffer))
                {
                fprintf (stderr,
                     "%s: Error reading selector shared memory.\n",
                     ProgName);
                return Active;
                }

            bool newActive = (strcmp (CtrlIDBuffer, MyCtrlID) == 0) ;
#endif

            
            if (newActive != Active)
                {
                if (newActive)
                        {
                        activate ();
                        }
                else
                        {
                        deactivate ();
                        }
                }
            }

        return Active;
        }  // end of active 

int reset ()
	{
	// fprintf (stderr, "reset called\n");
	return 0;
	}  // end of reset

int constrain (Constraint &constraint)
	{
	fprintf (stderr, "constrain called\n");
	return 0;
	}  // end of constrain

int force (Constraint &constraint)
	{
	fprintf (stderr, "force called\n");
	return 0;
	}  // end of force

int unconstrain ()
	{
	fprintf (stderr, "unconstrain called\n");
	return 0;
	}  // end of unconstrain

int scaleMovement (double scale)
	{
	fprintf (stderr, "scaleMovement called\n");
	return 0;
	}  // end of scaleMovement

int unscaleMovement ()  // sets scale to 1
	{
	fprintf (stderr, "unscaleMovement called\n");
	return 0;
	}  // end of unscaleMovement

int limitBox (double box[2][3])
	{
	fprintf (stderr, "limitBox called\n");
	return 0;
	}  // end of limitBox

int unlimitBox ()
	{
	fprintf (stderr, "unlimitBox called\n");
	return 0;
	}  // end of unlimitBox

int show ()
	{
	fprintf (stderr, "show called\n");
	return 0;
	}  // end of show

int hide ()
	{
	fprintf (stderr, "hide called\n");
	return 0;
	}  // end of hide 

int quit ()
	{
	fprintf (stderr, "quit called\n");
	EXIT (0);
	return 0;
	}  // end of quit



//////////////////////////////////////////////////////////


char *EndPtNodeName[4] = 
    {const_cast<char*>("hssEndPt"), const_cast<char*>("hssEndPtDragHL"), const_cast<char*>("hssEPDelHL"), const_cast<char*>("hssEPAuxHL")};
char *SegNodeName[4] = 
    {const_cast<char*>("hssSeg"), const_cast<char*>("hssSegDragHL"), const_cast<char*>("hssSegDelHL"), const_cast<char*>("hssSegAuxHL")};
char *SegHandleNodeName[4] = 
    {const_cast<char*>("hssHand"), const_cast<char*>("hssHandDragHL"), const_cast<char*>("hssHandDelHL"), const_cast<char*>("hssHandAuxHL")};

static double PtHotSpotDist = 0.02;
static double LnHotSpotDist = 0.02;
double PtHotSpotDistSq = 0.02 * 0.02;
double LnHotSpotDistSq = 0.02 * 0.02;
static bool DisplayLenLabels = false;
int MaxSegments = 200;


static void
loadScaledGlyph (char *nodeName, char *fn, double scale, bool yzOnly)
	{

	if (scale == 1.0)
		{
		sendSgeMsg ("LOAD %s %s\n", nodeName, fn);
		}
	else
		{
		char innerNodeName[1000];
		strcpy (innerNodeName, nodeName);
		strcat (innerNodeName, ".inner");

		sendSgeMsg ("LOAD %s %s\n", innerNodeName, fn);

		if (yzOnly)
			{
			sendSgeMsg ( "SCS %s 0 0 0  0 0 0  1 %g %g\n", 
					nodeName, scale, scale);
			}
		else
			{
			sendSgeMsg ( "SCS %s 0 0 0  0 0 0  %g\n", 
					nodeName, scale);
			}

		sendSgeMsg ("ADDCHILD %s %s\n", innerNodeName, nodeName);
		}


	}  // end of loadScaledGlyph


static int 
setupObjectQuad (
  char *objNodeName[4], 
  char fn[4][1000], 
  double scale, 
  bool yzScaleOnly)
	{

#if 0
	sendSgeMsg ("LOAD %s %s\n", objNodeName[0], fn[0]);
	sendSgeMsg ("LOAD %s %s\n", objNodeName[1], fn[1]);
	sendSgeMsg ("LOAD %s %s\n", objNodeName[2], fn[2]);
	sendSgeMsg ("LOAD %s %s\n", objNodeName[3], fn[3]);
#else

	loadScaledGlyph (objNodeName[0], fn[0], scale, yzScaleOnly);
	loadScaledGlyph (objNodeName[1], fn[1], scale, yzScaleOnly);
	loadScaledGlyph (objNodeName[2], fn[2], scale, yzScaleOnly);
	loadScaledGlyph (objNodeName[3], fn[3], scale, yzScaleOnly);

#endif

	return 0;
	}  // end of setupObjectQuad


static int
setupDefaultEndPtObjects (double scale)
	{

	// For these default objects, we will generate them on the fly
	// rather than relying on the existence of explicitly named
	// object files.



#if 0
	executeCmd (    "savg-cone 50 open revisedNormals | "
			"savg-scale 0.1 0.1 1.0 | "
                        "savg-translate 0.0 0.0 -1.0 > %s/tmp1.savg\n", 
			TmpDirName);
#else
	executeCmd (    "savg-cone 50 open revisedNormals | "
			"savg-scale 0.1 0.1 1.0 | "
                        "savg-translate 0.0 0.0 -1.0 |"
			"savg-scale %g > %s/tmp1.savg\n", 
			scale,
			TmpDirName);

#endif

	executeCmd ("savg-rotate 0 0 180 < %s/tmp1.savg > %s/tmp2.savg",
		TmpDirName, TmpDirName);

	executeCmd ("cat %s/tmp1.savg %s/tmp2.savg > %s/cone_z.savg", 
		TmpDirName, TmpDirName, TmpDirName);


	executeCmd ("cat %s/cone_z.savg | savg-rotate 0 0 90 > %s/cone_x.savg",
				TmpDirName, TmpDirName);

	executeCmd ("cat %s/cone_z.savg | savg-rotate 0 90 0 > %s/cone_y.savg",
				TmpDirName, TmpDirName);



	executeCmd ("cat %s/cone_x.savg %s/cone_y.savg %s/cone_z.savg | "
		  "savg-scale 0.02 | savg-rgb 0.9 0.9 0.9 > %s/endPt.savg",
				TmpDirName, TmpDirName, TmpDirName, TmpDirName);

	sendSgeMsg ("LOAD %s %s/endPt.savg\n",   EndPtNodeName[0], TmpDirName);


	executeCmd ("cat %s/endPt.savg | savg-rotate 10 10 10 | "
		  "savg-rgb 0.9 0.9 0.6 > %s/endPtDragHL.savg",
				TmpDirName, TmpDirName);

	sendSgeMsg ("LOAD %s %s/endPtDragHL.savg\n", 
				EndPtNodeName[1], TmpDirName);

	executeCmd ("cat %s/endPt.savg | savg-rotate 10 10 10 | "
		  "savg-rgb 0.9 0.5 0.5 > %s/endPtDelHL.savg",
				TmpDirName, TmpDirName);

	sendSgeMsg ("LOAD %s %s/endPtDelHL.savg\n", 
				EndPtNodeName[2], TmpDirName);


	executeCmd ("cat %s/endPt.savg | savg-scale 1.2 | "
		  "savg-rgb 1.0 0.3 0.3 > %s/endPtAuxHL.savg",
				TmpDirName, TmpDirName);

	sendSgeMsg ("LOAD %s %s/endPtAuxHL.savg\n", 
				EndPtNodeName[3], TmpDirName);



#if 0
	if ( ! Debug )
		{
		executeCmd ("rm %s/tmp1.savg %s/tmp2.savg", 
				TmpDirName, TmpDirName);
		executeCmd ("rm %s/cone_x.savg %s/cone_y.savg %s/cone_z.savg",
				TmpDirName, TmpDirName, TmpDirName);
		}


#endif


	return 0;
	}  // end of setupDefaultEndPtObjects  


static int
setupDefaultSegObjects (double scale)
	{

#if 0
	executeCmd ("savg-cylinder 4 open | savg-scale 0.002 0.002 1 | "
			"savg-rotate 0 0 90 | "
			"savg-rgb 0.7 0.7 0.7 > %s/seg.savg",   TmpDirName);

	executeCmd ("savg-cylinder 4 open | savg-scale 0.002 0.002 1 | "
			"savg-rotate 0 0 90 | "
			"savg-rgb 0.9 0.9 0.6 > %s/segDragHL.savg", TmpDirName);

	executeCmd ("savg-cylinder 4 open | savg-scale 0.002 0.002 1 | "
			"savg-rotate 0 0 90 | "
			"savg-rgb 0.9 0.3 0.3 > %s/segDelHL.savg", TmpDirName);

	executeCmd ("savg-cylinder 4 open | savg-scale 0.0025 0.0025 1 | "
			"savg-rotate 0 0 90 | "
			"savg-rgb 0.9 0.6 0.6 > %s/segAuxHL.savg", TmpDirName);


#else

	executeCmd ("savg-cylinder 4 open | savg-scale %g %g 1 | "
			"savg-rotate 0 0 90 | "
			"savg-rgb 0.9 0.9 0.9 > %s/seg.savg",   
			0.002*scale, 0.002*scale,
			TmpDirName);

	executeCmd ("savg-cylinder 4 open | savg-scale %g %g 1 | "
			"savg-rotate 0 0 90 | "
			"savg-rgb 0.9 0.9 0.6 > %s/segDragHL.savg", 
			0.002*scale, 0.002*scale,
			TmpDirName);

	executeCmd ("savg-cylinder 4 open | savg-scale %g %g 1 | "
			"savg-rotate 0 0 90 | "
			"savg-rgb 0.9 0.5 0.5 > %s/segDelHL.savg", 
			0.002*scale, 0.002*scale,
			TmpDirName);

	executeCmd ("savg-cylinder 4 open | savg-scale %g %g 1 | "
			"savg-rotate 0 0 90 | "
			"savg-rgb 1.0 0.3 0.3 > %s/segAuxHL.savg", 
			0.0025*scale, 0.0025*scale,
			TmpDirName);


#endif



	sendSgeMsg ("LOAD %s %s/seg.savg\n",   SegNodeName[0], TmpDirName);
	sendSgeMsg ("LOAD %s %s/segDragHL.savg\n", SegNodeName[1], TmpDirName);
	sendSgeMsg ("LOAD %s %s/segDelHL.savg\n", SegNodeName[2], TmpDirName);
	sendSgeMsg ("LOAD %s %s/segAuxHL.savg\n", SegNodeName[3], TmpDirName);


	return 0;
	}  // end of setupDefaultSegObjects


static int
setupDefaultSegHandleObjects (double scale)
	{

        double finalScale = 0.006*scale;

        // Create handles.  Each handle is an octahedron.  
        // savg-sphere 8 creates the polygons, but with sphere normals.
        // savg-add-normals -replace  replaces sphere normals with poly norms
       
	executeCmd (
	   "savg-sphere 8 | savg-add-normals -replace |  savg-scale %g | "
	   "savg-rgb 0.9 0.9 0.9 > %s/tmp.savg", finalScale, TmpDirName);
	executeCmd (
	   // "hev-osgconv %s/tmp.savg %s/hand.osg > /dev/null", 
	          "osgconv %s/tmp.savg %s/hand.osg > /dev/null", 
           TmpDirName, TmpDirName);


	executeCmd (
	   "savg-sphere 8 | savg-add-normals -replace |  savg-scale %g | "
	   "savg-rgb 0.9 0.9 0.6 > %s/tmp.savg", finalScale, TmpDirName);
	executeCmd (
	   // "hev-osgconv %s/tmp.savg %s/handDragHL.osg > /dev/null", 
	          "osgconv %s/tmp.savg %s/handDragHL.osg > /dev/null", 
           TmpDirName, TmpDirName);


	executeCmd (
	   "savg-sphere 8 | savg-add-normals -replace |  savg-scale %g | "
	   "savg-rgb 0.9 0.5 0.5 > %s/tmp.savg", finalScale, TmpDirName);
	executeCmd (
	   // "hev-osgconv %s/tmp.savg %s/handDelHL.osg > /dev/null", 
	          "osgconv %s/tmp.savg %s/handDelHL.osg > /dev/null", 
           TmpDirName, TmpDirName);


	executeCmd (
	   "savg-sphere 8 | savg-add-normals -replace |  savg-scale %g | "
	   "savg-rgb 1.0 0.3 0.3 > %s/tmp.savg", finalScale, TmpDirName);
	executeCmd (
	   // "hev-osgconv %s/tmp.savg %s/handAuxHL.osg > /dev/null", 
	          "osgconv %s/tmp.savg %s/handAuxHL.osg > /dev/null", 
           TmpDirName, TmpDirName);



	sendSgeMsg ("LOAD %s %s/hand.osg\n", 
					SegHandleNodeName[0], TmpDirName);
	sendSgeMsg ("LOAD %s %s/handDragHL.osg\n", 
					SegHandleNodeName[1], TmpDirName);
	sendSgeMsg ("LOAD %s %s/handDelHL.osg\n", 
					SegHandleNodeName[2], TmpDirName);
	sendSgeMsg ("LOAD %s %s/handAuxHL.osg\n", 
					SegHandleNodeName[3], TmpDirName);

#if 0
	if ( ! Debug )
		{
		executeCmd ("rm %s/tmp.savg", TmpDirName);
		}
#endif

	return 0;
	}  // end of setupDefaultSegHandleObjects



int setupObjects (char objectFileNames[3][4][1000], double glyphScale)
	{
	// objectFileNames[0]  -  end point markers
	// objectFileNames[1]  -  line segments
	// objectFileNames[2]  -  line segment center handles

	int rtn = 0;

	// fprintf (stderr, "setupSegmentObjects called\n");

	if (objectFileNames[0][0][0] == 0)
		{
		rtn += setupDefaultEndPtObjects (glyphScale);
		}
	else
		{
		rtn += setupObjectQuad (EndPtNodeName, objectFileNames[0], 
						glyphScale, false);
		}


	if (objectFileNames[1][0][0] == 0)
		{
		rtn += setupDefaultSegObjects (glyphScale);
		}
	else
		{
		rtn += setupObjectQuad (SegNodeName, objectFileNames[1], 
						glyphScale, true);
		}


	if (objectFileNames[2][0][0] == 0)
		{
		rtn += setupDefaultSegHandleObjects (glyphScale);
		}
	else
		{
		rtn += setupObjectQuad (SegHandleNodeName, objectFileNames[2], 
						glyphScale, false);
		}

	// JGH the following line is a kludge.  
	// Should load its own object for cursor.
	CursorNodeName = EndPtNodeName[0];
	sendSgeMsg ("DCS %s\n", CursorDCSName);
	sendSgeMsg ("ADDCHILD %s %s\n", CursorNodeName, CursorDCSName);

	setLenLabelScale (glyphScale);

    setEditDistances (PtHotSpotDist*glyphScale, LnHotSpotDist*glyphScale);

	return rtn;
	}  // end of setupObjects



int setEditDistances (double ptDist, double lnDist)
	{


	PtHotSpotDist = ptDist;
	LnHotSpotDist = lnDist;

	PtHotSpotDistSq = PtHotSpotDist * PtHotSpotDist;
	LnHotSpotDistSq = LnHotSpotDist * LnHotSpotDist;

#if 0
	if (Verbose)
		{
		fprintf (stderr, 
			"setEditDistances: pt %g ln %g    ptSq %g lnSq %g\n",
			PtHotSpotDist, LnHotSpotDist, 
			PtHotSpotDistSq, LnHotSpotDistSq);
		}
#endif

	return 0;
	}  // end of setEditDistances


int lenLabel (bool lenLabelOn)
	{
	// fprintf (stderr, "lenLabel called\n");
	DisplayLenLabels = lenLabelOn;
	return 0;
	}  // end of lenLabel




int setMaxSegments (int maxSegments)
	{
	// fprintf (stderr, "setMaxSegments called\n");
	if (maxSegments > PTSHM_MAX_NUM_SEGS)
		{
		fprintf (stderr, 
		  "%s: Specified number of segments (%s) exceeds maximum (%d).\n",
		  ProgName, maxSegments, PTSHM_MAX_NUM_SEGS);
		return -1;
		}
	MaxSegments = maxSegments;
	
	return 0;
	}  // end of setMaxSegments

//////////////////////////////////////////////////////////

static dtkMatrix getMatrix(osg::Matrix &o)
    {
        dtkMatrix m ;
        if (o(0,3) != 0.f || o(1,3) != 0.f || o(2,3) != 0.f || o(3,3) != 1.f)
        {
        fprintf (stderr, 
            "%s: getMatrix: Error converting matrices.\n", ProgName);
        }
        else
        {
            osg::Vec3d s, tv ;
            osg::Quat q, so ;
	    iris::Decompose(o, &tv, &q, &s) ;
#define SCALETOLERANCE (.0001)
            if (fabs(s.x()-s.y())>SCALETOLERANCE ||
                fabs(s.x()-s.z())>SCALETOLERANCE ||
                fabs(s.y()-s.y())>SCALETOLERANCE)
                {
                fprintf (stderr, 
                        "%s: getMatrix: "
                        "Error converting matrices (non-uniform scaling).\n", 
                        ProgName);
                }

            m.scale(s.x()) ;
            m.quat(q.x(), q.y(), q.z(), q.w()) ;
            m.translate(tv.x(), tv.y(), tv.z()) ;
        }
        return m ;
    }  // end of getMatrix










// #define WAND_OFFSET (0.05)
#define WAND_OFFSET (0.00)

static int
xformInToOut (float inCoord[6], float outCoord[6], float offset[3])
	{


        // JGH: this routine has become a no-op because I removed
        // any transformation of in and out coords

        memcpy (outCoord, inCoord, 6*sizeof(float));

	return 0;
	}  // end of xformInToOut


static float CursorCoords[6] = { 0, 0, 0, 0, 0, 0 };
static float CursorOffset[3] = { 0, 0, 0 };

void
setYOffset (float yOffset)
	{
	CursorOffset[1] = yOffset;
	}  // end of setYOffset

void
getCursorLocation (float xyz[3])
	{
	memcpy (xyz, CursorCoords, 3*sizeof(float));
	}  // end of getCursorLocation

static int
getWandishCoord (float inCoord[6]) 
        {
        if (WandishIsCoord)
            {
	    return WandishShm->read (inCoord);
            }
        else
            {
            double matVals[16];
            matVals[0] = 123;
	    if ( ! WandishShm->read (matVals) )
		{
                // derive inCoord from mat
                osg::Matrix mat (matVals);
                dtkCoord dc = iris::MatrixToCoord (mat);
                dc.get (inCoord);
                return 0;
                }
            }

        return -1;
        } // end of getWandishCoord


int
trackInCoords ()
	{
// fprintf (stderr, "trackInCoords\n");

	static float prevInCoords[6] = {HUGE_VALF, HUGE_VALF, HUGE_VALF, HUGE_VALF, HUGE_VALF, HUGE_VALF};

	// get input coords
	float inCoord[6];
#if 0
	if (WandishShm->read (inCoord, sizeof(inCoord)))
		{
		fprintf (stderr, "%s: Error reading tracked input coords.\n",
			ProgName);
		return -1;
		}
#else
	if ( getWandishCoord (inCoord) )
		{
		fprintf (stderr, "%s: Error reading tracked input coords.\n",
			ProgName);
		return -1;
		}
#endif

#if 0
	DebugIter ++;
	if (DebugIter <= 5)
			{
			fprintf (stderr, "inCoord = %g %g %g %g %g %g\n",
				inCoord[0],
				inCoord[1],
				inCoord[2],
				inCoord[3],
				inCoord[4],
				inCoord[5]);
			}
#endif


	if ( 
		(prevInCoords[0] == inCoord[0]) &&
		(prevInCoords[1] == inCoord[1]) &&
		(prevInCoords[2] == inCoord[2]) &&
		(prevInCoords[3] == inCoord[3]) &&
		(prevInCoords[4] == inCoord[4]) &&
		(prevInCoords[5] == inCoord[5]) )
		{
		return 0;
		}

	memcpy (prevInCoords, inCoord, 6*sizeof(float));


	// transform to out coordinates
	float outCoord[6];
	if (xformInToOut (inCoord, outCoord, CursorOffset))
		{
		return -1;
		}

	memcpy (CursorCoords, outCoord, 6*sizeof(float));

	// write out sge command to move cursor
	sendSgeMsg ("DCS %s  %f %f %f  %f %f %f\n",
		CursorDCSName,
		outCoord[0], outCoord[1], outCoord[2],
		outCoord[3], outCoord[4], outCoord[5]);

#if 0
	if (DebugIter <= 5)
			{
			fprintf (stderr, "outCoord = %g %g %g %g %g %g\n",
				outCoord[0],
				outCoord[1],
				outCoord[2],
				outCoord[3],
				outCoord[4],
				outCoord[5]);
			}
#endif


	}  // end of trackInCoords



#if 0
int userInteractions ()
	{
	if (Active)
		{
		trackInCoords ();
		}

	// fprintf (stderr, "userInteractions called\n");
	return 0;
	}  // end of userInteractions
#endif


