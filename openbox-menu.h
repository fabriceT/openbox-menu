/*
 *      openbox-menu.h -  this file is part of openbox-menu
 *      Copyright (C) 2011,12 mimas <mimasgpc@free.fr>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU Lesser General Public License as published
 *      by the Free Software Foundation; version 3.0 only.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#define VERSION "0.5.0"

#define APPMENU_SIZE 30

guint app_is_visible(MenuCacheApp *, guint32);
gchar *get_item_icon_path (MenuCacheItem*);

typedef struct {
	/* Configuration */
	gchar    *output;
	guint32   show_flag;
	GString  *builder;      /* */
	gchar    *terminal_cmd; /* command to launch program in a terminal */
	gboolean  comment;      /* display description instead of name */
	gboolean  sn;           /* startup notification */
	gboolean  no_icons;     /* icons disabled */
} OB_Menu;
