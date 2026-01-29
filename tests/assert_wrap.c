/**
 * @file assert_wrap.c
 * @author Ramil Tenishev (tenishevram@gmail.com)
 * @brief Linker wrapper for __assert_fail to enable cmocka expect_assert_failure
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

/**
 * @brief Wrapper for __assert_fail that redirects to cmocka's mock_assert
 * 
 * This function is called instead of the real __assert_fail when linking with
 * -Wl,--wrap=__assert_fail. It allows cmocka's expect_assert_failure() to work.
 * 
 * @param assertion The assertion expression string
 * @param file Source file where assertion failed
 * @param line Line number where assertion failed
 * @param function Function name where assertion failed
 */
void __wrap___assert_fail(const char *assertion, const char *file,
                          unsigned int line, const char *function)
{
    (void)function;
    mock_assert(0, assertion, file, line);
}
