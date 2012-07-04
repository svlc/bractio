
#TODO add header dependencies

SHELL = /bin/sh
CC=gcc
AR=ar
# "-lz for zlib"
CFLAGS=-std=c99 -lz -pedantic -Wall -Werror -Wextra -ggdb #-DNDEBUG

# library source
LSRC=$(wildcard lib/*.c)
LOBJ=$(patsubst lib/%.c, obj/%.o, $(LSRC))

# program source
PSRC=$(wildcard src/*.c)
POBJ=$(patsubst %.c, %.o, $(PSRC))

# library target
LTARG=build/libapm.a
# program target
PTARG=bin/prog


.PHONY	: all
all	: $(PTARG)


$(PTARG): bin $(LTARG) $(POBJ)
	$(CC) $(CFLAGS) $(POBJ) -Lbuild -lapm -o $@


$(LTARG): build obj $(LOBJ)
	$(AR) -cru $@ $(LOBJ)


# overwriting implicit rules for c source files
obj/%.o	: lib/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

# these rules have no prerequisites
obj	: ; @mkdir obj

bin	: ; @mkdir bin

build	: ; @mkdir build


.PHONY:	clean
clean:
	rm -f lib/*~
	rm -rf obj/ bin/ build/
	rm -f src/main.o

