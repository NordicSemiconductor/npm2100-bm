/**
 * Copyright (c) 2025, Nordic Semiconductor ASA
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_gpio.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrfx_gpiote.h"

#include "i2c.h"
#include "util.h"

#include "adc_npm2100.h"
#include "regulator_npm2100.h"
#include "mfd_npm2100.h"
#include "gpio_npm2100.h"

#define HOST_SDA_PIN NRF_GPIO_PIN_MAP(0, 26)
#define HOST_SCL_PIN NRF_GPIO_PIN_MAP(0, 27)
#define HOST_INT_PIN NRF_GPIO_PIN_MAP(1, 12)

#define PMIC_LDO_CTRL_PIN 0
#define PMIC_INT_OUT_PIN  1

#define V_TO_UV(v) ((v) * 1000000)

struct i2c_dev npm2100_pmic = { .addr = 0x74 };

static bool pmic_interrupt;

/**
 * @brief Timer interrupt handler.
 */
static void in_pin_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    (void)pin;
    (void)action;

    if (pin == HOST_INT_PIN) {
        nrfx_gpiote_in_event_disable(HOST_INT_PIN);
        pmic_interrupt = true;
    }
}

/**
 * @brief Set up nPM2100 LDO.
 *
 * LDOSW to operate in LDO mode: off by default, High Power on GPIO0 button press (2.5V).
 */
static void npm2100_ldo_setup(void)
{
    int ret;

    ret = regulator_npm2100_set_mode(&npm2100_pmic, NPM2100_SOURCE_LDOSW, NPM2100_REG_OPER_OFF | NPM2100_REG_FORCE_HP);
    APP_ERROR_CHECK(ret);

    ret = gpio_npm2100_config(&npm2100_pmic, PMIC_LDO_CTRL_PIN, NPM2100_GPIO_MODE_GPIO, NPM2100_GPIO_CONFIG_INPUT | NPM2100_GPIO_CONFIG_PULLUP);
    APP_ERROR_CHECK(ret);

    ret = regulator_npm2100_pin_ctrl(&npm2100_pmic, NPM2100_SOURCE_LDOSW, PMIC_LDO_CTRL_PIN, true);
    APP_ERROR_CHECK(ret);

    ret = regulator_npm2100_set_voltage(&npm2100_pmic, NPM2100_SOURCE_LDOSW, V_TO_UV(2.5), V_TO_UV(2.5));
    APP_ERROR_CHECK(ret);

    ret = regulator_npm2100_enable(&npm2100_pmic, NPM2100_SOURCE_LDOSW);
    APP_ERROR_CHECK(ret);
}

/**
 * @brief Set up nPM2100 timer.
 *
 * nPM2100 will generate interrupts on GPIO0 connected to HOST_INTERRUPT_PIN
 * when the timer expires.
 */
static void npm2100_timer_setup(uint32_t period_ms) {
    int ret;

    ret = mfd_npm2100_stop_timer(&npm2100_pmic);
    APP_ERROR_CHECK(ret);

    /* it takes some time for the timer status to turn IDLE */
    nrf_delay_ms(3);
    ret = mfd_npm2100_set_timer(&npm2100_pmic, period_ms, NPM2100_TIMER_MODE_GENERAL_PURPOSE);
    APP_ERROR_CHECK(ret);

    ret = mfd_npm2100_enable_events(&npm2100_pmic, BIT(NPM2100_EVENT_SYS_TIMER_EXPIRY));
    APP_ERROR_CHECK(ret);

    /* nPM2100 GPIO1 as interrupt output, active high */
    ret = gpio_npm2100_config(&npm2100_pmic, PMIC_INT_OUT_PIN, NPM2100_GPIO_MODE_IRQ_HIGH, NPM2100_GPIO_CONFIG_OUTPUT);
    APP_ERROR_CHECK(ret);

    /* Interrupt is level-triggered, if a new event occurs while processing the current events, it will
     * trigger again after the interrupt is reenabled */
    nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(false);
    ret = nrfx_gpiote_init();
    APP_ERROR_CHECK(ret);
    ret = nrfx_gpiote_in_init(HOST_INT_PIN, &in_config, in_pin_handler);
    APP_ERROR_CHECK(ret);

    nrfx_gpiote_in_event_enable(HOST_INT_PIN, true);
}

/**
 * @brief Function for reading data from nPM2100 ADC.
 */
static int read_sensor_data(void)
{
    int ret;
    int32_t val[3];

    for (int chan=NPM2100_ADC_CHAN_VBAT; chan<=NPM2100_ADC_CHAN_VOUT; chan++) {
        ret = adc_npm2100_take_reading(&npm2100_pmic, chan);
        APP_ERROR_CHECK(ret);
        nrf_delay_us(100);

        ret = adc_npm2100_get_result(&npm2100_pmic, chan, &val[chan]);
        APP_ERROR_CHECK(ret);
    }

    NRF_LOG_INFO("Vbat: %d.%.3d V, Vout: %d.%.3d V, Die temp: %d.%.3d°C",
            val[0] / 1000000, val[0] % 1000000 / 1000,
            val[2] / 1000000, val[2] % 1000000 / 1000,
            val[1] / 1000000, val[1] % 1000000 / 1000);
    
    return 0;
}

/**
 * @brief Function for main application entry.
 */
int main(void)
{
    int ret;
    uint32_t events;

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("nPM2100 example started");
    NRF_LOG_FLUSH();

    ret = i2c_init(&npm2100_pmic, HOST_SDA_PIN, HOST_SCL_PIN);
    APP_ERROR_CHECK(ret);

    npm2100_ldo_setup();

    npm2100_timer_setup(2000);
    
    ret = mfd_npm2100_start_timer(&npm2100_pmic);
    APP_ERROR_CHECK(ret);

    NRF_LOG_INFO("nPM2100 PMIC device OK");
    NRF_LOG_FLUSH();

    while (true)
    {
        while (!pmic_interrupt) {
            __WFE();
        }

        /* process events before reenabling the interrupt */
        ret = mfd_npm2100_process_events(&npm2100_pmic, &events);
        APP_ERROR_CHECK(ret);

        if (events & BIT(NPM2100_EVENT_SYS_TIMER_EXPIRY)) {
            /* restart the timer, display ADC measurements */
            ret = mfd_npm2100_start_timer(&npm2100_pmic);
            APP_ERROR_CHECK(ret);
            read_sensor_data();
        }

        pmic_interrupt = false;
        nrfx_gpiote_in_event_enable(HOST_INT_PIN, true);

        NRF_LOG_FLUSH();
    }
}
