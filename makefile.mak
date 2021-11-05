ifeq ($(OS),Windows_NT)
run : compile
	./bin/test-windows-threads-x86-64.exe
compile :
	gcc ./src/ourprogram.c ./src/concurrencylib.c ./src/userprogram.c -o ./bin/test-windows-threads-x86-64.exe
else ifeq ($(shell uname -s),Linux)
run : compile
	./bin/test-linux-threads-x86-64.exe
compile :
	gcc -pthread ./src/ourprogram.c ./src/concurrencylib.c ./src/userprogram.c -o ./bin/test-linux-threads-x86-64.exe
else ifeq ($(shell uname -s),Darwin)
run : compile
	./bin/test-macos-threads-x86-64.exe
compile :
	gcc -pthread ./src/ourprogram.c ./src/concurrencylib.c ./src/userprogram.c -o ./bin/test-macos-threads-x86-64.exe
else
run :
	@echo "Error: Unsupported OS"
endif