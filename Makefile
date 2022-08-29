CFLAGS = -Wall -Wvla -std=c++11

all: compile

compile: refactor.cpp
	g++ $(CFLAGS) refactor.cpp logger.cpp time.cpp -o refactor
	chmod +x refactor
	sudo mv refactor /usr/bin

clean:
	sudo rm -fr /usr/bin/refactor
