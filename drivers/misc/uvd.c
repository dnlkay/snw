#include <linux/kernel.h>
#include <linux/uvd.h>
#include <linux/platform_device.h>

#define UVD_NOTIFY_ADD   0x01
#define UVD_NOTIFY_DEL   0x02
#define UVD_NOTIFY_ALARM 0x03
#define UVD_NOTIFY_PEACE 0x04

/* struct raw_notifier_head uvd_example_chain */
static BLOCKING_NOTIFIER_HEAD(uvd_notifier_list);

static int uvd_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	/* dbg */ printk("%s dev:%#x...\n", __func__, dev);
	/* dgb: dev:0x808c9358 */
	dev_info(dev, "%s\n", __func__);

	blocking_notifier_call_chain(&uvd_notifier_list,
	                             UVD_NOTIFY_ADD, pdev);
	blocking_notifier_call_chain(&uvd_notifier_list,
	                             UVD_NOTIFY_ALARM, pdev);
	return 0;
}

static int __exit uvd_remove(struct platform_device *pdev)
{
	blocking_notifier_call_chain(&uvd_notifier_list,
	                             UVD_NOTIFY_DEL, pdev);
	return 0;
}

static int uvd_counter_notify(struct notifier_block *nb,
                              unsigned long action,
                              void *data)
{
	static int counter = 0;
	struct device *dev = data;

	/* dbg */ printk("%s data:%#x...\n", __func__, data);
	/* dbg: can't get good address for 'dev' from 'data'? */
	/* dgb: data:0x808c934858 */
	printk(KERN_INFO "event %lu is fired!\n", action);

	switch (action) {
	case UVD_NOTIFY_ADD:
		counter++;
		break;
	case UVD_NOTIFY_DEL:
		counter--;
		break;
	case UVD_NOTIFY_ALARM:
		counter = 0;
		break;
	case UVD_NOTIFY_PEACE:
	default:
		break;
	}

	return NOTIFY_STOP;
}

static struct notifier_block uvd_counter_nb = {
	.notifier_call = uvd_counter_notify,
};


static int uvd_message_notify(struct notifier_block *nb,
                              unsigned long action,
                              void *data)
{
	struct device *dev = data;

	/* dbg */ printk("%s data:%#x...\n", __func__, data);
	printk(KERN_INFO "event %lu is fired!\n", action);

	switch (action) {
	case UVD_NOTIFY_ADD:
		pr_info("%s: added...\n", __func__);
		break;
	case UVD_NOTIFY_DEL:
		pr_info("%s: deleted...\n", __func__);
		break;
	case UVD_NOTIFY_ALARM:
		pr_info("%s: alarmed...\n", __func__);
		break;
	case UVD_NOTIFY_PEACE:
	default:
		break;
	}

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
