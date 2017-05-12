/************************************************************************\
  Exercice pII-16

  (c) 2005-2017 Christophe Blaess
  http://www.blaess.fr/christophe/

\************************************************************************/

	#include <linux/module.h>
	#include <linux/proc_fs.h>
	#include <linux/sched.h>
	#include <linux/version.h>

	#include <asm/uaccess.h>


	static ssize_t exemple_read  (struct file *, char __user *, size_t, loff_t *);

	static const struct file_operations exemple_proc_fops = {
		.read   = exemple_read,
	};


static int __init exemple_init (void)
{
	struct proc_dir_entry *dir;
	if ((dir = proc_mkdir("current", NULL)) == NULL)
		return -EBUSY;
	if (proc_create(THIS_MODULE->name, S_IFREG | 0644, dir, & exemple_proc_fops) == NULL)
		return -EBUSY;

	return 0;
}


static void __exit exemple_exit (void)
{
	remove_proc_entry("current", NULL);
}


static ssize_t exemple_read(struct file * filp, char __user * u_buffer, size_t max, loff_t * offset)
{
	char buffer[128];
	int  nb;

	snprintf(buffer, 128, "%u\n", current->pid);

	nb = strlen(buffer) - (*offset);
	if (nb <= 0)
		return 0;
	if (nb > max)
		nb = max;
	if (copy_to_user(u_buffer, & (buffer[*offset]), nb) != 0)
		return -EFAULT;
	(*offset) += nb;

	return nb;
}

	module_init(exemple_init);
	module_exit(exemple_exit);

	MODULE_DESCRIPTION("/proc write callback.");
	MODULE_AUTHOR("Christophe Blaess <Christophe.Blaess@Logilin.fr>");
	MODULE_LICENSE("GPL");

