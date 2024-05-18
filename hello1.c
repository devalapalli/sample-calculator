#include <linux/module.h>   // Needed by all kernel modules
#include <linux/kernel.h>   // Needed for KERN_INFO
#include <linux/fs.h>       // Needed for file operations
#include <linux/device.h>   // Needed for device creation

#define DEVICE_NAME "hello_device"   // Device name

static int major_number;    // Device major number

// File operation functions
static int hello_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Hello, device opened\n");
    return 0;
}

static int hello_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Hello, device closed\n");
    return 0;
}

// Define file operations
static struct file_operations hello_fops = {
    .open = hello_open,
    .release = hello_release,
};

static int __init hello_init(void)
{
    // Register the character device
    major_number = register_chrdev(0, DEVICE_NAME, &hello_fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register hello device\n");
        return major_number;
    }
    printk(KERN_INFO "Hello, device registered with major number %d\n", major_number);
    return 0;
}

static void __exit hello_exit(void)
{
    // Unregister the character device
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Hello, device unregistered\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Hello, world! Device Driver");
