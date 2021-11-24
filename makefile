CXX = g++
CXXFLAGS = -Wall -g
LIBS = -L. -lcryptominisat5

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
		$(OBJ_DIR)/polynomial_tranfer_function.o \
		$(OBJ_DIR)/gnd_potential.o \
		$(OBJ_DIR)/vdd_potential.o

$(MAIN): $(OBJS)
	$(CXX) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(MAIN) all-tests

clean:
	rm $(OBJ_DIR)/*.o $(MAIN) runTests

check:
	cppcheck $(SRC_DIR)/* --language=c++ --enable=all --suppress=missingIncludeSystem

all-tests:
	cmake $(TEST_DIR)/CMakeLists.txt
	(cd $(TEST_DIR) && make)
	mv $(TEST_DIR)/runTests .
tests:
	./runTests 2>/dev/null