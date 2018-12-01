CC = gcc -O3

IFLAGS  = -I/comp/40/include -I/usr/sup/cii40/include/cii
CFLAGS  = -g -std=gnu99 -Wall -Wextra -Werror -pedantic $(IFLAGS)
LDFLAGS = -g -L/comp/40/lib64 -L/usr/sup/cii40/lib64
LDLIBS  = -l40locality -lcii40 -lm

EXECS   = um writetests

all: $(EXECS)

um: um.o mem_interface.o io_dev.o ops_interface.o bitpack.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# tests: test_segment.o mem_interface.o io_dev.o ops_interface.o bitpack.o
# 	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

writetests: umlab.o umlabwrite.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECS)  *.o