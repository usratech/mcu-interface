/**
 * @file test_nomcu_gpio.c
 * @author Ramil Tenishev (tenishevram@gmail.com)
 * @brief Unit tests for nomcu GPIO implementation
 * @version 0.1
 * @date 2026-01-29
 * 
 * @copyright USRA Technologies. Copyright (c) 2026
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
