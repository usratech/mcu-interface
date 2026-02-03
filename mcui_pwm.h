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

enum __mcui_pwm_instance {
    MCUI_PWM0 = 0,
    MCUI_PWM1,
    MCUI_PWM2,
    MCUI_PWM3,
    /* at the end */
    MCUI_PWM_COUNT
};

typedef struct {
    uint32_t frequency_hz;
    uint16_t *pins;
    uint16_t *duty_cycles;
    uint8_t channels;
    bool pins_off_state;
    bool active_high;
} mcui_pwm_cfg_t;

/**************************************************************************************************
 *                                          Functions
 *************************************************************************************************/

void mcui_pwm_start(uint8_t pwm, const mcui_pwm_cfg_t* cfg);
void mcui_pwm_stop(uint8_t pwm, const uint16_t* pins, uint8_t pin_count);

#ifdef __cplusplus
}
#endif
