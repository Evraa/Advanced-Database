run: output
	./output
	
output: readfile.o main.o openAddressing.o 
	g++ -o output main.o openAddressing.o readfile.o 

readfile.o: readfile.cpp readfile.h
	g++ -c readfile.cpp 
openAddressing.o:openAddressing.cpp
	g++ -c openAddressing.cpp
main.o: main.cpp
	g++ -c main.cpp

clean:
	rm *.o openAddressing output
