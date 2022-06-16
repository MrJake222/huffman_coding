.DEFAULT_GOAL := all
SUFFIXES += .d

# ------
MAIN := main
TEST := test

LIBS := $(wildcard libs/*.cpp)
TESTS := $(wildcard tests/*.cpp)

MAIN_DEPS := $(LIBS)
TEST_DEPS := $(LIBS) $(TESTS)

MAIN_LD := 
TEST_LD := -lgtest -lgtest_main

NODEPS := clean

# ------
CXX := g++
CXXFLAGS := -O2 -Wall -g

# ------
EXECS := $(MAIN) $(TEST)
SOURCES := $(MAIN).cpp $(TEST).cpp $(LIBS) $(TESTS)
OBJECTS := $(SOURCES:.cpp=.o)
DEPFILES := $(SOURCES:.cpp=.d)

# Don't create dependencies when we're cleaning, for instance
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
    # If doesnt exists, ignore
    -include $(DEPFILES)
endif

# ------
all: $(MAIN) $(TEST)

$(MAIN): $(MAIN).o $(MAIN_DEPS:.cpp=.o)
	$(CXX) $^ -o $@ $(MAIN_LD)
		
$(TEST): $(TEST).o $(TEST_DEPS:.cpp=.o)
	$(CXX) $^ -o $@ $(TEST_LD)

runtests: $(MAIN) $(TEST)
	@./test
	@echo -e "\n"
	@./test.sh

# ------
%.d: %.cpp
	@ $(CXX) $(CXXFLAGS) -MM -MT $(<:.cpp=.d) $< -MF $@

%.o: %.cpp %.d
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ------
clean:
	rm -f $(EXECS)
	rm -f $(OBJECTS)
	rm -f $(DEPFILES)
