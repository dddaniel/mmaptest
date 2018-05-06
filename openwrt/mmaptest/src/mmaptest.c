#include <linux/mm.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>

#define DRIVER_MAJOR	24
#define DEV_NAME	"mmaptest"

static struct class *mmaptest_dev_class;
static dev_t dev = MKDEV(DRIVER_MAJOR, 0);

static struct mmaptest_priv_struct {
	//void *vm;
	unsigned long vm;
} mmaptest_priv;

static void mmaptest_mmap_open(struct vm_area_struct *area)
{
}

static void mmaptest_mmap_close(struct vm_area_struct *area)
{
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(4,15,0)
static int mmaptest_mmap_fault(struct vm_fault *vmf)
#else
static int mmaptest_mmap_fault(struct vm_area_struct *area, struct vm_fault *vmf)
#endif
{
	const unsigned long offset = vmf->pgoff << PAGE_SHIFT;
	struct page *page;
	unsigned long pa;

	if (offset >= PAGE_SIZE)
		return VM_FAULT_SIGBUS;

 	page = virt_to_page(mmaptest_priv.vm + offset);
	pa = page_to_phys(page);
	pr_info("mmap page %lx at va %lx\n", pa, mmaptest_priv.vm);
	get_page(page);
	vmf->page = page;
	return 0;
}

static const struct vm_operations_struct mmaptest_ops_fault = {
	.open =	mmaptest_mmap_open,
	.close = mmaptest_mmap_close,
	.fault = mmaptest_mmap_fault,
};

static int mmaptest_mmap(struct file *file, struct vm_area_struct *area)
{
	area->vm_ops = &mmaptest_ops_fault;
	area->vm_flags |= VM_DONTEXPAND | VM_DONTDUMP;
	mmaptest_mmap_open(area);
	return 0;
}

static int mmaptest_open(struct inode *inode, struct file *file)
{
	if (mmaptest_priv.vm == 0) {
#if 0
		long flags = GFP_KERNEL | __GFP_HIGHMEM | __GFP_ZERO;

		mmaptest_priv.vm = __vmalloc(PAGE_SIZE, flags, PAGE_KERNEL);
#endif
		mmaptest_priv.vm = get_zeroed_page(GFP_KERNEL);
		if (mmaptest_priv.vm == 0)
			return -ENOMEM;

		memset((void *)mmaptest_priv.vm, 0xff, PAGE_SIZE);
	}
	return 0;
}

static int mmaptest_release(struct inode *inode, struct file *file)
{
	if (mmaptest_priv.vm) {
		free_page(mmaptest_priv.vm);
		mmaptest_priv.vm = 0;
	}
	return 0;
}

static struct file_operations mmaptest_fops = {
	.owner = THIS_MODULE,
	.mmap = mmaptest_mmap,
	.open = mmaptest_open,
	.release = mmaptest_release
};

static int __init mmaptest_init(void)
{
	int rc;

	rc = register_chrdev(DRIVER_MAJOR, DEV_NAME, &mmaptest_fops);
	if (rc) {
		pr_info("unable to register chrdev driver %d\n", DRIVER_MAJOR);
		return rc;
	}

        mmaptest_dev_class = class_create(THIS_MODULE, DEV_NAME);
	if (IS_ERR(mmaptest_dev_class)) {
		unregister_chrdev(DRIVER_MAJOR, DEV_NAME);
		rc = PTR_ERR(mmaptest_dev_class);
		return rc;
	}

	device_create(mmaptest_dev_class, 0, dev, NULL, DEV_NAME);
	memset(&mmaptest_priv, 0, sizeof(mmaptest_priv));
	return rc;
}

static void __exit mmaptest_exit(void)
{
	unregister_chrdev(DRIVER_MAJOR, DEV_NAME);
	device_destroy(mmaptest_dev_class, dev);
	class_destroy(mmaptest_dev_class);
}

module_init(mmaptest_init);
module_exit(mmaptest_exit);
MODULE_LICENSE("GPL");
