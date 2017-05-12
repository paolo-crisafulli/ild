/************************************************************************\
  TP compteur
    "Ecriture de drivers et programmation noyau Linux"
  Chapitre "Driver en mode caracteres"

  (c) 2005-2017 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/cdev.h>
	#include <linux/delay.h>
	#include <linux/device.h>
	#include <linux/fs.h>
	#include <linux/miscdevice.h>
	#include <linux/module.h>
	#include <linux/mutex.h>
	#include <linux/sched.h>

	#include <asm/uaccess.h>


	static ssize_t exemple_read  (struct file * filp, char * buffer,
	                              size_t length, loff_t * offset);
        static ssize_t exemple_write (struct file * filp, const char * buffer,
                                      size_t length, loff_t * offset);

	static struct file_operations fops_exemple = {
		.owner   =  THIS_MODULE,
		.read    =  exemple_read,
		.write   =  exemple_write,
	};

	static struct miscdevice exemple_misc_driver = {
		    .minor          = MISC_DYNAMIC_MINOR,
		    .name           = THIS_MODULE->name,
		    .fops           = & fops_exemple,
	};

	#define TAB_SIZE 16
	#define MSG_LEN  32

	static char messages[TAB_SIZE][MSG_LEN];

	static int head = 0;
	static int tail = 0;


static int __init exemple_init (void)
{
	printk(KERN_INFO "%s init\n", THIS_MODULE->name);
	return misc_register(& exemple_misc_driver);
}


static void __exit exemple_exit (void)
{
	misc_deregister(& exemple_misc_driver);
	printk(KERN_INFO "%s exit\n", THIS_MODULE->name);
}

static int count_msg(void) {
	int count;

	count = tail >= head ? tail - head : 32 - head + tail;

	printk(KERN_INFO "head=%d\n", head);
	printk(KERN_INFO "tail=%d\n", tail);
	printk(KERN_INFO "count=%d\n", count);

	return count;
}

static ssize_t exemple_read(struct file * filp, char * u_buffer,
                            size_t length, loff_t * offset)
{
        int msg_len;

        if (count_msg() == 0 || *offset != 0)
                return 0;

        msg_len = strlen(messages[head]);

        if (length < msg_len)
                return -EINVAL;

        if (copy_to_user(u_buffer, messages[head], msg_len) != 0)
                return -EFAULT;

	head = (head + 1) % TAB_SIZE;

	*offset += msg_len;

	return msg_len;
}

static ssize_t exemple_write(struct file * filp, const char * u_buffer,
                            size_t length, loff_t * offset)
{
        if (count_msg() == TAB_SIZE)
                return -ENOMEM;

        if (length >= MSG_LEN)
                return -EINVAL;

        if (copy_from_user(messages[tail], u_buffer, length) != 0)
                return -EFAULT;

	tail = (tail + 1) % TAB_SIZE;

	return length;
}

	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("TP périphérique \"file de messages\", p V-22");
	MODULE_AUTHOR("Paolo Crisafulli");
	MODULE_LICENSE("GPL");

