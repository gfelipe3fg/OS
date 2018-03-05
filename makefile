memsim: memory.o memsim.o util.o
	gcc -o memsim memory.o memsim.o util.o

memsim.o: memsim.c 
	gcc -c memsim.c

memory.o: memory.c memory.h util.h 
	gcc -c memory.c

util.o: util.c util.h
	gcc -c util.c

clean:
	rm *.o memsim
