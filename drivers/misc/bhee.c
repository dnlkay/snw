#include <linux/kernel.h>
#include <linux/platform_device.h>

static int bhee_probe(struct platform_device *pdev)
{
	return 0;
}

static int __exit bhee_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver bhee_driver = {
	.probe		= bhee_probe,
	.remove		= __exit_p(bhee_remove),
	.driver		= {
		.name	= "bhee",
	},
};

static int __init bhee_init(void)
{
	platform_driver_register(&bhee_driver);
	return 0;
}
late_initcall(bhee_init);
