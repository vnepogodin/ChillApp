CC := gcc
CFLAGS := -ffast-math -O2
LDFLAGS := -lui -lm
all: main server delete

main: main.o
	$(CC) $< $(LDFLAGS) -o chill
server: server.o
	$(CC) $< -o $@

clean:
	rm chill server
delete:
	rm *.o
install:
	cp chill /usr/local/bin/chill
	chmod 755 /usr/local/bin/chill
