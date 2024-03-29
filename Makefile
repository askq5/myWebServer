NOSOURCE :=webServer.cpp
SOURCE :=$(wildcard *.cpp)
SOURCE :=$(filter-out $(NOSOURCE),$(SOURCE))
OBJS := $(patsubst %.cpp, %.o,$(SOURCE))


	 

CFLAGS:= -std=c++11 -g  -Wall
CXXFLAGS:=$(CFLAGS) 
CC:=g++
LDFLAGS+= -pthread -lmysqlclient

TARGET:=output/webServer
.PHONY : objs clean veryclean rebuild all  debug
all:$(TARGET) clean
	#@echo $(OBJS)
rebuild: veryclean all
clean:
	find . -name '*.o' | xargs rm -f
veryclean:
	find . -name '*.o' | xargs rm -f
	find . -name webServer | xargs rm -f
debug:
	@echo $(SOURCE)
	@echo $(OBJS)

$(TARGET):  $(OBJS)  webServer.o
	$(CC) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
#webServer.o:webServer.cpp HttpServer.h TcpServer.h
#	$(CC) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
#%.o:%.cpp
#	$(CC) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)