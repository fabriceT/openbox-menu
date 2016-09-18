#include <stdio.h>

#include <glib.h>

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

/*
void test_safename3()
{
	char* filename = safe_name(NULL);

	g_assert_null(filename);
}
*/
