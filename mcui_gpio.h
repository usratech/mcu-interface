/**
 * @file mcui_gpio.h
 * @author Ramil Tenishev (tenishevram@gmail.com)
 * @brief This header file implements a generic interface for MCU gpio capabilities.
 * @version 0.1
 * @date 2026-01-29
 * 
 * @copyright USRA Technologies. Copyright (c) 2026
 * 
 */

#pragma once 

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus 
extern "C" {
#endif

/**************************************************************************************************
 *                                          Definitions
 *************************************************************************************************/

typedef uint16_t mcui_pin_t;

enum __mcui_gpio_mode {
    MCUI_GPIO_MODE_INPUT = 0,
    MCUI_GPIO_MODE_OUTPUT,
    /* at the end */
    MCUI_GPIO_MODE_COUNT,
};

enum __mcui_gpio_pull {
    MCUI_GPIO_PULL_NO = 0,
    MCUI_GPIO_PULL_UP,
    MCUI_GPIO_PULL_DOWN,
    /* at the end */
    MCUI_GPIO_PULL_COUNT,
};

enum __mcui_gpio_int_mode {
    MCUI_GPIO_INT_MODE_NONE = 0,
    MCUI_GPIO_INT_MODE_LTOH,
    MCUI_GPIO_INT_MODE_HTOL,
    MCUI_GPIO_INT_MODE_BOTH,
    /* at the end */
    MCUI_GPIO_INT_MODE_COUNT,
};

enum __mcui_gpio_sense {
    MCUI_GPIO_SENSE_NONE = 0,
    MCUI_GPIO_SENSE_LOW,
    MCUI_GPIO_SENSE_HIGH,
    /* at the end */
    MCUI_GPIO_SENSE_COUNT,
};

typedef void (*mcui_gpio_int_callback_t)(uint16_t pin);

typedef struct {
    mcui_gpio_int_callback_t callback;
    uint8_t mode;
    uint8_t int_mode;
    uint8_t pull;
    uint8_t sense; /* wake up sensing */
} mcui_gpio_cfg_t;


/**************************************************************************************************
 *                                          Functions
 *************************************************************************************************/

/**
 * @brief Configure GPIO pin
 * 
 * @param pin pin number
 * @param cfg configuration pointer
 */
void mcui_gpio_configure(uint16_t pin, mcui_gpio_cfg_t *cfg);

/**
 * @brief Set to the default state (inactive)
 * 
 * @param pin pin number
 */
void mcui_gpio_disable(uint16_t pin);

/**
 * @brief Gets pin value (input or output, both) 
 * 
 * @param pin pin number
 * @return bool pin value
 */
bool mcui_gpio_get(uint16_t pin);

/**
 * @brief Sets pin value
 * 
 * @param pin pin number
 * @param value output value 
 */
void mcui_gpio_set(uint16_t pin, bool value);

#ifdef __cplusplus
}
#endif
