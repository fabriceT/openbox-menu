#include <glib.h>
#include <string.h>

#include "../src/openbox-menu.h"

/*
typedef struct {
	gchar    *output;
	guint32   show_flag;
	GString  *builder;      /
	gchar    *terminal_cmd;
	gboolean  comment;
	gboolean  sn;
	gboolean  no_icons;
	gboolean  persistent;
	gchar    *menu_file;
	gchar    *template;
	guint     code;
} OB_Menu;
*/


void test_set_desktop_flag ()
{
	OB_Menu* ctx = context_new();
	context_set_desktop_flag (ctx, SHOW_IN_XFCE);

	gint flag = context_get_desktop_flag(ctx);

	g_assert_cmpint(flag, ==, SHOW_IN_XFCE);
}


void test_add_desktop_flag ()
{
	OB_Menu* ctx = context_new();
	context_set_desktop_flag (ctx, SHOW_IN_XFCE);
	context_add_desktop_flag (ctx, SHOW_IN_KDE);

	gint flag = context_get_desktop_flag(ctx);

	g_assert_cmpint(flag, ==, SHOW_IN_XFCE | SHOW_IN_KDE);
}


void test_set_terminal_cmd()
{
	OB_Menu* ctx = context_new();
	context_set_terminal_cmd (ctx, "xterm");

	gchar* cmd = context_get_terminal_cmd(ctx);

	g_assert_cmpstr(cmd, ==, "xterm");
}


void test_persistent_true ()
{
	OB_Menu* ctx = context_new();
	context_set_persistent(ctx, TRUE);

	gboolean mode = context_get_persistent(ctx);

	g_assert_true(mode);
}


void test_persistent_false ()
{
	OB_Menu* ctx = context_new();

	gboolean mode = context_get_persistent(ctx);

	g_assert_false (mode);
}
