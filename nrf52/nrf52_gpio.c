/**
 * @file nrf52_gpio.c
 * @author Ramil Tenishev (tenishevram@gmail.com)
 * @brief Implementation of mcui_gpio for nRF52 family SoCs within nRF5 SDK 17
 * @version 0.1
 * @date 2026-01-29
 * 
 * @copyright USRA Technologies. Copyright (c) 2026
 * 
 */

#include "mcui_gpio.h"

#include "nrfx_gpiote.h"

/**************************************************************************************************
 *                                          Definitions
 *************************************************************************************************/
#define PIN_TO_NRF_PIN(pin) (NRF_GPIO_PIN_MAP(pin >> 8, pin & 0xff))

#define INT_HANDLERS_MAX_NUMBER 10

typedef struct {
    uint16_t pin;
    mcui_gpio_int_callback_t callback;
} int_handler_list_item_t; 

/**************************************************************************************************
 *                                  Static function prototypes
 *************************************************************************************************/
static bool check_cfg(mcui_gpio_cfg_t *cfg);
static void int_handler_register(uint16_t pin, mcui_gpio_cfg_t *cfg);
static void int_handler_unregister(uint16_t pin);
static void nrfx_gpiote_evt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

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
    [MCUI_GPIO_DRIVE_D0S1] = NRF_GPIO_PIN_D0S1,
    [MCUI_GPIO_DRIVE_D0H1] = NRF_GPIO_PIN_D0H1,
    [MCUI_GPIO_DRIVE_S0D1] = NRF_GPIO_PIN_S0D1,
    [MCUI_GPIO_DRIVE_H0D1] = NRF_GPIO_PIN_H0D1,
};

static int_handler_list_item_t int_handlers[INT_HANDLERS_MAX_NUMBER];

/**************************************************************************************************
 *                                          Functions
 *************************************************************************************************/
void mcui_gpio_configure(uint16_t pin, mcui_gpio_cfg_t *cfg)
{
    assert(check_cfg(cfg));

    uint32_t nrf_pin = PIN_TO_NRF_PIN(pin);

    if (cfg->int_mode == MCUI_GPIO_INT_MODE_NONE) {
        nrf_gpio_cfg(nrf_pin, mode_mapper[cfg->mode], input_mapper[cfg->mode], 
                pull_mapper[cfg->pull], drive_mapper[cfg->drive], sense_mapper[cfg->sense]);
        return;
    }
    
    /* configure an interrupt mode */
    nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    in_config.sense = int_mode_mapper[cfg->int_mode];

    if (!nrfx_gpiote_is_init()) {
        APP_ERROR_CHECK(nrfx_gpiote_init());
    }
    in_config.pull = pull_mapper[cfg->pull];
    APP_ERROR_CHECK(nrfx_gpiote_in_init(nrf_pin, &in_config, nrfx_gpiote_evt_handler));
    /* FIXME: add a mutex to protect the list of handlers */
    int_handler_register(pin, cfg);
    nrfx_gpiote_in_event_enable(nrf_pin, true);
}

void mcui_gpio_disable(mcui_pin_t pin)
{
    uint32_t nrf_pin = PIN_TO_NRF_PIN(pin);

    nrf_gpio_cfg_default(nrf_pin);
    nrfx_gpiote_in_uninit(nrf_pin);
    nrfx_gpiote_in_event_disable(nrf_pin);
    /* FIXME: add a mutex to protect the list of handlers */
    int_handler_unregister(pin);
}

bool kt_bsp_gpio_get(mcui_pin_t pin)
{
    uint32_t nrf_pin = PIN_TO_NRF_PIN(pin);

    return nrf_gpio_pin_read(nrf_pin);
}

void mcui_gpio_set(mcui_pin_t pin, bool value)
{
    uint32_t nrf_pin = PIN_TO_NRF_PIN(pin);

    if (value) {
        nrf_gpio_pin_set(nrf_pin);
    }
    else {
        nrf_gpio_pin_clear(nrf_pin);
    }
}

/**************************************************************************************************
 *                                          Static functions
 *************************************************************************************************/
static bool check_cfg(mcui_gpio_cfg_t *cfg)
{
    if (cfg->mode >= MCUI_GPIO_MODE_COUNT) { return false; }
    if (cfg->int_mode >= MCUI_GPIO_INT_MODE_COUNT) { return false; }
    if (cfg->pull >= MCUI_GPIO_PULL_COUNT) { return false; }
    if (cfg->sense >= MCUI_GPIO_SENSE_COUNT) { return false; }
    if (cfg->drive >= MCUI_GPIO_DRIVE_COUNT) { return false; }

    if (cfg->int_mode != MCUI_GPIO_INT_MODE_NONE && cfg->callback == NULL) { 
        return false; 
    }
    if (cfg->int_mode != MCUI_GPIO_INT_MODE_NONE && cfg->mode != MCUI_GPIO_MODE_INPUT) { 
        return false; 
    }
    if (cfg->mode == MCUI_GPIO_MODE_OUTPUT && cfg->pull != MCUI_GPIO_PULL_NO) {
        return false;
    }
    return true;
}

static void int_handler_register(uint16_t pin, mcui_gpio_cfg_t *cfg)
{
    for (uint8_t i = 0; i < INT_HANDLERS_MAX_NUMBER; i++) {
        if ((int_handlers[i].pin == 0) && (int_handlers[i].callback == NULL)) {
            int_handlers[i].pin = pin;
            int_handlers[i].callback = cfg->callback;
            break;
        }
    }
}

static void int_handler_unregister(uint16_t pin)
{
    for (uint8_t i = 0; i < INT_HANDLERS_MAX_NUMBER; i++) {
        if ((int_handlers[i].pin == pin) && (int_handlers[i].callback != NULL)) {
            memset(&int_handlers[i], 0, sizeof(int_handler_list_item_t));
            break;
        }
    }
}

static void nrfx_gpiote_evt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    for (uint8_t i = 0; i < INT_HANDLERS_MAX_NUMBER; i++) {
        if (pin == PIN_TO_NRF_PIN(int_handlers[i].pin)) {
            int_handlers[i].callback(int_handlers[i].pin);
        }
    }
}
