/**
 * MIT License
 * 
 * Copyright (c) 2026 USRA Technologies
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
#define MCUI_PIN_DEF(port, pin) (uint16_t)((uint16_t)port << 8 | (uint16_t)pin & 0xff)

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

enum __mcui_gpio_drive {
    MCUI_GPIO_DRIVE_S0S1 = 0,
    MCUI_GPIO_DRIVE_H0S1,
    MCUI_GPIO_DRIVE_S0H1,
    MCUI_GPIO_DRIVE_H0H1,
    MCUI_GPIO_DRIVE_D0S1,
    MCUI_GPIO_DRIVE_D0H1,
    MCUI_GPIO_DRIVE_S0D1,
    MCUI_GPIO_DRIVE_H0D1,
    /* at the end */
    MCUI_GPIO_DRIVE_COUNT,
};

typedef void (*mcui_gpio_int_callback_t)(uint16_t pin);

typedef struct {
    mcui_gpio_int_callback_t callback;
    uint8_t mode;
    uint8_t int_mode;
    uint8_t pull;
    uint8_t sense; /* wake up sensing */
    uint8_t drive;
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
