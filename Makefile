CC=g++
CFLAGS=-Wall -O3 -std=gnu++2a -Iinclude -Wno-deprecated-declarations
TARGET=app.out
OBJS=src/main.o\
	src/BasicVariableInfo.o\
	src/ArgumentParser.o\
	src/Timer.o\
	src/jsoncpp.o\
	src/WebRequestManager.o\
	src/PrinterInfo.o\
	src/GithubRequestManager.o\
	
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

clean:
	find . -name "*.o" -exec rm {} \;
	find . -name "*.out" -exec rm {} \;