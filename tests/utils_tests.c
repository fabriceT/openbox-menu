//#include <stdio.h>

#include <glib.h>
//#include <string.h>

#include "../src/openbox-menu.h"



void test_safename1()
{
	char* filename = safe_name("foobar");

	g_assert_cmpstr(filename, ==, "foobar");
}

void test_safename2()
{
	char* filename = safe_name("&<>\"");

	g_assert_cmpstr(filename, ==, "&amp;&lt;&gt;&quot;");
}


void test_safename3()
{
	char* filename = safe_name(NULL);

	g_assert (filename == NULL);
}


void test_application_menu_with_prefix()
{
	g_setenv("XDG_MENU_PREFIX", "mytest-", TRUE);
	gchar* result = get_default_application_menu();

	g_assert_cmpstr(result, ==, "mytest-applications.menu");
}

void test_application_menu_without_prefix()
{
	g_unsetenv("XDG_MENU_PREFIX");
	gchar* result = get_default_application_menu();

	g_assert_cmpstr(result, ==, "applications.menu");
}


