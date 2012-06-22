
NAME=crash

default: all

all: debug tests

debug:
	gcc -o bin/$(NAME) src/main.c src/executioner.c libs/path_utils.c libs/str_utils.c -Wall -g -DDEBUG

tests: clean_tests
	gcc -o bin/test_utilslib tests/test_utilslib.c libs/str_utils.c -Wall -g -DDEBUG 
	gcc -o bin/test_output tests/test_output.c -Wall -g -DDEBUG
	gcc -o bin/test_laco tests/test_laco.c -Wall -g -DDEBUG

	
clean:
	rm -f bin/$(NAME)
	
clean_tests:
	rm -f bin/test_*
