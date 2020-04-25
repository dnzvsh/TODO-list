ERROR_OPTIONS=-Wall -Werror -Wextra

all: bin/todo format

bin/todo: build/main.o build/interface.o build/database_func.o
	gcc $(ERROR_OPTIONS)  build/interface.o build/main.o build/database_func.o -o bin/todo `pkg-config --cflags --libs gtk+-3.0` -lsqlite3

build/main.o: src/main.c src/interface.h src/database_func.h
	gcc -I src $(ERROR_OPTIONS) -c src/main.c -o build/main.o `pkg-config --cflags --libs gtk+-3.0` -lsqlite3

build/interface.o: src/interface.c src/interface.h src/database_func.h
	gcc -I src $(ERROR_OPTIONS) -c src/interface.c -o build/interface.o `pkg-config --cflags --libs gtk+-3.0` -lsqlite3

build/database_func.o: src/database_func.c src/database_func.h
	gcc -I src $(ERROR_OPTIONS) -c src/database_func.c -o build/database_func.o `pkg-config --cflags --libs gtk+-3.0` -lsqlite3

format: src/main.c
	clang-format -i src/main.c src/interface.h src/interface.c

bin/todo-test: build/test/todo_test.o
	gcc $(ERROR_OPTIONS) build/test/main.o build/test/todo_test.o build/src/interface.o -o bin/todo-test

build/test/main.o: thirdparty/ctest.h 
	gcc -I thirdparty -c test/main.c -o build/test/main.o

build/test/todo_test.o: src/interface.h thirdparty/ctest.h 
	gcc -I thirdparty -I src -c test/todo_test.c -o build/test/todo_test.o


clear:
	rm -rf build/*.o bin/todo bin/todo-test 
