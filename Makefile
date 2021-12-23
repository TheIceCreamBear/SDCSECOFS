#Run with make
ifeq ($(OS),Windows_NT)
run : compile
	./bin/windows-threads-x86-64.exe
compile :
	gcc ./src/ourprogram.c ./src/concurrencylib.c ./src/userprogram.c ./src/vcuserlibrary.c -o ./bin/windows-threads-x86-64.exe
else ifeq ($(shell uname -s),Linux)
run : compile
	./bin/linux-threads-x86-64.exe
compile :
	gcc -pthread ./src/ourprogram.c ./src/concurrencylib.c ./src/userprogram.c ./src/vcuserlibrary.c -o ./bin/linux-threads-x86-64.exe
else ifeq ($(shell uname -s),Darwin)
run : compile
	./bin/macos-threads-x86-64.exe
compile :
	gcc -pthread ./src/ourprogram.c ./src/concurrencylib.c ./src/userprogram.c ./src/vcuserlibrary.c -o ./bin/macos-threads-x86-64.exe
else
run :
	@echo "Error: Unsupported OS"
endif