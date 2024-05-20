#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#define DEVICE_NAME "hello_world"
#define BUFFER_SIZE 1024

static char message[BUFFER_SIZE] = "Hello, world!\n";
static int message_len = sizeof(message);
static int major_number;
static struct class *hello_class = NULL;
static struct device *hello_device = NULL;

static int device_open(struct inode *inode, struct file *file) {
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t device_read(struct file *file, char __user *buffer, size_t length, loff_t *offset) {
    printk(KERN_INFO "device_read: i got call from user space\n");      
 int bytes_read = 0;
    if (*offset >= message_len)
        return 0;
    while (length && message_len) {
        put_user(message[*offset], buffer++);
        *offset += 1;
        length--;
        bytes_read++;
    }
    return bytes_read;
}

static ssize_t device_write(struct file *file, const char __user *buffer, size_t length, loff_t *offset) {
    printk(KERN_INFO "write: i got call from user space\n");   
 int i;
    for (i = 0; i < length && i < BUFFER_SIZE; ++i) {
        get_user(message[i], buffer + i);
    }
    message_len = i;
    return i;
}

static struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
};

static int __init hello_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Hello World failed to register a major number\n");
        return major_number;
    }
    printk(KERN_INFO "Hello World: registered correctly with major number %d\n", major_number);

    // Create device class
    hello_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(hello_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create device class\n");
        return PTR_ERR(hello_class);
    }

    // Create device
    hello_device = device_create(hello_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(hello_device)) {
        class_destroy(hello_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create device\n");
        return PTR_ERR(hello_device);
    }

    return 0;
}

static void __exit hello_exit(void) {
    device_destroy(hello_class, MKDEV(major_number, 0));
    class_unregister(hello_class);
    class_destroy(hello_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Goodbye, world!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple Hello World module");

