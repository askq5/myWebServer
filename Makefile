MAINSOURCE:=webServer.cpp
SOURCE:=$(wildcard *.cpp)
override SOURCE:=$(filter-out &(MAINSOURCE),$(SOURCE))
OBJS := $(SOURCE)
OBJS := $(patsubst %.cpp,%.o,$(SOURCE))


	 

CFLAGS:= -std=c++11 -g  -pthread -Wall
CXXFLAGS:=$(CFLAGS) 
CC:=g++
LDFLAGS+= -pthread

TARGET:=webServer
.PHONY : objs clean veryclean rebuild all  debug
all:$(TARGET)
	#@echo $(OBJS)
rebuild: veryclean all
clean:
	find . -name '*.o' | xargs rm -f
veryclean:
	find . -name '*.o' | xargs rm -f
	find . -name $(TARGET) | xargs rm -f
debug:
	@echo $(OBJS)

$(TARGET): $(OBJS) webServer.o
	$(CC) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
#webServer.o:webServer.cpp HttpServer.h TcpServer.h
#	$(CC) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
#%.o:%.cpp
#	$(CC) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)