all: test1 test2

test1: test1.c ../jeeps/jeeps.a
	gcc test1.c ../jeeps/jeeps.a -lm -lusb -o test1

test2: test2.c
	make -C ../jeeps
	gcc test2.c ../jeeps/jeeps.a -lm -lusb -o test2
