TARGET = main
CC = gcc
CXX = g++

OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
OPTIONS = -Wall -g
INCDIRS =
LINKLIBS =  -lpthread
FLAGS = $(OPTIONS) $(INCDIRS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LINKLIBS) $(FLAGS) -o $(TARGET)
%.o: %.cpp
	$(CC) $(FLAGS) -o $@ -c $<

clean:
	$(RM) $(TARGET) $(OBJECTS)
