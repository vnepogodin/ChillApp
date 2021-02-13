CC := gcc
CFLAGS := -Wall -Wextra -ffast-math -O2
LDFLAGS := -lui -lm
OBJECTS := src/helpers.o src/config.o
all: $(OBJECTS) main server

src/main.o:
	$(CC) $(shell pkg-config --cflags gtk+-3.0) -c -o $@ src/main.c

main: src/main.o
	$(CC) $< src/helpers.o $(shell pkg-config --libs gtk+-3.0) $(LDFLAGS) -o chill
server: src/server.o
	$(CC) $< $(OBJECTS) -lconfig -lm -o chill_$@

clean:
	@rm chill chill_server
delete:
	@rm src/*.o
run:
	./chill_server

prepare:
	@if [ ! -d "${HOME}/.config/chill_app" ]; then mkdir -Z ${HOME}/.config/chill_app; fi
	@cp -Z etc/* ${HOME}/.config/chill_app

install:
	@install -Z chill /usr/bin
	@install -Z chill_server /usr/bin
