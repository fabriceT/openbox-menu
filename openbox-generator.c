//      openbox-generator.c - this file is part of openbox-menu
//      Copyright (C) 2011 mimas <mimasgpc@free.fr>
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU Lesser General Public License as published
//      by the Free Software Foundation; version 3.0 only.
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


#include <menu-cache.h>
#include "openbox-menu.h"

extern gboolean no_icon;
extern gboolean comment_name;
extern gboolean show_flag;
extern gchar *terminal_cmd;


static void
menu_directory (MenuCacheApp *dir)
{
	gchar *dir_id = sanitize (menu_cache_item_get_id (MENU_CACHE_ITEM(dir)));
	gchar *dir_name = sanitize (menu_cache_item_get_name (MENU_CACHE_ITEM(dir)));

#ifdef CAN_I_HAZ_ICONS
	gchar *dir_icon = NULL;
	if (no_icon == TRUE || (dir_icon = get_item_icon_path (MENU_CACHE_ITEM(dir))) == NULL)
	{
		g_print ("<menu id=\"openbox-%s\"\n"
		         "      label=\"%s\">\n", dir_id, dir_name);
	}
	else
	{
		g_print ("<menu id=\"openbox-%s\"\n"
		         "      label=\"%s\"\n"
		         "      icon=\"%s\">\n", dir_id, dir_name, dir_icon);
		g_free (dir_icon);
	}
#else
	g_print ("<menu id=\"openbox-%s\"\n"
	         "      label=\"%s\"\n", dir_id, dir_name); 
#endif
	g_free (dir_id);
	g_free (dir_name);
}


static void
menu_application (MenuCacheApp *app)
{
	gchar *exec_name = NULL;

	gboolean use_terminal = menu_cache_app_get_use_terminal(app);

	if (comment_name)
		exec_name = sanitize(menu_cache_item_get_comment (MENU_CACHE_ITEM(app)));

	if (exec_name == NULL)
		exec_name = sanitize(menu_cache_item_get_name (MENU_CACHE_ITEM(app)));

	gchar *exec_cmd = clean_exec (menu_cache_app_get_exec (MENU_CACHE_APP(app)));

#ifdef CAN_I_HAZ_ICONS
	gchar *exec_icon = NULL;
	if (no_icon == TRUE || (exec_icon = get_item_icon_path (MENU_CACHE_ITEM(app))) == NULL)
		g_print ("<item label=\"%s\">\n", exec_name);
	else
	{
		g_print ("<item label=\"%s\" icon=\"%s\">\n", exec_name, exec_icon);
		g_free (exec_icon);
	}
#else
	g_print ("<item label=\"%s\">\n", exec_name);
#endif

	g_print ("  <action name=\"Execute\"><command>\n"
	         "    <![CDATA[%s %s]]>\n"
	         "  </command></action>\n"
	         "</item>\n",
	         (use_terminal)?terminal_cmd:"",
	         exec_cmd);

	g_free (exec_name);
	g_free (exec_cmd);
}


/* Main loop for openbox menu generator */
void
generate_openbox_menu (MenuCacheDir *dir)
{
	GSList *l = NULL;

	for (l = menu_cache_dir_get_children (dir); l; l = l->next)
		switch ((guint) menu_cache_item_get_type (MENU_CACHE_ITEM(l->data)))
		{
			case MENU_CACHE_TYPE_DIR:
				menu_directory (l->data);
				generate_openbox_menu (MENU_CACHE_DIR(l->data));
				g_print ("</menu>\n");
				break;

			case MENU_CACHE_TYPE_SEP:
				g_print ("<separator />\n");
				break;

			case MENU_CACHE_TYPE_APP:
				if (app_is_visible (MENU_CACHE_APP(l->data), show_flag))
					menu_application (l->data);
		}
}
