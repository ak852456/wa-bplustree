all: exec

CC = g++
INCLUDE = .
CFLAGS = -g -Wall -ansi
MEMORY_SIZE = 400M
DATA_PATH = Trace/100W/NDK_0.txt
 
exec: main.o wa-bpt.o pcm.o
	$(CC) -o exec main.o wa-bpt.o pcm.o

main.o: main.cpp wa-bpt.h pcm.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c main.cpp

wa-bpt.o: wa-bpt.cpp wa-bpt.h 
	$(CC) -I$(INCLUDE) $(CFLAGS) -c wa-bpt.cpp

pcm.o: pcm.cpp pcm.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c pcm.cpp

clean:
	rm -rf exec main.o wa-bpt.o pcm.o

run: ./exec
	./exec $(MEMORY_SIZE) $(DATA_PATH)

