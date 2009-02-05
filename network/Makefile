%.o: %.c
	gcc -c -o $@ $< $(CFLAGS)

udp_client: udp_client.o
	gcc -o $@ $^ $(LFLAGS)
	
udp_server: udp_server.o
	gcc -o $@ $^ $(LFLAGS)

.PHONY : clean

clean:
	rm -f udp_client udp_client.o
	rm -f udp_server udp_server.o
