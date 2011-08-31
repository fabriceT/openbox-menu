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

#include "openbox-menu.h"

gchar *terminal_cmd = "xterm -e";
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


// return all the string content before %
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


guint
app_is_visible(MenuCacheApp *app, guint32 de_flag)
{
	gint32 flags = menu_cache_app_get_show_flags (app);

	if (flags < 0)
		return !(- flags & de_flag);
	else
		return menu_cache_app_get_is_visible(MENU_CACHE_APP(app), de_flag);
}


gchar *
get_icon_path (const gchar* name)
{
	GtkIconInfo *icon_info;
	gchar* icon = NULL;
	gchar* tmp_name = NULL;
	guint i;

	if (g_path_is_absolute (name))
		return g_strdup (name);

	/*  We remove the file extension as gtk_icon_theme_lookup_icon can't
	 *  lookup a theme icon for, ie, 'geany.png'. It has to be 'geany'.
	 */
	for (i = strlen(name); i > 1; i--)
	{
			if (name[i] == '.')
			{
				tmp_name = strndup (name, i);
				break;
			}
	}

	if (tmp_name)
	{
		icon_info = gtk_icon_theme_lookup_icon (icon_theme, tmp_name, 16, GTK_ICON_LOOKUP_NO_SVG);
		g_free (tmp_name);
	}
	else
		icon_info = gtk_icon_theme_lookup_icon ( icon_theme, name, 16, GTK_ICON_LOOKUP_NO_SVG);

	if (icon_info)
	{
		icon = g_strdup (gtk_icon_info_get_filename (icon_info));
		gtk_icon_info_free (icon_info);
		return icon;
	}

	return NULL;
}


static void
display_menu (MenuCache* menu, gpointer userdata)
{
	MenuCacheDir * dir = menu_cache_get_root_dir (menu);
	if (G_UNLIKELY(dir == NULL))
	{
		g_warning ("Can't get menu root dir");
		return;
	}

	// we get the number of entries found in menu
	// No need to free the list, it's menu-cache internal.
	GSList *l = menu_cache_dir_get_children (dir);
	if (g_slist_length (l) != 0)
		generate_openbox_menu(dir);
	else
		g_print ("<item label=\"Menu not found. Please specify a menu specification file.\"></item>");
}


int
main (int argc, char **argv)
{
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
	MenuCache *menu_cache = menu_cache_lookup_sync (app_menu?*app_menu:"applications.menu");
	if (! menu_cache )
	{
		g_warning ("Cannot connect to menu-cache :/");
		return 1;
	}

	reload_notify_id = menu_cache_add_reload_notify (menu_cache, (GFunc) display_menu, NULL);
	menu_cache_reload (menu_cache);
	g_print ("</openbox_pipe_menu>");
	menu_cache_unref (menu_cache);

	return 0;
}
