objects = writer.o

writer : $(objects)
	cc -o writer $(objects)

writer.o : writer.c
	cc -c writer.c

clean :
	rm writer $(objects)