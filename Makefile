CC = g++
#CFLAGS = -g -Wall --std=c++0x
CFLAGS = -Ofast --std=c++0x
PROG = puzzler
TEMPFILES = core core.* *.o temp.* *.out *~

all: $(PROG)

$(PROG): %: %.cpp dlx.cpp socket.cpp
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TEMPFILES) $(PROG)

