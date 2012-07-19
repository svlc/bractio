
#TODO add header dependencies (high priority)

SHELL = /bin/sh

# clear, then define suffix list
.SUFFIXES:
.SUFFIXES: .c .o

CC = gcc
AR = ar

# "-lz for zlib"
CFLAGS = -pedantic -Wall -Werror -Wextra -ggdb# -DNDEBUG

# these options are mandatory, do not let the 'make user' suppress them
override CFLAGS += -std=c99 -lz

# library source
LSRC = $(wildcard lib/*.c)
LOBJ = $(patsubst lib/%.c,obj/%.o,$(LSRC))

# program source
PSRC = $(wildcard src/*.c)
POBJ = $(patsubst %.c,%.o,$(PSRC))


LIBNAME = rapm
# library target
LTARG = build/lib$(LIBNAME).a
# program target
PTARG = bin/prog


VERSION = alfa
DISTDIR = librapm-$(VERSION)
# all files important for distribution
DISTFILES = lib/ LICENCE Makefile README samples/ src/ TODO


.PHONY: all
all   : $(PTARG)


$(PTARG): bin $(LTARG) $(POBJ)
	$(CC) $(CFLAGS) $(POBJ) -Lbuild -l$(LIBNAME) -o $@


$(LTARG): build obj $(LOBJ)
	$(AR) -cru $@ $(LOBJ)


# define new rule with higher priority compared to implicit %.c rule
obj/%.o: lib/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# directory creation, these rules have no prerequisites
obj       : ; @mkdir obj
bin       : ; @mkdir bin
build     : ; @mkdir build
$(DISTDIR): ; @mkdir $(DISTDIR)

# create gzip archive
dist: $(DISTDIR)
	@cp -r $(DISTFILES) $(DISTDIR)
	tar -czf $(DISTDIR).tar.gz $(DISTDIR)
	@rm -rf $(DISTDIR)


.PHONY: clean
clean :
	rm -f lib/*~
	rm -rf obj/ bin/ build/
	rm -f src/main.o
	rm -rf $(DISTDIR).tar.gz

