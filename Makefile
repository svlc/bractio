# This Makefile partially follows the "GNU coding standards"
# <https://www.gnu.org/prep/standards/>

SHELL = /bin/sh

# clear, then define suffix list
.SUFFIXES:
.SUFFIXES: .c .h .o

CC = gcc
AR = ar
INSTALL = install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644

# OPTCFLAGS is to be passed to make command to add some extra flags
CFLAGS =-pedantic -Wall -Werror -Wextra -ggdb -D_XOPEN_SOURCE\
$(OPTCFLAGS)# -DNDEBUG


# these options are mandatory, do not let the 'make user' suppress them
# "-lz is for zlib"
override CFLAGS += -std=c99 -lz

# library {source,header,object} files
LSRC = $(wildcard libbract/*.c)
LHDR = $(wildcard libbract/*.h)
LOBJ = $(patsubst libbract/%.c,obj/%.o,$(LSRC))

# program source
PSRC = $(wildcard src/*.c)
POBJ = $(patsubst %.c,%.o,$(PSRC))

LIBNAME = bract
# library target
LTARG = build/lib$(LIBNAME).a
# program name
PNAME = bractio
# program target
PTARG = bin/$(PNAME)

RST_FILES = $(wildcard *.rst)
HTML_FILES = $(patsubst %.rst,%.html,$(RST_FILES))
PDF_FILES = $(patsubst %.rst,%.pdf,$(RST_FILES))

VERSION = 0.1
DISTDIR = bract-$(VERSION)
# all files important for distribution
DISTFILES = doc/ INSTALL libbract/ LICENCE Makefile README.rst samples/ src/

prefix = /usr/local
datarootdir = $(prefix)/share
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
mandir = $(datarootdir)/man
man1dir = $(mandir)/man1

.PHONY: all dist installdirs install uninstall html pdf clean
all   : $(PTARG)


$(PTARG): bin $(LTARG) $(POBJ)
	$(CC) $(CFLAGS) $(POBJ) -Lbuild -l$(LIBNAME) -o $@


$(LTARG): build $(LOBJ)
	$(AR) -cru $@ $(LOBJ)


# define new rule with higher priority compared to implicit %.c rule

# "obj" must be "order-only prerequisite", otherwise "obj"
# causes all source files to be recompiled over and over again

# This way every source file depends on all header files
# (there is more proper, but quite annoying solution:
# gnu.org/software/make/manual/html_node/Automatic-Prerequisites.html)
obj/%.o: libbract/%.c $(LHDR) | obj
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

# mkdir's "-p" option may be problematic on some systems
installdirs:
	mkdir -p $(DESTDIR)$/$(bindir)
	mkdir -p $(DESTDIR)$/$(man1dir)

install: installdirs
	$(INSTALL_PROGRAM) bin/$(PNAME) $(DESTDIR)/$(bindir)
	-$(INSTALL_DATA) doc/$(PNAME).1 $(DESTDIR)/$(man1dir)
	-gzip $(DESTDIR)$/$(man1dir)/$(PNAME).1

uninstall:
	rm $(DESTDIR)/usr/local/bin/$(PNAME)
	rm $(DESTDIR)$/$(man1dir)/$(PNAME).1.gz

html:	$(HTML_FILES)

pdf:	$(PDF_FILES)

%.pdf: %.rst
	rst2pdf $(<)

%.html: %.rst
	rst2html2 --report=3 $(<) > $(@)

clean :
	rm -rf obj/ bin/ build/
	rm -f src/$(PNAME).o
	rm -f $(DISTDIR).tar.gz
	rm -f $(HTML_FILES) $(PDF_FILES)
	# delete all regular files ending in "~"
	find . -name "*~" -type f -print0 | xargs -0 /bin/rm -f
