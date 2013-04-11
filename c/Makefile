all: ws

C:=gcc
CFLAGS:=-Wall -Werror -g -O0 -fno-omit-frame-pointer -pthread

ws: main.o file_manager.o pool.o 
	$(C) $(CFLAGS) -o $@ $^

%.o: %.c
	$(C) $(CFLAGS) -o $@ -c $<

clean:
	-rm *.o
	-rm *~
	-rm ws
