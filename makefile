aemo : aemo.o 
	cc -o aemo aemo.o mqtt.o http.o parser.o -lcurl -lcjson -lpaho-mqtt3c -pthread

aemo.o : aemo.c
	cc -c aemo.c mqtt.c http.c parser.c

clean :
	rm aemo aemo.o mqtt.o http.o parser.o 
        
        
