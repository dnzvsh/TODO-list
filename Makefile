ERROR_OPTIONS=-Wall -Werror -Wextra

all: bin/todo format

bin/todo: build/main.o build/task.o
	gcc $(ERROR_OPTIONS) build/task.o build/main.o -o bin/todo `pkg-config --cflags --libs gtk+-3.0` -lsqlite3

build/main.o: src/main.c src/task.h
	gcc -I src $(ERROR_OPTIONS) -c src/main.c -o build/main.o `pkg-config --cflags --libs gtk+-3.0` -lsqlite3

build/task.o: src/task.c src/task.h
	gcc -I src $(ERROR_OPTIONS) -c src/task.c -o build/task.o `pkg-config --cflags --libs gtk+-3.0` -lsqlite3

format: src/main.c
	clang-format -i src/main.c src/task.h src/task.c

bin/todo-test: build/test/todo_test.o
	gcc $(ERROR_OPTIONS) build/test/main.o build/test/todo_test.o build/src/task.o -o bin/todo-test

build/test/main.o: thirdparty/ctest.h 
	gcc -I thirdparty -c test/main.c -o build/test/main.o

build/test/todo_test.o: src/task.h thirdparty/ctest.h 
	gcc -I thirdparty -I src -c test/todo_test.c -o build/test/todo_test.o


clear:
	rm -rf build/*.o bin/todo bin/todo-test 
