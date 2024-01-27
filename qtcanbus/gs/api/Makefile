all: winusbtest.exe

CC=i686-w64-mingw32-gcc
CFLAGS=-Wall
LDLIBS += -lsetupapi -lole32 -lwinusb

SOURCES += main.c candle.c candle_ctrl_req.c

OBJECTS  := $(SOURCES:.c=.o)

HEADERS += ch_9.h candle.h candle_defs.h candle_ctrl_req.h

%.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $< -o $@

winusbtest.exe: $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	rm -f *.o winusbtest.exe

