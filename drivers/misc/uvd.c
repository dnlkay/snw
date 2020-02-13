#include <linux/kernel.h>
#include <linux/uvd.h>
#include <linux/platform_device.h>

/* struct raw_notifier_head uvd_example_chain */
static BLOCKING_NOTIFIER_HEAD(uvd_example_chain);

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

static int uvd_example_event(struct notifier_block *nb,
                             unsigned long event,
                             void *ptr)
{
	printk(KERN_INFO "event %lu is fired!\n", event);

	return NOTIFY_STOP;
}

static struct notifier_block uvd_example_notifier = {
	.notifier_call = uvd_example_event,
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
	blocking_notifier_chain_register(&uvd_example_chain,
                                         &uvd_example_notifier);

	platform_driver_register(&uvd_driver);
	platform_device_register(&uvd_device);
	return 0;
}
late_initcall(uvd_init);
