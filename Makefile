ERROR_OPTIONS=-Wall -Werror -Wextra 

all: bin/todo format bin/database-test

bin/todo: build/src/main.o build/src/interface.o build/src/database_func.o
	gcc $(ERROR_OPTIONS)  build/src/interface.o build/src/main.o build/src/database_func.o -o bin/todo `pkg-config --cflags --libs gtk+-3.0` -lsqlite3

build/src/main.o: src/main.c src/interface.h src/database_func.h
	gcc -I src $(ERROR_OPTIONS) -c src/main.c -o build/src/main.o `pkg-config --cflags --libs gtk+-3.0` -lsqlite3

build/src/interface.o: src/interface.c src/interface.h src/database_func.h
	gcc -I src $(ERROR_OPTIONS) -c src/interface.c -o build/src/interface.o `pkg-config --cflags --libs gtk+-3.0` -lsqlite3

build/src/database_func.o: src/database_func.c src/database_func.h
	gcc -I src $(ERROR_OPTIONS) -c src/database_func.c -o build/src/database_func.o `pkg-config --cflags --libs gtk+-3.0` -lsqlite3

bin/database-test: build/test/database_func_test.o build/test/main.o build/test/database_func.o
	gcc $(ERROR_OPTIONS) build/test/database_func.o build/test/main.o build/test/database_func_test.o -o bin/database-test `pkg-config --cflags --libs gtk+-3.0` -lsqlite3

build/test/database_func.o: src/database_func.c src/database_func.h
	gcc -I src -c src/database_func.c -o build/test/database_func.o `pkg-config --cflags --libs gtk+-3.0` -lsqlite3

build/test/main.o: test/main.c thirdparty/ctest.h 
	gcc -I thirdparty -c test/main.c -o build/test/main.o `pkg-config --cflags --libs gtk+-3.0` -lsqlite3

build/test/database_func_test.o: test/database_func_test.c src/database_func.h thirdparty/ctest.h 
	gcc -I thirdparty -I src -c test/database_func_test.c -o build/test/database_func_test.o `pkg-config --cflags --libs gtk+-3.0` -lsqlite3

format: 
	clang-format -i src/main.c src/interface.h src/interface.c src/database_func.h src/database_func.c 

clear:
	rm -rf build/*.o bin/todo bin/todo-test 
