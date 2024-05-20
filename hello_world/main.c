#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_PATH "/dev/hello_world"
#define BUFFER_SIZE 1024

int main() {
    int fd;
    char read_buffer[BUFFER_SIZE];
    char write_buffer[] = "Hello from user space!\n";

    // Open the device file
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device file");
        return EXIT_FAILURE;
    }

    // Write to the device
    if (write(fd, write_buffer, sizeof(write_buffer)) < 0) {
        perror("Failed to write to the device");
        close(fd);
        return EXIT_FAILURE;
    }

    // Read from the device
    if (read(fd, read_buffer, BUFFER_SIZE) < 0) {
        perror("Failed to read from the device");
        close(fd);
        return EXIT_FAILURE;
    }

    // Print the read message
    printf("Message from kernel: %s", read_buffer);

    // Close the device file
    close(fd);

    return EXIT_SUCCESS;
}

