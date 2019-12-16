CC = g++
CFLAGS = -pg -g -Wall
# CFLAGS = -g -Wall --std=c++0x
# CFLAGS = -Ofast
PROG = puzzler
TEMPFILES = core core.* *.o temp.* *.out *~

all: $(PROG)

$(PROG): %: %.cpp dlx.cpp
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TEMPFILES) $(PROG)

