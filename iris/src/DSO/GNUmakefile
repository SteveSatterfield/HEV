# Note: This GNUMakefile might not link properly if the make
#       in ../lib has not already been done. This can be
#       ensured by doing the make in ../..

#ifdef COMMENT
DSOS = \
	arcFrontWindow.so 				\
	fourKFrontWindow.so 				\
	hdFrontWindow.so 				\
#endif

DSOS = \
	\
	wandJoystickXYtranNav.so					\
	wandJoystickXYZtranOnlyNav.so				\
	wandJoystickZNav.so 					\
	wandJoystickYNav.so 					\
	\
	hdFrontWindow.so 						\
	hdBackWindow.so 						\
	hdLeftWindow.so 						\
	hdRightWindow.so 						\
	hdFloorWindow.so 						\
	hdCeilingWindow.so 					\
	\
	fourkFrontWindow.so 						\
	fourkBackWindow.so 						\
	fourkLeftWindow.so 						\
	fourkRightWindow.so 						\
	fourkFloorWindow.so 						\
	fourkCeilingWindow.so 					\
	\
	raveFrontWindow.so 						\
	raveBackWindow.so 						\
	raveLeftWindow.so 						\
	raveRightWindow.so 						\
	raveFloorWindow.so 						\
	raveCeilingWindow.so 					\
	\
	powerwallWindow.so 						\
	\
	raveFrontSimWindow.so 					\
	raveBackSimWindow.so 					\
	raveLeftSimWindow.so 					\
	raveRightSimWindow.so 					\
	raveFloorSimWindow.so 					\
	raveCeilingSimWindow.so 				\
	\
	raveFrontCubeOffscreenWindow.so 					\
	raveBackCubeOffscreenWindow.so 					\
	raveLeftCubeOffscreenWindow.so 					\
	raveRightCubeOffscreenWindow.so 					\
	raveFloorCubeOffscreenWindow.so 					\
	raveCeilingCubeOffscreenWindow.so 				\
	\
	raveFrontCubeWindow.so 					\
	raveBackCubeWindow.so 					\
	raveLeftCubeWindow.so 					\
	raveRightCubeWindow.so 					\
	raveFloorCubeWindow.so 					\
	raveCeilingCubeWindow.so 				\
	\
	raveFrontCubeOverlapWindow.so 					\
	raveBackCubeOverlapWindow.so 					\
	raveLeftCubeOverlapWindow.so 					\
	raveRightCubeOverlapWindow.so 					\
	raveFloorCubeOverlapWindow.so 					\
	raveCeilingCubeOverlapWindow.so 				\
	\
	powerwallSimWindow.so 					\
	\
	buttonMapper.so 						\
	offscreenWindow.so						\
	desktopWindow.so 						\
	desktopWindowKeyboardControl.so 		\
	example.so 								\
	exampleNav.so 							\
	frameGrabber.so 						\
	keyboardControl.so 						\
	monoDisplayMod.so 						\
	nullNav.so 								\
	osgdb_iris.so 							\
	osgNav.so 								\
	setHeadWandMatrix.so 					\
	sharedMemNav.so 						\
	shmChildParentTransformation.so 		\
	shmNodeToNodeTransformation.so 			\
	tim.so 									\
	toggleObjects.so 						\
	trackballNav.so 						\
	wandJoystickNav.so 						\
	writeState.so 							




include $(IRIS_HOME)/gnumakefile.rules

CPPFLAGS += -DVERBOSE
CPPFLAGS += -w

# We only need explicit build rules when DSO .o name is
# different than the .cpp name.

# DSOs for front, left, and floor all use immersiveWindow.cpp
# In the future, we can bring the powerwall into this scheme.


hdFrontWindow.o: hdImmersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=hdFront \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=hdFront \
			$(INC) -o $@ $<

hdBackWindow.o: hdImmersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=hdBack \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=hdBack \
			$(INC) -o $@ $<

hdLeftWindow.o: hdImmersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=hdLeft \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=hdLeft \
			$(INC) -o $@ $<

hdRightWindow.o: hdImmersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=hdRight \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=hdRight \
			$(INC) -o $@ $<

hdFloorWindow.o: hdImmersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=hdFloor \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=hdFloor \
			$(INC) -o $@ $<

hdCeilingWindow.o: hdImmersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=hdCeiling \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=hdCeiling \
			$(INC) -o $@ $<


fourkFrontWindow.o: fourkImmersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=fourkFront \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=fourkFront \
			$(INC) -o $@ $<

fourkBackWindow.o: fourkImmersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=fourkBack \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=fourkBack \
			$(INC) -o $@ $<

fourkLeftWindow.o: fourkImmersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=fourkLeft \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=fourkLeft \
			$(INC) -o $@ $<

fourkRightWindow.o: fourkImmersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=fourkRight \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=fourkRight \
			$(INC) -o $@ $<

fourkFloorWindow.o: fourkImmersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=fourkFloor \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=fourkFloor \
			$(INC) -o $@ $<

fourkCeilingWindow.o: fourkImmersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=fourkCeiling \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=fourkCeiling \
			$(INC) -o $@ $<


raveFrontWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=raveFront \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=raveFront \
			$(INC) -o $@ $<

raveBackWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=raveBack \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=raveBack \
			$(INC) -o $@ $<

raveLeftWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=raveLeft \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=raveLeft \
			$(INC) -o $@ $<

raveRightWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=raveRight \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=raveRight \
			$(INC) -o $@ $<

raveFloorWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=raveFloor \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=raveFloor \
			$(INC) -o $@ $<

raveCeilingWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=raveCeiling \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSCR_NAME=raveCeiling \
			$(INC) -o $@ $<

# DSOs for simulated front, left, and floor are compiled as
# above but with -DSIM
raveFrontSimWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSIM -DSCR_NAME=raveFront \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSIM -DSCR_NAME=raveFront \
			$(INC) -o $@ $<

raveBackSimWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSIM -DSCR_NAME=raveBack \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSIM -DSCR_NAME=raveBack \
			$(INC) -o $@ $<

raveLeftSimWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSIM -DSCR_NAME=raveLeft \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSIM -DSCR_NAME=raveLeft \
			$(INC) -o $@ $<

raveRightSimWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSIM -DSCR_NAME=raveRight \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSIM -DSCR_NAME=raveRight \
			$(INC) -o $@ $<

raveFloorSimWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSIM -DSCR_NAME=raveFloor \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSIM -DSCR_NAME=raveFloor \
			$(INC) -o $@ $<

raveCeilingSimWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSIM -DSCR_NAME=raveCeiling \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DSIM -DSCR_NAME=raveCeiling \
			$(INC) -o $@ $<



# DSOs for simulated front, left, and floor are compiled as
# above but with -DSIM
raveFrontCubeWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DSCR_NAME=raveFront \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DSCR_NAME=raveFront \
			$(INC) -o $@ $<

raveBackCubeWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DSCR_NAME=raveBack \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DSCR_NAME=raveBack \
			$(INC) -o $@ $<

raveLeftCubeWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DSCR_NAME=raveLeft \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DSCR_NAME=raveLeft \
			$(INC) -o $@ $<

raveRightCubeWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DSCR_NAME=raveRight \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DSCR_NAME=raveRight \
			$(INC) -o $@ $<

raveFloorCubeWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DSCR_NAME=raveFloor \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DSCR_NAME=raveFloor \
			$(INC) -o $@ $<

raveCeilingCubeWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DSCR_NAME=raveCeiling \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DSCR_NAME=raveCeiling \
			$(INC) -o $@ $<


# Another set of Cube with Offscreen
raveFrontCubeOffscreenWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOFFSCREEN -DSCR_NAME=raveFront \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOFFSCREEN -DSCR_NAME=raveFront \
			$(INC) -o $@ $<

raveBackCubeOffscreenWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOFFSCREEN -DSCR_NAME=raveBack \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOFFSCREEN -DSCR_NAME=raveBack \
			$(INC) -o $@ $<

raveLeftCubeOffscreenWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOFFSCREEN -DSCR_NAME=raveLeft \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOFFSCREEN -DSCR_NAME=raveLeft \
			$(INC) -o $@ $<

raveRightCubeOffscreenWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOFFSCREEN -DSCR_NAME=raveRight \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOFFSCREEN -DSCR_NAME=raveRight \
			$(INC) -o $@ $<

raveFloorCubeOffscreenWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOFFSCREEN -DSCR_NAME=raveFloor \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOFFSCREEN -DSCR_NAME=raveFloor \
			$(INC) -o $@ $<

raveCeilingCubeOffscreenWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOFFSCREEN -DSCR_NAME=raveCeiling \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOFFSCREEN -DSCR_NAME=raveCeiling \
			$(INC) -o $@ $<


# Another set of Cube with Overlap
raveFrontCubeOverlapWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOVERLAP -DSCR_NAME=raveFront \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOVERLAP -DSCR_NAME=raveFront \
			$(INC) -o $@ $<

raveBackCubeOverlapWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOVERLAP -DSCR_NAME=raveBack \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOVERLAP -DSCR_NAME=raveBack \
			$(INC) -o $@ $<

raveLeftCubeOverlapWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOVERLAP -DSCR_NAME=raveLeft \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOVERLAP -DSCR_NAME=raveLeft \
			$(INC) -o $@ $<

raveRightCubeOverlapWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOVERLAP -DSCR_NAME=raveRight \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOVERLAP -DSCR_NAME=raveRight \
			$(INC) -o $@ $<

raveFloorCubeOverlapWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOVERLAP -DSCR_NAME=raveFloor \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOVERLAP -DSCR_NAME=raveFloor \
			$(INC) -o $@ $<

raveCeilingCubeOverlapWindow.o: immersiveWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOVERLAP -DSCR_NAME=raveCeiling \
			$(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
			-DCUBE -DOVERLAP -DSCR_NAME=raveCeiling \
			$(INC) -o $@ $<

# For simulated powerwall, compile with -DSIM
powerwallSimWindow.o: powerwallWindow.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -DSIM $(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -DSIM $(INC) -o $@ $<

# Future rules for when powerwall uses immersiveWindow.cpp
#
# powerwallWindow.o: immersiveWindow.cpp
# 	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
# 			-DSCR_NAME=powerwall \
# 			$(INC) -o $@ $<
# 

# powerwallSimWindow.o: immersiveWindow.cpp
# 	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) \
# 			-DSIM -DSCR_NAME=powerwall \
# 			$(INC) -o $@ $<
# 

wandJoystickZNav.o: wandJoystickNav.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -DZROTATE $(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -DZROTATE $(INC) -o $@ $<

wandJoystickYNav.o: wandJoystickNav.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -DYROTATE $(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -DYROTATE $(INC) -o $@ $<

wandJoystickXYtranNav.o: wandJoystickNav.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -DXYTRANSLATE $(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -DXYTRANSLATE $(INC) -o $@ $<

wandJoystickXYZtranOnlyNav.o: wandJoystickNav.cpp
	@dtk-msgNotice -p Makefile compiling $<
	@dtk-msgNotice -p Makefile $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -DXYZTRANSLATEONLY $(INC) -o $@ $<
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -DXYZTRANSLATEONLY $(INC) -o $@ $<




