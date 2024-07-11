CC=g++
CFLAGS=-I.

client: client.cpp request.cpp helpers.cpp buffer.cpp utils.cpp commands.cpp
	$(CC) -o client client.cpp request.cpp helpers.cpp buffer.cpp utils.cpp commands.cpp -Wall

run: client
	./client

clean:
	rm -f *.o client
