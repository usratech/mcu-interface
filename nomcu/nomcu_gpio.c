/**
 * @file nomcu_gpio.c
 * @author Ramil Tenishev (tenishevram@gmail.com)
 * @brief nomcu implementation of the GPIO interface
 * @version 0.1
 * @date 2026-01-29
 * 
 * @copyright USRA Technologies. Copyright (c) 2026
 * 
 */

#include <assert.h>
#include <string.h>

#include "mcui_gpio.h"

/**************************************************************************************************
 *                                          Definitions
 *************************************************************************************************/

#define NOMCU_PIN_NUMBER_MAX 16

/**************************************************************************************************
 *                                          Static function prototypes
 *************************************************************************************************/

static bool check_cfg(uint16_t pin, mcui_gpio_cfg_t *cfg);
static void __nomcu_gpio_interrupt_handler(uint16_t pin);

/**************************************************************************************************
 *                                          Variables
 *************************************************************************************************/

static bool gpio_values[NOMCU_PIN_NUMBER_MAX];
static mcui_gpio_cfg_t gpio_configs[NOMCU_PIN_NUMBER_MAX];

/**************************************************************************************************
 *                                          Functions
 *************************************************************************************************/

void mcui_gpio_configure(uint16_t pin, mcui_gpio_cfg_t *cfg)
{
    assert(pin < NOMCU_PIN_NUMBER_MAX);
    assert(cfg);
    assert(check_cfg(pin, cfg));

    memcpy(&gpio_configs[pin], cfg, sizeof(mcui_gpio_cfg_t)); 
}

void mcui_gpio_disable(uint16_t pin)
{
    assert(pin < NOMCU_PIN_NUMBER_MAX);

    memset(&gpio_configs[pin], 0, sizeof(mcui_gpio_cfg_t));
}

bool mcui_gpio_get(uint16_t pin)
{
    assert(pin < NOMCU_PIN_NUMBER_MAX);

    return gpio_values[pin];
}

void mcui_gpio_set(uint16_t pin, bool value)
{
    assert(pin < NOMCU_PIN_NUMBER_MAX);

    gpio_values[pin] = value;
}

void __nomcu_gpio_input_toggle(uint16_t pin)
{
    assert(pin < NOMCU_PIN_NUMBER_MAX);
    assert(gpio_configs[pin].mode == MCUI_GPIO_MODE_INPUT);

    gpio_values[pin] = gpio_values[pin] ? false : true;

    if (!gpio_configs[pin].callback || !gpio_configs[pin].int_mode) {
        return;
    }

    __nomcu_gpio_interrupt_handler(pin);
}

/**************************************************************************************************
 *                                          Static functions
 *************************************************************************************************/

static bool check_cfg(uint16_t pin, mcui_gpio_cfg_t *cfg)
{
    return true;
}

static void __nomcu_gpio_interrupt_handler(uint16_t pin)
{
    if (((gpio_configs[pin].int_mode == MCUI_GPIO_INT_MODE_LTOH) && gpio_values[pin]) ||
        ((gpio_configs[pin].int_mode == MCUI_GPIO_INT_MODE_HTOL) && !gpio_values[pin]) ||
        (gpio_configs[pin].int_mode == MCUI_GPIO_INT_MODE_BOTH)) {
        
        gpio_configs[pin].callback(pin);
    }
}
