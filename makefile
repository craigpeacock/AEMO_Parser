aemo : aemo.o 
	cc -o aemo aemo.o -lcurl -lcjson

aemo.o : aemo.c
	cc -c aemo.c

clean :
	rm aemo aemo.o 
        
        