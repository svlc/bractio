SHELL = /bin/sh

# clear, then define suffix list
.SUFFIXES:
.SUFFIXES: .c .h .o

CC = gcc
AR = ar


CFLAGS =-pedantic -Wall -Werror -Wextra -ggdb -D_XOPEN_SOURCE# -DNDEBUG


# these options are mandatory, do not let the 'make user' suppress them
# "-lz is for zlib"
override CFLAGS += -std=c99 -lz

# library {source,header,object} files
LSRC = $(wildcard lib/*.c)
LHDR = $(wildcard lib/*.h)
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


$(LTARG): build $(LOBJ)
	$(AR) -cru $@ $(LOBJ)


# define new rule with higher priority compared to implicit %.c rule

# @note "obj" must be "order-only prerequisite", otherwise "obj"
# causes all source files to be recompiled over and over again

# @note this way every source file depends on all header files
# (there is more proper, but quite annoying solution:
# gnu.org/software/make/manual/html_node/Automatic-Prerequisites.html)
obj/%.o: lib/%.c $(LHDR) | obj
	$(CC) $(CFLAGS) -c -o $@ $<


# directory creation, these rules have no prerequisites
obj       : ; -@mkdir obj
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

