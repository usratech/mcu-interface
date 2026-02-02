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

#define MCUI_ADC_ERROR_NONE     0
#define MCUI_ADC_ERROR_BUSY    -1
#define MCUI_ADC_ERROR_SDK     -2
#define MCUI_ADC_ERROR_UNKNOWN -3

#define MCUI_ADC_SCAN_MAX_CHANNELS 4

typedef uint16_t mcui_adc_pin_t;

enum __mcui_adc {
    MCUI_ADC0 = 0,
    MCUI_ADC1,
    MCUI_ADC2,
    /* at the end */
    MCUI_ADC_COUNT
};

enum __mcui_adc_mode {
    MCUI_ADC_MODE_SINGLE_END = 0,
    MCUI_ADC_MODE_DIFFERENTIAL,
    /* at the end */
    MCUI_ADC_MODE_COUNT
};

enum __mcui_adc_resolution {
    MCUI_ADC_RESOLUTION_8BIT = 0,
    MCUI_ADC_RESOLUTION_10BIT,
    MCUI_ADC_RESOLUTION_12BIT,
    MCUI_ADC_RESOLUTION_16BIT,
    /* at the end */
    MCUI_ADC_RESOLUTION_COUNT
};

typedef void (*mcui_adc_callback_t)(void *buffer, size_t size);

typedef struct {
    mcui_adc_callback_t callback;
    uint16_t scan_pins[MCUI_ADC_SCAN_MAX_CHANNELS];
    uint16_t positive_pin;
    uint16_t negative_pin;
    uint8_t channels_in_use;
    uint8_t mode;
    uint8_t resolution;
    uint16_t oversample;
    float vref;
    float gain;
} mcui_adc_cfg_t;


/**************************************************************************************************
 *                                          Functions
 *************************************************************************************************/

/**
 * @brief Sample a single value from ADC
 * 
 * @param adc ADC number
 * @param cfg configuration pointer
 * @param out_value output value pointer
 * @return int error code
 */
int mcui_adc_sample_single(uint8_t adc, mcui_adc_cfg_t *cfg, int *out_value);

/**
 * @brief Sample a buffer from ADC
 * 
 * @param adc ADC number
 * @param cfg configuration pointer
 * @param buffer buffer pointer
 * @param size buffer size in bytes
 * @return int error code
 */
int mcui_adc_sample_buffer(uint8_t adc, mcui_adc_cfg_t *cfg, void *buffer, size_t size);

#ifdef __cplusplus
}
#endif
