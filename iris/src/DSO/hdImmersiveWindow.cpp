#include <dtk.h>
#include <dtk/dtkDSO_loader.h>

#include <iris/Augment.h>
#include <iris/ImmersivePane.h>
#include <iris/Utils.h>

/*
 * This code was copied from immersive.cpp and modifed for a generic HD display.
 *
 * Refer to that file for coments. This file differes that it only generates,
 * the DSO for generic HD monitor that is 20.5 inches wide and 12 inches
 * high. And the string "hd" was globally substituted to "Rave".
 *
 */

//#if 0
#if 1
/****************************/
// Debug output: a stack backtrace
#include <execinfo.h>
#define BT_BUF_SIZE 1000000
void
printBacktrace (void)
{
    int j, nptrs;
    void *buffer[BT_BUF_SIZE];
    char **strings;
    fflush (stderr);

    fprintf (stderr, "\n-------\n");
    nptrs = backtrace(buffer, BT_BUF_SIZE);
    fprintf(stderr, "backtrace() returned %d addresses\n", nptrs);

    /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
       would produce similar output to the following: */

    strings = backtrace_symbols(buffer, nptrs);

    for (j = 0; j < nptrs; j++)
        fprintf(stderr, "%s\n", strings[j]);

    free(strings);

    fprintf (stderr, "-------\n\n");

    fflush (stderr);

} // end of printBacktrace
/****************************/
#endif




//////////////////////////////////////////////////////////////////////////
//
// Here are some preprocessor macros that are used so that we can unify
// the code for the different displays. 
//
// This might be overkill, but it's not really not too bad.
//
// We require that the symbol SCR_NAME has been defined as one of the
// following:  raveFront, raveBack,  raveLeft, raveRight
//             raveFloor, raveCeiling, or powerwall.
//

// We define inner versions of these so that symbol substitution is done.
#define CONCAT_INNER(a,b)   a ## b 
#define CONCAT(a,b)  CONCAT_INNER(a,b)
#define STRINGIFY_INNER(a) #a
#define STRINGIFY(a) STRINGIFY_INNER(a)


// Define another screen name symbol which is SCR_NAME with _SN at the end.
#define SCR_NAME_SN CONCAT(SCR_NAME, _SN)

// Define cpp symbols for the valid SCR_NAME_SNs.  
// Because of how these are used, they must be the screen number plus 100.

#define hdFront_SN   (100)
#define hdLeft_SN    (100)
#define hdFloor_SN   (100)

#define hdBack_SN    (100)
#define hdRight_SN   (100)
#define hdCeiling_SN (100)

// The screen number used by OSG
#define SCR_NUM (SCR_NAME_SN-100)

// NOTE: The screen numbers correspond to the display names:
//              :0.0 :0.1 :0.2 :0.3
//       This is why the windows created here appear on the
//       correct RAVE screen. At least that is my understanding.


// Test the validity of SCR_NAME
#ifdef SCR_NAME
    // If SCR_NAME_SN is not one of the pre-defined non-zero screen numbers,
    // then we have an error.  
    // Note that undefined symbols evaluate to zero in the #if tests,
    // so we really only need to test against zero.
    #if ( ! SCR_NAME_SN )
        // compile time error
        #error "SCR_NAME is not valid."
    #endif
#else
    // compile time error
    #error "SCR_NAME is not defined."
#endif


#if defined(SIM) && defined(CUBE)
    #error "Cannot specify both SIM and CUBE preprocessor symbols."
#endif




// These defines create variable names corresponding to the
// specified screen name. 
// (E.g. raveFrontVp versus raveLeftVp versus ... )
#ifdef CUBE
    #ifdef OVERLAP
         #define VP              CONCAT ( SCR_NAME , CubeOverlapVp )
         #define CENTER          CONCAT ( SCR_NAME , CubeOverlapCenter )
         #define SCR_SIZE        CONCAT ( SCR_NAME , CubeOverlapSize )
    #else
         #define VP              CONCAT ( SCR_NAME , CubeVp )
         #define CENTER          CONCAT ( SCR_NAME , CubeCenter )
         #define SCR_SIZE        CONCAT ( SCR_NAME , CubeSize )
    #endif
#else
    #define VP              CONCAT ( SCR_NAME , Vp )
    #define CENTER          CONCAT ( SCR_NAME , Center )
    #define SCR_SIZE        CONCAT ( SCR_NAME , Size )
#endif


#define ORIENTATION     CONCAT ( SCR_NAME , Orientation )
#define PIX_PER_HD    CONCAT ( SCR_NAME , PixelPerhdUnit)
#define SIM_WIN_OFFSET  CONCAT ( SCR_NAME , SimWinOffset)




// Here's a quoted string for the screen name
#define SCR_NAME_STR STRINGIFY(SCR_NAME)

// The class name that we're creating
#if defined(SIM)
    #define CLASS_NAME CONCAT (SCR_NAME, SimWindow)
#elif defined(CUBE)
    #ifdef OVERLAP
         #define CLASS_NAME CONCAT (SCR_NAME, CubeOverlapWindow)
    #elif defined(OFFSCREEN)
         #define CLASS_NAME CONCAT (SCR_NAME, CubeOffscreenWindow)
    #else
         #define CLASS_NAME CONCAT (SCR_NAME, CubeWindow)
    #endif
#else
    #define CLASS_NAME CONCAT (SCR_NAME, Window)
#endif



// The class name as a quoted string
#define CLASS_NAME_STR STRINGIFY(CLASS_NAME)


//
// end of the pre-processor stuff
//
/////////////////////////////////////



// Use namespace so this DSO don't interfere with other DSO's
namespace CLASS_NAME
{
    

    ////////////////////////////////////////////////////////////////////////
    //
    // DISPLAY PARAMETERS
    //
    // Probably only need to change the parameters specified in this section.
    //

    // The pixel dimensions of the full screen. 
    // ASSUMPTION: All screens are the same.
    int fullDisplayPixelDim[2] = {1080, 1080};


    // Viewports: x, y, w, h
    int hdFrontVp[4] =   {0, 0, 1080, 1080};
    int hdLeftVp[4] =    {0, 0, 1080, 1080};
    int hdFloorVp[4] =   {0, 0, 1080, 1080};
    int hdBackVp[4] =    {0, 0, 1080, 1080};
    int hdRightVp[4] =   {0, 0, 1080, 1080};
    int hdCeilingVp[4] = {0, 0, 1080, 1080};

    // Actual physical dimensions of the screens (viewport).
    // In this case we're using inches.
//    double hdFrontSize[2] =   { 20.5, 12.0 };
//    double hdBackSize[2] =    { 20.5, 12.0 };
//    double hdLeftSize[2] =    { 20.5, 12.0 };
//    double hdRightSize[2] =   { 20.5, 12.0 };
//    double hdFloorSize[2] =   { 20.5, 12.0 };
//    double hdCeilingSize[2] = { 20.5, 12.0 };

    double hdFrontSize[2] =   { 12.0, 12.0 };
    double hdBackSize[2] =    { 12.0, 12.0 };
    double hdLeftSize[2] =    { 12.0, 12.0 };
    double hdRightSize[2] =   { 12.0, 12.0 };
    double hdFloorSize[2] =   { 12.0, 12.0 };
    double hdCeilingSize[2] = { 12.0, 12.0 };

    // Physical length of one display unit (RAVE coord unit)
    // Same units should be used as for Size params.
    // In this case we're using inches.
  //    double sizeOfOnehdUnit = 10.25; 
    double sizeOfOnehdUnit = 6.0; 

    // Current situation is that all screens have same pixel/rave unit.
    double PixelPerhdUnit[2];  // This will be calculated below
    double *hdFrontPixelPerhdUnit = PixelPerhdUnit;
    double *hdBackPixelPerhdUnit = PixelPerhdUnit;
    double *hdLeftPixelPerhdUnit = PixelPerhdUnit;
    double *hdRightPixelPerhdUnit = PixelPerhdUnit;
    double *hdFloorPixelPerhdUnit = PixelPerhdUnit;
    double *hdCeilingPixelPerhdUnit = PixelPerhdUnit;

    //
    // End of section of parameters to change.
    // 
    ////////////////////////////////////////////////////////////////////////
    





    ////////////////////////////////////////////////////////////////////////
    //  Probably don't need to change below here.

    class CLASS_NAME : public iris::Augment
    {
    public:
        CLASS_NAME ():iris::Augment(CLASS_NAME_STR) 
        {
        
            setDescription("%s", getName()) ;

            ///////////////////////////////////////////////////////////
            // Some of the following calculations are not used in some
            // of the DSO's, but they are short and only done once.


            // Specify screen orientations; these should probably not change.
            // If you do change the orientations, you are probably breaking
            // the assumptions about how the screen boundaries match up.
            // In this case, you should also look at changing the calculation
            // of the centers below.
            //
            // ASSUMPTIONS: 
            //      front   perpendicular to Y
            //      left    perpendicular to X
            //      floor   perpendicular to Z
            //      back    perpendicular to Y
            //      right   perpendicular to X
            //      ceiling perpundicular to Z
            //
            //      powerwall perpendicular to Y

            //      viewports:
            //          right edge of left    ==  left edge of front 
            //          bottom edge of left   ==  top edge of floor
            //          bottom edge of front  ==  right edge of floor
            //          left edge of right    ==  right edge of front
            //          top edge of left      ==  top edge of ceiling

            osg::Quat hdFrontOrientation = iris::EulerToQuat(0.f, 0.f, 0.f);
            osg::Quat hdLeftOrientation = iris::EulerToQuat(90.f, 0.f, 0.f);
            osg::Quat hdFloorOrientation= iris::EulerToQuat(90.f, -90.f, 0.f);

            osg::Quat hdBackOrientation = iris::EulerToQuat(180.f, 0.f, 0.f);
            osg::Quat hdRightOrientation = iris::EulerToQuat(-90.f, 0.f, 0.f);
            osg::Quat hdCeilingOrientation=iris::EulerToQuat(90.f, 90.f, 0.f);



            // Calculate the RAVE coords of the corner point at which the 
            // three screen meet.
            //
            // ASSUMPTION: Corner where screens converge is one unit out
            //             from RAVE origin in appropriate directions.
            double frontLeftFloorCornerhdCoords[3] = {-1.0, 1.0, -1.0};


            // Calculate the number of pixels in a RAVE unit.
            // ASSUMPTION: All screens have the same pixel per rave unit.
            //
            // Note that this calculation is being done with the parameters
            // for the screen for this compilation.
            PixelPerhdUnit[0] = VP[2] / (SCR_SIZE[0]/sizeOfOnehdUnit);
            PixelPerhdUnit[1] = VP[3] / (SCR_SIZE[1]/sizeOfOnehdUnit);


            //////
            // Now we are calculating the centers for all of the screens, 
            // but we only use one of the centers.  This is a bit
            // wasteful; we could change it to calculate only the
            // center that we need.


            // Calculate the centers of each of the screens in RAVE coords.
            // ASSUMPTION: Corners converge as described above.
            //             Note that centers based on offsets from corner.
            osg::Vec3 hdFrontCenter = 
              osg::Vec3 ( 
                frontLeftFloorCornerhdCoords[0] + 
                                (hdFrontSize[0]/2.0)/sizeOfOnehdUnit, 
                frontLeftFloorCornerhdCoords[1],
                frontLeftFloorCornerhdCoords[2] + 
                                (hdFrontSize[1]/2.0)/sizeOfOnehdUnit
                );

            osg::Vec3 hdLeftCenter = 
              osg::Vec3 ( 
                frontLeftFloorCornerhdCoords[0],
                frontLeftFloorCornerhdCoords[1] -
                                (hdLeftSize[0]/2.0)/sizeOfOnehdUnit, 
                frontLeftFloorCornerhdCoords[2] + 
                                (hdLeftSize[1]/2.0)/sizeOfOnehdUnit
                );

            // ASSUMPTION: X dir of floor matches X dir of left,
            //   that top edge of floor vp coincides with bottom edge of 
            //   left screen, and that righta edge of floor coincides
            //   with bottom edge of front screen.
            osg::Vec3 hdFloorCenter = 
              osg::Vec3 ( 
                frontLeftFloorCornerhdCoords[0] +
                                (hdFloorSize[1]/2.0)/sizeOfOnehdUnit,
                frontLeftFloorCornerhdCoords[1] -
                                (hdFloorSize[0]/2.0)/sizeOfOnehdUnit, 
                frontLeftFloorCornerhdCoords[2]
                );



            osg::Vec3 hdBackCenter = 
              osg::Vec3 ( 
                         hdFrontCenter[0],
                         -1*hdFrontCenter[1],
                         hdFrontCenter[2]

                );



            osg::Vec3 hdRightCenter = 
              osg::Vec3 ( 
                         -1*hdLeftCenter[0],
                         hdLeftCenter[1],
                         hdLeftCenter[2]

                );


            osg::Vec3 hdCeilingCenter = 
              osg::Vec3 ( 
                         hdFloorCenter[0],
                         hdFloorCenter[1],
                         -1*hdFloorCenter[2]

                );



            //
            ///////////////////////////////////////////////////////////
            //
            // Here's some stuff that is always the same:

            osg::ref_ptr<iris::Window> window = new iris::Window(SCR_NAME_STR) ;
            osg::GraphicsContext::Traits* const traits = window->getTraits() ;
            osg::ref_ptr<iris::ImmersivePane> pane = 
                            new iris::ImmersivePane(window.get(),SCR_NAME_STR) ;
            window->addPane(pane) ;
	    //            window->setStereo (true) ;
            window->setStereo (false) ;

            pane->setNear(.1f) ;
            pane->setFar(1000.f) ;
            traits->supportsResize = false ;
            traits->doubleBuffer = true;

            traits->windowDecoration = false;
            traits->screenNum = SCR_NUM ;

            //
           ///////////////////////////////////////////////////////////
            //
            // Here's stuff that varies based on parameters


            // These are introduced to facilitate modifications for the 
            // simulator windows.
            int pixOffset[2] = {0,0};
            double pixScale = 1.0;




            // Set screen X and Y locations for window? Origin at upper left.
            // ASSUMPTION: RAVE window covers entire screen and all screens are
            //             the same size.
            traits->x = pixOffset[0];
            traits->y = pixOffset[1];

            traits->width  = VP[2] * pixScale + 0.5;
            traits->height = VP[3] * pixScale + 0.5;

            osg::Vec2 vpExtent = osg::Vec2 ( VP[2]/PIX_PER_HD[0], 
                                             VP[3]/PIX_PER_HD[1] ) ;


	    fprintf(stderr,"vp[0]=%d vp[1]=%d vp[2]=%d vp[3]=%d \n",
		    VP[0], VP[1], VP[2], VP[3]);
            pane->setViewport (VP[0], VP[1], VP[2], VP[3]);


            // Place the screens in the rave coordinate system.

            // ASSUMPTION: Pixel per rave unit is constant in all directions
            //             and on all screens.
            pane->setExtent ( vpExtent );

            pane->setCenter ( CENTER );

            pane->setOrientation ( ORIENTATION );



            dtkMsg.add(DTKMSG_DEBUG, "\n");
            
            dtkMsg.add(DTKMSG_DEBUG, "%s : Screen Number: %d\n",
                     CLASS_NAME_STR, 
                     traits->screenNum );
            dtkMsg.add(DTKMSG_DEBUG, "%s : Window:   loc: %d %d   w/h: %d %d\n",
                     CLASS_NAME_STR, 
                     traits->x, traits->y, traits->width, traits->height);
           dtkMsg.add(DTKMSG_DEBUG, "%s : Viewport: loc: %d %d   w/h: %d %d\n",
                     CLASS_NAME_STR, 
                     VP[0], VP[1], VP[2], VP[3]);
            dtkMsg.add(DTKMSG_DEBUG, "%s : Extent: %f %f\n",
                     CLASS_NAME_STR, 
                     vpExtent[0], vpExtent[1] );
            dtkMsg.add(DTKMSG_DEBUG, "%s : Center: %f %f %f\n",
                     CLASS_NAME_STR, 
                     CENTER[0], CENTER[1], CENTER[2]);
            dtkMsg.add(DTKMSG_DEBUG, "%s : Orientation Quat : %f %f %f %f\n",
                     CLASS_NAME_STR, 
                     ORIENTATION[0], ORIENTATION[1], 
                     ORIENTATION[2], ORIENTATION[3]);
            double h, p, r;
            iris::QuatToEuler (ORIENTATION, &h, &p, &r);
            dtkMsg.add(DTKMSG_DEBUG, "%s : Orientation Euler: %f %f %f\n",
                     CLASS_NAME_STR, 
                     h, p, r);
            dtkMsg.add(DTKMSG_DEBUG, "\n");

            // printBacktrace ();

            ///////////
            // dtkAugment::dtkAugment() will not validate the object
            validate() ;

        } ;  // end of constructor

    } ;   // end of class CLASS_NAME

} ;  // end of namespace CLASS_NAME


/************ DTK C++ dtkAugment loader/unloader functions ***************
 *
 * All DTK dtkAugment DSO files are required to declare these two
 * functions.  These function are called by the loading program to get
 * your C++ objects loaded.
 *
 *************************************************************************/

static dtkAugment* dtkDSO_loader(dtkManager* manager, void* p)
{
    return new CLASS_NAME::CLASS_NAME;
}

static int dtkDSO_unloader(dtkAugment* augment)
{
#if 0
    delete augment;
#endif
    return DTKDSO_UNLOAD_CONTINUE;
}

