memsim: main.o memsim.o FIFO.o
	gcc -o memsim main.o memsim.o FIFO.o
  
main.o: main.c memsim.h
	gcc -c main.c
  
memsim.o: memsim.c memsim.h
	gcc -c memsim.c
  
FIFO.o: FIFO.c memsim.h
	gcc -c FIFO.c
