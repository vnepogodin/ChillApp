CC := gcc
CFLAGS := -ffast-math -O2
LDFLAGS := -lui -lm
all: main server delete

main: src/main.o
	$(CC) $< $(LDFLAGS) -o chill
server: src/server.o
	$(CC) $< -o chill_$@

clean:
	@rm chill chill_server
delete:
	@rm src/*.o
run:
	./chill_server
install:
	@if [ ! -d "/etc/chill_app" ]; then mkdir -Z /etc/chill_app; fi
	@cp -Z etc/config /etc/chill_app
	@cp -Z chill /usr/bin
	@cp -Z chill_server /usr/bin
	@chmod 755 /usr/bin/chill
	@chmod 755 /usr/bin/chill_server
