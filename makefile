CC = gcc

srcs = ex_libnl.c
objects = ex_libnl.o
LIB_PATH = /usr/lib/
INC_PATH = /usr/include/
ALLOC_PATH = /usr/include/libnl3/
CFLAGS = -Wall -g
LDFLAGS = -lnl-3

ex_libnl : $(objects)
	$(CC) -o ex_libnl -L$(LIB_PATH) -I$(INC_PATH)  -I/usr/include/libnl3/  $(LDFLAGS) $(objects) 

.c.o:
	$(CC) $(CFLAGS) -c $*.c  -I/usr/include/libnl3/ $(LDFLAGS) 

.PHONY : clean
clean :
	rm ex_libnl $(objects)

## gcc ex_libnl.c -Wall -g -I/usr/lib/ -I/usr/include/libnl3/  -o mmm -lnl-3
##RULE:	DEPENDENCY LINE
##[tab]ACTION LINE(S)
##DEPENDENCY LINE:	TARGET FILES:SOURCE FILES
## -o ex_libnl -L$(LIB_PATH) -I$(INC_PATH) $(LDFLAGS) $(objects) 
##gcc -Wall -g   -c -o ex_libnl.o ex_libnl.c




##gcc ex_libnl.c -Wall -g -I/usr/lib/ -I/usr/include/libnl3/  -o mmm -lnl-3
##gcc ex_libnl.c -Wall -g -I/usr/lib/ -I/usr/include/libnl3/ -lnl-3 -lnl # works with both genl_connect  and allocate
