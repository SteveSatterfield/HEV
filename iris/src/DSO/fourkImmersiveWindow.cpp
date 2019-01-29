
/*

  Some asumptions (inspired by immersiveWindow.cpp)

     - Windows cover the display.

     - One unit in HEV, aka RAVE/CAVE, unit is 
       half the physical monitor width in landscape
       orientation ( withd > height).

     - For example a 4k (3849x2160) monitor which is 36" (wide) x 24" (hight),
       1 hev = 18" 
    
     - The monitor is prependicular to hev Y
        +Y is straight ahead
	+Y is left to right
	+Z is up

*/


#include <dtk.h>
#include <dtk/dtkDSO_loader.h>

#include <iris/Augment.h>
#include <iris/ImmersivePane.h>
#include <iris/Utils.h>

// do this so this DSO's callbacks don't interfere with other DSO's
namespace fourkWindow
{
    
    class fourkWindow : public iris::Augment
    {
    public:
      fourkWindow():iris::Augment("fourkFrontWindow") 
	{
	
	    setDescription("%s- Fourk",getName()) ;

	    int width = 3840 ;
	    int height = 2160 ;
	    int x = 0 ;
	    int y = 0 ;
	    float displayXinches=36.;
	    float displayYinches=24.;
	    float aspect=(float)height/(float)width;

	    int sn = 1 ;
	    //int sn = 0 ;

	    // sgs 10/1/12 osg::ref_ptr<iris::Window> window = new iris::Window("Fourk") ;
	    osg::ref_ptr<iris::Window> window = new iris::Window("Front") ;

	    osg::GraphicsContext::Traits* const traits = window->getTraits() ;

	    fprintf(stderr, "traits entry values:\n");
	    fprintf(stderr, "\n\n\n\tx= %d, y= %d, width= %d, height= %d\n\n\n\n",traits->x, traits->y, traits->width, traits->height);


	    traits->supportsResize = false ;
	    traits->screenNum = sn ;
	    traits->x = x ;

	    // this is measured from the top
	    fprintf(stderr,"traits->x %d %d\n");
	    traits->x = x;
	    traits->y = y ;
	    traits->width = width ;
	    traits->height = height ;
	    traits->windowDecoration = false;

	    window->setStereo(true) ;
	    //window->setStereo(false) ;
	    traits->doubleBuffer = true;

	    fprintf(stderr, "\n\n\n\tx= %d, y= %d, width= %d, height= %d\n\n\n\n",traits->x, traits->y, traits->width, traits->height);
osg::ref_ptr<iris::ImmersivePane> pane = new iris::ImmersivePane(window.get()) ;
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
    return new fourkWindow::fourkWindow ;
}

static int dtkDSO_unloader(dtkAugment* augment)
{
#if 0
    delete augment;
#endif
    return DTKDSO_UNLOAD_CONTINUE;
}

