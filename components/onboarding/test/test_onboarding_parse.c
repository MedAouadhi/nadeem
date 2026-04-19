#include "unity.h"
#include "onboarding.h"
#include <string.h>

TEST_CASE("onboarding_parse_form: basic fields", "[onboarding]")
{
    const char body[] = "ssid=MyNet&password=hello+world&provision_token=abc";
    char s[64] = {0}, p[64] = {0}, t[64] = {0};
    int n = onboarding_parse_form(body, sizeof body - 1, s, sizeof s, p, sizeof p, t, sizeof t);
    TEST_ASSERT_EQUAL(3, n);
    TEST_ASSERT_EQUAL_STRING("MyNet", s);
    TEST_ASSERT_EQUAL_STRING("hello world", p);
    TEST_ASSERT_EQUAL_STRING("abc", t);
}

TEST_CASE("onboarding_parse_form: urlencoded bytes", "[onboarding]")
{
    const char body[] = "ssid=foo%20bar&password=%3Fqux";
    char s[64] = {0}, p[64] = {0}, t[64] = {0};
    int n = onboarding_parse_form(body, sizeof body - 1, s, sizeof s, p, sizeof p, t, sizeof t);
    TEST_ASSERT_EQUAL(2, n);
    TEST_ASSERT_EQUAL_STRING("foo bar", s);
    TEST_ASSERT_EQUAL_STRING("?qux", p);
}
