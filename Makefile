CC = g++
CFLAGS = -O0 -fPIC -g -I./include -c
OUT = ../lib/libPlugins.so

OBJS =	src/plugin_proxy.o \
        src/meta_object.o \
        src/class_loader.o \
        src/plugins.o

all:	createLibrary

createLibrary:	$(OBJS)
		$(CC) -shared -o $(OUT) $(OBJS) 

.cpp.o:
	$(CC) $(CFLAGS) -o $*.o $*.cpp

clean:
	rm -f src/*.o $(OUT) *~
