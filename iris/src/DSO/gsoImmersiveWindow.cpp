
// Display type specified in the environment variable HEV_DISPLAY_TYPE
//
//  Value                            Use
//  -----                            ----
//
//  ARC                               when running at NC A&T
//
//  gso                               when debugging with a 4K UHD TV
//
//  UHD                               when running on a 4K UHD TV





#if defined(ARC)

#define gso_screenSize_X 11520         // 6*1920
#define gso_screenSize_Y 4320          // 4*1080

#define gso_screen_X    0
#define gso_screen_Y    0

#define gso_screenInches_X 211.22      // physical size of ARC in inches
#define gso_screenInches_Y 90.5

#define gso_OneUnitInches 105.61       // 1 CAVE unit, aka HEV unit, aka RAVE unit, aka unit, in inches: 211.22/2




#elif defined(gso)

#define gso_screenSize_X 3840          // ARC6 scaled by 1/3 to fit 4K UHD TV: 11520/3
#define gso_screenSize_Y 1440          //                                      4320/3

#define gso_screen_X    0
#define gso_screen_Y    0

#define gso_screenInches_X 36.         // Physical size of 40" 4K UHD TV
#define gso_screenInches_Y 20.5

#define gso_OneUnitInches 18.0         // 36/2




#else   // UHD

#define gso_screenSize_X 3840           // 2*1920
#define gso_screenSize_Y 2160           // 2*1080

#define gso_screen_X    0
#define gso_screen_Y    0

#define gso_screenInches_X 36.
#define gso_screenInches_Y 20.5

#define gso_OneUnitInches 18.0

#endif















#include <dtk.h>
#include <dtk/dtkDSO_loader.h>

#include <iris/Augment.h>
#include <iris/ImmersivePane.h>
#include <iris/Utils.h>

/*
    This code is intended to serve for the display DSO's for the GSO
    front, back, left, right, floor, ceiling and pwrwall in both the
    actual immersive enviornments and for the simulator.

    The DSO's produded by this code are invoked by programs such
    as iris-viewer, iris-DISPLAYfromWindow, iris-immersiveWindowBelow,
    and iris-wandMouse.

    The preprocessor symbol SCR_NAME must be defined as one of the
    following:
                gsoFront, gsoBack,
                gsoLeft,  gsoRight,
                gsoFloor, gsoCeiling

             or 
                pwrwall.

    The preprocessor symbol SIM should be defined only if you want to
    compile the simulator DSOs

    The preprocessor symbol CUBE should be defined only if you want to
    compile the non-overlapping cube map DSOs.

    The preprocessor symbol CUBE should be defined only if you want to
    compile the cube map DSOs. The preprocessor symbol OVERLAP may
    also be defined with CUBE to produce display overlap. Otherwise
    the DSOs are non-overlapping. The preprocessor symbol OFFSCREEN
    may also be defined with CUBE to produce offscreen rendering.
    Otherwise the DSOs use on-screen rendering.

    You should never specify CUBE and SIM at the same time.

    The simulator DSOs are intended to provide desktop windows that 
    correspond to the CAVE (GSO) device. These windows are at a lower
    resolution than the CAVE displays so that they can all be displayed
    without overlap on the desktop. Even though the CAVE currently 
    has only three screens, we provide six DSO which include the three
    screens that we currently do not have in the actual CAVE.

    The cube map DSOs are sort of like the simulator DSO in that they
    provide six "screens" that are intended to be displayed in six
    separate windows on the desktop. These "screens" are the faces
    of an axis-aligned cube that is centered at the origin and extends 
    from -1 to 1 in each dimension. 


    This code can produce DSO's with these names:
        gsoFrontWindow      gsoBackWindow
        gsoLeftWindow       gsoRightWindow
        gsoFloorWindow      gsoCeilingWindow

        pwrwallWindow

        gsoFrontSimWindow   gsoBackSimWindow
        gsoLeftSimWindow    gsoRightSimWindow
        gsoFloorSimWindow   gsoCeilingSimWIndow

        pwrwallSimWindow


        gsoFrontCubeWindow      gsoBackCubeWindow
        gsoLeftCubeWindow       gsoRightCubeWindow
        gsoFloorCubeWindow      gsoCeilingCubeWindow

        gsoFrontCubeOffscreenWindow      gsoBackCubeOffscreenWindow
        gsoLeftCubeOffscreenWindow       gsoRightCubeOffscreenWindow
        gsoFloorCubeOffscreenWindow      gsoCeilingCubeOffscreenWindow

        gsoFrontCubeOverlapWindow      gsoBackCubeOverlapWindow
        gsoLeftCubeOverlapWindow       gsoRightCubeOverlapWindow
        gsoFloorCubeOverlapWindow      gsoCeilingCubeOverlapWindow



    To specify the parameters for each display, see the section below
    labelled DISPLAY PARAMETERS.

    Because of the generalization of the code to handle all the screens,
    the code is more complicated than would be needed if each of the
    them were each coded separately. I've tried to simplify the parameters
    that need to be changed, but this complicates the rest of the code.

    The term "GSO coordinates" and "display coordinates" are both
    used below. These two terms are synonymous. (This is the
    coordinate system that was called dpf coordinates when it was
    based on SGI Performer.)

    Assumptions for GSO displays:

        - All DISPLAY sizes for the real CAVE screens are identical 
            in pixels (for example 3840x2160), even if a smaller 
            area is actually used for the physical displays. 
            This assumption would be easy to change.

        - We will create windows that cover the entire DISPLAY

        - We will create viewports in each window that are places
            as specified under DISPLAY PARAMETERS

        - The left edge of the front viewport coincides with 
                the right edge of the left viewport

        - The bottom edge of the left viewport coincides with 
                the top edge of the floor viewport

        - The bottom edge of the front viewport coincides with 
                the right edge of the floor viewport

        - The following points coincide:
                - the lower left corner of the front viewport
                - the lower right corner of the left viewport
                - the upper right corner of the floor viewport

        - One unit in GSO coordinate is 48 inches.

        - The GSO coordinate system is aligned with the physical
            screens as follows:
                - The front wall is perpendicular to GSO Y
                - The left wall is perpendicular to GSO X
                - The floor is perpendicular to GSO Z

        - The GSO coord origin is aligned such that the corner
                where the screens converge is at (-1, 1, -1).

                ALTERNATIVE: The GSO origin is aligned with 
                    the center of the front wall and the
                    horizontal center of the left wall.

        - The Cube DSOs represent "screens" that are the faces
            of an axis-aligned cube that is centered at the 
            origin and extends from -1 to 1 in each dimension. 


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
// following:  gsoFront, gsoBack,  gsoLeft, gsoRight
//             gsoFloor, gsoCeiling, or pwrwall.
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
#define gsoFront_SN   (101)
#define gsoLeft_SN    (102)
#define gsoFloor_SN   (103)

#define gsoBack_SN    (104)
#define gsoRight_SN   (105)
#define gsoCeiling_SN (106)

#define pwrwall_SN    (101)

#define ARCFront_SN   (101)
#define UHDFront_SN   (101)

// The screen number used by OSG
#define SCR_NUM (SCR_NAME_SN-100)

// NOTE: The screen numbers correspond to the display names:
//              :0.0 :0.1 :0.2 :0.3
//       This is why the windows created here appear on the
//       correct GSO screen. At least that is my understanding.


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
// (E.g. gsoFrontVp versus gsoLeftVp versus ... )
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
#define PIX_PER_GSO    CONCAT ( SCR_NAME , PixelPerGsoUnit)
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
    int fullDisplayPixelDim[2] = {gso_screenSize_X, gso_screenSize_Y};


    // Viewports: x, y, w, h
    int gsoFrontVp[4] =   {gso_screen_X, gso_screen_Y, gso_screenSize_X, gso_screenSize_Y};
    int gsoLeftVp[4] =    {0, 0, gso_screenSize_X, gso_screenSize_Y};
    int gsoFloorVp[4] =   {0, 0, gso_screenSize_X, gso_screenSize_Y};
    int gsoBackVp[4] =    {0, 0, gso_screenSize_X, gso_screenSize_Y};
    int gsoRightVp[4] =   {0, 0, gso_screenSize_X, gso_screenSize_Y};
    int gsoCeilingVp[4] = {0, 0, gso_screenSize_X, gso_screenSize_Y};

    int pwrwallVp[4] =   {0, 0, gso_screenSize_X, gso_screenSize_Y};

    int gsoFrontCubeVp[4] =   {0, 0, 512, 512};
    int gsoLeftCubeVp[4] =    {0, 0, 512, 512};
    int gsoFloorCubeVp[4] =   {0, 0, 512, 512};
    int gsoBackCubeVp[4] =    {0, 0, 512, 512};
    int gsoRightCubeVp[4] =   {0, 0, 512, 512};
    int gsoCeilingCubeVp[4] = {0, 0, 512, 512};

    int gsoFrontCubeOverlapVp[4] =   {0, 0, 768, 768};
    int gsoLeftCubeOverlapVp[4] =    {0, 0, 768, 768};
    int gsoFloorCubeOverlapVp[4] =   {0, 0, 768, 768};
    int gsoBackCubeOverlapVp[4] =    {0, 0, 768, 768};
    int gsoRightCubeOverlapVp[4] =   {0, 0, 768, 768};
    int gsoCeilingCubeOverlapVp[4] = {0, 0, 768, 768};

    // Actual physical dimensions of the screens (viewport).
    // In this case we're using inches.
    double gsoFrontSize[2] =   { gso_screenInches_X, gso_screenInches_Y };
    double gsoBackSize[2] =    { gso_screenInches_X, gso_screenInches_Y };
    double gsoLeftSize[2] =    { gso_screenInches_X, gso_screenInches_Y };
    double gsoRightSize[2] =   { gso_screenInches_X, gso_screenInches_Y };
    double gsoFloorSize[2] =   { gso_screenInches_X, gso_screenInches_Y };
    double gsoCeilingSize[2] = { gso_screenInches_X, gso_screenInches_Y };

    // Pseudo-physical dimensions of the cube displays. We think of 
    // them as being 8 foot (20 inch) squares.
    double gsoFrontCubeSize[2] =   { gso_screenInches_Y, gso_screenInches_Y };
    double gsoBackCubeSize[2] =    { gso_screenInches_Y, gso_screenInches_Y };
    double gsoLeftCubeSize[2] =    { gso_screenInches_Y, gso_screenInches_Y };
    double gsoRightCubeSize[2] =   { gso_screenInches_Y, gso_screenInches_Y };
    double gsoFloorCubeSize[2] =   { gso_screenInches_Y, gso_screenInches_Y };
    double gsoCeilingCubeSize[2] = { gso_screenInches_Y, gso_screenInches_Y };

    double gsoFrontCubeOverlapSize[2] =   { gso_screenInches_Y, gso_screenInches_Y };
    double gsoBackCubeOverlapSize[2] =    { gso_screenInches_Y, gso_screenInches_Y };
    double gsoLeftCubeOverlapSize[2] =    { gso_screenInches_Y, gso_screenInches_Y };
    double gsoRightCubeOverlapSize[2] =   { gso_screenInches_Y, gso_screenInches_Y };
    double gsoFloorCubeOverlapSize[2] =   { gso_screenInches_Y, gso_screenInches_Y };
    double gsoCeilingCubeOverlapSize[2] = { gso_screenInches_Y, gso_screenInches_Y };


    // Physical length of one display unit (GSO coord unit)
    // Same units should be used as for Size params.
    // In this case we're using inches.
    double sizeOfOneGsoUnit = gso_OneUnitInches ;

    // Current situation is that all screens have same pixel/gso unit.
    double PixelPerGsoUnit[2];  // This will be calculated below
    double *gsoFrontPixelPerGsoUnit = PixelPerGsoUnit;
    double *gsoBackPixelPerGsoUnit = PixelPerGsoUnit;
    double *gsoLeftPixelPerGsoUnit = PixelPerGsoUnit;
    double *gsoRightPixelPerGsoUnit = PixelPerGsoUnit;
    double *gsoFloorPixelPerGsoUnit = PixelPerGsoUnit;
    double *gsoCeilingPixelPerGsoUnit = PixelPerGsoUnit;
    double *pwrwallPixelPerGsoUnit = PixelPerGsoUnit;

    // These offsets describe how the simulator (and cube) windows are 
    // positioned relative to one another on DISPLAY 0.0.
    double gsoFrontSimWinOffset[2] =   {1.0, 0.0};
    double gsoBackSimWinOffset[2] =    {1.1, 0.1};
    double gsoLeftSimWinOffset[2]  =   {0.0, 0.0};
    double gsoRightSimWinOffset[2]  =  {0.1, 0.1};
    double gsoFloorSimWinOffset[2] =   {0.0, 1.0};
    double gsoCeilingSimWinOffset[2] = {0.1, 1.1};
    double pwrwallSimWinOffset[2] =   {0.0, 0.0};


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
            //      pwrwall perpendicular to Y

            //      viewports:
            //          right edge of left    ==  left edge of front 
            //          bottom edge of left   ==  top edge of floor
            //          bottom edge of front  ==  right edge of floor
            //          left edge of right    ==  right edge of front
            //          top edge of left      ==  top edge of ceiling

            osg::Quat gsoFrontOrientation = iris::EulerToQuat(0.f, 0.f, 0.f);
            osg::Quat gsoLeftOrientation = iris::EulerToQuat(90.f, 0.f, 0.f);
            osg::Quat gsoFloorOrientation= iris::EulerToQuat(90.f, -90.f, 0.f);

            osg::Quat gsoBackOrientation = iris::EulerToQuat(180.f, 0.f, 0.f);
            osg::Quat gsoRightOrientation = iris::EulerToQuat(-90.f, 0.f, 0.f);
            osg::Quat gsoCeilingOrientation=iris::EulerToQuat(90.f, 90.f, 0.f);

            osg::Quat pwrwallOrientation= iris::EulerToQuat(0.f, 0.f, 0.f);


            // Calculate the GSO coords of the corner point at which the 
            // three screen meet.
            //
            // ASSUMPTION: Corner where screens converge is one unit out
            //             from GSO origin in appropriate directions.
            double frontLeftFloorCornerGsoCoords[3] = {-1.0, 1.0, -1.0};


            // Calculate the number of pixels in a GSO unit.
            // ASSUMPTION: All screens have the same pixel per gso unit.
            //
            // Note that this calculation is being done with the parameters
            // for the screen for this compilation.
            PixelPerGsoUnit[0] = VP[2] / (SCR_SIZE[0]/sizeOfOneGsoUnit);
            PixelPerGsoUnit[1] = VP[3] / (SCR_SIZE[1]/sizeOfOneGsoUnit);


            //////
            // Now we are calculating the centers for all of the screens, 
            // but we only use one of the centers.  This is a bit
            // wasteful; we could change it to calculate only the
            // center that we need.


            // Calculate the centers of each of the screens in GSO coords.
            // ASSUMPTION: Corners converge as described above.
            //             Note that centers based on offsets from corner.
            osg::Vec3 gsoFrontCenter = 
              osg::Vec3 ( 
                frontLeftFloorCornerGsoCoords[0] + 
                                (gsoFrontSize[0]/2.0)/sizeOfOneGsoUnit, 
                frontLeftFloorCornerGsoCoords[1],
                frontLeftFloorCornerGsoCoords[2] + 
                                (gsoFrontSize[1]/2.0)/sizeOfOneGsoUnit
                );

            osg::Vec3 gsoLeftCenter = 
              osg::Vec3 ( 
                frontLeftFloorCornerGsoCoords[0],
                frontLeftFloorCornerGsoCoords[1] -
                                (gsoLeftSize[0]/2.0)/sizeOfOneGsoUnit, 
                frontLeftFloorCornerGsoCoords[2] + 
                                (gsoLeftSize[1]/2.0)/sizeOfOneGsoUnit
                );

            // ASSUMPTION: X dir of floor matches X dir of left,
            //   that top edge of floor vp coincides with bottom edge of 
            //   left screen, and that righta edge of floor coincides
            //   with bottom edge of front screen.
            osg::Vec3 gsoFloorCenter = 
              osg::Vec3 ( 
                frontLeftFloorCornerGsoCoords[0] +
                                (gsoFloorSize[1]/2.0)/sizeOfOneGsoUnit,
                frontLeftFloorCornerGsoCoords[1] -
                                (gsoFloorSize[0]/2.0)/sizeOfOneGsoUnit, 
                frontLeftFloorCornerGsoCoords[2]
                );



            osg::Vec3 gsoBackCenter = 
              osg::Vec3 ( 
                         gsoFrontCenter[0],
                         -1*gsoFrontCenter[1],
                         gsoFrontCenter[2]

                );



            osg::Vec3 gsoRightCenter = 
              osg::Vec3 ( 
                         -1*gsoLeftCenter[0],
                         gsoLeftCenter[1],
                         gsoLeftCenter[2]

                );


            osg::Vec3 gsoCeilingCenter = 
              osg::Vec3 ( 
                         gsoFloorCenter[0],
                         gsoFloorCenter[1],
                         -1*gsoFloorCenter[2]

                );

            // The following enforces consitency between the 
            //   Gaithersburg and Boulder installations. 
            //   Makes the pwrwall like the GSO front wall.
            osg::Vec3 pwrwallCenter = gsoFrontCenter;


            // For the cube DSOs, we don't really have to calculate the 
            // centers as we know them based on the geometry of the cube.
            //
            // ASSUMPTION:  The Cube DSOs represent "screens" that are the 
            // faces of an axis-aligned cube that is centered at the origin.
            osg::Vec3 gsoFrontCubeCenter   = osg::Vec3 ( 0.0,  1.0,  0.0);
            osg::Vec3 gsoBackCubeCenter    = osg::Vec3 ( 0.0, -1.0,  0.0);
            osg::Vec3 gsoLeftCubeCenter    = osg::Vec3 (-1.0,  0.0,  0.0);
            osg::Vec3 gsoRightCubeCenter   = osg::Vec3 ( 1.0,  0.0,  0.0);
            osg::Vec3 gsoFloorCubeCenter   = osg::Vec3 ( 0.0,  0.0, -1.0);
            osg::Vec3 gsoCeilingCubeCenter = osg::Vec3 ( 0.0,  0.0,  1.0);

            osg::Vec3 gsoFrontCubeOverlapCenter   = osg::Vec3 ( 0.0,  1.0,  0.0);
            osg::Vec3 gsoBackCubeOverlapCenter    = osg::Vec3 ( 0.0, -1.0,  0.0);
            osg::Vec3 gsoLeftCubeOverlapCenter    = osg::Vec3 (-1.0,  0.0,  0.0);
            osg::Vec3 gsoRightCubeOverlapCenter   = osg::Vec3 ( 1.0,  0.0,  0.0);
            osg::Vec3 gsoFloorCubeOverlapCenter   = osg::Vec3 ( 0.0,  0.0, -1.0);
            osg::Vec3 gsoCeilingCubeOverlapCenter = osg::Vec3 ( 0.0,  0.0,  1.0);






            //
            ///////////////////////////////////////////////////////////
            //
            // Here's some stuff that is always the same:

	    ///////////////////////////////////////////////////////////////////////////////////////////////////
            osg::ref_ptr<iris::Window> window = new iris::Window(SCR_NAME_STR) ;
            osg::GraphicsContext::Traits* const traits = window->getTraits() ;
            osg::ref_ptr<iris::ImmersivePane> pane = 
                            new iris::ImmersivePane(window.get(),SCR_NAME_STR) ;
            window->addPane(pane) ;
            window->setStereo (true) ;

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


            // For the simulator or cube displays, change the size and 
            // position of the windows, place them on DISPLAY 0.0, and so on.

#if defined(SIM)
            pixScale = 0.45; // Make simulator windows a bit smaller than
                             // half the size of the real windows.
#elif defined(CUBE)
            pixScale = 1.0;
#else
            pixScale = 1.0;
#endif

#if defined(SIM) || defined(CUBE)

	    // Offset the windows so that they are side-by-side.
	    // The hard-coded numeric offsets are intended to account for
	    // window borders. These are very ad hoc offsets. The window
	    // manager (in my testing) is changing the actual offsets 
	    // used for the window locations.

	    //fprintf(stderr,"pixScale=%g\n",pixScale);

	    pixOffset[0] = 
	      SIM_WIN_OFFSET[0] * (fullDisplayPixelDim[0] * pixScale + 50);
	    pixOffset[1] = 
	      SIM_WIN_OFFSET[1] * (fullDisplayPixelDim[1] * pixScale + 70);

	    window->setStereo (false) ;


	    //start USE_SCREENS_1_2_3_FOR_SIMULATOR hack
	    if (getenv("USE_SCREENS_1_2_3_FOR_SIMULATOR") == NULL) {

	      traits->windowDecoration = true;
	      traits->screenNum = 0;

#if defined(OFFSCREEN)
          dtkMsg.add(DTKMSG_DEBUG, "%s : setting pbuffer to true\n",
                     CLASS_NAME_STR);
          traits->pbuffer = true;
          // don't read DISPLAY for cube offscreen windows
#else
	      traits->readDISPLAY() ; //    use whatever DISPLAY is set to
#endif // OFFSCREEN


	    } else {

	      // This section is a hack to spread simulator windows 
	      // on to other graphics cards for cube map grabs. Once
	      // we implemet off screen rendering, this will not be needed.
	      // -SGS

	      traits->windowDecoration = false;

	      int sgs_scr_num;
	      sgs_scr_num=SCR_NUM;

	      if (sgs_scr_num == 4) {
		sgs_scr_num=1;
	      } else if (sgs_scr_num == 5) {
		sgs_scr_num=2;
	      } else if (sgs_scr_num == 6) {
		sgs_scr_num=3;
	      }

	      traits->screenNum = sgs_scr_num;

	      // end of hack

	    }


#endif






            // Set screen X and Y locations for window? Origin at upper left.
            // ASSUMPTION: GSO window covers entire screen and all screens are
            //             the same size.
            traits->x = pixOffset[0];
            traits->y = pixOffset[1];

            traits->width  = VP[2] * pixScale + 0.5;
            traits->height = VP[3] * pixScale + 0.5;

            osg::Vec2 vpExtent = osg::Vec2 ( VP[2]/PIX_PER_GSO[0], 
                                             VP[3]/PIX_PER_GSO[1] ) ;

#ifdef OVERLAP
            // If we want overapped field of view between adjacent screens, 
            // we expand the viewport extent. This should probably only
            // be done with the CUBE DSOs.
            vpExtent[0] *= 1.1;
            vpExtent[2] *= 1.1;
#endif

            // Scale viewport (only needed for sim windows)
            for (int i = 0; i < 4; i++) 
            {
                VP[i] = 0.5 + (VP[i] * pixScale);
            }

            pane->setViewport (VP[0], VP[1], VP[2], VP[3]);


            // Place the screens in the gso coordinate system.

            // ASSUMPTION: Pixel per gso unit is constant in all directions
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

