run : compile
	./bin/test-windows-threads-x86-64.exe

compile :
	gcc ./src/ourprogram.c ./src/concurrencylib.c ./src/userprogram.c -o ./bin/test-windows-threads-x86-64.exe