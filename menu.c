//      openbox-menu - a dynamic menu for openbox
//      Copyright (C) 2010,2011 mimas <mimasgpc@free.fr>
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; version 3 of the License.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

#include <stdio.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <menu-cache.h>

#define VERSION "0.3.6.2"

gchar *terminal_cmd = "sakura -e";
guint32 show_flag = 0; // it was set to N_KNOWN_DESKTOPS (wrong value as it included KDE and GNOME).
gpointer reload_notify_id = NULL;
gboolean comment_name = FALSE;
gboolean no_icon = FALSE;
GtkIconTheme* icon_theme = NULL;

// Convert &,<,> and " to html entities
char *
sanitize (const char *name)
{
	if (name == NULL) return NULL;

	GString *cmd = g_string_sized_new (256);

	for (;*name; ++name)
	{
		switch(*name)
		{
			case '&':
				g_string_append (cmd, "&amp;");
				break;
			case '<':
				g_string_append (cmd, "&lt;");
				break;
			case '>':
				g_string_append (cmd, "&gt;");
				break;
			case '"':
				g_string_append (cmd, "&quote;");
				break;
			default:
				g_string_append_c (cmd, *name);
		}
	}
	return g_string_free (cmd, FALSE);
}


GtkIconInfo *
get_icon_path (const gchar* name)
{
	GtkIconInfo *icon_info;
	icon_info  = gtk_icon_theme_lookup_icon ( icon_theme, name, 16, GTK_ICON_LOOKUP_NO_SVG);

	return icon_info;
}


char *
clean_exec (const char* exec)
{
	GString* cmd = g_string_sized_new (256);

	for (;*exec; ++exec)
	{
		if (G_UNLIKELY(*exec == '%'))
			break;
		else
			g_string_append_c (cmd, *exec);
	}
	return g_string_free (cmd, FALSE);
}


static guint
app_is_visible(MenuCacheApp *app, guint32 de_flag)
{
	gint32 flags = menu_cache_app_get_show_flags (app);

	if (flags < 0)
		return !(- flags & de_flag);
	else
		return menu_cache_app_get_is_visible(MENU_CACHE_APP(app), de_flag);
}


static void
openbox_menu_directory_start (MenuCacheApp *dir)
{
	gchar *dir_id;
	gchar *dir_name;
	GtkIconInfo *dir_icon = NULL;

	dir_id = sanitize (menu_cache_item_get_id (MENU_CACHE_ITEM(dir)));
	dir_name = sanitize (menu_cache_item_get_name (MENU_CACHE_ITEM(dir)));

	if (!no_icon)
		dir_icon = get_icon_path (menu_cache_item_get_icon (MENU_CACHE_ITEM(dir)));

	if (dir_icon == NULL || no_icon == TRUE)
	{
		printf("<menu id=\"openbox-%s\"\n"
		       "      label=\"%s\">\n", dir_id, dir_name);
	}
	else
	{
		printf ("<menu id=\"openbox-%s\"\n"
		        "      label=\"%s\"\n"
		        "      icon=\"%s\">\n",
		        dir_id,
		        dir_name,
		        gtk_icon_info_get_filename (dir_icon));
		gtk_icon_info_free (dir_icon);
	}

	g_free (dir_id);
	g_free (dir_name);
}

static void
openbox_menu_directory_end (void)
{
	printf ("</menu>\n");
}

static void
openbox_menu_separator (void)
{
	printf ("<separator />\n");
}

static void
openbox_menu_application (MenuCacheApp *app)
{
	gboolean use_terminal = FALSE;
	gchar *exec_cmd;
	gchar *exec_name;
	GtkIconInfo *exec_icon = NULL;

	use_terminal = menu_cache_app_get_use_terminal(app);
	if (comment_name)
		exec_name = sanitize(menu_cache_item_get_comment (MENU_CACHE_ITEM(app)));

	if (!comment_name || exec_name == NULL)
		exec_name = sanitize(menu_cache_item_get_name (MENU_CACHE_ITEM(app)));

	exec_cmd = clean_exec (menu_cache_app_get_exec (MENU_CACHE_APP(app)));

	if (!no_icon)
		exec_icon = get_icon_path (menu_cache_item_get_icon (MENU_CACHE_ITEM(app)));

	if (exec_icon == NULL || no_icon == TRUE)
		printf("<item label=\"%s\">\n", exec_name);
	else
		printf("<item label=\"%s\" icon=\"%s\">\n", exec_name, gtk_icon_info_get_filename (exec_icon));

	printf("  <action name=\"Execute\"><command>\n"
	       "    <![CDATA[%s %s]]>\n"
	       "  </command></action>\n"
	       "</item>\n",
	       (use_terminal)?terminal_cmd:"",
	       exec_cmd);

	if (exec_icon)
		gtk_icon_info_free (exec_icon);

	g_free (exec_name);
	g_free (exec_cmd);
}

void
generate_menu (MenuCacheDir *dir)
{
	GSList *l = NULL;

	for (l = menu_cache_dir_get_children (dir); l; l = l->next)
		switch ((guint) menu_cache_item_get_type (MENU_CACHE_ITEM(l->data)))
		{
			case MENU_CACHE_TYPE_DIR:
				openbox_menu_directory_start (l->data);
				generate_menu(MENU_CACHE_DIR(l->data));
				openbox_menu_directory_end ();
				break;

			case MENU_CACHE_TYPE_SEP:
				openbox_menu_separator ();
				break;

			case MENU_CACHE_TYPE_APP:
				if (app_is_visible (MENU_CACHE_APP(l->data), show_flag))
					openbox_menu_application (l->data);
		}
}


static void
display_menu (MenuCache* menu, gpointer userdata)
{
	GSList *l = NULL;
	MenuCacheDir *dir = NULL;

	dir = menu_cache_get_root_dir (menu);
	if (dir == NULL)
	{
		g_error ("Can't get menu root dir");
		return;
	}

	// we get the number of entries found in menu
	// No need to free the list, it's menu-cache internal.
	l = menu_cache_dir_get_children (dir);
	if (g_slist_length (l) != 0)
		generate_menu(dir);
	else
		g_print ("<item label=\"Menu not found. Please specify a menu specification file.\"></item>");
}


int
main (int argc, char **argv)
{
	MenuCache *menu_cache;
	gchar **app_menu = NULL; //"applications.menu";
	GOptionContext *context = NULL;
	gboolean show_gnome = FALSE;
	gboolean show_kde = FALSE;
	gboolean show_xfce = FALSE;
	gboolean show_rox = FALSE;
	GOptionEntry entries[] = {
		{ "comment",  'c', 0, G_OPTION_ARG_NONE, &comment_name, "Display comment instead of application name", NULL },
		{ "noicons",  'i', 0, G_OPTION_ARG_NONE, &no_icon, "Don't display icons in menu", NULL },
		{ "terminal", 't', 0, G_OPTION_ARG_STRING, &terminal_cmd, "Terminal command (default sakura -e)", "cmd" },
		{ "gnome",    'g', 0, G_OPTION_ARG_NONE, &show_gnome, "Display GNOME entries", NULL },
		{ "kde",      'k', 0, G_OPTION_ARG_NONE, &show_kde, "Display KDE entries", NULL },
		{ "xfce",     'x', 0, G_OPTION_ARG_NONE, &show_xfce, "Display XFCE entries", NULL },
		{ "rox",      'r', 0, G_OPTION_ARG_NONE, &show_rox, "Display ROX entries", NULL },
		{ G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY, &app_menu, NULL, "[menu]" },
		{NULL}
	};

	setlocale (LC_ALL, "");

	context = g_option_context_new (" - Openbox menu generator " VERSION);
	g_option_context_set_help_enabled (context, TRUE);
	g_option_context_add_main_entries (context, entries, NULL);

	if (!g_option_context_parse (context, &argc, &argv, NULL))
		return 1;

	g_option_context_free (context);

	if (show_gnome)
		show_flag |= SHOW_IN_GNOME;
	if (show_kde)
		show_flag |= SHOW_IN_KDE;
	if (show_xfce)
		show_flag |= SHOW_IN_XFCE;
	if (show_rox)
		show_flag |= SHOW_IN_ROX;


	if (!no_icon)
	{
		gtk_init (&argc, &argv);
		gdk_init(&argc, &argv);
		icon_theme = gtk_icon_theme_get_default ();
	}

	g_print ("<openbox_pipe_menu xmlns=\"http://openbox.org/\""
	         "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
	         "  xsi:schemaLocation=\"http://openbox.org/"
	         "  file:///usr/share/openbox/menu.xsd\">\n");
	// menu_cache_lookup displays a log message to stdout.
	// This is annoying but Openbox doesn't seem to care about.
	menu_cache = menu_cache_lookup (app_menu?*app_menu:"applications.menu");
	if (! menu_cache )
	{
		g_error ("Cannot connect to menu-cache :/");
		return 1;
	}

	reload_notify_id = menu_cache_add_reload_notify (menu_cache, (GFunc) display_menu, NULL);
	menu_cache_reload (menu_cache);
	g_print ("</openbox_pipe_menu>");
	menu_cache_unref (menu_cache);

	return 0;
}
