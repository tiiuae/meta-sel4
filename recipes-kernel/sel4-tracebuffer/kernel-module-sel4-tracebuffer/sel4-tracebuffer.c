/*
 * sel4_tracebuffer.c - Description description description
 */
#include <linux/of.h>
#include <linux/init.h>			/* Needed for the macros */
#include <linux/err.h>
#include <linux/kernel.h>		/* Needed for pr_info()  */
#include <linux/module.h>		/* Needed by all modules */
#include <linux/debugfs.h>
#include <linux/platform_device.h>	/* Needed by platform_driver_register 
					   and platform_driver_unregister */
#include <linux/vmalloc.h>
#include <asm/page.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/memblock.h>
#include <linux/types.h>
#include <linux/seq_file.h>

#include "sel4-support.h"

struct sel4_tracebuffer_platform_data {
	unsigned long   mem_size;
	phys_addr_t     mem_address;
	void		*vaddr;
	size_t		entries;
	int		enabled;

	struct mutex mutex;
};

struct sel4_tracebuffer_platform_data pdata_local = {0};

static void *sel4_tracebuffer_seq_start(struct seq_file *s, loff_t *pos)
{
	loff_t *spos = kmalloc(sizeof(loff_t), GFP_KERNEL);

	if (!spos || *pos >= pdata_local.entries)
		return NULL;

	*spos = *pos;
	return spos;
}

static void *sel4_tracebuffer_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	loff_t *spos = (loff_t *) v;

	*pos = ++(*spos);
	if (!spos || *pos >= pdata_local.entries)
		return NULL;

	return spos;
}

static void sel4_tracebuffer_seq_stop(struct seq_file *s, void *v)
{
	kfree (v);
}

static int sel4_tracebuffer_seq_show(struct seq_file *s, void *v)
{
	loff_t *spos = (loff_t *) v;
	benchmark_track_kernel_entry_t entry;
	void *vaddr_offset = pdata_local.vaddr + (*spos * sizeof(benchmark_track_kernel_entry_t));

	if (!spos)
		return 0;

	memcpy(&entry, vaddr_offset, sizeof(benchmark_track_kernel_entry_t));

	seq_printf(s, "%25s -- %12lld -- %12lld -- 0x%10lx\n", 
			entry.entry.path < ARRAY_SIZE(entry_names) ? 
				entry_names[entry.entry.path] : "Wrong_Entry_Type",
			entry.start_time, entry.duration, (uintptr_t)entry.entry.next);
	return 0;
}

static int sel4_trace_data_seq_show(struct seq_file *s, void *v)
{
	loff_t *spos = (loff_t *) v;
	benchmark_track_kernel_entry_t entry;
	void *vaddr_offset = pdata_local.vaddr + (*spos * sizeof(benchmark_track_kernel_entry_t));

	if (!spos)
		return 0;

	memcpy(&entry, vaddr_offset, sizeof(benchmark_track_kernel_entry_t));

	return seq_write(s, &entry, sizeof(entry));
}

static struct seq_operations sel4_tracebuffer_seq_ops = {
	.start = sel4_tracebuffer_seq_start,
	.next  = sel4_tracebuffer_seq_next,
	.stop  = sel4_tracebuffer_seq_stop,
	.show  = sel4_tracebuffer_seq_show
};

static struct seq_operations sel4_trace_data_seq_ops = {
	.start = sel4_tracebuffer_seq_start,
	.next  = sel4_tracebuffer_seq_next,
	.stop  = sel4_tracebuffer_seq_stop,
	.show  = sel4_trace_data_seq_show
};

static int sel4_tracebuffer_seq_open(struct inode *inode, struct file *file)
{
	struct seq_operations *seq_ops = inode->i_private;

	if (!seq_ops) {
		pr_err("No seq_operations were provided\n");
		return -EIO;
	}

	if (!mutex_trylock(&pdata_local.mutex)) {
		pr_err("device busy\n");
		return -EBUSY;
	}

	return seq_open(file, seq_ops);
}

static int sel4_tracebuffer_seq_release(struct inode *inode, struct file *file)
{

	mutex_unlock(&pdata_local.mutex);

	return seq_release(inode, file);
}

#if 0
int sel4_mmap(struct file *filp, struct vm_area_struct *vma)
{
	int ret;
	const void *const vaddr  = pdata_local.vaddr;
	const unsigned long size = pdata_local.mem_size;
	unsigned long len        = vma->vm_end - vma->vm_start;

	unsigned long pfn = virt_to_phys((void *)pdata_local.mem) >> PAGE_SHIFT;

	printk("%s: trying mmap: 0x%lx@0x%llx", __func__, len, vma->vm_start);

	if (len > size) {
		printk("Requested size is too big: 0x%x > 0x%x\n", len, size);
		return -EIO;
	}


//	ret = remap_pfn_range(vma, vma->vm_start, virt_to_phys(vaddr) >> PAGE_SHIFT, len, vma->vm_page_prot);
	ret = remap_pfn_range(vma, vma->vm_start, pfn, len, vma->vm_page_prot);
	if (ret < 0) {
		printk("could not map the address area\n");
		return -EIO;
	}

	return 0;
}
#endif

static int sel4_open(struct inode *inode, struct file *file)
{
	if(!mutex_trylock(&pdata_local.mutex)) {
		pr_err("device busy\n");
		return -EBUSY;
	}

	return 0;
}

static int sel4_release(struct inode *inode, struct file *file)
{
	mutex_unlock(&pdata_local.mutex);

	return 0;
}

static ssize_t sel4_trace_on_read(struct file *file, char __user *buffer, 
				  size_t len, loff_t *offset)
{
	char buff[3];

	snprintf(buff, sizeof(buff), "%d\n", pdata_local.enabled);

	return simple_read_from_buffer(buffer, len, offset, buff, sizeof(buff));
}

static ssize_t sel4_trace_on_write(struct file *file, const char __user *buffer,
				   size_t len, loff_t *offset)
{
	char fun[10];
	ssize_t ret;

	ret = sscanf(buffer, "%10s", fun);
	if (ret != 1)
		return -EINVAL;

	if (!strcmp("start", fun) || !strcmp("enable", fun) || !strcmp("1", fun)) {
		printk("%s: start sel4 tracing...\n", __func__);
		seL4_BenchmarkResetLog();
		pdata_local.entries = 0;
		pdata_local.enabled = 1;
	} else if (!strcmp("stop", fun) || !strcmp("disable", fun) || !strcmp("0", fun)) {
		printk("%s: stop sel4 tracing...\n", __func__);
		pdata_local.entries = seL4_BenchmarkFinalizeLog();
		pdata_local.enabled = 0;
	} else {
		pr_err("Supported commands are: start, stop, enable, disable, 0, 1\n");
	}

	return len;
}

#if 0
/*  mmap handler to map kernel space to user space  
 *
 */
static int mchar_mmap(struct file *filp, struct vm_area_struct *vma)
{
    int ret = 0;
    struct page *page = NULL;
    unsigned long size = (unsigned long)(vma->vm_end - vma->vm_start);

    printk("%s: \n", __func__);
    printk("%s: \n", __func__);
    printk("%s: \n", __func__);
    printk("%s: \n", __func__);

    if (size > MAX_SIZE) {
	    printk("Size too big: %ld > %ld\n", size, MAX_SIZE);
        ret = -EINVAL;
        goto out;  
    } 
   
    page = virt_to_page((unsigned long)sh_mem + (vma->vm_pgoff << PAGE_SHIFT)); 
    ret = remap_pfn_range(vma, vma->vm_start, page_to_pfn(page), size, vma->vm_page_prot);
    if (ret != 0) {
	    printk("remap failed: %d\n", ret);
        goto out;
    }   

out:
    return ret;
}
#endif

static const struct file_operations trace_fops = {
	.owner   = THIS_MODULE,
        .open    = sel4_tracebuffer_seq_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = sel4_tracebuffer_seq_release,
};

static const struct file_operations trace_data_fops = {
	.owner   = THIS_MODULE,
        .open    = sel4_tracebuffer_seq_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
//	.mmap    = sel4_mmap,
	.release = sel4_tracebuffer_seq_release,
};

static const struct file_operations trace_on_fops = {
	.owner   = THIS_MODULE,
        .open    = sel4_open,
	.read    = sel4_trace_on_read,
        .write   = sel4_trace_on_write,
	.release = sel4_release,
};

static struct dentry *sel4_tracebuffer_dir;

void sel4_tracebuffer_register_sysfs(void)
{
	printk("%s: debug stufff\n", __func__);

        sel4_tracebuffer_dir = debugfs_create_dir("sel4_tracebuffer", NULL);

	/* 
	 * trace -- txt data
	 */
        debugfs_create_file("trace", 0444, sel4_tracebuffer_dir,
			    &sel4_tracebuffer_seq_ops, &trace_fops);
	/* 
	 * tracedata -- binary data
	 */
	debugfs_create_file("tracedata", 0444, sel4_tracebuffer_dir,
			    &sel4_trace_data_seq_ops, &trace_data_fops);

	/*
	 * trace_on -- trace on (1) / trace off (0)
	 */
	debugfs_create_file("trace_on", 0644, sel4_tracebuffer_dir,
			    NULL, &trace_on_fops);

	mutex_init(&pdata_local.mutex);
}

void sel4_tracebuffer_unregister_sysfs(void)
{
	debugfs_remove_recursive(sel4_tracebuffer_dir);
}

static void *sel4_tracebuffer_vmap(phys_addr_t start, size_t size,
		unsigned int memtype)
{
	struct page **pages;
	phys_addr_t page_start;
	unsigned int page_count;
	pgprot_t prot;
	unsigned int i;
	void *vaddr;

	page_start = start - offset_in_page(start);
	page_count = DIV_ROUND_UP(size + offset_in_page(start), PAGE_SIZE);

	if (memtype)
		prot = pgprot_noncached(PAGE_KERNEL);
	else
		prot = pgprot_writecombine(PAGE_KERNEL);

	pages = kmalloc_array(page_count, sizeof(struct page *), GFP_KERNEL);
	if (!pages) {
		printk("%s: Failed to allocate array for %u pages\n",
		       __func__, page_count);
		return NULL;
	}

	for (i = 0; i < page_count; i++) {
		phys_addr_t addr = page_start + i * PAGE_SIZE;
		pages[i] = pfn_to_page(addr >> PAGE_SHIFT);
	}
	vaddr = vmap(pages, page_count, VM_MAP, prot);
	kfree(pages);

	/*
	 * Since vmap() uses page granularity, we must add the offset
	 * into the page here, to get the byte granularity address
	 * into the mapping to represent the actual "start" location.
	 */
	return vaddr + offset_in_page(start);
}

static void *sel4_tracebuffer_iomap(phys_addr_t start, size_t size,
		unsigned int memtype, char *label)
{
	void *va;

	if (!request_mem_region(start, size, label ?: "ramoops")) {
		printk("%s: request mem region (%s 0x%llx@0x%llx) failed\n",
			__func__, label ?: "ramoops",
			(unsigned long long)size, (unsigned long long)start);
		return NULL;
	}

	if (memtype)
		va = ioremap(start, size);
	else
		va = ioremap_wc(start, size);

	/*
	 * Since request_mem_region() and ioremap() are byte-granularity
	 * there is no need handle anything special like we do when the
	 * vmap() case in sel4_tracebuffer_ram_vmap() above.
	 */
	return va;
}

static void *sel4_tracebuffer_map(phys_addr_t start, phys_addr_t size, int memtype)
{
	void *vaddr = NULL;

	if (pfn_valid(start >> PAGE_SHIFT))
		vaddr = sel4_tracebuffer_vmap(start, size, memtype);
	else
		vaddr = sel4_tracebuffer_iomap(start, size, memtype, "");

	if (!vaddr) {
		printk("%s: Failed to map 0x%llx pages at 0x%llx\n", __func__,
			(unsigned long long)size, (unsigned long long)start);
		return NULL;
	}

	return vaddr;
}

static int sel4_tracebuffer_parse_dt(struct platform_device *pdev,
                                     struct sel4_tracebuffer_platform_data *pdata)
{
	struct resource *res;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		printk("%s: failed to locate DT /sel4-tracebuffer resource\n", __func__);
		return -EINVAL;
	}

	pdata->mem_size = resource_size(res);
	pdata->mem_address = res->start;

	pdata->vaddr = sel4_tracebuffer_map(pdata->mem_address, pdata->mem_size, 1);

	printk("%s: map phaddr:0x%llx to vaddr: 0x%llx\n", __func__, pdata->mem_address, (uint64_t)pdata->vaddr);

	return 0;
}

#if 0
char *alloc_mmap_pages(int npages)
{
    int i;
    char *mem = kmalloc(PAGE_SIZE * npages, GFP_KERNEL);

    printk("%s: \n", __func__);

    if (!mem) {
	    printk("%s: Can't allocate memory\n", __func__);
	    return mem;
    }

    for(i = 0; i < npages * PAGE_SIZE; i += PAGE_SIZE)
	    SetPageReserved(virt_to_page(((unsigned long)mem) + i));

    return mem;
}

void free_mmap_pages(void *mem, int npages)
{
    int i;

    printk("%s: \n", __func__);

    for(i = 0; i < npages * PAGE_SIZE; i += PAGE_SIZE)
	    ClearPageReserved(virt_to_page(((unsigned long)mem) + i));

    kfree(mem);
}
#endif

static int sel4_tracebuffer_probe(struct platform_device *pdev)
{
        struct device *dev = &pdev->dev;
        struct sel4_tracebuffer_platform_data *pdata = dev->platform_data;
        int err = -EINVAL;

	if (pdata_local.vaddr) {
		printk("%s: sel4 tracebuffer: only one instance is allowed\n", __func__);
		return err;
	}

        if (dev_of_node(dev) && !pdata) {
                pdata = &pdata_local;
                memset(pdata, 0, sizeof(*pdata));

                err = sel4_tracebuffer_parse_dt(pdev, pdata);

                if (err < 0)
                        return err;
        } else {
		printk("%s: wrong configuration\n", __func__);
		return err;
	}

	printk("%s: probed sel4 trace buffer 0x%lx@0x%llx\n", __func__, pdata->mem_size, pdata->mem_address);

	sel4_tracebuffer_register_sysfs();

	err = 0;
	return err;
}

static int sel4_tracebuffer_remove(struct platform_device *pdev)
{
	sel4_tracebuffer_unregister_sysfs();
	return 0;
}

static const struct of_device_id dt_match[] = {
        { .compatible = "sel4_tracebuffer" },
        { .compatible = "memory,sel4_tracebuffer" },
        {}
};

static struct platform_driver sel4_tracebuffer_driver = {
        .probe          = sel4_tracebuffer_probe,
        .remove         = sel4_tracebuffer_remove,
        .driver         = {
                .name           = "sel4_tracebuffer",
                .of_match_table = dt_match,
        },
};

static int __init sel4_tracebuffer_init(void)
{
	int ret;

	ret = platform_driver_register(&sel4_tracebuffer_driver);
	if (ret != 0) {
		printk("%s: can not register sel4_tracebuffer driver\n", __func__);
	}

	return ret;
}

static void __exit sel4_tracebuffer_exit(void)
{
	platform_driver_unregister(&sel4_tracebuffer_driver);
}

postcore_initcall(sel4_tracebuffer_init);
module_exit(sel4_tracebuffer_exit);

MODULE_LICENSE("GPL");
