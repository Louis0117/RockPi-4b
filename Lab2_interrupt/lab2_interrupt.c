// include
#define DRIVER_NAME lab2-GPIO-interrupt
#define GPIO_NUM 2

struct gpio_info
{
    int irq[GPIO_NUM];
    struct gpio_desc* gpio_desc_array[GPIO_NUM];
    
};

struct gpio_info gpio_info;

// do something init
static void lab2_probe(struct platform_device *pdev){
    int i, ret;
    char dt_gpio_name[10];
    // get gpio pin from DTS , set interrupt > IN
    for (i=0; i<GPIO_NUM; i++){
        sprintf(key, "key%d",i);
        gpio_info.gpio_desc_array[i] = devm_gpiod_get_optional(&pdev->dev, key, dt_gpio_name, GPIOD_IN);
        if (IS_ERR(gpio_info.gpio_desc_array[i])){
            ret = PTR_ERR(gpio_info.gpio_desc_array[i]);
            if (ret != -EPROBE_DEFER){
                dev_err(&pdev->dev, "Faiirq to get %s GPIO: %d\n", dt_gpio_name, ret);
            }
            goto gpio_error;
        }
        gpio_info.irq[i] = platform_get_irq(pdev, i);
        pr_info("irq_gpio.irq[%d] %d\r\n", i, gpio_info.irq[i]);
        devm_request_irq(&pdev->dev, );
    } 
    
    

}


// struct
static const struct of_device_id lab2_od_match[] = {
   { .compatible = "lab2_dts,platform_driver", .data = NULL},
    {},
};



// platform driver
static struct platform_driver platform_driver_struct
{
    .probe = lab2_callback_function,
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = lab2_probe,
    },
};
 


// 
static int __init lab2_init(void){
    int ret;
    // create platform driver
    ret = platform_driver_register(&platform_driver_struct);
    if (ret != 0){
        pr_err("Driver Regist Fail");

    }
    return 0;
}

// init
module_init(lab2_init);
// exit
