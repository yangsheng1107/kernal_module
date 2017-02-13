#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>  /* Needed for the copy_to_user */
#include <linux/slab.h>     /* Needed for the kzalloc */
//http://blog.csdn.net/waldmer/article/details/17676001
MODULE_LICENSE("Dual BSD/GPL") ;
MODULE_AUTHOR("yangsheng");
MODULE_DESCRIPTION("This is simple module example");
MODULE_VERSION("1.0");

#define MODULE_NAME "demo"
#define CLASS_NAME "demo_class"
// /dev/demoX - /dev/demoY
#define DEV_NAME "demo%d"

#define MAX_DEV_NUM 2

static int demo_major = 0;
static int demo_minor = 0;

struct cdd_cdev{
    struct cdev cdev;
    struct device *dev_device;
    char kbuf[100];
};

struct cdd_cdev *cdd_cdevp = NULL;
struct class *dev_class = NULL;

static int demo_open(struct inode *inode, struct file *filp) {
    struct cdd_cdev *pcdevp = NULL;
    printk(KERN_ALERT "%s : (major,minor)=(%d,%d)\n",__func__, imajor(inode), iminor(inode));
    pcdevp = container_of(inode->i_cdev, struct cdd_cdev, cdev);

    filp->private_data = pcdevp;
    return 0;
}

static int demo_close(struct inode *inode, struct file *filp) {
    printk(KERN_ALERT "%s : (major,minor)=(%d,%d)\n",__func__, imajor(inode), iminor(inode));
    return 0;
}

static ssize_t demo_read(struct file *filp, char *buf, size_t size, loff_t *f_pos) {
    struct cdd_cdev *cdevp = filp->private_data;

    if (size < strlen(cdevp->kbuf)) return -EINVAL;
    if (copy_to_user(buf, cdevp->kbuf, strlen(cdevp->kbuf))) return -EFAULT;
    *f_pos += strlen(cdevp->kbuf);
    printk(KERN_ALERT "%s : (size=%zu)\n",__func__, size);
    return 0;
}

static ssize_t demo_write(struct file *filp, const char *buf, size_t size, loff_t *f_pos) {
    size_t ssize;
    struct cdd_cdev *cdevp = filp->private_data;

    ssize = (size > sizeof(cdevp->kbuf)) ? sizeof(cdevp->kbuf) : size ;
    if (copy_from_user(cdevp->kbuf,buf,ssize)) return -EFAULT;
    *f_pos += ssize;
    printk(KERN_ALERT "%s : (size=%zu)\n",__func__, ssize);
    return ssize;
}

static struct file_operations demo_fops = {
    .open = demo_open,
    .release = demo_close,
    .read = demo_read,
    .write = demo_write,
};


static int __init demo_init(void)
{
    int ret = 0, i =0;
    dev_t dev = MKDEV(demo_major, demo_minor);

    ret = alloc_chrdev_region(&dev, demo_minor, MAX_DEV_NUM, MODULE_NAME);
    if (ret < 0) {
        printk(KERN_ALERT "simple: unable to get major %d\n", demo_major);
        return -1;
    }
    demo_major = MAJOR(dev);

    cdd_cdevp = kzalloc(sizeof(struct cdd_cdev)*MAX_DEV_NUM, GFP_KERNEL);
    if(cdd_cdevp == NULL)
    {
        printk(KERN_ALERT "kzalloc failed!\n");
        unregister_chrdev_region(MKDEV(demo_major, demo_minor), MAX_DEV_NUM);
        return -1;
    }

    dev_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(dev_class)){
        printk(KERN_ALERT "class_create failed!\n");
        kfree(cdd_cdevp);
        unregister_chrdev_region(MKDEV(demo_major, demo_minor), MAX_DEV_NUM);
        return PTR_ERR(dev_class);
    }

    for (i=0;i<MAX_DEV_NUM;i++)
    {
        cdev_init(&(cdd_cdevp[i].cdev), &demo_fops);

        cdev_add(&(cdd_cdevp[i].cdev), MKDEV(demo_major, i), 1);

        // add/dev/xxx
        cdd_cdevp[i].dev_device = device_create(dev_class, NULL, MKDEV(demo_major, i), NULL, DEV_NAME, i);
        if (IS_ERR(cdd_cdevp[i].dev_device)){
            printk(KERN_ALERT "Failed to create the device\n");
            return PTR_ERR(cdd_cdevp[i].dev_device);
        }
    }

    return 0;
}

static void __exit demo_exit(void)
{
    int i=0;

    for (i=0;i<MAX_DEV_NUM;i++)
    {
        device_destroy(dev_class, MKDEV(demo_major, i));
        cdev_del(&(cdd_cdevp[i].cdev));
    }
    // remove form /dev/xxx
    class_unregister(dev_class);
    class_destroy(dev_class);

    kfree(cdd_cdevp);
    unregister_chrdev_region(MKDEV(demo_major, demo_minor), MAX_DEV_NUM);

}

module_init(demo_init);
module_exit(demo_exit);
