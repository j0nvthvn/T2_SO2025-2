CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lpthread

TARGET = doom_simulator
OBJS = main.o parser.o simulation.o sync.o hero.o monster.o combat.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

main.o: main.c config.h parser.h simulation.h sync.h hero.h monster.h
	$(CC) $(CFLAGS) -c main.c

parser.o: parser.c parser.h config.h
	$(CC) $(CFLAGS) -c parser.c

simulation.o: simulation.c simulation.h config.h
	$(CC) $(CFLAGS) -c simulation.c

sync.o: sync.c sync.h config.h
	$(CC) $(CFLAGS) -c sync.c

hero.o: hero.c hero.h config.h simulation.h sync.h combat.h
	$(CC) $(CFLAGS) -c hero.c

monster.o: monster.c monster.h config.h simulation.h sync.h combat.h
	$(CC) $(CFLAGS) -c monster.c

combat.o: combat.c combat.h config.h sync.h
	$(CC) $(CFLAGS) -c combat.c

clean:
	rm -f $(TARGET) $(OBJS)

test: $(TARGET)
	./$(TARGET) config.txt

# https://stackoverflow.com/questions/5134891/how-do-i-use-valgrind-to-find-memory-leaks
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) config.txt

# https://valgrind-org.translate.goog/docs/manual/hg-manual.html?_x_tr_sl=en&_x_tr_tl=es&_x_tr_hl=es&_x_tr_pto=tc
helgrind: $(TARGET)
	valgrind --tool=helgrind ./$(TARGET) config.txt

.PHONY: all clean test valgrind helgrind