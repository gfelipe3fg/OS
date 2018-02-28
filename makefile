memsim: simulator.o memory.o main.o
	gcc -o memsim simulator.o memory.o main.o

main.o: main.c simulator.h
	gcc -c main.c
  
simulator.o: simulator.c simulator.h memory.h 
	gcc -c simulator.c

memory.o: memory.c memory.h 
	gcc -c memory.c

clean:
	rm *.o memsim
