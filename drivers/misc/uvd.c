#include <linux/kernel.h>
#include <linux/uvd.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#define UVD_NOTIFY_ADD   0x01
#define UVD_NOTIFY_DEL   0x02
#define UVD_NOTIFY_ALARM 0x03
#define UVD_NOTIFY_PEACE 0x04

/* struct raw_notifier_head uvd_example_chain */
static BLOCKING_NOTIFIER_HEAD(uvd_notifier_list);

/* commented out as it is replaced to INIT_WORK */
//DECLARE_WORK(name, void (*function)(void *), void *data);
struct work_struct uvd_work;
struct workqueue_struct *uvd_wq;
static spinlock_t uvd_lock;

static void uvd_yell_work(struct work_struct *work)
{
#if 0 /* hard-lockup detection test */
	staic int uvd_cnt = 0;
	if (uvd_cnt > 15) {
		printk("[DEBUG] spin_lock after spin_lock...\n");
		spin_lock_irq(&uvd_lock);
	}
	spin_lock_irq(&uvd_lock);
	printk("spin_lock... %d\n", uvd_cnt);
	spin_unlock_irq(&uvd_lock);
	printk("spin_unlock...\n");
	msleep(50*1000);
	uvd_cnt++;
	queue_work(uvd_wq, &uvd_work);
#else
	printk("%s popped out... (%#x)\n", __func__, (u32)work);
	msleep(15 * 1000);
	queue_work(uvd_wq, &uvd_work);
#endif
	return;
}

static int uvd_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	uvd_wq = create_workqueue("uvd_wq");
	INIT_WORK(&uvd_work, uvd_yell_work);
	queue_work(uvd_wq, &uvd_work);

	/* dbg */ printk("%s dev:%#x...\n", __func__, (u32)dev);
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
	cancel_work_sync(&uvd_work);
	flush_workqueue(uvd_wq);
	destroy_workqueue(uvd_wq);

	blocking_notifier_call_chain(&uvd_notifier_list,
	                             UVD_NOTIFY_DEL, pdev);
	return 0;
}

static int uvd_counter_notify(struct notifier_block *nb,
                              unsigned long action,
                              void *data)
{
	static int counter = 0;
//	struct device *dev = data;

	/* dbg */ printk("%s data:%#x...\n", __func__, (u32)data);
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
//	struct device *dev = data;

	/* dbg */ printk("%s data:%#x...\n", __func__, (u32)data);
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
