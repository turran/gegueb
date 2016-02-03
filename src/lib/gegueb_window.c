/* Gegueb Egueb based Glib, Gdk, Gtk extensions
 * Copyright (C) 2013 - 2014 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#include "gegueb_private.h"

#include "gegueb_main.h"
#include "gegueb_window.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Gegueb_Window
{
	Egueb_Dom_Window *ewin;
	GdkWindow *win;
} Gegueb_Window;
/*----------------------------------------------------------------------------*
 *                               Event handlers                               *
 *----------------------------------------------------------------------------*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                     DOM Window descriptor interface                        *
 *----------------------------------------------------------------------------*/
static void _gegueb_window_destroy(void *data)
{
}

static int _gegueb_window_width_get(void *data)
{
	Gegueb_Window *thiz = data;
	return 0;
}

static int _gegueb_window_height_get(void *data)
{
	Gegueb_Window *thiz = data;
	return 0;
}

static void * _gegueb_window_timeout_set(void *data,
		Egueb_Dom_Window_Descriptor_Timeout_Cb cb,
		int64_t delay, void *user_data)
{
	return NULL;
}

static void _gegueb_window_timeout_clear(void *data, void *timeout)
{
}

static Egueb_Dom_Window_Descriptor _dom_descriptor = {
	/* .version 		= */ EGUEB_DOM_WINDOW_DESCRIPTOR_VERSION,
	/* .destroy 		= */ _gegueb_window_destroy,
	/* .inner_width_get 	= */ _gegueb_window_width_get,
	/* .inner_height_get 	= */ _gegueb_window_height_get,
	/* .outer_width_get 	= */ _gegueb_window_width_get,
	/* .outer_height_get 	= */ _gegueb_window_height_get,
	/* .timeout_set 	= */ _gegueb_window_timeout_set,
	/* .timeout_clear 	= */ _gegueb_window_timeout_clear,
};
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Window * gegueb_window_new(Egueb_Dom_Node *doc,
		int x, int y, int w, int h)
{
	Gegueb_Window *thiz;
	Egueb_Dom_Window *ret = NULL;
	GdkWindowAttr attr;
	GdkWindow *win;

	win = gdk_window_new(NULL, &attr, 0);
	thiz = calloc(1, sizeof(Gegueb_Window));
	thiz->win = win;
	thiz->ewin = egueb_dom_window_new(&_dom_descriptor, doc, thiz);

	return thiz->ewin;
}
