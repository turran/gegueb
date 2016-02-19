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

#include "gegueb_surface_private.h"
#include "gegueb_document_private.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Gegueb_Window
{
	Egueb_Dom_Window *ewin;
	Gegueb_Document *doc;
	GdkWindow *win;
	GdkRegion *regions;
	Eina_List *damages;
} Gegueb_Window;

static void _gegueb_window_draw(Gegueb_Window *thiz)
{
	cairo_t *cr;

	//gdk_window_begin_paint_region(thiz->win, thiz->regions);
	cr = gdk_cairo_create(thiz->win);
	gdk_cairo_region(cr, thiz->regions);
	gegueb_document_draw(thiz->doc, cr, thiz->damages);
	gdk_region_empty(thiz->regions);
	//gdk_window_end_paint(thiz->win);
	//gdk_window_flush(thiz->win);
	thiz->damages = NULL;
}

static void _gegueb_window_damages(GdkRectangle *rect, gpointer user_data)
{
	Gegueb_Window *thiz = user_data;
	gdk_window_invalidate_rect(thiz->win, rect, FALSE); 
}

static void _gegueb_event_cb(GdkEvent *event, gpointer user_data)
{
	Gegueb_Window *thiz = user_data;

	switch(event->type)
	{
		case GDK_EXPOSE:
		case GDK_DAMAGE:
		{
			GdkRectangle *rects;
			Eina_Rectangle *r;
			int nrects;
			int i;

			gdk_region_union(thiz->regions, event->expose.region);
			gdk_region_get_rectangles(event->expose.region, &rects, &nrects);
			for (i = 0; i < nrects; i++)
			{
				r = malloc(sizeof(Eina_Rectangle));
				r->x = rects[i].x;
				r->y = rects[i].y;
				r->w = rects[i].width;
				r->h = rects[i].height;
				thiz->damages = eina_list_append(thiz->damages, r);
			}

			if (!event->expose.count)
			{
				_gegueb_window_draw(thiz);
			}
		}
		break;

		case GDK_CONFIGURE:
		gegueb_document_configure(thiz->doc, event->configure.width,
				event->configure.height);
		_gegueb_window_draw(thiz);
		break;

		case GDK_DELETE:
		egueb_dom_window_close_notify(thiz->ewin);
		break;

		case GDK_MOTION_NOTIFY:
		//egueb_dom_input_feed_mouse_move(thiz->input, event->motion.x, event->motion.y);
		break;

		case GDK_BUTTON_PRESS:
		//egueb_dom_input_feed_mouse_down(thiz->input, event->button.button);
		break;

		case GDK_BUTTON_RELEASE:
		//egueb_dom_input_feed_mouse_up(thiz->input, event->button.button);
		break;

		case GDK_KEY_PRESS:
		printf("ui key press\n");
		break;

		case GDK_KEY_RELEASE:
		printf("ui key release\n");
		break;

		case GDK_ENTER_NOTIFY:
		printf("ui enter\n");
		break;

		case GDK_LEAVE_NOTIFY:
		printf("ui leave\n");
		break;

		case GDK_FOCUS_CHANGE:
		printf("ui focus\n");
		break;
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                     DOM Window descriptor interface                        *
 *----------------------------------------------------------------------------*/
static void _gegueb_window_destroy(void *data)
{
	Gegueb_Window *thiz = data;

	gegueb_document_free(thiz->doc);
	gdk_window_destroy(thiz->win);
	g_free(thiz);
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
		Egueb_Dom_Window_Timeout_Cb cb,
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
	Egueb_Dom_Node *topmost;
	Egueb_Dom_Feature *f;
	GdkWindowAttr attr;
	gint event_mask;

	if (!doc) return NULL;

	topmost = egueb_dom_document_document_element_get(doc);
	if (!topmost)
	{
		ERR("The document does not have a topmost element");
		goto no_topmost; 
	}

	/* check it it has the render feature */
	f = egueb_dom_node_feature_get(topmost, EGUEB_DOM_FEATURE_RENDER_NAME, NULL);
	if (!f)
	{
		ERR("The topmost element does not have a render feature");
		goto no_feature;
	}
	egueb_dom_feature_unref(f);

	f = egueb_dom_node_feature_get(topmost, EGUEB_DOM_FEATURE_WINDOW_NAME, NULL);
	if (!f)
	{
		ERR("The topmost element does not have a window feature");
		goto no_feature;
	}

	/* sanitize the width/height */
	/* in case the user does not provide a size */
	if (w <= 0 || h <= 0)
	{
		Egueb_Dom_Feature_Window_Hint_Data wdata;
		int whints;

		whints = egueb_dom_feature_window_hints_get(f, &wdata);
		if (whints & EGUEB_DOM_FEATURE_WINDOW_HINT_PREFERRED)
		{
			if (wdata.pref_width != -1 && w == -1)
				w = wdata.pref_width;
			if (wdata.pref_height != -1 && h == -1)
				h = wdata.pref_height;
		}

		if (whints & EGUEB_DOM_FEATURE_WINDOW_HINT_MIN_MAX)
		{
			if (wdata.min_width != -1 && w < wdata.min_width)
				w = wdata.min_width;
			if (wdata.min_height != -1 && h < wdata.min_height)
				h = wdata.min_height;

			if (wdata.max_width != -1 && w > wdata.max_width)
				w = wdata.max_width;
			if (wdata.max_height != -1 && h > wdata.max_height)
				h = wdata.max_height;
		}
	}

	if (w <= 0 || h <= 0)
	{
		ERR("Invalid size of the window %d %d", w, h);
		egueb_dom_feature_unref(f);
		goto no_feature;
	}
	egueb_dom_feature_unref(f);
	egueb_dom_feature_window_size_set(f, w, h);

	thiz = g_new0(Gegueb_Window, 1);
	/* set the event mask */
	event_mask = GDK_EXPOSURE_MASK;

	f = egueb_dom_node_feature_get(topmost,
			EGUEB_DOM_FEATURE_UI_NAME, NULL);
	if (f)
	{
		event_mask |= GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK |
				GDK_BUTTON_RELEASE_MASK | GDK_KEY_PRESS_MASK |
				GDK_KEY_RELEASE_MASK;
		egueb_dom_feature_unref(f);
	}

	/* set the window attributes */
	attr.width = w;
	attr.height = h;
	attr.window_type = GDK_WINDOW_TOPLEVEL;
	attr.event_mask = event_mask;

	thiz->regions = gdk_region_new();
	thiz->doc = gegueb_document_new();
	gegueb_document_document_set(thiz->doc, doc);
	thiz->win = gdk_window_new(NULL, &attr, 0);
	gdk_event_handler_set(_gegueb_event_cb, thiz, NULL);
	thiz->ewin = egueb_dom_window_new(&_dom_descriptor, doc, thiz);
	gdk_window_show(thiz->win);
	egueb_dom_node_unref(topmost);

	return thiz->ewin;

no_feature:
	egueb_dom_node_unref(topmost);
no_topmost:
	egueb_dom_node_unref(doc);

	return NULL;
}
