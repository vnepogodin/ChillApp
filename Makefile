CC := gcc
CFLAGS := -ffast-math -O2
LDFLAGS := -lui -lm
all: main server delete

main: src/main.o
	$(CC) $< $(LDFLAGS) -o chill
server: src/server.o
	$(CC) $< -o chill_$@

clean:
	rm chill chill_server
delete:
	rm src/*.o
install:
	cp chill /usr/local/bin/chill
	cp chill_server /usr/local/bin/chill_server
	chmod 755 /usr/local/bin/chill
	chmod 755 /usr/local/bin/chill_server
