CC = gcc

LDFLAGS = -lpthread

TARGET = doom_simulator
OBJS = main.o parser.o simulation.o sync.o hero.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

main.o: main.c config.h parser.h simulation.h sync.h hero.h
	$(CC) -c main.c

parser.o: parser.c parser.h config.h simulation.h sync.h hero.h
	$(CC) -c parser.c

simulation.o: simulation.c parser.h config.h simulation.h sync.h hero.h
	$(CC) -c simulation.c

sync.o: sync.c parser.h config.h simulation.h sync.h hero.h
	$(CC) -c sync.c

hero.o: hero.c parser.h config.h simulation.h sync.h hero.h

clean:
	rm -f $(TARGET) $(OBJS)

test: $(TARGET)
	./$(TARGET) config.txt

.PHONY: all clean test