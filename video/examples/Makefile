LFLAGS=`pkg-config --libs gstreamer-0.10` -L/usr/lib/gstreamer-0.10 -Wl,--rpath /usr/lib/gstreamer-0.10/ -lgstudp
CFLAGS=`pkg-config --cflags gstreamer-0.10` -I./gst-plugins-good-0.10.9 -I./gst-plugins-good-0.10.9/win32/common

all: version factory pipeline mainloop udpsrc multicast

%.o: %.c
	gcc -c -o $@ $< $(CFLAGS)

version: version.o
	gcc -o $@ $^ $(LFLAGS)

factory: factory.o
	gcc -o $@ $^ $(LFLAGS)
	
pipeline: pipeline.o
	gcc -o $@ $^ $(LFLAGS)
	
mainloop: mainloop.o
	gcc -o $@ $^ $(LFLAGS)

udpsrc: udpsrc.o
	gcc -o $@ $^ $(LFLAGS)

multicast: multicast.o
	gcc -o $@ $^ $(LFLAGS)

.PHONY : clean

clean:
	rm -f version version.o
	rm -f factory factory.o
	rm -f pipeline pipeline.o
	rm -f mainloop mainloop.o
	rm -f	udpsrc udpsrc.o
	rm -f	multicast	multicast.o
