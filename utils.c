//      utils.c - this file is part of openbox-menu
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

#include <glib.h>
#ifdef WITH_ICONS
	#include <gtk/gtk.h>
#endif
#include <string.h>

#include "openbox-menu.h"

/****f* utils/safe_name
 * FUNCTION
 *   Convert &, <, > and " signs to html entities
 *
 * OUTPUT
 *   A gchar that needs to be freed.
 ****/
gchar *
safe_name (const char *name)
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
				g_string_append (cmd, "&quot;");
				break;
			default:
				g_string_append_c (cmd, *name);
		}
	}
	return g_string_free (cmd, FALSE);
}


/****f* utils/clean_exec
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

	g_return_val_if_fail(exec,"");
		
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
					if (item_icon_path (MENU_CACHE_ITEM(app)))
					{
						g_string_append_printf (cmd, "--icon %s",
						    item_icon_path (MENU_CACHE_ITEM(app)));
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

extern GtkIconTheme *icon_theme;

/****f* utils/item_icon_path
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
item_icon_path (MenuCacheItem *item)
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
