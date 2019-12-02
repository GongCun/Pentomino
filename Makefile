CC = g++
# CFLAGS = -pg -Wall --std=c++0x
CFLAGS = -Ofast --std=c++0x
PROG = puzzler
TEMPFILES = core core.* *.o temp.* *.out *~

all: $(PROG)

puzzler.o : puzzler.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(PROG): %: %.o
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TEMPFILES) $(PROG)

