CONTIKI_PROJECT = node
all: $(CONTIKI_PROJECT)

TARGET = zoul
RADIO = cc1120
DEFINES=ZOUL_CONF_USE_CC1120_RADIO=1

MODULES += os/services/shell

CONTIKI=../../..

PLATFORMS_EXCLUDE = sky nrf52dk native

# force Orchestra from command line
MAKE_WITH_ORCHESTRA ?= 0
# force Security from command line
MAKE_WITH_SECURITY ?= 0

MAKE_MAC = MAKE_MAC_TSCH

include $(CONTIKI)/Makefile.dir-variables
MODULES += $(CONTIKI_NG_SERVICES_DIR)/shell

ifeq ($(MAKE_WITH_ORCHESTRA),1)
MODULES += $(CONTIKI_NG_SERVICES_DIR)/orchestra
endif

ifeq ($(MAKE_WITH_SECURITY),1)
CFLAGS += -DWITH_SECURITY=1
endif

include $(CONTIKI)/Makefile.include
