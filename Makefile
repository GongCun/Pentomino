CC = g++
CFLAGS = -g -Wall --std=c++0x
PROG = puzzler
TEMPFILES = core core.* *.o temp.* *.out *~

all: $(PROG)

puzzler.o : puzzler.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(PROG): %: %.o
	$(CC) $< -o $@

clean:
	rm -f $(TEMPFILES) $(PROG)

