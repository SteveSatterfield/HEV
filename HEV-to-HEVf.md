HEV to HEVf Diffs
===

Here are the significant changes and additions made going from HEV (at time f fork) to HEVf:

- external/dtk/src/programs/sharedMemUtils/dtk-writeShm.cpp
  (I should include here diff of the code snippet)
  
- external/dtk/src/programs/sharedMemUtils/dtk-writeLine.cpp
  (I should include here diff of the code snippet)

- external/dtk/src/DSOs/serviceDSO/is900cf/is900cf.cpp:2380:20:
  error: ISO C++ forbids comparison between pointer and integer [-fpermissive]

  Since we don't use the is900c tracker, I just commented this out
  of the CMakeLISTS.txt. In fact, let's go ahead and comment out most the
  stuff here.

- external/osg/OpenSceneGraph-3.2.0/src/osgPlugins/x/directx.cpp

  We can live without the directx pluging for now, so comment out and
  move on. 12/7/2018 Turns out, since the src tree is extracted fresh
  on each full make  install, disabling x requires commenting it out
  of the CMakeLists.txt just after the untar top level GNUmakefile.

-  errors building vtk. 

   Comment out in GNUmakefile for now and move on. Deal with this later.

-  errors building bulletphysics.

   Comment out in GNUmakefile for now and move on. Deal with this later.

   ignore things dependent on bulletPhysics: hev-animateWindow
   

-  iris/src/include/iris/ImmersivePane.h

   $ diff iris/src/include/iris/ImmersivePane.h ~/hev_ncat/iris/src/include/iris/ImmersivePane.h
   29a30,37
   >         ImmersivePane(bool addToList = true) : Pane(NULL,NULL)
   > 	{ 
   > 	    if (addToList) 
   > 	    {
   > 		_paneList.push_back(this) ;
   > 	    }
   > 	} ;
   > 


-  iris/src/include/iris/PerspectivePane.h
-  iris/src/include/iris/OrtthoPane.h

   Similar to previous.



-   external/.fixperm.sh really should not be in the public distribution.
    Deal with this later,


-   idea/volvis

    Has a problem, comment out of GNUmakefile and deal with it later,

-   iris/src/program/*.cpp

    convert statements like "return 1 ;" to "exit(1);"

-    idea/src/hev-convertDataArray/

     I think this needs volvis, deal with later

-   idea/src/hev-playAudioFilesGui/
    idea/src/hev-playAudioFiles/

     Fedoa 28 miss alsa audio, deal with this later

-   idea/src/hev-relativeMove.cpp
-   idea/src/hev-segmentSelection/hssOps.cpp

    < 	{HUGE, HUGE, HUGE, HUGE, HUGE, HUGE, HUGE, HUGE, HUGE};
    ---
     >       {HUGE_VAL, HUGE_VAL, HUGE_VAL, HUGE_VAL, HUGE_VAL, HUGE_VAL, HUGE_VAL, HUGE_VAL, HUGE_VAL};


-   idea/src/hev-stitch/hev-stitch.cpp

    convert '\0' to NULL

-   idea/src/vtkUtilities
-   idea/src/hev-hydview

    deal with these later

-   idea/src/hev-animator

    change statements like "return 1" to "returs(1);"

-    iris-exitOnThreeButtons

     usleep to sleep
     