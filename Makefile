CC := gcc
CFLAGS := -ffast-math -O2
LDFLAGS := -lui -lm
all: config main server delete

config: src/config.o

main: src/main.o
	$(CC) $< $(LDFLAGS) -o chill
server: src/server.o
	$(CC) $< src/config.o -lconfig -o chill_$@

clean:
	@rm chill chill_server
delete:
	@rm src/*.o
run:
	./chill_server

prepare:
	@if [ ! -d "${HOME}/.config/chill_app" ]; then mkdir -Z ${HOME}/.config/chill_app; fi
	@cp -Z etc/config ${HOME}/.config/chill_app

install:
	@install -Z chill /usr/bin
	@install -Z chill_server /usr/bin
