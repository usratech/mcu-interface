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
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 *                                          Definitions
 *************************************************************************************************/

enum __mcui_i2c {
    MCUI_I2C0 = 0,
    MCUI_I2C1,
    MCUI_I2C2,
    /* at the end */
    MCUI_I2C_COUNT
};

typedef struct {
    uint16_t sda_pin;
    uint16_t scl_pin;
    uint32_t frequency_hz;
} mcui_i2c_cfg_t;

/**************************************************************************************************
 *                                          Functions
 *************************************************************************************************/

void mcui_i2c_init(uint8_t i2c, const mcui_i2c_cfg_t* cfg);
int mcui_i2c_write(uint8_t i2c, const uint8_t* data, size_t length);
int mcui_i2c_read(uint8_t i2c, uint8_t* data, size_t length);

#ifdef __cplusplus
}
#endif
