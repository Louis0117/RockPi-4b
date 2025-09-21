KERNELDIR = "/home/cadtc/rockchip-bsp/kernel"
#INSTALLDIR =
PWD := $(shell pwd)


obj-m := gpio_leds_DTS.o

USR_CC ?= aarch64-linux-gnu-gcc
USR_CFLAGS ?= -O2 -Wall
USR_LDFLAGS ?=


USER_BIN := lab1_led_ctl
USER_SRC := lab1_led_ctl.c

all: modules user

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

user: $(USER_BIN)

$(USER_BIN): $(USER_SRC)
	$(USR_CC) $(USR_CFLAGS) -o $@ $< $(USR_LDFLAGS)


install:
	scp *.ko $(INSTALLDIR)
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order  Module.symvers

.PHONY: modules install clean
