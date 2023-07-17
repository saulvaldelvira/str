.PHONY: clean libs install uninstall
CC = gcc
CCFLAGS = -Wall -Wextra -Werror -pedantic -g -O3 -fPIC

CFILES = str.c wstr.c
HFILES = str.h wstr.h
OFILES = $(patsubst %.c, %.o, $(CFILES))

AR = ar
ARFLAGS = rcs

INSTALL_PATH ?= /usr/local

libs: $(OFILES) $(HFILES)
	@ $(CC) $(CCFLAGS) -shared -o libstr.so $(OFILES)
	@ $(AR) $(ARFLAGS) libstr-static.a $(OFILES)

install: libs
	@ sudo su -c '\
	  install -d $(INSTALL_PATH)/lib ;\
	  install -m 644 libstr* $(INSTALL_PATH)/lib ;\
	  install -d $(INSTALL_PATH)/include ;\
	  install -m 644 *.h $(INSTALL_PATH)/include ;\
	  ldconfig $(INSTALL_PATH)/lib '

uninstall:
	@ sudo su -c '\
	  rm -f $(INSTALL_PATH)/lib/libstr.so ;\
	  rm -f $(INSTALL_PATH)/lib/libstr.a ;\
	  rm -f $(INSTALL_PATH)/include/str.h ;\
	  rm -f $(INSTALL_PATH)/include/wstr.h ;\
	  ldconfig $(INSTALL_PATH)/lib '

.c.o:
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	rm -f *.o libstr*
