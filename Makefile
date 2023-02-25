CXX = g++                              # C++ compiler to use
CXXFLAGS = -Wall -Wextra -Werror       # Compiler flags
LDFLAGS =                              # Linker flags
LDLIBS =                               # Libraries to link

SRC = fgtl.cpp helper.cpp              # Source files
OBJ = $(SRC:.cpp=.o)                   # Object files

all: fglt                             # Default target

fglt: $(OBJ)                           # Link object files into an executable
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.cpp                            # Compile each source file into an object file
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:                                # Remove all object files and the executable
	rm -f $(OBJ) fglt