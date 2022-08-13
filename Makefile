CFLAGS = -Wall -pedantic -std=c99

all: compile

compile: refactor.c
	gcc $(CFLAGS) refactor.c -o refactor
	chmod +x refactor
	sudo mv refactor /usr/bin

clean:
	sudo rm -fr /usr/bin/refactor
