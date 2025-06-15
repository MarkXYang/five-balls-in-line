CXX = g++
CXXFLAGS = $(shell pkg-config --cflags gtkmm-4.0)
LIBS = $(shell pkg-config --libs gtkmm-4.0)
TARGET = color_lines_gtk
SOURCES = src/main.cpp src/MainWindow.cpp src/Ball.cpp src/GameGrid.cpp src/Pathfinder.cpp src/Solver.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
