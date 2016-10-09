#include "openbox-menu.h"

OB_Menu*
context_new()
{
	OB_Menu* context =  g_slice_new0 (OB_Menu);

	return context;
}

/****** TERMINAL ******/

void
context_set_terminal_cmd (OB_Menu* ctx, gchar* cmd)
{
	ctx->terminal_cmd = cmd;
}


gchar*
context_get_terminal_cmd (OB_Menu* ctx)
{
	return ctx->terminal_cmd;
}

/****** DESCRIPTION DISPLAY ******/

void context_set_comment(OB_Menu* ctx, gboolean val)
{
	ctx->comment = val;
}


gboolean context_get_comment(OB_Menu* ctx)
{
	return ctx->comment;
}

/****** SHOW FLAG ******/

void
context_set_desktop_flag (OB_Menu* ctx, int flag)
{
	ctx->show_flag = flag;
}


void
context_add_desktop_flag (OB_Menu* ctx, int flag)
{
	ctx->show_flag |= flag;
}


int
context_get_desktop_flag (OB_Menu* ctx)
{
	return ctx->show_flag;
}


/****** SYSTEM NOTIFICATION ******/
void
context_set_sn (OB_Menu* ctx, int flag)
{
	ctx->sn = flag;
}


gboolean
context_get_sn (OB_Menu* ctx)
{
	return ctx->sn;
}


/****** PERSISTENT MODE ******/

void
context_set_persistent (OB_Menu* ctx, gboolean flag)
{
	ctx->persistent = flag;
}

gboolean
context_get_persistent (OB_Menu* ctx)
{
	return ctx->persistent;
}


/****** DESTRUCTOR ******/

void
context_free (OB_Menu *context)
{
	if (context->output)
		g_free (context->output);

	if (context->menu_file)
		g_free (context->menu_file);

	g_slice_free (OB_Menu, context);
}
