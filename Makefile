CC = gcc

LDFLAGS = -lpthread

TARGET = doom_simulator
OBJS = main.o parser.o simulation.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

main.o: main.c config.h parser.h simulation.h
	$(CC) -c main.c

parser.o: parser.c parser.h config.h simulation.h
	$(CC) -c parser.c

simulation.o: simulation.c parser.h config.h simulation.h
	$(CC) -c simulation.c

clean:
	rm -f $(TARGET) $(OBJS)

test: $(TARGET)
	./$(TARGET) config.txt

.PHONY: all clean test