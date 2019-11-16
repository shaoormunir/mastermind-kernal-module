/* YOUR FILE-HEADER COMMENT HERE */

/*
 * This file uses kernel-doc style comments, which is similar to
 * Javadoc and Doxygen-style comments.  See
 * ~/linux/Documentation/kernel-doc-nano-HOWTO.txt for details.
 */

/*
 * Getting compilation warnings?  The Linux kernel is written against
 * C89, which means:
 *  - No // comments, and 
 *  - All variables must be declared at the top of functions.
 * Read ~/linux/Documentation/CodingStyle to ensure your project
 * compiles without warnings.
 */

#define pr_fmt(fmt) "MM: " fmt

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>

#define NUM_PEGS 4
#define NUM_COLORS 6

/** true if user is in the middle of a game */
static bool game_active;

/** code that player is trying to guess */
static int target_code[NUM_PEGS];

/** tracks number of guesses user has made */
static unsigned num_guesses;

/** result of most recent user guess */
static char last_result[4];

/** buffer that records all of user's guesses and their results */
static char *user_view;

bool compare_strings(const char *source_string, size_t source_size, const char *dest_string, size_t dest_size)
{
	bool areEqual = true;
	size_t i;
	size_t j;
	for (i = 0, j = 0; i < source_size && j < dest_size; i++, j++)
	{
		if (source_string[i] != dest_string[j])
		{
			areEqual = false;
			break;
		}
	}
	return areEqual;
}

void initialize_game(void)
{
	size_t i;
	target_code[0] = 4;
	target_code[1] = 2;
	target_code[2] = 1;
	target_code[3] = 1;
	num_guesses = 0;
	for (i = 0; i < 4096; i++)
	{
		user_view[i] = 0;
	}

	game_active = true;
	last_result[0] = 'B';
	last_result[1] = '-';
	last_result[2] = 'W';
	last_result[3] = '-';
}

/**
 * mm_num_pegs() - calculate number of black pegs and number of white pegs
 * @target: target code, up to NUM_PEGS elements
 * @guess: user's guess, up to NUM_PEGS elements
 * @num_black: *OUT* parameter, to store calculated number of black pegs
 * @num_white: *OUT* parameter, to store calculated number of white pegs
 *
 * You do not need to modify this function.
 *
 */
static void
mm_num_pegs(int target[], int guess[], unsigned *num_black, unsigned *num_white)
{
	size_t i;
	size_t j;
	bool peg_black[NUM_PEGS];
	bool peg_used[NUM_PEGS];

	*num_black = 0;
	for (i = 0; i < NUM_PEGS; i++)
	{
		if (guess[i] == target[i])
		{
			(*num_black)++;
			peg_black[i] = true;
			peg_used[i] = true;
		}
		else
		{
			peg_black[i] = false;
			peg_used[i] = false;
		}
	}

	*num_white = 0;
	for (i = 0; i < NUM_PEGS; i++)
	{
		if (peg_black[i])
			continue;
		for (j = 0; j < NUM_PEGS; j++)
		{
			if (guess[i] == target[j] && !peg_used[j])
			{
				peg_used[j] = true;
				(*num_white)++;
				break;
			}
		}
	}
}

/**
 * mm_read() - callback invoked when a process reads from
 * /dev/mm
 * @filp: process's file object that is reading from this device (ignored)
 * @ubuf: destination buffer to store output
 * @count: number of bytes in @ubuf
 * @ppos: file offset (in/out parameter)
 *
 * Write to @ubuf the last result of the game, offset by
 * @ppos. Copy the lesser of @count and (string length of @last_result
 * - *@ppos). Then increment the value pointed to by @ppos by the
 * number of bytes copied. If @ppos is greater than or equal to the
 * length of @last_result, then copy nothing.
 *
 * If no game is active, instead copy to @ubuf up to four '?'
 * characters.
 *
 * Return: number of bytes written to @ubuf, or negative on error
 */
static ssize_t mm_read(struct file *filp, char __user *ubuf, size_t count,
					   loff_t *ppos)
{
	size_t bytes_to_copy = 4;
	if (bytes_to_copy > count && count > 0)
	{
		bytes_to_copy = count;
	}
	printk("%lld", (long long int)*ppos);
	printk("Bytes to be written are %d", bytes_to_copy);
	if (*ppos >= 4)
	{
		printk("The offset is greater than or equal to the bytes to be copied.");
		return -1;
	}
	else
	{
		int copy_result = 0;
		if (game_active)
		{
			printk("The game is active and the result is written.");
			copy_result = copy_to_user(ubuf + *ppos, last_result, bytes_to_copy);
		}
		else
		{
			printk("The game is not active and the default value is written.");
			copy_result = copy_to_user(ubuf + *ppos, "????", bytes_to_copy);
		}
		if (copy_result != 0)
		{
			return -1;
		}
		*ppos += bytes_to_copy;
		return bytes_to_copy;
	}
}

/**
 * mm_write() - callback invoked when a process writes to /dev/mm
 * @filp: process's file object that is reading from this device (ignored)
 * @ubuf: source buffer from user
 * @count: number of bytes in @ubuf
 * @ppos: file offset (ignored)
 *
 * If the user is not currently playing a game, then return -EINVAL.
 *
 * If @count is less than NUM_PEGS, then return -EINVAL. Otherwise,
 * interpret the first NUM_PEGS characters in @ubuf as the user's
 * guess. Calculate how many are in the correct value and position,
 * and how many are simply the correct value. Then update
 * @num_guesses, @last_result, and @user_view.
 *
 * <em>Caution: @ubuf is NOT a string; it is not necessarily
 * null-terminated.</em> You CANNOT use strcpy() or strlen() on it!
 *
 * Return: @count, or negative on error
 */
static ssize_t mm_write(struct file *filp, const char __user *ubuf,
						size_t count, loff_t *ppos)
{
	/* FIXME */
	return -EPERM;
}

/**
 * mm_mmap() - callback invoked when a process mmap()s to /dev/mm
 * @filp: process's file object that is mapping to this device (ignored)
 * @vma: virtual memory allocation object containing mmap() request
 *
 * Create a read-only mapping from kernel memory (specifically,
 * @user_view) into user space.
 *
 * Code based upon
 * <a href="http://bloggar.combitech.se/ldc/2015/01/21/mmap-memory-between-kernel-and-userspace/">http://bloggar.combitech.se/ldc/2015/01/21/mmap-memory-between-kernel-and-userspace/</a>
 *
 * You do not need to modify this function.
 *
 * Return: 0 on success, negative on error.
 */
static int mm_mmap(struct file *filp, struct vm_area_struct *vma)
{
	unsigned long size = (unsigned long)(vma->vm_end - vma->vm_start);
	unsigned long page = vmalloc_to_pfn(user_view);
	if (size > PAGE_SIZE)
		return -EIO;
	vma->vm_pgoff = 0;
	vma->vm_page_prot = PAGE_READONLY;
	if (remap_pfn_range(vma, vma->vm_start, page, size, vma->vm_page_prot))
		return -EAGAIN;

	return 0;
}

/**
 * mm_ctl_write() - callback invoked when a process writes to
 * /dev/mm_ctl
 * @filp: process's file object that is writing to this device (ignored)
 * @ubuf: source buffer from user
 * @count: number of bytes in @ubuf
 * @ppos: file offset (ignored)
 *
 * Copy the contents of @ubuf, up to the lesser of @count and 8 bytes,
 * to a temporary location. Then parse that character array as
 * following:
 *
 *  start - Start a new game. If a game was already in progress, restart it.
 *  quit  - Quit the current game. If no game was in progress, do nothing.
 *
 * If the input is neither of the above, then return -EINVAL.
 *
 * <em>Caution: @ubuf is NOT a string;</em> it is not necessarily
 * null-terminated, nor does it necessarily have a trailing
 * newline. You CANNOT use strcpy() or strlen() on it!
 *
 * Return: @count, or negative on error
 */
static ssize_t mm_ctl_write(struct file *filp, const char __user *ubuf,
							size_t count, loff_t *ppos)
{
	size_t i;
	printk("Writing the value to the requried place.\n");
	char temp_array[8] = {};
	size_t temp_length = 8;
	printk("the value of count is %d", count);
	if (count < 8)
	{
		temp_length = count;
	}
	printk("The value of temp_length is %ld", (long int)temp_length);
	for (i = 0; i < temp_length; i++)
	{
		temp_array[i] = ubuf[i];
	}

	if (compare_strings(temp_array, temp_length, "start", 5))
	{
		printk("Starting the game");
		initialize_game();
	}
	else if (compare_strings(temp_array, temp_length, "quit", 4))
	{
		printk("Quitting the game.");
		game_active = false;
	}

	return count;
}

/** strcut to handle call backs to dev/mm */
static const struct file_operations mm_operations = {
	.read = mm_read,
	.write = mm_write,
	.mmap = mm_mmap,
};

static struct miscdevice mastermind_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "mm",
	.fops = &mm_operations,
	.mode = 0666,
};

/** strcut to handle call backs to dev/mm */
static const struct file_operations mm_ctl_operations = {
	.write = mm_ctl_write,
};

static struct miscdevice mastermind_ctl_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "mm_ctl",
	.fops = &mm_ctl_operations,
	.mode = 0666,
};

/**
 * mastermind_init() - entry point into the Mastermind kernel module
 * Return: 0 on successful initialization, negative on error
 */
static int __init mastermind_init(void)
{
	int error;
	pr_info("Initializing the game.\n");
	user_view = vmalloc(PAGE_SIZE);
	if (!user_view)
	{
		pr_err("Could not allocate memory\n");
		return -ENOMEM;
	}

	/* YOUR CODE HERE */
	printk("Registering main device.");
	error = misc_register(&mastermind_device);
	if (error)
	{
		pr_err("can't misc_register :(\n");
		return error;
	}

	printk("Registering control device.");
	error = misc_register(&mastermind_ctl_device);
	if (error)
	{
		pr_err("can't misc_register :(\n");
		return error;
	}
	return 0;
}

/**
 * mastermind_exit() - called by kernel to clean up resources
 */
static void __exit mastermind_exit(void)
{
	pr_info("Freeing resources.\n");
	vfree(user_view);

	/* YOUR CODE HERE */
	misc_deregister(&mastermind_device);
	misc_deregister(&mastermind_ctl_device);
}

module_init(mastermind_init);
module_exit(mastermind_exit);

MODULE_DESCRIPTION("CS421 Mastermind Game");
MODULE_LICENSE("GPL");