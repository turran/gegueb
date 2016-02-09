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

#if BUILD_EGUEB_SMIL
#include <Egueb_Smil.h>
#endif

/*
 * TODO:
 * We might need to add a enesim_renderer_commit() function. To just let the
 * properties be committed without actually drawing. Otherwise in case the
 * window is not shown it will keep damaging the same areas even if it has
 * a new set of properties set
 * Missing features:
 * IO
 * Script
 * Video
 * UI
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Gegueb_Window
{
	Egueb_Dom_Window *ewin;
	/* features */
	Egueb_Dom_Feature *fwin;
	Egueb_Dom_Feature *fren;
	Egueb_Dom_Input *input;
#if BUILD_EGUEB_SMIL
	Egueb_Dom_Feature *fanim;
#endif
	Egueb_Dom_Node *doc;
	Enesim_Surface *s;
	cairo_surface_t *cs;
	Eina_List *damages;
	GdkRegion *regions;
	GdkWindow *win;
	guint idle_id;
	guint anim_id;
} Gegueb_Window;

#if BUILD_EGUEB_SMIL
static gboolean _gegueb_timer_cb(void *data)
{
	Gegueb_Window *thiz = data;

	egueb_smil_feature_animation_tick(thiz->fanim);
	return TRUE;
}
#endif

static void _gegueb_surface_free(void *buffer_data, void *data)
{
	cairo_surface_t *cs = data;
	cairo_surface_destroy(cs);
}

static void _gegueb_window_draw(Gegueb_Window *thiz)
{
	Eina_Rectangle *r;
	cairo_t *cr;

	if (!thiz->s)
		return;

	if (!thiz->damages)
		return;

	egueb_dom_feature_render_draw_list(thiz->fren, thiz->s,
			ENESIM_ROP_FILL, thiz->damages, 0, 0, NULL);
	//gdk_window_begin_paint_region(thiz->win, thiz->regions);
	cr = gdk_cairo_create(thiz->win);
	gdk_cairo_region(cr, thiz->regions);
	/* blit */
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_set_source_surface(cr, thiz->cs, 0, 0);
	cairo_paint(cr);
	cairo_destroy(cr);

	EINA_LIST_FREE(thiz->damages, r)
	{
		free(r);
	}
	thiz->damages = NULL;
	gdk_region_empty(thiz->regions);
	//gdk_window_end_paint(thiz->win);
	//gdk_window_flush(thiz->win);
}


static Eina_Bool _gegueb_window_damages(Egueb_Dom_Feature *f,
		Eina_Rectangle *area, void *data)
{
	Gegueb_Window *thiz = data;
	GdkRectangle rect;

	rect.x = area->x;
	rect.y = area->y;
	rect.width = area->w;
	rect.height = area->h;
	gdk_window_invalidate_rect(thiz->win, &rect, FALSE); 
}

static gboolean _gegueb_window_idle_cb(gpointer user_data)
{
	Gegueb_Window *thiz = user_data;

	egueb_dom_document_process(thiz->doc);
	if (!thiz->fren)
		goto done;
	if (!thiz->s)
		goto done;
	egueb_dom_feature_render_damages_get(thiz->fren, thiz->s,
				_gegueb_window_damages, thiz);
done:
	thiz->idle_id = 0;
	return FALSE;
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
		if (thiz->s)
		{
			int w, h;

			enesim_surface_size_get(thiz->s, &w, &h);
			if (w == event->configure.width &&
					h == event->configure.height)
				return;
			enesim_surface_unref(thiz->s);
		}
		thiz->cs = cairo_image_surface_create (CAIRO_FORMAT_RGB24,
				event->configure.width,
				event->configure.height);
		thiz->s = enesim_surface_new_data_from(ENESIM_FORMAT_ARGB8888,
				event->configure.width,
				event->configure.height,
				EINA_FALSE,
				cairo_image_surface_get_data(thiz->cs),
				cairo_image_surface_get_stride(thiz->cs),
				_gegueb_surface_free, thiz->cs);
		egueb_dom_feature_window_size_set(thiz->fwin,
				event->configure.width,
				event->configure.height);
		_gegueb_window_draw(thiz);
		break;

		case GDK_DELETE:
		egueb_dom_window_close_notify(thiz->ewin);
		break;

		case GDK_MOTION_NOTIFY:
		egueb_dom_input_feed_mouse_move(thiz->input, event->motion.x, event->motion.y);
		break;

		case GDK_BUTTON_PRESS:
		egueb_dom_input_feed_mouse_down(thiz->input, event->button.button);
		break;

		case GDK_BUTTON_RELEASE:
		egueb_dom_input_feed_mouse_up(thiz->input, event->button.button);
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
/*----------------------------------------------------------------------------*
 *                               Event handlers                               *
 *----------------------------------------------------------------------------*/
static void _gegueb_document_process_cb(Egueb_Dom_Event *ev, void *data)
{
	Gegueb_Window *thiz = data;

	if (thiz->idle_id)
		return;
	thiz->idle_id = g_idle_add(_gegueb_window_idle_cb, thiz);
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

	egueb_dom_feature_unref(thiz->fwin);
	egueb_dom_feature_unref(thiz->fren);
	if (thiz->input)
		egueb_dom_input_unref(thiz->input);
#if BUILD_EGUEB_SMIL
	egueb_dom_feature_unref(thiz->fanim);
#endif
	egueb_dom_node_unref(thiz->doc);
	gdk_window_destroy(thiz->win);
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

	thiz = calloc(1, sizeof(Gegueb_Window));

	/* check it it has the render feature */
	thiz->fren = egueb_dom_node_feature_get(topmost, EGUEB_DOM_FEATURE_RENDER_NAME, NULL);
	if (!thiz->fren)
	{
		ERR("The topmost element does not have a render feature");
		goto no_render;
	}

	thiz->fwin = egueb_dom_node_feature_get(topmost, EGUEB_DOM_FEATURE_WINDOW_NAME, NULL);
	if (!thiz->fwin)
	{
		ERR("The topmost element does not have a window feature");
		goto no_window;
	}

	/* sanitize the width/height */
	/* in case the user does not provide a size */
	if (w <= 0 || h <= 0)
	{
		Egueb_Dom_Feature_Window_Hint_Data wdata;
		int whints;

		whints = egueb_dom_feature_window_hints_get(thiz->fwin, &wdata);
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
		goto no_size;
	}
	egueb_dom_feature_window_size_set(thiz->fwin, w, h);
	/* check whenever we need to process */
	egueb_dom_event_target_event_listener_add(
			EGUEB_DOM_EVENT_TARGET_CAST(doc),
			EGUEB_DOM_EVENT_PROCESS, _gegueb_document_process_cb,
			EINA_FALSE, thiz);
	/* check for conditional features */
#if BUILD_EGUEB_SMIL
	/* check for animation feature */
	thiz->fanim = egueb_dom_node_feature_get(topmost,
			EGUEB_SMIL_FEATURE_ANIMATION_NAME, NULL);
	/* register our own timer for the anim in case we have one */
	if (thiz->fanim)
	{
		int fps;

		egueb_smil_feature_animation_fps_get(thiz->fanim, &fps);
		if (fps > 0)
		{
			thiz->anim_id = g_timeout_add((1.0/fps) * 1000,
					_gegueb_timer_cb, thiz);
		}
	}
#endif
	/* set the event mask */
	event_mask = GDK_EXPOSURE_MASK;

	f = egueb_dom_node_feature_get(topmost,
			EGUEB_DOM_FEATURE_UI_NAME, NULL);
	if (f)
	{
		thiz->input = egueb_dom_feature_ui_input_get(f);
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
	thiz->doc = doc;
	thiz->win = gdk_window_new(NULL, &attr, 0);
	gdk_event_handler_set(_gegueb_event_cb, thiz, NULL);
	thiz->idle_id = g_idle_add(_gegueb_window_idle_cb, thiz);

	thiz->ewin = egueb_dom_window_new(&_dom_descriptor, doc, thiz);
	gdk_window_show(thiz->win);
	egueb_dom_node_unref(topmost);

	return thiz->ewin;

no_size:
	egueb_dom_feature_unref(thiz->fwin);
no_window:
	egueb_dom_feature_unref(thiz->fren);
no_render:
	egueb_dom_node_unref(topmost);
no_topmost:
	egueb_dom_node_unref(doc);
	free(thiz);

	return NULL;
}
