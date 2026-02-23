# Compiler to use
CXX = g++

# Compiler flags
# -Wall -Wextra: Show most warnings
# -std=c++17: Use the C++17 standard
# -O2: Optimization level 2
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

# The name of the resulting executable
TARGET = cpu_emu

# Source files
SRC = mainfile.cpp

# Default target: build the executable
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Rule to run the program after building
run: $(TARGET)
	./$(TARGET)

# Clean up build files and the executable
clean:
	rm -f $(TARGET)
	rm -f halt_finish.txt

# Phony targets (targets that aren't actual files)
.PHONY: all clean run