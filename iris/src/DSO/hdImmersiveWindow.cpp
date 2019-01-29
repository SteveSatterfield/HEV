#include <dtk.h>
#include <dtk/dtkDSO_loader.h>

#include <iris/Augment.h>
#include <iris/ImmersivePane.h>
#include <iris/Utils.h>

// do this so this DSO's callbacks don't interfere with other DSO's
namespace hdWindow
{
    
    class hdWindow : public iris::Augment
    {
    public:
      hdWindow():iris::Augment("hdFrontWindow") 
	{
	
	    setDescription("%s- Hd",getName()) ;

	    int width = 1920 ;
	    int height = 1080 ;
	    int x = 0 ;
	    int y = 0 ;
	    int sn = 1 ;
	    //int sn = 0 ;
	    float aspect = (float)width/(float)height ;
	    //float zoffset = 2.f/dpf_inches ;
	    float zoffset = 0.f ;
	    // sgs 10/1/12 osg::ref_ptr<iris::Window> window = new iris::Window("Hd") ;
	    osg::ref_ptr<iris::Window> window = new iris::Window("Front") ;

	    osg::GraphicsContext::Traits* const traits = window->getTraits() ;
	    traits->supportsResize = false ;
	    traits->screenNum = sn ;
	    traits->x = x ;
	    // this is measured from the top
	    traits->y = y ;
	    traits->width = width ;
	    traits->height = height ;
	    traits->windowDecoration = true;
	    //window->setStereo(true) ;
	    window->setStereo(false) ;
	    traits->doubleBuffer = true;
	    
	    osg::ref_ptr<iris::ImmersivePane> pane = new iris::ImmersivePane(window.get()) ;
	    window->addPane(pane) ;
	    
	    //pane->setExtent(osg::Vec2(2.f, 2.f*aspect)) ;
	    pane->setExtent(osg::Vec2(1.f, 1.f)) ;
	    //pane->setExtent(osg::Vec2(2.f, 2.f)) ;
	    pane->setCenter(osg::Vec3(0.f, 1.f, zoffset)) ;
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
    return new hdWindow::hdWindow ;
}

static int dtkDSO_unloader(dtkAugment* augment)
{
#if 0
    delete augment;
#endif
    return DTKDSO_UNLOAD_CONTINUE;
}

