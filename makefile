CXX = g++
# TODO change -g to -O3 in the end
CXXFLAGS = -Wall -g -DBOOST_ALL_NO_LIB -DCGAL_EIGEN3_ENABLED -DCGAL_USE_GMPXX=1 -isystem /usr/include/eigen3  -frounding-math -pthread
LIBS = -L. -lcryptominisat5 -isystem libtensorflow2 -ltensorflow -isystem cppflow /usr/lib/x86_64-linux-gnu/libgmpxx.so /usr/lib/x86_64-linux-gnu/libmpfr.so /usr/lib/x86_64-linux-gnu/libgmp.so -pthread 
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
		$(OBJ_DIR)/cgal_test.o

$(MAIN): $(OBJS)
	$(CXX) -o $@ $^ $(LIBS)

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