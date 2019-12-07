CC = g++
CFLAGS = -std=c++11
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:%.cpp=%.o)

all:clean fs compress
clean:
	rm -f *.o  fs-sim.zip cout.txt cerr.txt
compile:
%.o:%.cpp
	${CC} ${CFLAGS} -c $^ -o $@

fs: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) 

compress:
	zip fs-sim.zip *.cpp *.h README.md Makefile

copy:
	cp clean_disk11 clean_disk
remove:
	-rm -f disk* input* clean_disk* trivial* std*


	
