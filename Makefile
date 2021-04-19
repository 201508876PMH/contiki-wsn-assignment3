CONTIKI_PROJECT = main
all: $(CONTIKI_PROJECT)

CONTIKI = ../../contiki-ng
MAKE_NET = MAKE_NET_NULLNET
TARGET_LIBFILES += -lm
include $(CONTIKI)/Makefile.include
