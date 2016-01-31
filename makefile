CC := g++
TARGET := server
INCLUDE := -I./
LIBS := -lpthread

CXXFLAGS := -g -D_REENTRANT
OBJECTS := epoll.o main.o pool.o task.o

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(LIBS)
%.o: %.cpp
	$(CC) -c $(CXXFLAGS) $(INCLUDE) $< -o $@

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJECTS)
