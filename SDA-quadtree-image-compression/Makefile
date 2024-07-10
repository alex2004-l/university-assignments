.PHONY: build clean run

build: main.o queue.o qtree.o 
	gcc -o quadtree -lm -Wall main.o queue.o qtree.o

main.o: main.c queue.h qtree.h 
	gcc -c main.c

queue.o: queue.c queue.h qtree.h
	gcc -c queue.c

qtree.o: qtree.c qtree.h 
	gcc -c qtree.c

clean:
	rm *.o quadtree
run:
	./quadtree
zip:
	zip tema2.zip main.c queue.c queue.h qtree.c qtree.h Makefile README.md