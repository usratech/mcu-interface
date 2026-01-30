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

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "mcui_gpio.h"

/* Maximum valid pin for nomcu implementation (matches NOMCU_PIN_NUMBER_MAX = 16) */
#define TEST_PIN_0           0
#define TEST_PIN_1           1
#define TEST_PIN_2           2
#define TEST_PIN_3           3
#define TEST_MAX_VALID_PIN   15          /* Last valid pin (0-15) */
#define TEST_INVALID_PIN     16          /* First invalid pin */

/**************************************************************************************************
 *                                          mcui_gpio_configure tests
 *************************************************************************************************/

static void test_mcui_gpio_configure_bad_pin(void **state)
{
    (void)state;
    
    mcui_gpio_cfg_t cfg = {
        .mode = MCUI_GPIO_MODE_OUTPUT,
        .pull = MCUI_GPIO_PULL_NO,
        .int_mode = MCUI_GPIO_INT_MODE_NONE,
        .callback = NULL,
        .sense = false
    };
    
    /* Test that invalid pin triggers assertion */
    expect_assert_failure(mcui_gpio_configure(TEST_INVALID_PIN, &cfg));
}

static void test_mcui_gpio_configure_null_cfg(void **state)
{
    (void)state;
    
    /* Test that NULL cfg triggers assertion */
    expect_assert_failure(mcui_gpio_configure(TEST_PIN_0, NULL));
}

static void test_mcui_gpio_configure_ok(void **state)
{
    (void)state;
    
    /* Test input mode configuration */
    mcui_gpio_cfg_t input_cfg = {
        .mode = MCUI_GPIO_MODE_INPUT,
        .pull = MCUI_GPIO_PULL_UP,
        .int_mode = MCUI_GPIO_INT_MODE_NONE,
        .callback = NULL,
        .sense = false
    };
    
    mcui_gpio_configure(TEST_PIN_1, &input_cfg);
    
    /* Test output mode configuration */
    mcui_gpio_cfg_t output_cfg = {
        .mode = MCUI_GPIO_MODE_OUTPUT,
        .pull = MCUI_GPIO_PULL_NO,
        .int_mode = MCUI_GPIO_INT_MODE_NONE,
        .callback = NULL,
        .sense = false
    };
    
    mcui_gpio_configure(TEST_PIN_2, &output_cfg);
    
    /* Test output and verify set/get */
    mcui_gpio_set(TEST_PIN_2, true);
    assert_true(mcui_gpio_get(TEST_PIN_2));
    
    mcui_gpio_set(TEST_PIN_2, false);
    assert_false(mcui_gpio_get(TEST_PIN_2));
    
    /* Test with pull-down */
    mcui_gpio_cfg_t pulldown_cfg = {
        .mode = MCUI_GPIO_MODE_INPUT,
        .pull = MCUI_GPIO_PULL_DOWN,
        .int_mode = MCUI_GPIO_INT_MODE_NONE,
        .callback = NULL,
        .sense = false
    };
    
    mcui_gpio_configure(TEST_PIN_3, &pulldown_cfg);
    
    /* Test disable */
    mcui_gpio_disable(TEST_PIN_3);
    
    assert_true(1);
}

int main(void)
{

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_mcui_gpio_configure_bad_pin),
        cmocka_unit_test(test_mcui_gpio_configure_null_cfg),
        cmocka_unit_test(test_mcui_gpio_configure_ok)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
