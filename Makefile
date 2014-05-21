
OBJS =	src/aes-opencl-test.o \
	src/aes-internal-dec.o \
	src/aes-internal-enc.o \
	src/aes-internal.o \
	src/logging.o \
	src/opencl.o

CFLAGS =   -Wall -std=c99
CXXFLAGS = -Wall -std=c++11 -I/opt/AMDAPP/include/
LDFLAGS = -lOpenCL

aes-opencl-test: $(OBJS)
	g++  $(LDFLAGS) -o $@ $(OBJS)

%.o : %.cc
	g++ $(CXXFLAGS) -o $@ -c $<

%.o : %.c
	gcc $(CFLAGS) -o $@ -c $<
