CC = g++
CFLAGS = -pg -g -Wall --std=c++0x
# CFLAGS = -g -Wall --std=c++0x
# CFLAGS = -Ofast --std=c++0x
# CFLAGS = -Ofast --std=c++0x -maix64
PROG = puzzler sudoku
TEMPFILES = core core.* *.o temp.* *.out *~

all: $(PROG)

$(PROG): %: %.cpp dlx.cpp socket.cpp
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TEMPFILES) $(PROG)

