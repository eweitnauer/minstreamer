LFLAGS=`pkg-config --libs gstreamer-0.10` -L/usr/lib/gstreamer-0.10 -Wl,--rpath /usr/lib/gstreamer-0.10/ -lgstudp
CFLAGS=`pkg-config --cflags gstreamer-0.10` -I./gst-plugins-good-0.10.9 -I./gst-plugins-good-0.10.9/win32/common

all: soa_videostreamer

%.o: %.c
	gcc -c -o $@ $< $(CFLAGS)

soa_videostreamer: soa_videostreamer.c
	g++ -o $@ $^ $(LFLAGS) $(CFLAGS)

.PHONY : clean

clean:
	rm -f	soa_videostreamer soa_videostreamer.o
