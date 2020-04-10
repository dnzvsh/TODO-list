all: bin/todo

bin/todo: build/main.o
	gcc -Wall -Werror build/main.o -o bin/todo `pkg-config --cflags --libs gtk+-3.0`

build/main.o: src/main.c
	gcc -Wall -Werror -c src/main.c -o build/main.o `pkg-config --cflags --libs gtk+-3.0`