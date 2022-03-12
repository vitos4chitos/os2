#pragma once

#include <stdbool.h>

#define BUF_LEN 9000
#define MAJOR_NUM 100
#define DEVICE_NAME "char_dev"
#define DEVICE_FILE_NAME "char_dev"

#define IOCTL_GET_VM_AREA_STRUCT _IOWR(MAJOR_NUM, 0, int)
#define IOCTL_GET_TASK_CPUTIME _IOWR(MAJOR_NUM, 1, int)
