CXX = g++
# TODO change -g to -O3 in the end
CXXFLAGS = -Wall -g
CGALFLAGS = -DBOOST_ALL_NO_LIB -DCGAL_EIGEN3_ENABLED -DCGAL_USE_GMPXX=1 -isystem /usr/include/eigen3 -frounding-math -pthread 
PCLFLAGS = -I/usr/include/pcl-1.10 -I/usr/include/eigen3
LIBS = -L. -lcryptominisat5 -isystem libtensorflow2 -ltensorflow -isystem cppflow /usr/lib/x86_64-linux-gnu/libgmpxx.so \
		/usr/lib/x86_64-linux-gnu/libmpfr.so /usr/lib/x86_64-linux-gnu/libgmp.so -pthread /usr/lib/x86_64-linux-gnu/libpcl_common.so \
		/usr/lib/x86_64-linux-gnu/libpcl_io.so /usr/lib/x86_64-linux-gnu/libpcl_apps.so /usr/lib/x86_64-linux-gnu/libpcl_outofcore.so \
		/usr/lib/x86_64-linux-gnu/libpcl_people.so /usr/lib/x86_64-linux-gnu/libboost_system.so /usr/lib/x86_64-linux-gnu/libboost_filesystem.so \
		/usr/lib/x86_64-linux-gnu/libboost_date_time.so /usr/lib/x86_64-linux-gnu/libboost_iostreams.so /usr/lib/x86_64-linux-gnu/libboost_regex.so \
		/usr/lib/x86_64-linux-gnu/libqhull.so /usr/lib/libOpenNI.so /usr/lib/libOpenNI2.so /usr/lib/x86_64-linux-gnu/libvtkChartsCore-7.1.so.7.1p.1 \
		/usr/lib/x86_64-linux-gnu/libvtkInfovisCore-7.1.so.7.1p.1 /usr/lib/x86_64-linux-gnu/libfreetype.so /usr/lib/x86_64-linux-gnu/libz.so \
		/usr/lib/x86_64-linux-gnu/libjpeg.so /usr/lib/x86_64-linux-gnu/libpng.so /usr/lib/x86_64-linux-gnu/libtiff.so \
		/usr/lib/x86_64-linux-gnu/libexpat.so /usr/lib/x86_64-linux-gnu/libvtkIOGeometry-7.1.so.7.1p.1 \
		/usr/lib/x86_64-linux-gnu/libvtkIOLegacy-7.1.so.7.1p.1 /usr/lib/x86_64-linux-gnu/libvtkIOPLY-7.1.so.7.1p.1 \
		/usr/lib/x86_64-linux-gnu/libvtkRenderingLOD-7.1.so.7.1p.1 /usr/lib/x86_64-linux-gnu/libvtkViewsContext2D-7.1.so.7.1p.1 \
		/usr/lib/x86_64-linux-gnu/libvtkViewsCore-7.1.so.7.1p.1 /usr/lib/x86_64-linux-gnu/libvtkRenderingContextOpenGL2-7.1.so.7.1p.1 \
		/usr/lib/x86_64-linux-gnu/libvtkRenderingOpenGL2-7.1.so.7.1p.1 \
		/usr/lib/x86_64-linux-gnu/libflann_cpp.so /usr/lib/x86_64-linux-gnu/libpcl_surface.so /usr/lib/x86_64-linux-gnu/libpcl_keypoints.so \
		/usr/lib/x86_64-linux-gnu/libpcl_tracking.so /usr/lib/x86_64-linux-gnu/libpcl_recognition.so /usr/lib/x86_64-linux-gnu/libpcl_registration.so \
		/usr/lib/x86_64-linux-gnu/libpcl_stereo.so /usr/lib/x86_64-linux-gnu/libpcl_segmentation.so /usr/lib/x86_64-linux-gnu/libpcl_features.so \
		/usr/lib/x86_64-linux-gnu/libpcl_filters.so /usr/lib/x86_64-linux-gnu/libpcl_sample_consensus.so \
		/usr/lib/x86_64-linux-gnu/libpcl_ml.so /usr/lib/x86_64-linux-gnu/libpcl_visualization.so \
		/usr/lib/x86_64-linux-gnu/libpcl_search.so /usr/lib/x86_64-linux-gnu/libpcl_kdtree.so
SRC_DIR = ./src
OBJ_DIR = ./obj
TEST_DIR = ./testing
MAIN = main
OBJS =  $(OBJ_DIR)/main.o \
		$(OBJ_DIR)/circuit_file_parser.o \
		$(OBJ_DIR)/nor_gate.o \
		$(OBJ_DIR)/transition_schedule.o \
		$(OBJ_DIR)/circuit_simulator.o \
		$(OBJ_DIR)/circuit_input.o \
		$(OBJ_DIR)/polynomial_sis_transfer_function.o \
		$(OBJ_DIR)/polynomial_mis_transfer_function.o \
		$(OBJ_DIR)/gnd_potential.o \
		$(OBJ_DIR)/vdd_potential.o \
		$(OBJ_DIR)/ann_sis_transfer_function.o \
		$(OBJ_DIR)/cgal_test.o \
		$(OBJ_DIR)/pcl_test.o

$(MAIN): $(OBJS)
	$(CXX) -o $@ $^ $(LIBS)

$(OBJ_DIR)/cgal_test.o : $(SRC_DIR)/cgal_test.cpp
	$(CXX) $(CXXFLAGS) $(CGALFLAGS) -c -o $@ $<

$(OBJ_DIR)/pcl_test.o : $(SRC_DIR)/pcl_test.cpp
	$(CXX) $(CXXFLAGS) $(PCLFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(MAIN) all-tests

clean:
	rm $(OBJ_DIR)/*.o $(MAIN) runTests

check:
	cppcheck $(SRC_DIR)/*cpp --language=c++ --enable=all --suppress=missingIncludeSystem

all-tests:
	cmake $(TEST_DIR)/CMakeLists.txt
	(cd $(TEST_DIR) && make)
	mv $(TEST_DIR)/runTests .
tests:
	./runTests 2>/dev/null