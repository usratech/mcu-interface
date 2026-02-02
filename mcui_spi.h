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

enum __mcui_spi_instance {
    MCUI_SPI0 = 0,
    MCUI_SPI1,
    MCUI_SPI2,
    /* at the end */
    MCUI_SPI_COUNT
};

typedef struct {
    uint16_t mosi_pin;
    uint16_t miso_pin;
    uint16_t sck_pin;
    uint16_t cs_pin;
    uint32_t frequency_hz;
    uint8_t spi;
    uint8_t mode;
} mcui_spi_t;

/**************************************************************************************************
 *                                          Functions
 *************************************************************************************************/

int mcui_spi_transfer(mcui_spi_t spi, const uint8_t* tx_data, uint8_t* rx_data, size_t length);

#ifdef __cplusplus
}
#endif
