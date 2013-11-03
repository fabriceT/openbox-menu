//      ob_display.c - this file is part of openbox-menu
//      Copyright (C) 2010-13 mimas <mimasgpc@free.fr>
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

#include "openbox-menu.h"

/****f* ob_display/menu_directory
 * FUNCTION
 *   create a menu entry for a directory.
 *
 * NOTES
 *   this menu entry has to be closed by "</menu>".
 ****/
void
menu_directory (MenuCacheApp *dir, OB_Menu *context)
{
	gchar *dir_id = safe_name (menu_cache_item_get_id (MENU_CACHE_ITEM(dir)));
	gchar *dir_name = safe_name (menu_cache_item_get_name (MENU_CACHE_ITEM(dir)));

#ifdef WITH_ICONS
	if (!context->no_icons)
	{
		gchar *dir_icon = item_icon_path (MENU_CACHE_ITEM(dir));

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

/****f* ob_display/menu_application
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
		exec_name = safe_name (menu_cache_item_get_comment (MENU_CACHE_ITEM(app)));
	else
		exec_name = safe_name (menu_cache_item_get_name (MENU_CACHE_ITEM(app)));

	exec_cmd = clean_exec (app);

#ifdef WITH_ICONS
	if (!context->no_icons)
	{
		exec_icon = item_icon_path (MENU_CACHE_ITEM(app));
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

/****f* ob_display/menu_generate
 * FUNCTION
 *   main routine of menu creation.
 *
 * NOTES
 *   It calls itself when 'dir' type is MENU_CACHE_TYPE_DIR.
 ****/
void
menu_generate (MenuCacheDir *dir, OB_Menu *context)
{
	GSList *l = NULL;

	for (l = menu_cache_dir_get_children (dir); l; l = l->next)
		switch ((guint) menu_cache_item_get_type (MENU_CACHE_ITEM(l->data)))
		{
			case MENU_CACHE_TYPE_DIR:
				menu_directory (l->data, context);
				menu_generate (MENU_CACHE_DIR(l->data), context);
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


/****f* ob_display/menu_add_header
 * FUNCTION
 *   Add a header to XML openbox-menu XML output
 *
 * INPUTS
 *   * builder, a GString builder
 *   * filename, the file containing the header content.
 ****/
void
menu_add_header (GString *builder, gchar *filename)
{
	gchar *content = NULL;

	if (filename)
	{
		if (g_file_get_contents (filename, &content, NULL, NULL))
		{
			g_string_append (builder, content);
			g_free (content);
			return;
		}
		else
		{
			g_warning ("Can't load header file %s", filename);
		}
	}

	g_string_append (builder,
	    "<openbox_pipe_menu xmlns=\"http://openbox.org/\""
	    "  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
	    "  xsi:schemaLocation=\"http://openbox.org/"
	    "  file:///usr/share/openbox/menu.xsd\">\n");
}

/****f* ob_display/menu_add_footer
 * FUNCTION
 *   Add a footer to XML openbox-menu XML output
 *
 * INPUTS
 *   * builder, a GString builder
 *   * filename, the file containing the footer content.
 ****/
void
menu_add_footer (GString *builder, gchar *filename)
{
	gchar *content = NULL;

	if (filename)
	{
		if (g_file_get_contents (filename, &content, NULL, NULL))
		{
			g_string_append (builder, content);
			g_free (content);
			return;
		}
		else
		{
			g_warning ("Can't load footer file %s", filename);
		}
	}

	g_string_append (builder,
	    "</openbox_pipe_menu>\n");
}

/****f* ob_display/menu_display
 * FUNCTION
 *   it begins and closes the menu content, write it into a file or
 *   display it.
 *
 * INPUTS
 *   * menu
 *   * file, the filename where the menu content should be written to.
 *     If file is 'NULL' then the menu content is displayed.
 *
 * RETURN VALUE
 *    Nothing. A MenuCacheReloadNotify callback returns void.
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
menu_display (MenuCache *menu, OB_Menu *context)
{
	MenuCacheDir *dir = menu_cache_get_root_dir (menu);
	if (G_UNLIKELY(dir == NULL))
	{
		g_warning ("Can't get menu root directory");
		context->code = MENU_DIR_ERROR;
		return;
	}

	GSList *l = menu_cache_dir_get_children (dir);

	if (g_slist_length (l) != 0) {
		context->builder = g_string_sized_new (16 * 1024);
		menu_add_header (context->builder, context->header_file);
		menu_generate (dir, context);
		menu_add_footer (context->builder, context->footer_file);

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
		return;
	}
	else
	{
		g_warning ("Menu seems to be empty. Check openbox-menu parameters.");
		context->code = MENU_EMPTY_ERROR;
		return;
	}
}
