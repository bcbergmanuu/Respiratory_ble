#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/device.h>

#include "pwm_config.h"
#include <stdio.h>
#include <string.h>
#include <nrfx_pwm.h>


#define ZEPHYR_USER_NODE DT_PATH(zephyr_user)

static const struct pwm_dt_spec pwm = PWM_DT_SPEC_GET(DT_PATH(zephyr_user));

//static uint32_t period;
//static struct k_work beep_work;
//static volatile bool beep_active;

//static const struct gpio_dt_spec sw0_gpio = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
//static const struct gpio_dt_spec sw1_gpio = GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios);

/* ensure SW0 & SW1 are on same gpio controller */
// BUILD_ASSERT(DT_SAME_NODE(DT_GPIO_CTLR(DT_ALIAS(sw0), gpios), DT_GPIO_CTLR(DT_ALIAS(sw1), gpios)));

//static void beep(struct k_work *work)
// {
// 	/* The "period / 2" pulse duration gives 50% duty cycle, which
// 	 * should result in the maximum sound volume.
// 	 */
// 	pwm_set_dt(&pwm, 130, 65);
// 	k_sleep(K_SECONDS(10));

// 	/* Disable the PWM */
// 	pwm_set_pulse_dt(&pwm, 0);	
// }

// static void button_pressed(const struct device *dev, struct gpio_callback *cb,
// 			   uint32_t pins)
// {
	// if (beep_active) {
	// 	printk("Button press while beeping\n");
	// 	return;
	// }

	// beep_active = true;

	// if (pins & BIT(sw0_gpio.pin)) {
	// 	printk("A pressed\n");
	// 	if (period < PERIOD_MAX) {
	// 		period += PWM_USEC(50U);
	// 	}
	// } else {
	// 	printk("B pressed\n");
	// 	if (period > PERIOD_MIN) {
	// 		period -= PWM_USEC(50U);
	// 	}
	// }

	// printk("Period is %u us (%u Hz)\n", period / NSEC_PER_USEC,
	//        NS_TO_HZ(period));


	// printf("%uHz", NS_TO_HZ(period));

	//k_work_submit(&beep_work);
//}

int pwm_init()
{
	// static struct gpio_callback button_cb_data;

	// if (!pwm_is_ready_dt(&pwm)) {
	// 	printk("%s: device not ready.\n", pwm.dev->name);
	// 	return 0;
	// }

	// /* since sw0_gpio.port == sw1_gpio.port, we only need to check ready once */
	// if (!gpio_is_ready_dt(&sw0_gpio)) {
	// 	printk("%s: device not ready.\n", sw0_gpio.port->name);
	// 	return 0;
	// }

	// period = pwm.period;

	// gpio_pin_configure_dt(&sw0_gpio, GPIO_INPUT);
	// gpio_pin_configure_dt(&sw1_gpio, GPIO_INPUT);

	// gpio_pin_interrupt_configure_dt(&sw0_gpio, GPIO_INT_EDGE_TO_ACTIVE);
	// gpio_pin_interrupt_configure_dt(&sw1_gpio, GPIO_INT_EDGE_TO_ACTIVE);

	// gpio_init_callback(&button_cb_data, button_pressed,
	// 		   BIT(sw0_gpio.pin) | BIT(sw1_gpio.pin));

	// k_work_init(&beep_work, beep);
	// /* Notify with a beep that we've started */
	
    // k_work_submit(&beep_work);
	
    // gpio_add_callback(sw0_gpio.port, &button_cb_data);
     pwm_set_dt(&pwm, 250, 125); 

 	return 0;
 }

/**
 * Simple program to drive LED0 on the nrf5340 evaluation board using the nrfx driver for lower level access
 * Jonas Forssell
 */

//#define ZEPHYR_USER_NODE DT_PATH(zephyr_user)

// Use Zephyr macro to get access to the LED0 on the board
//#define OUTPUT_PIN DT_GPIO_PIN(DT_ALIAS(gpios), gpios)
//const struct gpio_dt_spec signal = GPIO_DT_SPEC_GET(ZEPHYR_USER_NODE, signal_gpios);

// Use nrfx to create a PWM instance which we will connect to the LED0 later
//static nrfx_pwm_t m_pwm0 = NRFX_PWM_INSTANCE(0);

// // Declare variables holding PWM sequence values. In this example only one channel is used 
// nrf_pwm_values_individual_t seq_values[4] = {0};
// nrf_pwm_sequence_t const seq =
// {
//     .values.p_individual = seq_values,
//     .length          = NRF_PWM_VALUES_LENGTH(seq_values),
//     .repeats         = 0,
//     .end_delay       = 0
// };


// Set duty cycle between 0 and 100%
// void pwm_update_duty_cycle(uint8_t duty_cycle)
// {
    
//     // Check if value is outside of range. If so, set to 100%
//     if(duty_cycle >= 100)
//     {
//         seq_values->channel_0 = 100;
//     }
//     else
//     {
//         seq_values->channel_0 = duty_cycle;
//     }
    
//     nrfx_pwm_simple_playback(&m_pwm0, &seq, 1, NRFX_PWM_FLAG_LOOP);
// }

// #define OUTPUT_PIN	DT_GPIO_PIN(DT_ALIAS(led0), gpios)

// static int pwm_init(void)
// {
// 	nrfx_pwm_config_t const config0 =
//     {
//         .output_pins =
//         {
//             OUTPUT_PIN,                        // channel 0 now connected to LED0
//             // NRFX_PWM_PIN_NOT_USED,             // channel 1
//             // NRFX_PWM_PIN_NOT_USED,             // channel 2
//             // NRFX_PWM_PIN_NOT_USED,             // channel 3
//         },
//         .irq_priority = NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY,
//         .base_clock   = NRF_PWM_CLK_16MHz,
//         .count_mode   = NRF_PWM_MODE_UP,
//         .top_value    = 1,
//         .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
//         .step_mode    = NRF_PWM_STEP_AUTO		
//     };
// 	/* Connect PWM0 IRQ to nrfx_pwm_irq_handler */
// 	IRQ_CONNECT(DT_IRQN(DT_NODELABEL(pwm0)), DT_IRQ(DT_NODELABEL(pwm0), priority), nrfx_isr,
// 		    NRFX_PWM_INST_HANDLER_GET(0), 0);

//     // Init PWM without error handler
//     int ret = nrfx_pwm_init(&m_pwm0, &config0, NULL, NULL);
// 	if(ret != 0) {
// 		printf("error initiating PWM: %d", ret);
// 	} else {
// 		printf("pwm initiated");
// 	}

// 	    // PWM Sequence: Example with constant 50% duty cycle
//     static nrf_pwm_values_individual_t pwm_values = {1, 0,0,0};  // Adjust for duty cycle
//     static nrf_pwm_sequence_t const seq =
//     {
//         .values.p_individual = &pwm_values,
//         .length              = NRF_PWM_VALUES_LENGTH(pwm_values),
//         .repeats             = 0,  // No additional repeats
//         .end_delay           = 0   // No delay between loops
//     };

//     // Start PWM playback in infinite loop
//     nrfx_pwm_simple_playback(&m_pwm0, &seq, 0, NRFX_PWM_FLAG_LOOP);

	
//     return 0;
// }


// int main(void)
// {

//     // Start clock for accurate frequencies
//    // NRF_CLOCK->TASKS_HFCLKSTART = 1; 
//     // Wait for clock to start
//     // while(NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) 
//     //     ;
    
//     pwm_init();

//     for (;;)
//     {
//         for(int i = 0; i <= 100; i++)
//         {
//             k_msleep(10);
//             pwm_update_duty_cycle(i);
//         }
//     }
// }


/** @} */

SYS_INIT(pwm_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
