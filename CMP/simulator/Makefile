CC = gcc

CMP: main.o cmp.o memory.o regfile.o processor.o
	$(CC) -o $@ main.o cmp.o memory.o regfile.o processor.o
main.o: main.c cmp.c cmp.h memory.c memory.h regfile.c regfile.h processor.c processor.h
	$(CC) -c -g main.c
cmp.o: cmp.c cmp.h memory.c memory.h processor.c processor.h
	$(CC) -c -g cmp.c
memory.o: memory.c memory.h regfile.c regfile.h
	$(CC) -c -g memory.c
regfile.o: regfile.c regfile.h memory.c memory.h
	$(CC) -c -g regfile.c
processor.o: processor.c processor.h cmp.c cmp.h regfile.c regfile.h memory.c memory.h
	$(CC) -c -g processor.c
clean:
	rm -f main.o cmp.o memory.o regfile.o processor.o
