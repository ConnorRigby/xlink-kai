#CFLAGS=-g -ggdb -Wall -DDEBUG_SOCK -DUNIX
#CFLAGS=-g -Wall -DUNIX
CFLAGS=-Wall -DUNIX -fPIC

DEPS= kaiui.h kaiui_priv.h errors.h linux/netcode.h
OBJ = kaiui.o linux/netcode.o parse.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

libkaiui.a: $(OBJ)
	ar vcr $@ $^
	ranlib $@

.PHONY: clean dist

clean:
	rm -f *~ *.o *.a linux/*.o linux/*~ win32/*~

dist:
	cd .. && tar cjvf libkaiui.tar.bz2 libkaiui/
