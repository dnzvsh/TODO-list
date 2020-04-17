all: bin/todo format

bin/todo: build/main.o build/task.o
	gcc -Wall -Werror build/task.o build/main.o -o bin/todo `pkg-config --cflags --libs gtk+-3.0` -lsqlite3

build/main.o: src/main.c src/task.h
	gcc -I src -Wall -Werror -c src/main.c -o build/main.o `pkg-config --cflags --libs gtk+-3.0` -lsqlite3

build/task.o: src/task.c src/task.h
	gcc -I src -Wall -Werror -c src/task.c -o build/task.o `pkg-config --cflags --libs gtk+-3.0` -lsqlite3

format: src/main.c
	clang-format -i src/main.c src/task.h src/task.c

clear:
	rm -rf build/*.o bin/todo