/*
* taskdriver.c -- a taskdriver module
*
*/
 
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
 
#include <linux/kernel.h>       /* printk() */
#include <linux/slab.h>         /* kmalloc() */
#include <linux/fs.h>           /* everything... */
#include <linux/errno.h>        /* error codes */
#include <linux/types.h>        /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>        /* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <linux/semaphore.h>

//#include <asm/system.h>         /* cli(), *_flags */
#include <asm/uaccess.h>        /* copy_*_user */
 
 
/*
* Our parameters which can be set at load time.
*/

 
int taskdriver_major =   0;
int taskdriver_minor =   0;
int memsize	= 255;

module_param(taskdriver_major, int, S_IRUGO);
module_param(taskdriver_minor, int, S_IRUGO);
module_param(memsize, int, S_IRUGO);

MODULE_AUTHOR("Mahmoud Elasmar-forked from -Antonio Sgorbissa");
MODULE_LICENSE("Dual BSD/GPL");


struct taskdriver_dev {
         char *data;  /* Pointer to data area */
	 int memsize;
         struct semaphore sem;     /* mutual exclusion semaphore    */
         struct cdev cdev;         /* structure for char devices */
};
 
struct taskdriver_dev taskdriver_device; 



int taskdriver_open(struct inode *inode, struct file *filp)
{
         struct taskdriver_dev *dev; 	/* a pointer to a taskdriver_dev structire */
 
         dev = container_of(inode->i_cdev, struct taskdriver_dev, cdev);
         filp->private_data = dev; /* stored here to be re-used in other system call*/
 
         return 0;           
}


int taskdriver_release(struct inode *inode, struct file *filp)
{
         return 0;
}


ssize_t taskdriver_read(struct file *filp, char __user *buf, size_t count,
                 loff_t *f_pos)
{
         struct taskdriver_dev *dev = filp->private_data; 

         ssize_t retval = 0;

         if (down_interruptible(&dev->sem))
                 return -ERESTARTSYS;
         if (count >= dev->memsize) 
                 goto out;
	    

         if (copy_to_user(buf, dev->data, count)) {
                 retval = -EFAULT;
                 goto out;
         }
         retval = count;

	 out:
	        up(&dev->sem);
		return retval;
}
 


ssize_t taskdriver_write(struct file *filp, const char __user *buf, size_t count,
                     loff_t *f_pos)
{
    struct taskdriver_dev *dev = filp->private_data;
    ssize_t retval = 0; /* return value */

    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    if (count >= dev->memsize)
        count = dev->memsize;

    /* Clear the data buffer */
    memset(dev->data, 0, dev->memsize);

    /* Copy data from user space */
    if (copy_from_user(dev->data, buf, count)) {
        retval = -EFAULT;
        goto out;
    }

    /* Log the written data into the kernel log */
    printk(KERN_INFO "%s", dev->data);

    retval = count;

out:
    up(&dev->sem);
    return retval;
}


struct file_operations taskdriver_fops = {
         .owner =    THIS_MODULE,
         .read =     taskdriver_read,
         .write =    taskdriver_write,
         .open =     taskdriver_open,
         .release =  taskdriver_release,
};

void taskdriver_cleanup_module(void)
{
         dev_t devno = MKDEV(taskdriver_major, taskdriver_minor);
 
         /* Free the cdev entries  */
         cdev_del(&taskdriver_device.cdev);

	 /* Free the memory */
         kfree(taskdriver_device.data);

	 unregister_chrdev_region(devno, 1);
}

int taskdriver_init_module(void)
{
         int result, err;
         dev_t dev = 0;
 

	 if (taskdriver_major) {   //the major number is given as a parameter
	            dev = MKDEV(taskdriver_major, taskdriver_minor);
	            result = register_chrdev_region(dev, 1, "taskdriver");
	 } 
	 else {		// otherwise
                 result = alloc_chrdev_region(&dev, taskdriver_minor, 1, "taskdriver");
                 taskdriver_major = MAJOR(dev);
	 }
	if (result < 0) {
                 printk(KERN_WARNING "taskdriver: can't get major %d\n", taskdriver_major);
                 return result;
        }

	/* Prepare the memory area */
	taskdriver_device.memsize = memsize;
	taskdriver_device.data = kmalloc(memsize * sizeof(char), GFP_KERNEL);
        memset(taskdriver_device.data, 0, memsize * sizeof(char));

        /* Initialize the semaphore */
        sema_init(&taskdriver_device.sem,1);

	/* Initialize cdev */
        cdev_init(&taskdriver_device.cdev, &taskdriver_fops);
        taskdriver_device.cdev.owner = THIS_MODULE;
        taskdriver_device.cdev.ops = &taskdriver_fops;
        err = cdev_add (&taskdriver_device.cdev, dev, 1);

        if (err)  printk(KERN_NOTICE "Error %d adding taskdriver", err);
	else
        	printk(KERN_NOTICE "taskdriver Added major: %d minor: %d", taskdriver_major, taskdriver_minor);
 
        return 0; 

}

module_init(taskdriver_init_module);
module_exit(taskdriver_cleanup_module);
