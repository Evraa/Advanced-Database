run: output 
	./output
	
output: readfile.o main.o extendible.o 
	g++ -o output main.o extendible.o readfile.o 
	
readfile.o: readfile.cpp readfile.h
	g++ -c readfile.cpp 
extendible.o:extendible.cpp
	g++ -c extendible.cpp
main.o: main.cpp
	g++ -c main.cpp

clean:
	rm *.o extendible output
