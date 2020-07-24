CC=g++
CFLAGS=-Wall -O3 -std=gnu++2a
TARGET=app.out
OBJS=main.o\
	BasicVariableInfo.o\
	ArgumentParser.o\
	Timer.o\
	jsoncpp.o\
	
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

clean:
	rm -rf ./*.out ./*.o