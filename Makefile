CFLAGS = -Wall -Wvla -std=c++11
LOGS_PATH = Logs/

all: compile

compile: refactor.cpp
	g++ $(CFLAGS) refactor.cpp $(LOGS_PATH)logger.cpp $(LOGS_PATH)time.cpp -o refactor
	chmod +x refactor
	sudo mv refactor /usr/bin

clean:
	sudo rm -fr /usr/bin/refactor
