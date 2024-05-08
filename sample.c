#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "arithmetic_device"
#define BUF_LEN 80

MODULE_LICENSE("GPL");

static int major_number;
static int device_open_count = 0;
static char msg[BUF_LEN];
static char *msg_ptr;

// Function prototypes
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static long perform_operation(int num1, int num2, char op) {
    long result = 0;
    switch (op) {
        case '+':
            result = num1 + num2;
            break;
        case '-':
            result = num1 - num2;
            break;
        case '*':
            result = num1 * num2;
            break;
        case '/':
            if (num2 != 0)
                result = num1 / num2;
            else
                printk(KERN_ALERT "Error: Division by zero\n");
            break;
        default:
            printk(KERN_ALERT "Error: Unsupported operation\n");
            break;
    }
    return result;
}

static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

static int __init arithmetic_device_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register a major number\n");
        return major_number;
    }

    printk(KERN_INFO "Registered correctly with major number %d\n", major_number);
    printk(KERN_INFO "To communicate with the device, please create a device file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, major_number);

    return 0;
}

static void __exit arithmetic_device_exit(void) {
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Unregistered the device\n");
}

static int device_open(struct inode *inode, struct file *file) {
    if (device_open_count)
        return -EBUSY;

    device_open_count++;
    try_module_get(THIS_MODULE);
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    device_open_count--;
    module_put(THIS_MODULE);
    return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset) {
    int bytes_read = 0;
    while (length && *msg_ptr) {
        put_user(*(msg_ptr++), buffer++);
        length--;
        bytes_read++;
    }
    return bytes_read;
}

static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t *off) {
    int num1, num2;
    char op;

    if (len < 7) {
        printk(KERN_ALERT "Insufficient data\n");
        return -EINVAL;
    }

    if (copy_from_user(msg, buff, len))
        return -EFAULT;

    sscanf(msg, "%d %c %d", &num1, &op, &num2);
    long result = perform_operation(num1, num2, op);
    sprintf(msg, "Result: %ld\n", result);

    msg_ptr = msg;
    return len;
}

module_init(arithmetic_device_init);
module_exit(arithmetic_device_exit);

