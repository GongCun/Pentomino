CC = g++
# CFLAGS = -pg -Wall
CFLAGS = -g -Wall --std=c++0x
# CFLAGS = -Ofast
PROG = puzzler dlx-test
TEMPFILES = core core.* *.o temp.* *.out *~

all: $(PROG)

$(PROG): %: %.cpp dlx.cpp
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TEMPFILES) $(PROG)

