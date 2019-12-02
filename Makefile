CC = g++
# CFLAGS = -pg -Wall
# CFLAGS = -g -Wall --std=c++0x
CFLAGS = -Ofast
PROG = puzzler
TEMPFILES = core core.* *.o temp.* *.out *~

all: $(PROG)

puzzler.o : puzzler.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(PROG): %: %.o
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TEMPFILES) $(PROG)

