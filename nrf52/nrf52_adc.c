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
#include "nrf_saadc.h"

/**************************************************************************************************
 *                                          Definitions
 *************************************************************************************************/

/* pins: 2,3,4,5 (0,1,2,3) and 28,29,30,31 (4,5,6,7) */
#define PIN_TO_CHANNEL(x) ((x < 6) ? (x - 2) : (x - 24))

#define NRF52_SAADC_CHANNELS 8

/**************************************************************************************************
 *                                  Static function prototypes
 *************************************************************************************************/

static bool check_cfg(mcui_adc_cfg_t *cfg);
static void saadc_event_handler(nrfx_saadc_evt_t const *p_event);
static void timer_event_handler(nrf_timer_event_t event_type, void *p_context);
static int setup_ppi_timer(uint32_t sample_rate_hz);
static void cleanup_ppi_timer(void);

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

static const uint8_t ref_to_internal_ref[MCUI_ADC_REF_COUNT] = {
    [MCUI_ADC_REF_INTERNAL] = NRF_SAADC_REFERENCE_INTERNAL,
    [MCUI_ADC_REF_EXTERNAL] = NRF_SAADC_REFERENCE_VDD4,
};

static mcui_mutex_t s_saadc_mutex;

static mcui_adc_callback_t s_buffer_callback;
static void *s_buffer_ptr;
static size_t s_buffer_size;
static uint8_t s_buffer_num_channels;

static const nrf_drv_timer_t s_timer = NRF_DRV_TIMER_INSTANCE(1);
static nrf_ppi_channel_t s_ppi_channel;
static bool s_timer_initialized;

/**************************************************************************************************
 *                                          Functions
 *************************************************************************************************/

int mcui_adc_sample_single(uint8_t adc, mcui_adc_cfg_t *cfg, int *out_value)
{
    assert(adc != 0);
    assert(check_cfg(cfg));
    assert(out_value);

    if (!s_saadc_mutex) {
        s_saadc_mutex = MCUI_MUTEX_CREATE();
        if (!s_saadc_mutex) {
            return MCUI_ADC_ERROR_UNKNOWN;
        }
    }

    if (MCUI_MUTEX_ACQUIRED_RET_CODE != MCUI_MUTEX_ACQUIRE(s_saadc_mutex)) {
        return MCUI_ADC_ERROR_BUSY;
    }

    nrf_saadc_value_t sample;
    nrfx_err_t err;

    uint8_t pos_ch = PIN_TO_CHANNEL(cfg->positive_pin);

    nrf_saadc_channel_config_t ch_cfg = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(
        ch_to_internal_ch[pos_ch]);
    ch_cfg.gain = gain_to_internal_gain[(uint8_t)cfg->gain];
    ch_cfg.reference = ref_to_internal_ref[cfg->reference];
    ch_cfg.acq_time = NRF_SAADC_ACQTIME_10US;

    if (cfg->mode == MCUI_ADC_MODE_DIFFERENTIAL) {
        uint8_t neg_ch = PIN_TO_CHANNEL(cfg->negative_pin);
        ch_cfg.pin_n = ch_to_internal_ch[neg_ch];
        ch_cfg.mode = NRF_SAADC_MODE_DIFFERENTIAL;
    }

    err = nrfx_saadc_init(NULL, NULL);
    if (err != NRFX_SUCCESS && err != NRFX_ERROR_INVALID_STATE) {
        MCUI_MUTEX_RELEASE(s_saadc_mutex);
        return MCUI_ADC_ERROR_SDK;
    }

    err = nrfx_saadc_channel_init(pos_ch, &ch_cfg);
    if (err != NRFX_SUCCESS) {
        nrfx_saadc_uninit();
        MCUI_MUTEX_RELEASE(s_saadc_mutex);
        return MCUI_ADC_ERROR_SDK;
    }

    nrfx_saadc_resolution_set(bits_to_internal_bits[cfg->resolution]);

    if (cfg->oversample > 0) {
        nrfx_saadc_oversample_set(cfg->oversample);
    }

    err = nrfx_saadc_sample_convert(pos_ch, &sample);
    if (err != NRFX_SUCCESS) {
        nrfx_saadc_channel_uninit(pos_ch);
        nrfx_saadc_uninit();
        MCUI_MUTEX_RELEASE(s_saadc_mutex);
        return MCUI_ADC_ERROR_SDK;
    }

    *out_value = (int)sample;

    nrfx_saadc_channel_uninit(pos_ch);
    nrfx_saadc_uninit();
    MCUI_MUTEX_RELEASE(s_saadc_mutex);

    return MCUI_ADC_ERROR_NONE;
}

int mcui_adc_sample_buffer(uint8_t adc, mcui_adc_cfg_t *cfg, void *buffer, size_t size)
{
    assert(adc != 0);
    assert(check_cfg(cfg));
    assert(buffer);
    assert(size);

    if (!s_saadc_mutex) {
        s_saadc_mutex = MCUI_MUTEX_CREATE();
        if (!s_saadc_mutex) {
            return MCUI_ADC_ERROR_UNKNOWN;
        }
    }

    if (MCUI_MUTEX_ACQUIRED_RET_CODE != MCUI_MUTEX_ACQUIRE(s_saadc_mutex)) {
        return MCUI_ADC_ERROR_BUSY;
    }

    nrfx_err_t err;

    s_buffer_callback = cfg->callback;
    s_buffer_ptr = buffer;
    s_buffer_size = size;

    nrfx_saadc_config_t saadc_cfg = NRFX_SAADC_DEFAULT_CONFIG;
    saadc_cfg.resolution = bits_to_internal_bits[cfg->resolution];

    if (cfg->oversample > 0) {
        saadc_cfg.oversample = cfg->oversample;
    }

    err = nrfx_saadc_init(&saadc_cfg, saadc_event_handler);
    if (err != NRFX_SUCCESS && err != NRFX_ERROR_INVALID_STATE) {
        MCUI_MUTEX_RELEASE(s_saadc_mutex);
        return MCUI_ADC_ERROR_SDK;
    }

    /* Configure scan channels */
    uint8_t num_channels = cfg->channels_in_use;
    if (num_channels > MCUI_ADC_SCAN_MAX_CHANNELS) {
        num_channels = MCUI_ADC_SCAN_MAX_CHANNELS;
    }

    for (uint8_t i = 0; i < num_channels; i++) {
        uint8_t ch = PIN_TO_CHANNEL(cfg->scan_pins[i]);

        nrf_saadc_channel_config_t ch_cfg = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(
            ch_to_internal_ch[ch]);
        ch_cfg.gain = gain_to_internal_gain[(uint8_t)cfg->gain];
        ch_cfg.reference = ref_to_internal_ref[cfg->reference];
        ch_cfg.acq_time = NRF_SAADC_ACQTIME_10US;

        err = nrfx_saadc_channel_init(i, &ch_cfg);
        if (err != NRFX_SUCCESS) {
            /* Clean up previously initialized channels */
            for (uint8_t j = 0; j < i; j++) {
                nrfx_saadc_channel_uninit(j);
            }
            nrfx_saadc_uninit();
            MCUI_MUTEX_RELEASE(s_saadc_mutex);
            return MCUI_ADC_ERROR_SDK;
        }
    }

    /* Store num_channels so the event handler can clean up */
    s_buffer_num_channels = num_channels;

    err = nrfx_saadc_buffer_convert((nrf_saadc_value_t *)buffer,
                                     size / sizeof(nrf_saadc_value_t));
    if (err != NRFX_SUCCESS) {
        for (uint8_t i = 0; i < num_channels; i++) {
            nrfx_saadc_channel_uninit(i);
        }
        nrfx_saadc_uninit();
        MCUI_MUTEX_RELEASE(s_saadc_mutex);
        return MCUI_ADC_ERROR_SDK;
    }

    /* Set up PPI + timer for continuous sampling at specified rate */
    int ret = setup_ppi_timer(cfg->sample_rate_hz);
    if (ret != MCUI_ADC_ERROR_NONE) {
        for (uint8_t i = 0; i < num_channels; i++) {
            nrfx_saadc_channel_uninit(i);
        }
        nrfx_saadc_uninit();
        MCUI_MUTEX_RELEASE(s_saadc_mutex);
        return ret;
    }

    /* Mutex is released in saadc_event_handler when conversion completes */
    return MCUI_ADC_ERROR_NONE;
}

/**************************************************************************************************
 *                                          Static functions
 *************************************************************************************************/

static void timer_event_handler(nrf_timer_event_t event_type, void *p_context)
{
    (void)event_type;
    (void)p_context;
    /* Empty handler - PPI handles triggering SAADC directly */
}

static int setup_ppi_timer(uint32_t sample_rate_hz)
{
    ret_code_t err;

    /* Initialize timer */
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_cfg.frequency = NRF_TIMER_FREQ_1MHz;

    err = nrf_drv_timer_init(&s_timer, &timer_cfg, timer_event_handler);
    if (err != NRF_SUCCESS && err != NRF_ERROR_INVALID_STATE) {
        return MCUI_ADC_ERROR_SDK;
    }
    s_timer_initialized = true;

    /* Calculate timer ticks for desired sample rate */
    uint32_t ticks = 1000000 / sample_rate_hz;
    nrf_drv_timer_extended_compare(&s_timer,
                                    NRF_TIMER_CC_CHANNEL0,
                                    ticks,
                                    NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
                                    false);

    /* Initialize PPI */
    err = nrf_drv_ppi_init();
    if (err != NRF_SUCCESS && err != NRF_ERROR_MODULE_ALREADY_INITIALIZED) {
        nrf_drv_timer_uninit(&s_timer);
        s_timer_initialized = false;
        return MCUI_ADC_ERROR_SDK;
    }

    /* Allocate PPI channel */
    err = nrf_drv_ppi_channel_alloc(&s_ppi_channel);
    if (err != NRF_SUCCESS) {
        nrf_drv_timer_uninit(&s_timer);
        s_timer_initialized = false;
        return MCUI_ADC_ERROR_SDK;
    }

    /* Connect timer COMPARE event to SAADC SAMPLE task */
    uint32_t timer_compare_event = nrf_drv_timer_compare_event_address_get(&s_timer,
                                                                            NRF_TIMER_CC_CHANNEL0);
    uint32_t saadc_sample_task = nrf_saadc_task_address_get(NRF_SAADC, NRF_SAADC_TASK_SAMPLE);

    err = nrf_drv_ppi_channel_assign(s_ppi_channel, timer_compare_event, saadc_sample_task);
    if (err != NRF_SUCCESS) {
        nrf_drv_ppi_channel_free(s_ppi_channel);
        nrf_drv_timer_uninit(&s_timer);
        s_timer_initialized = false;
        return MCUI_ADC_ERROR_SDK;
    }

    /* Enable PPI channel */
    err = nrf_drv_ppi_channel_enable(s_ppi_channel);
    if (err != NRF_SUCCESS) {
        nrf_drv_ppi_channel_free(s_ppi_channel);
        nrf_drv_timer_uninit(&s_timer);
        s_timer_initialized = false;
        return MCUI_ADC_ERROR_SDK;
    }

    /* Start timer */
    nrf_drv_timer_enable(&s_timer);

    return MCUI_ADC_ERROR_NONE;
}

static void cleanup_ppi_timer(void)
{
    nrf_drv_ppi_channel_disable(s_ppi_channel);
    nrf_drv_ppi_channel_free(s_ppi_channel);

    if (s_timer_initialized) {
        nrf_drv_timer_disable(&s_timer);
        nrf_drv_timer_uninit(&s_timer);
        s_timer_initialized = false;
    }
}

static void saadc_event_handler(nrfx_saadc_evt_t const *p_event)
{
    if (p_event->type == NRFX_SAADC_EVT_DONE) {
        cleanup_ppi_timer();

        for (uint8_t i = 0; i < s_buffer_num_channels; i++) {
            nrfx_saadc_channel_uninit(i);
        }
        nrfx_saadc_uninit();
        MCUI_MUTEX_RELEASE(s_saadc_mutex);

        if (s_buffer_callback) {
            s_buffer_callback(s_buffer_ptr, s_buffer_size);
        }
    }
}

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
