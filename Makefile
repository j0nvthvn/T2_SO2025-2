CC = gcc

LDFLAGS = -lpthread

TARGET = doom_simulator
OBJS = main.o parser.o

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

main.o: main.c config.h parser.h
	$(CC) -c main.c

parser.o: parser.c parser.h config.h
	$(CC) -c parser.c

clean:
	rm -f $(TARGET) $(OBJS)

test: $(TARGET)
	./$(TARGET) config.txt

.PHONY: all clean test