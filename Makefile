CC=g++
CFLAGS=-Wall -g -O3 -std=gnu++2a -Iinclude -Wno-deprecated-declarations -Wno-unneeded-internal-declaration  -I/usr/local/include -I/usr/local/Cellar/openssl\@1.1/1.1.1g/include -lcpprest  -lboost_system -lboost_thread-mt -lboost_chrono-mt -lssl -lcrypto  -L/usr/local/opt/openssl/lib
TARGET=app.out
OBJS=src/main.o\
	src/BasicVariableInfo.o\
	src/ArgumentParser.o\
	src/Timer.o\
	src/WebRequestManager.o\
	src/PrinterInfo.o\
	src/GithubRequestManager.o\
	src/Logger.o\
	
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) lib/jsoncpp.precompiled

clean:
	find . -name "*.o" -exec rm {} \;
	find . -name "*.out" -exec rm {} \;