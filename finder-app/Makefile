ifeq ($(CROSS_COMPILE), aarch64-none-linux-gnu-)
	compiler=aarch64-none-linux-gnu-gcc
else
	compiler=gcc
endif

objects = writer.o

writer : $(objects)
	$(compiler) -o writer $(objects)

writer.o : writer.c
	$(compiler) -c writer.c

.PHONY : clean
clean :
	rm writer $(objects)