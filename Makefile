CC = gcc

LDFLAGS = -lpthread

TARGET = doom_simulator
OBJS = main.o parser.o

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

main.o: main.c config.h parser.h
	$(CC) -c main.c