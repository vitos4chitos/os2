#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>

#include "kmod.h"



static int print_help()
{
    printf("Usage: ./cmod [-s|-p]... PATH\n");
}

int main(int argc, char * argv[])
{	
    if (argc < 2 || argc > 4) {
        return print_help();
    }

    bool print_vmareastruct = false;
    bool print_taskcputime = false;
    char * path = NULL;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-v") == 0) {
            print_vmareastruct = true;
        }
        else if (strcmp(argv[i], "-t") == 0) {
            print_taskcputime = true;
        }
        else if (!path) {
            path = argv[i];
        }
        else {
            return print_help();
        }
    }

    int fd = open("/dev/" DEVICE_FILE_NAME, O_RDONLY);

    if (fd == -1) {
        printf("Cannot open /dev/" DEVICE_FILE_NAME " device, please check your permissions");
        return -1;
    }

    if (print_vmareastruct) {
        char buf[BUF_LEN];
        strcpy(buf, path);

        if (ioctl(fd, IOCTL_GET_VM_AREA_STRUCT, buf) != 0) {
            printf("Cannot get struct vm for path %s\n", path);
            return -1;
        }

        printf("%s", buf);
        printf("%s\n", path);
        printf("%s\n", fd);
    }

    if (print_taskcputime) {
        char buf[BUF_LEN];

        if (ioctl(fd, IOCTL_GET_TASK_CPUTIME, buf) != 0) {
            printf("Cannot get struct pci_dev for path %s\n", path);
            return -1;
        }

        printf("%s", buf);
    }

    return 0;
}
