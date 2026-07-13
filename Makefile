CXX      := g++
CXXFLAGS := -std=c++17 -Wall -O2
LDFLAGS  := -lGL -lGLU -lglut
TARGET   := half-edge-viewer

all: $(TARGET)

$(TARGET): src/main.cpp src/Mesh.h src/HalfEdge.h
	$(CXX) $(CXXFLAGS) src/main.cpp -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET) models/modelo.obj 2

clean:
	rm -f $(TARGET)

.PHONY: all run clean
