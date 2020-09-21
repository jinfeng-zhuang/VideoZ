CC = gcc
CFLAGS = -O0 -Wall

parser: main.c loader.c ts_packet_parse.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o
	rm parser
