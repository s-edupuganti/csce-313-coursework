# makefile

all: client

common.o: common.h common.cpp
	g++ -g -w -std=c++17 -c common.cpp

TCPreqchannel.o: TCPreqchannel.h TCPreqchannel.cpp
	g++ -g -w -std=c++17 -c TCPreqchannel.cpp

client: client.cpp TCPreqchannel.o common.o
	g++ -g -w -std=c++17 -o client client.cpp TCPreqchannel.o common.o -lpthread -lrt

clean:
	rm -rf *.o client index
