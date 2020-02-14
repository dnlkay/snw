#include <linux/kernel.h>
#include <linux/uvd.h>
#include <linux/platform_device.h>

/* struct raw_notifier_head uvd_example_chain */
static BLOCKING_NOTIFIER_HEAD(uvd_notifier_list);

static int uvd_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	dev_info(dev, "%s\n", __func__);
	return 0;
}

static int __exit uvd_remove(struct platform_device *pdev)
{
	return 0;
}

#define UVD_NOTIFY_ADD   0x01
#define UVD_NOTIFY_DEL   0x02
#define UVD_NOTIFY_ALARM 0x03
#define UVD_NOTIFY_PEACE 0x04

/* call_chain( */

static int uvd_counter_notify(struct notifier_block *nb,
                              unsigned long action,
                              void *data)
{
	printk(KERN_INFO "event %lu is fired!\n", action);

	return NOTIFY_STOP;
}

static struct notifier_block uvd_counter_nb = {
	.notifier_call = uvd_counter_notify,
};


static int uvd_message_notify(struct notifier_block *nb,
                              unsigned long action,
                              void *data)
{
	printk(KERN_INFO "event %lu is fired!\n", action);

	return NOTIFY_STOP;
}

static struct notifier_block uvd_message_nb = {
	.notifier_call = uvd_message_notify,
};


static struct platform_driver uvd_driver = {
	.probe  	= uvd_probe,
	.remove 	= __exit_p(uvd_remove),
	.driver 	= {
		.name 	= "uvdrv",
	},
};

static struct platform_device uvd_device = {
	.name = "uvdrv",
};

static int __init uvd_init(void)
{
	blocking_notifier_chain_register(&uvd_notifier_list,
                                         &uvd_message_nb);
	blocking_notifier_chain_register(&uvd_notifier_list,
                                         &uvd_counter_nb);

	platform_driver_register(&uvd_driver);
	platform_device_register(&uvd_device);
	return 0;
}
late_initcall(uvd_init);
