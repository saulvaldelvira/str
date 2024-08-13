.PHONY: default clean libs install uninstall doxygen

CC := cc
CCFLAGS = -Wall -Wextra -Werror -pedantic -g -O3 -fPIC

CFILES = str.c wstr.c
HFILES = str.h wstr.h
OFILES = $(patsubst %.c, %.o, $(CFILES))
LIBFILES = libstr.so libstr-static.a

AR = ar
ARFLAGS = rcs

INSTALL_PATH ?= /usr/local

default: libs

libs: $(LIBFILES)

$(LIBFILES): $(OFILES) $(HFILES)
	@ echo " => libstr.so"
	@ $(CC) $(CCFLAGS) -shared -o libstr.so $(OFILES)

	@ echo " => libstr-static.so"
	@ $(AR) $(ARFLAGS) libstr-static.a $(OFILES)


install: libs
	  install -d $(INSTALL_PATH)/lib
	  install -m 644 libstr* $(INSTALL_PATH)/lib
	  install -d $(INSTALL_PATH)/include
	  install -m 644 *.h $(INSTALL_PATH)/include
	  ldconfig $(INSTALL_PATH)/lib

uninstall:
	  rm -f $(INSTALL_PATH)/lib/libstr.so
	  rm -f $(INSTALL_PATH)/lib/libstr.a
	  rm -f $(INSTALL_PATH)/include/str.h
	  rm -f $(INSTALL_PATH)/include/wstr.h
	  ldconfig $(INSTALL_PATH)/lib

doxygen: ./doxygen/
	@ echo -e "\
	/** @mainpage \n \
	    @verbinclude README \n \
	*//**\
	   @file str.h  string_t definition. \n \
	   @file wstr.h  wstring_t definition. \n */" > ./doxygen/doc.doxy
	@ doxygen .doxyfile
	@ rm -f ./doxygen/doc.doxy

%/:
	@ mkdir $@

.c.o:
	@ echo " CC $@"
	@ $(CC) $(CCFLAGS) -c -o $@ $<

clean:
	@ rm -rf *.o doxygen  $(LIBFILES)
