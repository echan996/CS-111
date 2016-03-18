CC = gcc
CFLAGS = -std=c11 -g -Wall -Wextra -Wno-unused
LAB = 1
DISTDIR = lab1-$(USER)
export PATH := /usr/local/cs/bin:$(PATH)

all: simpsh

SIMPSH_SOURCES = \
  simpsh.c
TESTS = $(test.sh)

SIMPSH_OBJECTS = $(subst .c,.o,$(SIMPSH_SOURCES))

DIST_SOURCES = \
  $(SIMPSH_SOURCES) Makefile test.sh README execline_test1.sh execline_test2.sh execline_test3.sh TimingResults.pdf

simpsh: $(SIMPSH_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(SIMPSH_OBJECTS)

dist: $(DISTDIR).tar.gz

$(DISTDIR).tar.gz: $(DIST_SOURCES)
	rm -fr $(DISTDIR)
	tar -czf $@.tmp $(DIST_SOURCES)
	mv $@.tmp $@

check: 
	sh test.sh
clean:
	rm -fr *.o *~ *.bak *.tar.gz core *.core *.tmp simpsh $(DISTDIR)

.PHONY: all dist check $(TEST_BASES) clean
