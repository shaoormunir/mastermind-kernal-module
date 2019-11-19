/** 
 * @Author:
 * This file contains the test cases for the mastermind game, implemented using a miscellanious device driver.
*/

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/user.h>
#include <stdbool.h>
#include <stdlib.h>

#define TEST_PART_1

#define TEST_PART_2

#define TEST_PART_3

#define TEST_PART_4

#define TEST_PART_5

static unsigned test_passed;
static unsigned test_failed;

static size_t USER_VIEW_LINE_SIZE = 22;

#define CHECK_IS_NOT_NULL(ptrA)             \
	do                                      \
	{                                       \
		if ((ptrA) != NULL)                 \
		{                                   \
			test_passed++;                  \
			printf("%d: PASS\n", __LINE__); \
		}                                   \
		else                                \
		{                                   \
			test_failed++;                  \
			printf("%d: FAIL\n", __LINE__); \
		}                                   \
		fflush(stdout);                     \
	} while (0);

#define CHECK_IS_EQUAL(valA, valB)          \
	do                                      \
	{                                       \
		if ((valA) == (valB))               \
		{                                   \
			test_passed++;                  \
			printf("%d: PASS\n", __LINE__); \
		}                                   \
		else                                \
		{                                   \
			test_failed++;                  \
			printf("%d: FAIL\n", __LINE__); \
		}                                   \
		fflush(stdout);                     \
	} while (0);

#define CHECK_IS_STRING_EQUAL(valA, valB, size) \
	do                                          \
	{                                           \
		int i = 0;                              \
		do                                      \
		{                                       \
			if ((valA[i]) != (valB[i]))         \
			{                                   \
				test_failed++;                  \
				printf("%d: FAIL\n", __LINE__); \
				break;                          \
			}                                   \
			i++;                                \
			fflush(stdout);                     \
		} while (i < size);                     \
		if (i == size)                          \
		{                                       \
			test_passed++;                      \
			printf("%d: PASS\n", __LINE__);     \
		}                                       \
	} while (0);

/**
 * open_mapping() - opens a mapping to the given device name in either read or write mode, depending
 * on the value of the read boolean
 * @device_name: name of the device to open the mapping to
 * @read: bool to specify if the mapping is to be opened in read or write mode
 * */
void *open_mapping(const char *device_name, bool read)
{
	int fd;
	if (read)
	{
		fd = open(device_name, O_RDONLY, 0);
		return mmap(NULL, PAGE_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);
	}
	else
	{
		fd = open(device_name, O_WRONLY);
		return mmap(NULL, PAGE_SIZE, PROT_WRITE, MAP_PRIVATE, fd, 0);
	}
	close(fd);
}
/**
 * close_mapping() - closes the mapping created to some file or device
 * @buffer: pointer to the mapping
 * */
void close_mapping(void *buffer)
{
	munmap(buffer, PAGE_SIZE);
}
/**
 * read_from_device() - reads data from the specified device into the destination array
 * @device_name: name of the device to read data from
 * @destination: destination array to store data into
 * @bytes_to_read: number of bytes to read from the device
 * */
ssize_t read_from_device(const char *device_name, void *destination, size_t bytes_to_read)
{
	int fd = open(device_name, O_RDONLY, 0);
	ssize_t result = read(fd, destination, bytes_to_read);
	close(fd);
	return result;
}
/**
 * write_to_device() - writes data to the specified device specified in the data array
 * @device_name: name of the device to write data to
 * @destination: array containing the data to be written
 * @bytes_to_read: number of bytes to write
 * */
ssize_t write_to_device(const char *device_name, void *data, size_t bytes_to_write)
{
	int fd = open(device_name, O_WRONLY, 0);
	ssize_t result = write(fd, data, bytes_to_write);
	close(fd);
	return result;
}
/**
 * print_user_view() - prints the user view to console
 * @user_view: pointer pointing towards the user data
 * */
void print_user_view(char *user_view)
{
	for (size_t i = 0; i < PAGE_SIZE / USER_VIEW_LINE_SIZE; i++)
	{
		printf("%s", user_view + (loff_t)(i * USER_VIEW_LINE_SIZE));
	}
}
/**
 * report_test_results() - prints the current status of tests passed and failed
 * */
void report_test_results(void)
{
	printf("%u tests passed, %u tests failed.\n", test_passed, test_failed);
}

int main(void)
{
	/** Test part 1 checks the output of the game if the game is not active */
#ifdef TEST_PART_1
	printf("Checking the output before starting game\n");
	char *last_result = malloc(4);
	read_from_device("/dev/mm", last_result, 4);
	CHECK_IS_STRING_EQUAL(last_result, "????", 4);
#endif
	/** test part 2 starts the game and checks the default output */
#ifdef TEST_PART_2
	printf("Starting a new game and checking the default output\n");
	write_to_device("/dev/mm_ctl", "start", 5);
	read_from_device("/dev/mm", last_result, 4);
	CHECK_IS_STRING_EQUAL(last_result, "B-W-", 4);
#endif
	/** part 3 checks the gameplay with different inputs and also prints the user view to console */
#ifdef TEST_PART_3
	printf("Checking gameplay with different inputs\n");
	write_to_device("/dev/mm", "4221", 4);
	read_from_device("/dev/mm", last_result, 4);
	CHECK_IS_STRING_EQUAL(last_result, "B3W0", 4);
	write_to_device("/dev/mm", "4211", 4);
	read_from_device("/dev/mm", last_result, 4);
	CHECK_IS_STRING_EQUAL(last_result, "B4W0", 4);
	write_to_device("/dev/mm", "1142", 4);
	read_from_device("/dev/mm", last_result, 4);
	CHECK_IS_STRING_EQUAL(last_result, "B0W4", 4);
	printf("Printing user view\n");
	char *user_view = (char *)open_mapping("/dev/mm", true);
	print_user_view(user_view);
	close_mapping((void *)user_view);
#endif
	/** part 4 checks some boundry condition by first trying to write less than 4 character to device and then trying to read more than 4 characters from the device */
#ifdef TEST_PART_4
	printf("Trying to write less than 4 characters to device\n");
	ssize_t result = write_to_device("/dev/mm", "14", 2);
	CHECK_IS_EQUAL(result, -1);
	printf("Trying to read more than 4 characters from device\n");
	result = read_from_device("/dev/mm", last_result, 54);
	CHECK_IS_EQUAL(result, 4);
#endif
	/** part 5 quits the game and checks the output for the inactive game */
#ifdef TEST_PART_5
	printf("Closing the device and checking output\n");
	write_to_device("/dev/mm_ctl", "quit", 4);
	read_from_device("/dev/mm", last_result, 4);
	CHECK_IS_STRING_EQUAL(last_result, "????", 4);
#endif
	report_test_results();
	return 0;
}
