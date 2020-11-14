output: readfile.o main.o multiple_hashing.o 
	g++ -o output main.o multiple_hashing.o readfile.o 

readfile.o: readfile.cpp readfile.h
	g++ -c readfile.cpp 
multiple_hashing.o:multiple_hashing.cpp
	g++ -c multiple_hashing.cpp
main.o: main.cpp
	g++ -c main.cpp

clean:
	rm *.o multiple_hashing output
