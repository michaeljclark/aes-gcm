
OBJS =	src/aes-opencl-test.o \
	src/aes-internal-dec.o \
	src/aes-internal-enc.o \
	src/aes-internal.o \
	src/logging.o \
	src/opencl.o

CFLAGS =   -O3 -Wall -std=c99
CXXFLAGS = -O3 -Wall -std=c++11 -I/opt/AMDAPP/include/
LDFLAGS = -lOpenCL

all: aes-opencl-test

clean:
	rm -f src/*.o aes-opencl-test

aes-opencl-test: $(OBJS)
	g++  $(LDFLAGS) -o $@ $(OBJS)

%.o : %.cc
	g++ $(CXXFLAGS) -o $@ -c $<

%.o : %.c
	gcc $(CFLAGS) -o $@ -c $<
