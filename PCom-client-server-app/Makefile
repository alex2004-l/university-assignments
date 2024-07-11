CC = g++
CFLAGS = -std=c++17 -Wall -Wextra

all: build

build: server subscriber

server: server.o common.o
	$(CC) $^ -o $@

server.o: server.cpp
	$(CC) $^ -c

subscriber: common.o subscriber.o
	$(CC) $^ -o $@

subscriber.o : subscriber.cpp
	$(CC) $^ -c

common.o: common.cpp
	$(CC) $^ -c

clean:
	rm *.o server subscriber