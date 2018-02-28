memsim: util.o simulator.o memory.o main.o
	gcc -o memsim util.o simulator.o memory.o main.o

main.o: main.c simulator.h
	gcc -c main.c
  
util.o: util.c util.h
	gcc -c util.c
  
simulator.o: simulator.c simulator.h memory.h util.h
	gcc -c simulator.c

memory.o: memory.c memory.h util.h
	gcc -c memory.c

clean:
	rm *.o memsim
