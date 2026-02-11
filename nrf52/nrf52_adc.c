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
#include "mcui_adc.h"
#include "mcui_port.h"

#include "nrfx_saadc.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"

/**************************************************************************************************
 *                                          Definitions
 *************************************************************************************************/

/* pins: 2,3,4,5 (0,1,2,3) and 28,29,30,31 (4,5,6,7) */
#define PIN_TO_CHANNEL(x) ((x < 6) ? (x - 2) : (x - 24))

#define NRF52_SAADC_CHANNELS 8

/**************************************************************************************************
 *                                          Prototypes
 *************************************************************************************************/

static bool check_cfg(mcui_adc_cfg_t *cfg);

/**************************************************************************************************
 *                                          Variables 
 *************************************************************************************************/

static const uint8_t ch_to_internal_ch[NRF52_SAADC_CHANNELS] = {
    NRF_SAADC_INPUT_AIN0, NRF_SAADC_INPUT_AIN1,
    NRF_SAADC_INPUT_AIN2, NRF_SAADC_INPUT_AIN3,
    NRF_SAADC_INPUT_AIN4, NRF_SAADC_INPUT_AIN5,
    NRF_SAADC_INPUT_AIN6, NRF_SAADC_INPUT_AIN7,
};

static const uint8_t gain_to_internal_gain[MCUI_ADC_GAIN_COUNT] = {
    [MCUI_ADC_GAIN_1_6] = NRF_SAADC_GAIN1_6, [MCUI_ADC_GAIN_1_5] = NRF_SAADC_GAIN1_5,
    [MCUI_ADC_GAIN_1_4] = NRF_SAADC_GAIN1_4, [MCUI_ADC_GAIN_1_3] = NRF_SAADC_GAIN1_3,
    [MCUI_ADC_GAIN_1_2] = NRF_SAADC_GAIN1_2, [MCUI_ADC_GAIN_1] = NRF_SAADC_GAIN1,
    [MCUI_ADC_GAIN_2] = NRF_SAADC_GAIN2, [MCUI_ADC_GAIN_4] = NRF_SAADC_GAIN4,
};

static const int bits_to_internal_bits[MCUI_ADC_RESOLUTION_COUNT] = {
    [MCUI_ADC_RESOLUTION_8BIT] = NRF_SAADC_RESOLUTION_8BIT,
    [MCUI_ADC_RESOLUTION_10BIT] = NRF_SAADC_RESOLUTION_10BIT,
    [MCUI_ADC_RESOLUTION_12BIT] = NRF_SAADC_RESOLUTION_12BIT,
    [MCUI_ADC_RESOLUTION_14BIT] = NRF_SAADC_RESOLUTION_14BIT,
};

/**************************************************************************************************
 *                                          Functions
 *************************************************************************************************/

int mcui_adc_sample_single(uint8_t adc, mcui_adc_cfg_t *cfg, int *out_value)
{
    assert(adc != 0);
    assert(check_cfg(cfg));
    assert(out_value);

}

int mcui_adc_sample_buffer(uint8_t adc, mcui_adc_cfg_t *cfg, void *buffer, size_t size)
{
    assert(adc != 0);
    assert(check_cfg(cfg));
    assert(buffer);
    assert(size);

}

/**************************************************************************************************
 *                                          Static functions
 *************************************************************************************************/

static bool check_cfg(mcui_adc_cfg_t *cfg)
{
    /* validate pins */

    if (cfg->positive_pin < 2 || cfg->positive_pin > 31) { return false; }
    if (cfg->positive_pin > 5 && cfg->positive_pin < 28) { return false; }

    if (cfg->negative_pin) {
        if (cfg->negative_pin < 2 || cfg->negative_pin > 31) { return false; }
        if (cfg->negative_pin > 5 && cfg->negative_pin < 28) { return false; }
    }

    switch (cfg->gain) {
    case MCUI_ADC_GAIN_1_6:
    case MCUI_ADC_GAIN_1_5:
    case MCUI_ADC_GAIN_1_4:
    case MCUI_ADC_GAIN_1_3:
    case MCUI_ADC_GAIN_1_2:
    case MCUI_ADC_GAIN_1:
    case MCUI_ADC_GAIN_2:
    case MCUI_ADC_GAIN_4:
        break; 
    default:
        return false;
    }

    switch (cfg->resolution) {
    case MCUI_ADC_RESOLUTION_8BIT:
    case MCUI_ADC_RESOLUTION_10BIT:
    case MCUI_ADC_RESOLUTION_12BIT:
    case MCUI_ADC_RESOLUTION_14BIT:
        break; 
    default:
        return false;
    }

    return true;
}
