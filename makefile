CXX = g++
CXXFLAGS = -Wall

SRC_DIR = ./src
OBJ_DIR = ./obj
TEST_DIR = ./testing
MAIN = main
OBJS =  $(OBJ_DIR)/main.o \
		$(OBJ_DIR)/circuit_file_parser.o

$(MAIN): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(MAIN)

clean:
	rm $(OBJ_DIR)/*.o $(MAIN)

check:
	cppcheck $(SRC_DIR)/*.cpp --enable=all --suppress=missingIncludeSystem

tests:
	cmake $(TEST_DIR)/CMakeLists.txt
	(cd $(TEST_DIR) && make)
	$(TEST_DIR)/runTests