#include <linux/init.h>
#include <linux/device.h>
#include <linux/netdevice.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mount.h>
#include <linux/sched.h>
#include <linux/path.h>
#include <linux/fdtable.h>
#include <linux/namei.h>
#include <linux/pagemap.h>
#include <linux/pci.h>
#include <linux/net.h>
#include <linux/mm_types.h>
#include <linux/sched/mm.h>
#include <linux/pid.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/sched/signal.h>
#include <linux/sched/cputime.h>
#include <linux/types.h>
#include "kmod.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KVN");
MODULE_DESCRIPTION("ioctl_driver");
MODULE_VERSION("1.0");

static struct class *cls;
char buffer[BUF_LEN] = {NULL};
int len_sock = 0;
int len = 0;


static int device_open(struct inode *inode, struct file *file) {
    pr_info("device_open(%p)\n", file);
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    pr_info("device_release(%p,%p)\n", inode, file);
    return 0;
}


static long device_ioctl(
        struct file *file,
        unsigned int cmd,
        unsigned long arg) {

    char path_arg[BUF_LEN];
    char __user *tmp = (char __user *) arg;

    struct path path;
    len = 0;

    switch (cmd) {
        case IOCTL_GET_VM_AREA_STRUCT:
            copy_from_user(path_arg, tmp, BUF_LEN);
            int pid_value;
            len_sock = 0;

             if(kstrtoint(path_arg, 10, &pid_value) != 0)
                 return -1;

            struct task_struct* ts = get_pid_task(find_get_pid(pid_value), PIDTYPE_PID);
            if(ts == NULL){
                copy_to_user(arg, "Pid is incorrect\n", 17);
                return 0;
            }
            struct mm_struct *mm = get_task_mm(ts);
            struct vm_area_struct *vma = mm->mmap;
	    if ( vma == NULL ) {
                printk("Cannot get vm_area_struct\n");
                copy_to_user(arg, "Cannot get vm_area_struct\n", 26);
                return 0;
            }
            memset(buffer, 0, BUF_LEN);
            if (vma){
        	len_sock += sprintf(buffer + len_sock, "The beginning of the memory area: %li\n", vma->vm_start);
        	len_sock += sprintf(buffer + len_sock, "The end of the memory area: %li\n", vma->vm_end);
        	len_sock += sprintf(buffer + len_sock, "Flags: %li\n", vma->vm_flags);
        	len_sock += sprintf(buffer + len_sock, "Offset (within vm_file): %li\n\n", vma->vm_pgoff);
        	if (len_sock>= 90000) return 0;
    	   }

            if(buffer[0] == NULL) len_sock = sprintf(buffer, "no vm\n");


            copy_to_user(arg, buffer, len_sock);

            break;

        case IOCTL_GET_TASK_CPUTIME: {

            copy_from_user(path_arg, tmp, BUF_LEN);
	    int pid_value;
	    len_sock = 0;

	    if(kstrtoint(path_arg, 10, &pid_value) != 0)
		return -1;

	    struct task_struct* ts = get_pid_task(find_get_pid(pid_value), PIDTYPE_PID);
	    if(ts == NULL){
		copy_to_user(arg, "Pid is incorrect\n", 17);
		return 0;
		}
	    struct thread_group_cputimer tgc = ts->signal->cputimer;
	    struct task_cputime_atomic tc = tgc.cputime_atomic;
	    if(&tgc == NULL){
		copy_to_user(arg, "Can't get thread_group_cputimer\n", 32);
		return 0;
		}

	memset(buffer, 0, BUF_LEN);

	len_sock += sprintf(buffer + len_sock, "Utime: %li\n", tc.utime);
	len_sock += sprintf(buffer + len_sock, "Stime: %li\n", tc.stime);
	len_sock += sprintf(buffer + len_sock, "Sum. Exec. Time: %li\n\n", tc.sum_exec_runtime);

	if(buffer[0] == NULL) len_sock = sprintf(buffer, "no vm\n");

	copy_to_user(arg, buffer, len_sock);
	break;
        }
        default:
            return -ENOTTY;
    }

    return 0;
}

static struct file_operations fops = {
        .open = device_open,
        .release = device_release,
        .unlocked_ioctl = device_ioctl
};

static int __init

m_init(void) {
    //buf = kmalloc(BUF_LEN, GFP_KERNEL);
    pr_info("INIT MODULE\n");

    int retval = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);

    /* Negative values signify an error */
    if (retval < 0) {
        pr_alert("%s failed with %d\n",
                 "Sorry, registering the character device ", retval);
        return retval;
    }

    cls = class_create(THIS_MODULE, DEVICE_FILE_NAME);
    device_create(cls, NULL, MKDEV(MAJOR_NUM, 0), NULL, DEVICE_FILE_NAME);

    pr_info("Device created on /dev/%s\n", DEVICE_FILE_NAME);

    return 0;
}

static void __exit

m_exit(void) {
    device_destroy(cls, MKDEV(MAJOR_NUM, 0));
    class_destroy(cls);

    /* Unregister the device */
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);

    pr_info("DEINIT MODULE\n");
}

module_init(m_init);
module_exit(m_exit);
