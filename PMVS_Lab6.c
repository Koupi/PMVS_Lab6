#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");

static int timeout = 2;
static int timecount;

static struct timer_list timer;

static ssize_t timecount_show(struct kobject *kobj, struct kobj_attribute *attr,
            char *buf)
{
    return sprintf(buf, "%d\n", timecount);
}

static void print_hello(unsigned long arg)
{
    printk("Hello, world! %d\n", timecount);
    mod_timer(&timer, jiffies + timeout * HZ);
    timecount--;
    if(timecount == 0) {
        del_timer(&timer);
    }
}

static ssize_t timecount_store(struct kobject *kobj, struct kobj_attribute *attr,
             const char *buf, size_t count)
{
    sscanf(buf, "%du", &timecount);

    if (timecount > 0) {
        timer.expires = jiffies + timeout * HZ;
        timer.function = print_hello;
        add_timer(&timer);
    } else {
        del_timer_sync(&timer);
    }

    return count;
}

static struct kobj_attribute timecount_attribute =
    __ATTR(timecount, 0664, timecount_show, timecount_store);

static struct attribute *attrs[] = {
    &timecount_attribute.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

static struct kobject *pmvs6_kobj;

static int __init pmvs6_init(void)
{
    int retval;

    pmvs6_kobj = kobject_create_and_add("mytimer", kernel_kobj);
    if (!pmvs6_kobj)
        return -ENOMEM;

    retval = sysfs_create_group(pmvs6_kobj, &attr_group);
    if (retval)
        goto error_group;

    init_timer(&timer);

    goto ok;

error_group:
    kobject_put(pmvs6_kobj);

ok:
    return retval;
}

static void __exit pmvs6_exit(void)
{
    kobject_put(pmvs6_kobj);
}

module_init(pmvs6_init);
module_exit(pmvs6_exit);

