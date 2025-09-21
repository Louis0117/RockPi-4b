// include
#include <linux/module.h>      // module_init, module_exit, MODULE_LICENSE, THIS_MODULE
#include <linux/kernel.h>      // printk(), pr_info(), pr_err() 等
#include <linux/fs.h>          // struct file_operations, alloc_chrdev_region()
#include <linux/cdev.h>        // cdev_init(), cdev_add(), cdev_del()
#include <linux/uaccess.h>     // copy_to_user(), copy_from_user()
//#include <linux/gpio.h>        // legacy GPIO API (gpio_request, gpio_direction_output)
//#include <linux/gpio/consumer.h> // gpiod_* API (gpiod_set_value, gpio_to_desc)
#include <linux/miscdevice.h>
#include <linux/types.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>

// macro
#define DRIVER_NAME "lab1-DTS"
#define DEVICE_NAME "lab1-led-DTS"
#define BASE_MINOR 0
#define MINOR_COUNT 1 
#define LED_NUM 4
#define LED_ON 1
#define LED_OFF 0
#define LAB1_IOC_MAGIC 'L'
#define LAB1_LED_ON  _IO(LAB1_IOC_MAGIC, 0x01)
#define LAB1_LED_OFF _IO(LAB1_IOC_MAGIC, 0x02)

static struct gpio_desc *gpio_desc_struct_array[LED_NUM];

/**
 * long my_driver_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
 * 
 * 
 * #define ENOTTY          25  Inappropriate ioctl for device 
 * #define ENODEV          19  No such device 
 * #define EINVAL          22 Invalid argument
 * 
 * 
 */
static long led_control(struct file *filp, unsigned int cmd, unsigned long arg){
    
    int i;

    if(_IOC_TYPE(cmd) != LAB1_IOC_MAGIC){
        pr_err("user input undefine parameter!");
        return -ENOTTY;
    }

    switch (cmd)
    {
    case LAB1_LED_ON:           
        for (i=0; i<LED_NUM; i++){
            gpiod_set_value(gpio_desc_struct_array[i], 0);
        }
        break;
    case LAB1_LED_OFF:
        for (i=0; i<LED_NUM; i++){
            gpiod_set_value(gpio_desc_struct_array[i], 1);
        }
        break;
    default:
        printk("user input undefine parameter!");
        return -ENOTTY;
        break;
    }
    return 0;
}

// implemnt callback function
/**
 * define callback function
 * static const struct file_operations my_fops = {};
*/
static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = led_control,
};

static struct  miscdevice led_device_node = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &fops,
};



static int lab1_platform_driver_probe(struct platform_device *pdev){
    int i;
    char dt_gpio_name[15];
    int ret;
    // intial GPIO
    for(i=0;i<LED_NUM;i++){
        sprintf(dt_gpio_name, "led%d", i);
        gpio_desc_struct_array[i] = devm_gpiod_get_optional(&pdev->dev, dt_gpio_name, GPIOD_OUT_HIGH);
        if (IS_ERR(gpio_desc_struct_array[i])){
            ret = PTR_ERR(gpio_desc_struct_array[i]);
            if (ret != -EPROBE_DEFER)
                dev_err(&pdev->dev, "Failed to get %s GPIO: %d\n",
					dt_gpio_name, ret);
			goto gpio_error;
        }
    }
    // Create Device Node
    ret = misc_register(&led_device_node);
    if (ret){
        dev_err(&pdev->dev, "misc_register fail\n");
        return ret;
    }
    return 0;

gpio_error:
    for(i=0;i<LED_NUM;i++){
        if (gpio_desc_struct_array[i]){
            gpiod_put(gpio_desc_struct_array[i]);
        }
    }
    return ret;
}

static const struct of_device_id leds_of_match[] = {
    { .compatible = "lab1_dts,platform_driver", .data = NULL},
    {},
};

static struct platform_driver gpio_led_platform_driver = 
{
    .probe = lab1_platform_driver_probe,
    //.remove = platform_driver_remove;
    .driver = {
        .name  = DRIVER_NAME,
        .of_match_table = leds_of_match,
    },
};



// module init function
static int __init lab1_init(void){
    int ret;
    // init platform driver
    ret = platform_driver_register(&gpio_led_platform_driver);
    if(ret){
        pr_err("%s execute fail\n", __func__);
        return ret;
    }
    pr_info("%s initial success\n", __func__);
    return 0;
}


static void lab1_exit(void){
    platform_driver_unregister(&gpio_led_platform_driver);
}

// init 
module_init(lab1_init);
// exit
module_exit(lab1_exit);
MODULE_LICENSE("GPL");
