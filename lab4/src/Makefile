CC = gcc

CFLAGS = -Wall -Wextra -fPIC -O2

LIB1_SRCS = der1.c bubble_sort.c

LIB2_SRCS = der2.c quick_sort.c

LIB1_OBJS = $(LIB1_SRCS:.c=.o)
LIB2_OBJS = $(LIB2_SRCS:.c=.o)

all: libLib1.so libLib2.so prog_static prog_dynamic

libLib1.so: $(LIB1_OBJS)
	$(CC) -shared -o $@ $^ -lm

libLib2.so: $(LIB2_OBJS)
	$(CC) -shared -o $@ $^ -lm

prog_static: prog_static.c libLib1.so
	$(CC)  -o prog_static prog_static.c -L. -lLib1 -Wl,-rpath=.

prog_dynamic: prog_dynamic.c
	$(CC)  -o $@ prog_dynamic.c

%.o: %.c
	$(CC)  -c $< -o $@

clean:
	rm -f *.o libLib1.so libLib2.so prog_static prog_dynamic
