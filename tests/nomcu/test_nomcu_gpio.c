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

/* Maximum valid pin for nomcu implementation */
#define TEST_VALID_PIN      MCUI_PIN1
#define TEST_INVALID_PIN    (MCUI_PIN10 + 1)

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
    expect_assert_failure(mcui_gpio_configure(TEST_VALID_PIN, NULL));
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
    
    mcui_gpio_configure(MCUI_PIN1, &input_cfg);
    
    /* Test output mode configuration */
    mcui_gpio_cfg_t output_cfg = {
        .mode = MCUI_GPIO_MODE_OUTPUT,
        .pull = MCUI_GPIO_PULL_NO,
        .int_mode = MCUI_GPIO_INT_MODE_NONE,
        .callback = NULL,
        .sense = false
    };
    
    mcui_gpio_configure(MCUI_PIN2, &output_cfg);
    
    /* Test output and verify set/get */
    mcui_gpio_set(MCUI_PIN2, true);
    assert_true(mcui_gpio_get(MCUI_PIN2));
    
    mcui_gpio_set(MCUI_PIN2, false);
    assert_false(mcui_gpio_get(MCUI_PIN2));
    
    /* Test with pull-down */
    mcui_gpio_cfg_t pulldown_cfg = {
        .mode = MCUI_GPIO_MODE_INPUT,
        .pull = MCUI_GPIO_PULL_DOWN,
        .int_mode = MCUI_GPIO_INT_MODE_NONE,
        .callback = NULL,
        .sense = false
    };
    
    mcui_gpio_configure(MCUI_PIN3, &pulldown_cfg);
    
    /* Test disable */
    mcui_gpio_disable(MCUI_PIN3);
    
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
