#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

int main() {
	int fd; // File descriptor for the video device
	struct v4l2_input input; // V4L2 input structure
	int index; // Index of the input

	// Open the video device
	fd = open("/dev/video0", O_RDWR);
	if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	// Get the index of the current input
	if (-1 == ioctl(fd, VIDIOC_G_INPUT, &index)) {
		perror("VIDIOC_G_INPUT");
		close(fd);
		exit(EXIT_FAILURE);
	}

	// Clear the input structure
	memset(&input, 0, sizeof(input));
	// Set the index obtained from VIDIOC_G_INPUT
	input.index = index;

	// Get information about the input
	if (-1 == ioctl(fd, VIDIOC_ENUMINPUT, &input)) {
		perror("VIDIOC_ENUMINPUT");
		close(fd);
		exit(EXIT_FAILURE);
	}

	// Print the name of the current input
	printf("Current input: %s\n", input.name);

	// Close the video device
	close(fd);

	return 0;
}

