all : compile run clean

compile: main.cpp os.cpp file.cpp
		@g++ -o test main.cpp os.cpp file.cpp

run: test
	@./test

clean: test
	@rm -f test 