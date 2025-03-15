# Compiler
CXX := clang++
CXXFLAGS := -std=c++17 -Wall -g -fdiagnostics-color=always -Wno-deprecated

# Paths
SRC_DIR := $(PWD)
DEP_INCLUDE := $(SRC_DIR)/dependencies/include
DEP_LIB := $(SRC_DIR)/dependencies/library

# Source files (excluding main files)
SRC := glad.c stb_image.cpp
OBJ := $(SRC:.cpp=.o)

# Main entry files
MAIN1 := main.cpp
MAIN2 := main2.cc
MAIN3 := main3.cc
MAIN4 := main4.cc



# Output binaries
TARGET1 := app
TARGET2 := app2
TARGET3 := app3
TARGET4 := app4


# Frameworks (MacOS-specific)
FRAMEWORKS := -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework CoreFoundation

# Libraries
LIBS := -L$(DEP_LIB) -lglfw.3.4

# Build main.cpp as app
$(TARGET1): $(OBJ) $(MAIN1:.cpp=.o)
	$(CXX) $(CXXFLAGS) -I$(DEP_INCLUDE) $(OBJ) $(MAIN1:.cpp=.o) $(LIBS) $(FRAMEWORKS) -o $(TARGET1)

# Build main2.cc as app2
$(TARGET2): $(OBJ) $(MAIN2:.cc=.o)
	$(CXX) $(CXXFLAGS) -I$(DEP_INCLUDE) $(OBJ) $(MAIN2:.cc=.o) $(LIBS) $(FRAMEWORKS) -o $(TARGET2)

# Build main3.cc as app3
$(TARGET3): $(OBJ) $(MAIN3:.cc=.o)
	$(CXX) $(CXXFLAGS) -I$(DEP_INCLUDE) $(OBJ) $(MAIN3:.cc=.o) $(LIBS) $(FRAMEWORKS) -o $(TARGET3)

# Build main4.cc as app4
$(TARGET4): $(OBJ) $(MAIN4:.cc=.o)
	$(CXX) $(CXXFLAGS) -I$(DEP_INCLUDE) $(OBJ) $(MAIN4:.cc=.o) $(LIBS) $(FRAMEWORKS) -o $(TARGET4)

# Compile C++ files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(DEP_INCLUDE) -c $< -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -I$(DEP_INCLUDE) -c $< -o $@

# Clean build files
clean:
	rm -f $(MAIN1:.cpp=.o) $(MAIN2:.cc=.o) $(MAIN3:.cc=.o) $(MAIN4:.cc=.o) $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) 

