//      openbox-menu - a dynamic menu for openbox
//      Copyright (C) 2010-12 mimas <mimasgpc@free.fr>
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
#include <glib/gprintf.h>
#ifdef WITH_ICONS
	#include <gtk/gtk.h>
#endif
#include <glib/gi18n.h>
#include <menu-cache.h>
#include <signal.h>
#include <locale.h>
#include <stdlib.h>

#include "openbox-menu.h"

GMainLoop *loop = NULL;
#ifdef WITH_ICONS
	GtkIconTheme *icon_theme;
#endif

/* from lxsession */
void sig_term_handler (int sig)
{
	g_warning ("Aborting");
	g_main_loop_quit (loop);
}

/****f* openbox-menu/get_safe_name
 * FUNCTION
 *   Convert &, <, > and " signs to html entities
 *
 * OUTPUT
 *   A gchar that needs to be freed.
 ****/
gchar *
get_safe_name (const char *name)
{
	g_return_val_if_fail (name != NULL, NULL);

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


/****f* openbox-menu/clean_exec
 * FUNCTION
 *   Remove %f, %F, %u, %U, %i, %c, %k from exec field.
 *   None of theses codes are interesting to manage here.
 *   %i, %c and %k codes are implemented but don't ask why we need them. :)
 *
 * OUTPUT
 *   A gchar that needs to be freed.
 *
 * NOTES
 *   %d, %D, %n, %N, %v and %m are deprecated and should be removed.
 *
 * SEE ALSO
 *   http://standards.freedesktop.org/desktop-entry-spec/latest/ar01s06.html
 ****/
gchar *
clean_exec (MenuCacheApp *app)
{
	gchar *filepath = NULL;
	const char *exec = menu_cache_app_get_exec (MENU_CACHE_APP(app));
	GString *cmd = g_string_sized_new (64);

	for (;*exec; ++exec)
	{
		if (*exec == '%')
		{
			++exec;
			switch (*exec)
			{
				/* useless and commonly used codes */
				case 'u':
				case 'U':
				case 'f':
				case 'F': break;
				/* deprecated codes */
				case 'd':
				case 'D':
				case 'm':
				case 'n':
				case 'N':
				case 'v': break;
				/* Other codes, more or less pointless to implement */
				case 'c':
					g_string_append (cmd, menu_cache_item_get_name (MENU_CACHE_ITEM(app)));
					break;
#if WITH_ICONS
				case 'i':
					if (get_item_icon_path (MENU_CACHE_ITEM(app)))
					{
						g_string_append_printf (cmd, "--icon %s",
						    get_item_icon_path (MENU_CACHE_ITEM(app)));
					}
					break;
#endif
				case 'k':
					filepath = menu_cache_item_get_file_path (MENU_CACHE_ITEM(app));
					if (filepath)
					{
						g_string_append (cmd, filepath);
						g_free (filepath);
					}
					break;
				/* It was not in the freedesktop specification. */
				default:
					g_string_append_c (cmd, '%');
					g_string_append_c (cmd, *exec);
					break;

			}
		}
		else
			g_string_append_c (cmd, *exec);
	}
	return g_strchomp (g_string_free (cmd, FALSE));
}

#if WITH_ICONS
/****f* openbox-menu/get_item_icon_path
 * OUTPUT
 *   return the path for the themed icon if item.
 *   If no icon found, it returns the "empty" icon path.
 *
 *   The returned string should be freed when no longer needed
 *
 * NOTES
 *   Imlib2, used by OpenBox to display icons, doesn't load SVG graphics.
 *   We have to use GTK_ICON_LOOKUP_NO_SVG flag to look up icons.
 *
 * TODO
 *   The "2nd fallback" is annoying, I have to think about this.
 ****/
gchar *
get_item_icon_path (MenuCacheItem *item)
{
	GtkIconInfo *icon_info = NULL;
	gchar *icon = NULL;
	gchar *tmp_name = NULL;

	const gchar *name = menu_cache_item_get_icon (MENU_CACHE_ITEM(item));

	if (name)
	{
		if (g_path_is_absolute (name))
			return g_strdup (name);

		/*  We remove the file extension as gtk_icon_theme_lookup_icon can't
		 *  lookup a theme icon for, ie, 'geany.png'. It has to be 'geany'.
		 */
		tmp_name = strndup (name, strrchr (name, '.') - name);
	#ifdef WITH_SVG
		icon_info = gtk_icon_theme_lookup_icon (icon_theme, tmp_name, 16, GTK_ICON_LOOKUP_GENERIC_FALLBACK);
	#else	
		icon_info = gtk_icon_theme_lookup_icon (icon_theme, tmp_name, 16, GTK_ICON_LOOKUP_NO_SVG | GTK_ICON_LOOKUP_GENERIC_FALLBACK);	
	#endif
		g_free (tmp_name);
	}

	if (!icon_info) /* 2nd fallback */
		icon_info = gtk_icon_theme_lookup_icon (icon_theme, "empty", 16, GTK_ICON_LOOKUP_NO_SVG);

	icon = g_strdup (gtk_icon_info_get_filename (icon_info));
	gtk_icon_info_free (icon_info);

	return icon;
}
#endif /* WITH_ICONS */


guint
app_is_visible(MenuCacheApp *app, guint32 de_flag)
{
	gint32 flags = menu_cache_app_get_show_flags (app);

	if (flags < 0)
		return !(- flags & de_flag);
	else
		return menu_cache_app_get_is_visible(MENU_CACHE_APP(app), de_flag);
}


/****f* openbox-menu/menu_directory
 * FUNCTION
 *   create a menu entry for a directory.
 *
 * NOTES
 *   this menu entry has to be closed by "</menu>".
 ****/
void
menu_directory (MenuCacheApp *dir, OB_Menu *context)
{
	gchar *dir_id = get_safe_name (menu_cache_item_get_id (MENU_CACHE_ITEM(dir)));
	gchar *dir_name = get_safe_name (menu_cache_item_get_name (MENU_CACHE_ITEM(dir)));

#ifdef WITH_ICONS
	if (!context->no_icons)
	{
		gchar *dir_icon = get_item_icon_path (MENU_CACHE_ITEM(dir));

		g_string_append_printf (context->builder,
		    "<menu id=\"openbox-%s\" label=\"%s\" icon=\"%s\">\n",
		    dir_id, dir_name, dir_icon);
		g_free (dir_icon);
	}
	else
#endif
	{
		g_string_append_printf (context->builder,
	      "<menu id=\"openbox-%s\" label=\"%s\">\n",
	      dir_id, dir_name);
	}

	g_free (dir_id);
	g_free (dir_name);
}


/****f* openbox-menu/menu_application
 * FUNCTION
 *   create a menu entry for an application.
 ****/
void
menu_application (MenuCacheApp *app, OB_Menu *context)
{
	gchar *exec_name = NULL;
	gchar *exec_icon = NULL;
	gchar *exec_cmd = NULL;

	/* is comment (description) or name displayed ? */
	if (context->comment && menu_cache_item_get_comment (MENU_CACHE_ITEM(app)))
		exec_name = get_safe_name (menu_cache_item_get_comment (MENU_CACHE_ITEM(app)));
	else
		exec_name = get_safe_name (menu_cache_item_get_name (MENU_CACHE_ITEM(app)));

	exec_cmd = clean_exec (app);

#ifdef WITH_ICONS
	if (!context->no_icons)
	{
		exec_icon = get_item_icon_path (MENU_CACHE_ITEM(app));
		g_string_append_printf (context->builder,
	      "<item label=\"%s\" icon=\"%s\"><action name=\"Execute\">",
	      exec_name,
	      exec_icon);
	}
	else
#endif
	{
		g_string_append_printf (context->builder,
	      "<item label=\"%s\"><action name=\"Execute\">",
	      exec_name);
	}

	if (context->sn && menu_cache_app_get_use_sn (app))
		g_string_append (context->builder,
	        "<startupnotify><enabled>yes</enabled></startupnotify>");

	if (menu_cache_app_get_use_terminal (app))
		g_string_append_printf (context->builder,
	        "<command><![CDATA[%s %s]]></command>\n</action></item>\n",
	        context->terminal_cmd,
	        exec_cmd);
	else
		g_string_append_printf (context->builder,
	        "<command><![CDATA[%s]]></command>\n</action></item>\n",
	        exec_cmd);

	g_free (exec_name);
	g_free (exec_icon);
	g_free (exec_cmd);
}


/****f* openbox-menu/generate_openbox_menu
 * FUNCTION
 *   main routine of menu creation.
 *
 * NOTES
 *   It calls itself when 'dir' type is MENU_CACHE_TYPE_DIR.
 ****/
void
generate_openbox_menu (MenuCacheDir *dir, OB_Menu *context)
{
	GSList *l = NULL;

	for (l = menu_cache_dir_get_children (dir); l; l = l->next)
		switch ((guint) menu_cache_item_get_type (MENU_CACHE_ITEM(l->data)))
		{
			case MENU_CACHE_TYPE_DIR:
				menu_directory (l->data, context);
				generate_openbox_menu (MENU_CACHE_DIR(l->data), context);
				g_string_append (context->builder, "</menu>\n");
				break;

			case MENU_CACHE_TYPE_SEP:
				g_string_append (context->builder, "<separator />\n");
				break;

			case MENU_CACHE_TYPE_APP:
				if (app_is_visible (MENU_CACHE_APP(l->data), 0))
					menu_application (l->data, context);
		}
}


/****f* openbox-menu/display_menu
 * FUNCTION
 *   it begins and closes the menu content, write it into a file or
 *   display it.
 *
 * INPUTS
 *   * menu
 *   * file, the filename where the menu content should be written to.
 *     If file is 'NULL' then the menu content is displayed.
 *
 * NOTES
 *   A 16 KiB GString is allocated for the content of the pipemenu.
 *   This should be enough prevent too many allocations while building
 *   the XML.
 *
 *   The size of the XML file created is around 8 KB in my computer but
 *   I don't have a lot of applications installed.
 ****/
void
display_menu (MenuCache *menu, OB_Menu *context)
{
	MenuCacheDir *dir = menu_cache_get_root_dir (menu);
	if (G_UNLIKELY(dir == NULL))
	{
		g_warning ("Can't get menu root dir");
		return;
	}

	GSList *l = menu_cache_dir_get_children (dir);

	if (g_slist_length (l) != 0) {
		context->builder = g_string_sized_new (16 * 1024);
		g_string_append (context->builder,
		    "<openbox_pipe_menu xmlns=\"http://openbox.org/\""
	        "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
	        "  xsi:schemaLocation=\"http://openbox.org/"
	        "  file:///usr/share/openbox/menu.xsd\">\n");
		generate_openbox_menu(dir, context);
		g_string_append (context->builder, "</openbox_pipe_menu>");

		gchar *buff = g_string_free (context->builder, FALSE);

		/* Has menu content to be saved in a file ? */
		if (context->output)
		{
			if (!g_file_set_contents (context->output, buff, -1, NULL))
				g_warning ("Can't write to %s\n", context->output);
			else
				g_message ("wrote to %s", context->output);
		}
		else /* No, so it's displayed on screen */
			g_print ("%s", buff);

		g_free (buff);
	}
	else
		g_warning ("Cannot create menu, check if the .menu file is correct");
}


gchar *
get_application_menu (void)
{
	gchar menu[APPMENU_SIZE];

	gchar *xdg_prefix = getenv("XDG_MENU_PREFIX");
	if (xdg_prefix)
	{
		g_snprintf (menu, APPMENU_SIZE, "%sapplications.menu", xdg_prefix);
	}
	else
		g_strlcpy (menu, "applications.menu", APPMENU_SIZE);

	return strdup (menu);
}


gboolean
check_application_menu (gchar *menu)
{
	gchar *menu_path = g_build_filename ("/etc","xdg", "menus", menu, NULL);
	if (!g_file_test (menu_path, G_FILE_TEST_EXISTS))
	{
		g_print ("File %s doesn't exists. Can't create menu\n", menu_path);
		g_free (menu_path);
		return FALSE;
	}
	else
	{
		g_free (menu_path);
		return TRUE;
	}
}


int
main (int argc, char **argv)
{

	gpointer reload_notify_id = NULL;
	GError *error = NULL;
	gchar  *menu = NULL;
	OB_Menu ob_context = { 0 };

	gboolean  show_gnome = FALSE;
	gboolean  show_kde = FALSE;
	gboolean  show_xfce = FALSE;
	gboolean  show_rox = FALSE;
	gboolean  persistent = FALSE;
	gchar   **app_menu = NULL;
	gchar    *output = NULL;
	gchar    *terminal = "xterm -e";
	/*
	 * TODO: Registered OnlyShowIn Environments
	 *  Ref: http://standards.freedesktop.org/menu-spec/latest/apb.html
	 *
	 * GNOME GNOME Desktop
	 * KDE   KDE Desktop
	 * LXDE  LXDE Desktop
	 * MATE  MATÉ Desktop
	 * Razor Razor-qt Desktop
	 * ROX   ROX Desktop
	 * TDE   Trinity Desktop
	 * Unity Unity Shell
	 * XFCE  XFCE Desktop
	 * Old   Legacy menu systems
	 */
	GOptionEntry entries[] = {
		{ "comment",   'c', 0, G_OPTION_ARG_NONE, &ob_context.comment,
		  "Show generic name instead of application name", NULL },
		{ "terminal",  't', 0, G_OPTION_ARG_STRING, &terminal,
		  "Terminal command (default xterm -e)", "cmd" },
		{ "gnome",     'g', 0, G_OPTION_ARG_NONE, &show_gnome,
		  "Show GNOME entries", NULL },
		{ "kde",       'k', 0, G_OPTION_ARG_NONE, &show_kde,
		  "Show KDE entries", NULL },
		{ "xfce",      'x', 0, G_OPTION_ARG_NONE, &show_xfce,
		  "Show XFCE entries", NULL },
		{ "rox",       'r', 0, G_OPTION_ARG_NONE, &show_rox,
		  "Show ROX entries", NULL },
		{ "persistent",'p', 0, G_OPTION_ARG_NONE, &persistent,
		  "stay active",    NULL },
		{ "sn",        's', 0, G_OPTION_ARG_NONE, &ob_context.sn,
		  "Enable startup notification", NULL },
		{ "output",    'o', 0, G_OPTION_ARG_STRING, &output,
		  "file to write data to", NULL },
		{ "noicons", 'i',   0, G_OPTION_ARG_NONE, &ob_context.no_icons,
		  "Don't display icons in menu", NULL },
		{ G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY, &app_menu,
		  NULL, "[file.menu]" },
		{NULL}
	};
	GOptionContext *help_context = NULL;

	setlocale (LC_ALL, "");

	help_context = g_option_context_new (" - Openbox menu generator " VERSION);
	g_option_context_set_help_enabled (help_context, TRUE);
	g_option_context_add_main_entries (help_context, entries, NULL);
	g_option_context_parse (help_context, &argc, &argv, &error);

	if (error)
	{
		g_print ("%s\n", error->message);
		g_error_free (error);
		return 1;
	}

	g_option_context_free (help_context);

#ifdef WITH_ICONS
	gtk_init (&argc, &argv);
	icon_theme = gtk_icon_theme_get_default ();
#endif

	if (output)
		ob_context.output = g_build_filename (g_get_user_cache_dir (), output, NULL);
	else
		ob_context.output =  NULL;

	if (terminal)
		ob_context.terminal_cmd = terminal;

	if (show_gnome) ob_context.show_flag |= SHOW_IN_GNOME;
	if (show_kde)   ob_context.show_flag |= SHOW_IN_KDE;
	if (show_xfce)  ob_context.show_flag |= SHOW_IN_XFCE;
	if (show_rox)   ob_context.show_flag |= SHOW_IN_ROX;

	if (!app_menu)
		menu = get_application_menu ();
	else
		menu = strdup (*app_menu);

	if (!check_application_menu (menu))
		return 1;

	// wait for the menu to get ready
	MenuCache *menu_cache = menu_cache_lookup_sync (menu);
	if (!menu_cache )
	{
		g_free (menu);
		g_warning ("Cannot connect to menu-cache :/");
		return 1;
	}

	// display the menu anyway
	display_menu(menu_cache, &ob_context);

	if (persistent)
	{
		// menucache used to reload the cache after a call to menu_cache_lookup* ()
		// It's not true anymore with version >= 0.4.0.
		reload_notify_id = menu_cache_add_reload_notify (menu_cache,
		                        (MenuCacheReloadNotify) display_menu,
		                        &ob_context);

		// install signals handler
		signal (SIGTERM, sig_term_handler);
		signal (SIGINT, sig_term_handler);

		// run main loop
		loop = g_main_loop_new (NULL, FALSE);
		g_main_loop_run (loop);
		g_main_loop_unref (loop);

		menu_cache_remove_reload_notify (menu_cache, reload_notify_id);
	}

	menu_cache_unref (menu_cache);
	g_free (menu);
	g_free (ob_context.output);

	return 0;
}
