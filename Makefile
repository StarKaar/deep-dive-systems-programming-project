# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Target name
TARGET = Kaatrix.exe

# Build rule
all:
	$(CXX) $(CXXFLAGS) src/Kaatrix.cpp -o $(TARGET) -lstdc++

# Cleanup rule
clean:
	rm -f $(TARGET)
