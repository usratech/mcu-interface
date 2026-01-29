/**
 * @file nrf52_gpio.c
 * @author Ramil Tenishev (tenishevram@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2026-01-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "mcui_gpio.h"

#include "nrfx_gpiote.h"

/**************************************************************************************************
 *                                          Variables
 *************************************************************************************************/

static const nrf_gpio_pin_sense_t sense_mapper[MCUI_GPIO_SENSE_COUNT] = {
    [MCUI_GPIO_SENSE_NONE] = NRF_GPIO_PIN_NOSENSE,
    [MCUI_GPIO_SENSE_LOW] = NRF_GPIO_PIN_SENSE_LOW,
    [MCUI_GPIO_SENSE_HIGH] = NRF_GPIO_PIN_SENSE_HIGH,
};

static const nrf_gpio_pin_input_t input_mapper[MCUI_GPIO_MODE_COUNT] = {
    [MCUI_GPIO_MODE_INPUT] = NRF_GPIO_PIN_INPUT_CONNECT,
    [MCUI_GPIO_MODE_OUTPUT] = NRF_GPIO_PIN_INPUT_DISCONNECT,
};

static const nrf_gpio_pin_dir_t mode_mapper[MCUI_GPIO_MODE_COUNT] = {
    [MCUI_GPIO_MODE_INPUT] = NRF_GPIO_PIN_DIR_INPUT,
    [MCUI_GPIO_MODE_OUTPUT] = NRF_GPIO_PIN_DIR_OUTPUT,
};

static const nrf_gpio_pin_pull_t pull_mapper[MCUI_GPIO_PULL_COUNT] = {
    [MCUI_GPIO_PULL_NO] = NRF_GPIO_PIN_NOPULL,
    [MCUI_GPIO_PULL_UP] = NRF_GPIO_PIN_PULLUP,
    [MCUI_GPIO_PULL_DOWN] = NRF_GPIO_PIN_PULLDOWN,
};

static const nrf_gpiote_polarity_t int_mode_mapper[MCUI_GPIO_INT_MODE_COUNT] = {
    [MCUI_GPIO_INT_MODE_LTOH] = NRF_GPIOTE_POLARITY_LOTOHI,
    [MCUI_GPIO_INT_MODE_HTOL] = NRF_GPIOTE_POLARITY_HITOLO,
    [MCUI_GPIO_INT_MODE_BOTH] = NRF_GPIOTE_POLARITY_TOGGLE,
};

static const nrf_gpio_pin_drive_t drive_mapper[MCUI_GPIO_DRIVE_COUNT] = {
    [MCUI_GPIO_DRIVE_S0S1] = NRF_GPIO_PIN_S0S1,
    [MCUI_GPIO_DRIVE_H0S1] = NRF_GPIO_PIN_H0S1,
    [MCUI_GPIO_DRIVE_S0H1] = NRF_GPIO_PIN_S0H1,
    [MCUI_GPIO_DRIVE_H0H1] = NRF_GPIO_PIN_H0H1,
    [MCUI_GPIO_DRIVE_S0S1] = NRF_GPIO_PIN_S0S1,
    [MCUI_GPIO_DRIVE_S0S1] = NRF_GPIO_PIN_S0S1,
    [MCUI_GPIO_DRIVE_S0S1] = NRF_GPIO_PIN_S0S1,
    [MCUI_GPIO_DRIVE_S0S1] = NRF_GPIO_PIN_S0S1,

};

/**************************************************************************************************
 *                                          Functions
 *************************************************************************************************/

void mcui_gpio_configure(uint16_t pin, mcui_gpio_cfg_t *cfg)
{
    assert(check_cfg(cfg));

    uint8_t port = pin >> 8;
    uint8_t pin = pin & 0xff;
    uint32_t nrf_pin = NRF_GPIO_PIN_MAP(port, pin);

    if (cfg->int_mode == MCUI_GPIO_INT_MODE_NONE) {

        nrf_gpio_cfg(nrf_pin, mode_mapper[cfg->mode], 
                     input_mapper[cfg->mode], 
                     pull_mapper[cfg->pull], 
                     NRF_GPIO_PIN_S0S1, 
                     sense_mapper[cfg->sense]);
        return;
    }
    
    /* configure an interrupt mode */
    ret_code_t err_code;
    nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    
    in_config.sense = int_mode_mapper[cfg->int_mode];

    if (!nrfx_gpiote_is_init()) {
        err_code = nrfx_gpiote_init();
        APP_ERROR_CHECK(err_code);
    }
    in_config.pull = pull_mapper[cfg->pull];
    err_code = nrfx_gpiote_in_init(nrf_pin, &in_config, (nrfx_gpiote_evt_handler_t)f);
    APP_ERROR_CHECK(err_code);
    nrfx_gpiote_in_event_enable(nrf_pin, true);
}

