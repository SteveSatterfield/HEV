
/*

  Some asumptions (inspired by immersiveWindow.cpp)

     - Windows cover the display.

     - One unit in HEV, aka RAVE/CAVE, unit is 
       half the physical monitor width in landscape
       orientation ( withd > height).

     - For example a 4k (3849x2160) monitor which is 36" (wide) x 24" (hight),
       1 hev = 18" 
    
     - The monitor is perpendicular to hev Y
        +Y is straight ahead
	+Y is left to right
	+Z is up

*/

#include <stdlib.h>

#include <dtk.h>
#include <dtk/dtkDSO_loader.h>

#include <iris/Augment.h>
#include <iris/ImmersivePane.h>
#include <iris/Utils.h>

// Use namespace so this DSO's callbacks don't interfere with other DSO's
namespace wsWindow
{

    class wsWindow : public iris::Augment
    {
    public:
      wsWindow():iris::Augment("wsFrontWindow") 
	{

	    setDescription("%s- Ws",getName()) ;

	    int width, height, x, y;
	    float displayXinches, displayYinches,scaleRelativeToUHD;

	    char *ws_type;
	    bool stereo_flag;
	    bool decoration_flag;
	

	    fprintf(stderr,"hello\n");
	    
	    if (NULL == (ws_type=getenv("HEV_DISPLAY_TYPE"))) {
		  ws_type="fhd";
		}
	    fprintf(stderr,"there\n");

	    x = 0 ;
	    y = 0 ;

	    if (strcmp(ws_type,"ARC6") == 0) {        // ARC6 (NC A&T Arc6)
	      width = 11520 ;                  // 6*1920
	      height = 4320 ;                  // 4*1080
	      displayXinches=211.22;
	      displayYinches=90.5;
	      scaleRelativeToUHD=displayXinches/36.;
	      stereo_flag=true;
	      decoration_flag=false;

	    } else if (strcmp(ws_type, "arc6") == 0) {   // ARC6 scaled by 1/3 to fit 4K UHD TV
	      width  = 3840 ;             // 11520/3
	      height = 1440 ;             // 4320/3
	      displayXinches=70.04;       // 211.22/3.
	      displayYinches=30.167;      //  90.5/3
	      scaleRelativeToUHD=70.04/36.;
	      stereo_flag=true;
	      decoration_flag=false;

	    } else if (strcmp(ws_type, "UHD") == 0) {   // UHD (4K UHD TV)
	      width = 3840 ;
	      height = 2160 ;
	      displayXinches=36.;
	      displayYinches=24.;
	      scaleRelativeToUHD=displayXinches/36.;
	      stereo_flag=false;
	      decoration_flag=false;

	    } else if (strcmp(ws_type, "uhd") == 0) {   // uhd (4K UHD monitor)
	      width = 3840 ;
	      height = 2160 ;
	      displayXinches=24.;
	      displayYinches=13.5;
	      scaleRelativeToUHD=displayXinches/36.;
	      stereo_flag=false;
	      decoration_flag=false;

	    } else if (strcmp(ws_type, "FHD") == 0) {   // FHD (1080p TV)
	      width = 1920 ;
	      height = 1080 ;
	      displayXinches=36.;
	      displayYinches=24.;
	      scaleRelativeToUHD=displayXinches/36.;
	      stereo_flag=false;
	      decoration_flag=false;

	    } else if (strcmp(ws_type, "fhd") == 0) {   // fhd (1080p monitor)
	      width = 1920 ;
	      height = 1080 ;
	      displayXinches=22.;
	      displayYinches=12.75;
	      scaleRelativeToUHD=displayXinches/36.;
	      stereo_flag=false;
	      decoration_flag=false;

	    } else {
	      fprintf(stderr,"HEV_DISPLAY_TYPE undefined, I'll make up something\n");
	      width = 1920 ;
	      height = 1080 ;
	      displayXinches=22.;
	      displayYinches=12.75;
	      scaleRelativeToUHD=displayXinches/36.;
	      stereo_flag=false;
	      decoration_flag=true;
	      
	    }

	    fprintf(stderr,"and here\n");

	    int sn = 1 ;

	    float aspect=(float)height/(float)width;

	    osg::ref_ptr<iris::Window> window = new iris::Window("Front") ;


	    osg::GraphicsContext::Traits* const traits = window->getTraits() ;
	    traits->supportsResize = false ;
	    traits->screenNum = sn ;

	    // Set screen X and Y locations for window? Origin at upper left.  
            // ASSUMPTION: RAVE window covers entire screen and all screens are
	    //             the same size.
	    traits->x = x;
	    traits->y = y ;
	    traits->width = width ;
	    traits->height = height ;


	    // use whatever DISPLAY is set to
	    traits->readDISPLAY() ;

	    traits->windowDecoration = decoration_flag;
	    window->setStereo(stereo_flag) ;

	    traits->doubleBuffer = true;


	    //	    osg::ref_ptr<iris::ImmersivePane> pane = new iris::ImmersivePane(window.get(),"wsFrontWindow" );
	    osg::ref_ptr<iris::ImmersivePane> pane = new iris::ImmersivePane(window.get() );

	    window->addPane(pane) ;

	    pane->setExtent(osg::Vec2(2.f, 2.f*aspect)) ;
	    pane->setCenter(osg::Vec3(0.f, 1.f, 0.f)) ;
	    pane->setOrientation(iris::EulerToQuat(0.f, 0.f, 0.f)) ;
	    pane->setNear(.1f) ;
	    pane->setFar(1000.f) ;

	    pane->setViewport(0, 0, width, height);


	    // dtkAugment::dtkAugment() will not validate the object
	    validate() ;
	} ;

    } ;

} ;

/************ DTK C++ dtkAugment loader/unloader functions ***************
 *
 * All DTK dtkAugment DSO files are required to declare these two
 * functions.  These function are called by the loading program to get
 * your C++ objects loaded.
 *
 *************************************************************************/

static dtkAugment* dtkDSO_loader(dtkManager* manager, void* p)
{
  return new wsWindow::wsWindow ;
}

static int dtkDSO_unloader(dtkAugment* augment)
{
#if 0
    delete augment;
#endif
    return DTKDSO_UNLOAD_CONTINUE;
}

