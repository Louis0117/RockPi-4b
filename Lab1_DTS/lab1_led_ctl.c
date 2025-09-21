// 控制 lab1-led-DTS：on/off
// build:  gcc -O2 -Wall -o lab1_led_ctl lab1_led_ctl.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

// 與 kernel driver 保持一致
#define DEVICE_DEFAULT "/dev/lab1-led-DTS"
#define LAB1_IOC_MAGIC 'L'
#define LAB1_LED_ON  _IO(LAB1_IOC_MAGIC, 0x01)
#define LAB1_LED_OFF _IO(LAB1_IOC_MAGIC, 0x02)

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s on|off [device]\n"
        "  default device: %s\n"
        "Examples:\n"
        "  %s on\n"
        "  %s off /dev/lab1-led-DTS\n",
        prog, DEVICE_DEFAULT, prog, prog);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    const char *cmd = argv[1];
    const char *dev = (argc >= 3) ? argv[2] : DEVICE_DEFAULT;

    int fd = open(dev, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "open %s failed: %s\n", dev, strerror(errno));
        return 2;
    }

    unsigned long req;
    if (!strcmp(cmd, "on")) {
        req = LAB1_LED_ON;
    } else if (!strcmp(cmd, "off")) {
        req = LAB1_LED_OFF;
    } else {
        usage(argv[0]);
        close(fd);
        return 1;
    }

    if (ioctl(fd, req) < 0) {
        fprintf(stderr, "ioctl(%s) failed: %s\n", cmd, strerror(errno));
        close(fd);
        return 3;
    }

    printf("LED %s OK\n", cmd);
    close(fd);
    return 0;
}

