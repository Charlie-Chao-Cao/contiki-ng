CONTIKI_PROJECT = udp-client udp-server
all: $(CONTIKI_PROJECT)

#TARGET = cc2538dk
TARGET = zoul
#BOARD=firefly-reva
#BOARD = muntjac
RADIO = cc1120
DEFINES=ZOUL_CONF_USE_CC1120_RADIO=1
#DEFINES=CC1120_CONF_RF_CFG=1

MODULES += os/services/shell

CONTIKI=../..
include $(CONTIKI)/Makefile.include
