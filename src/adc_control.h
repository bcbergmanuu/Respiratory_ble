#ifndef ADC_CONTROL__
#define ADC_CONTROL__



#define LOOPBACK_PIN_1A 3
#define LOOPBACK_PIN_1B 31
#define LOOPBACK_PIN_2A 4
#define LOOPBACK_PIN_2B 30
#define LOOPBACK_PIN_3A 28
#define LOOPBACK_PIN_3B 29
#define LOOPBACK_PIN_4A 26
#define LOOPBACK_PIN_4B 27
#define ANALOG_INPUT_A0 1
#define ANALOG_INPUT_A1 2
#define ANALOG_INPUT_A2 4

#define LED1_PIN        13
#define LED2_PIN        14
#define LED3_PIN        15
#define LED4_PIN        16

#define ANALOG_INPUT_TO_SAADC_AIN(x) ((x) + 1)
#define ANALOG_INPUT_TO_COMP_AIN(x) (x)

/**
 * @brief SAADC channel configuration for the single-ended mode with 3 us sample acquisition time.
 *        The 3 us acquisition time will work correctly when the source resistance of @p _pin_p input
 *        analog pin is less than 10 kOhm.
 *
 * This configuration sets up single-ended SAADC channel with the following options:
 * - resistor ladder disabled
 * - gain: 1/6
 * - reference voltage: internal 0.6 V
 * - sample acquisition time: 3 us
 * - burst disabled
 *
 * @param[in] _pin_p Positive input analog pin.
 * @param[in] _index Channel index.
 *
 * @sa nrfx_saadc_channel_t
 */
#define SAADC_CHANNEL_SE_ACQ_3US(_pin_p, _index)        \
{                                                       \
    .channel_config =                                   \
    {                                                   \
        .resistor_p = NRF_SAADC_RESISTOR_DISABLED,      \
        .resistor_n = NRF_SAADC_RESISTOR_DISABLED,      \
        .gain       = NRF_SAADC_GAIN1_6,                \
        .reference  = NRF_SAADC_REFERENCE_INTERNAL,     \
        .acq_time   = NRF_SAADC_ACQTIME_3US,            \
        .mode       = NRF_SAADC_MODE_SINGLE_ENDED,      \
        .burst      = NRF_SAADC_BURST_DISABLED,         \
    },                                                  \
    .pin_p          = (nrf_saadc_input_t)_pin_p,        \
    .pin_n          = NRF_SAADC_INPUT_DISABLED,         \
    .channel_index  = _index,                           \
}


extern void calibrate_and_start(struct k_work *work);
extern void adc_stop(struct k_work work)

#endif