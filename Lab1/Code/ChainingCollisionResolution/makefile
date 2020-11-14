run: output
	./output
	
output: readfile.o main.o chaining.o 
	g++ -o output main.o chaining.o readfile.o 
	
readfile.o: readfile.cpp readfile.h
	g++ -c readfile.cpp 
chaining.o:chaining.cpp
	g++ -c chaining.cpp
main.o: main.cpp
	g++ -c main.cpp

clean:
	rm *.o chaining output
