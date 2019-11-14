# Basic Makefile to pull in kernel's KBuild to build an out-of-tree
# kernel module

KDIR ?= /lib/modules/$(shell uname -r)/build

all: modules mastermind-test

mastermind-test: mastermind-test.c
	gcc --std=c99 -Wall -O2 -pthread -o $@ $^ -lm

modules:
	$(MAKE) -C $(KDIR) M=$$PWD $@

clean:
	$(MAKE) -C $(KDIR) M=$$PWD $@
	-rm mastermind-test
