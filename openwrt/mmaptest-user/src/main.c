#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

static int check_data(unsigned char *data, size_t page_size)
{
	unsigned i;

	printf("check memory ...");

	for (i = 0; i < page_size; i++) {
		if (data[i] != 0xff) {
			printf("FAIL (at byte %u)\n", i);
			return -1;
		}
	}
	printf("OK\n");
	return 0;
}

int main()
{
	void *addr;
	unsigned char *data;
	unsigned i;
	const size_t page_size = getpagesize();
	int fd = open("/dev/mmaptest", O_RDONLY);

	if (fd == -1) {
		perror("open failed");
		return -1;
	}

	addr = mmap(NULL, page_size, PROT_READ, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		perror("mmap failed");
		goto out;
	}
	printf("mmap addr: %p\n", addr);
	data = addr;

	for (i = 0; i < 10; i++) {
		if (!check_data(data, page_size))
			break;
		usleep(500000);
	}
	munmap(addr, page_size);
out:
	close(fd);
}
