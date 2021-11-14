TARGET = main
CC = gcc
CXX = g++

OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
OPTIONS = -Wall -std=c++11 -g
INCDIRS =
LINKLIBS =  -lpthread
FLAGS = $(OPTIONS) $(INCDIRS)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LINKLIBS) $(FLAGS) -o $(TARGET)
%.o: %.cpp
	$(CXX) $(FLAGS) -o $@ -c $<

clean:
	$(RM) $(TARGET) $(OBJECTS)
