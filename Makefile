CFLAGS=-std=gnu99 -m32 -pedantic -Wall -Werror -Wextra -Wconversion -D_XOPEN_SOURCE=700 -g -O0
CC=gcc $(CFLAGS)

BUILDDIR=build
OBJDIR=build/obj
VENDORS=vendors

.PHONY: all clean
.DEFAULT_GOAL := all

all: $(BUILDDIR)/feather_test $(BUILDDIR)/libfeather.a

clean:
	rm -f $(BUILDDIR)/feather_test
	rm -f $(BUILDDIR)/libfeather.a
	rm -f $(OBJDIR)/*.o

$(BUILDDIR)/feather_test: $(OBJDIR)/feather_test.o $(OBJDIR)/feather.o $(OBJDIR)/feather_context_switch.o
	$(CC) -o $@ $^

$(BUILDDIR)/libfeather.a: $(OBJDIR)/feather.o $(OBJDIR)/feather_context_switch.o
	ar -rcs $@ $^

$(OBJDIR)/feather.o: feather.c feather.h
	$(CC) -o $@ -c $<

$(OBJDIR)/feather_context_switch.o: feather_context_switch.S
	$(CC) -o $@ -c $<

$(OBJDIR)/feather_test.o: feather_test.c feather.h
	$(CC) -o $@ -c $<