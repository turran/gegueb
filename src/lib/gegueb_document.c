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

#if BUILD_EGUEB_SMIL
#include <Egueb_Smil.h>
#endif

#include "gegueb_surface_private.h"
#include "gegueb_document_private.h"
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
struct _Gegueb_Document
{
	/* features */
	Egueb_Dom_Feature *fwin;
	Egueb_Dom_Feature *fren;
	/* input */
	Egueb_Dom_Input *input;
#if BUILD_EGUEB_SMIL
	Egueb_Dom_Feature *fanim;
#endif
	Egueb_Dom_Node *doc;
	/* draw */
	Gegueb_Document_Damage_Cb damage_cb;
	gpointer damage_cb_data;
	Enesim_Surface *s;
	/* handlers */
	guint idle_id;
	guint anim_id;
};

#if BUILD_EGUEB_SMIL
static gboolean _gegueb_document_anim_timer_cb(void *data)
{
	Gegueb_Document *thiz = data;

	egueb_smil_feature_animation_tick(thiz->fanim);
	return TRUE;
}
#endif

static Eina_Bool _gegueb_document_damages(Egueb_Dom_Feature *f,
		Eina_Rectangle *area, void *data)
{
	Gegueb_Document *thiz = data;
	GdkRectangle rect;

	rect.x = area->x;
	rect.y = area->y;
	rect.width = area->w;
	rect.height = area->h;
	/* Call the callback provided */
	if (thiz->damage_cb)
		thiz->damage_cb(&rect, thiz->damage_cb_data);
	return EINA_TRUE;
}

static gboolean _gegueb_document_idle_cb(gpointer user_data)
{
	Gegueb_Document *thiz = user_data;

	egueb_dom_document_process(thiz->doc);
	if (!thiz->fren)
		goto done;
	if (!thiz->s)
		goto done;
	egueb_dom_feature_render_damages_get(thiz->fren, thiz->s,
				_gegueb_document_damages, thiz);
done:
	thiz->idle_id = 0;
	return FALSE;
}
/*----------------------------------------------------------------------------*
 *                               Event handlers                               *
 *----------------------------------------------------------------------------*/
static void _gegueb_document_process_cb(Egueb_Dom_Event *ev, void *data)
{
	Gegueb_Document *thiz = data;

	if (thiz->idle_id)
		return;
	thiz->idle_id = g_idle_add(_gegueb_document_idle_cb, thiz);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Gegueb_Document * gegueb_document_new(void)
{
	Gegueb_Document *thiz;

	thiz = g_new0(Gegueb_Document, 1);
	return thiz;
}

void gegueb_document_free(Gegueb_Document *thiz)
{
	gegueb_document_document_set(thiz, NULL);
	g_free(thiz);
}

void gegueb_document_damage_cb_set(Gegueb_Document *thiz,
		Gegueb_Document_Damage_Cb cb, gpointer user_data)
{
	thiz->damage_cb = cb;
	thiz->damage_cb_data = user_data;
}

void gegueb_document_document_set(Gegueb_Document *thiz,
		Egueb_Dom_Node *doc)
{
	/* cleanup */
	if (thiz->fren)
	{
		egueb_dom_feature_unref(thiz->fren);
		thiz->fren = NULL;
	}
	if (thiz->fwin)
	{
		egueb_dom_feature_unref(thiz->fwin);
		thiz->fwin = NULL;
	}
	if (thiz->fanim)
	{
		egueb_dom_feature_unref(thiz->fanim);
		thiz->fanim = NULL;
	}
	if (thiz->input)
	{
		egueb_dom_input_unref(thiz->input);
		thiz->input = NULL;
	}
	egueb_dom_node_unref(thiz->doc);

	/* setup */
	thiz->doc = doc;
	if (doc)
	{
		Egueb_Dom_Node *topmost;
		Egueb_Dom_Feature *f;

		topmost = egueb_dom_document_document_element_get(doc);
		if (!topmost)
		{
			ERR("The document does not have a topmost element");
			return;
		}

		thiz->fren = egueb_dom_node_feature_get(topmost,
				EGUEB_DOM_FEATURE_RENDER_NAME, NULL);
		thiz->fwin = egueb_dom_node_feature_get(topmost,
				EGUEB_DOM_FEATURE_WINDOW_NAME, NULL);
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
						_gegueb_document_anim_timer_cb,
						thiz);
			}
		}
#endif
		f = egueb_dom_node_feature_get(topmost,
				EGUEB_DOM_FEATURE_UI_NAME, NULL);
		if (f)
		{
			thiz->input = egueb_dom_feature_ui_input_get(f);
			egueb_dom_feature_unref(f);
		}
		/* check whenever we need to process */
		egueb_dom_event_target_event_listener_add(
				EGUEB_DOM_EVENT_TARGET_CAST(doc),
				EGUEB_DOM_EVENT_PROCESS, _gegueb_document_process_cb,
				EINA_FALSE, thiz);
		egueb_dom_node_unref(topmost);

		/* do the first process */
		thiz->idle_id = g_idle_add(_gegueb_document_idle_cb, thiz);
	}
}

void gegueb_document_configure(Gegueb_Document *thiz,
		int w, int h)
{
	if (thiz->s)
	{
		int ow, oh;

		enesim_surface_size_get(thiz->s, &ow, &oh);
		if (ow == w && oh == h)
			return;
		enesim_surface_unref(thiz->s);
	}
	thiz->s = gegueb_surface_sw_new(w, h);

	if (thiz->fwin)
		egueb_dom_feature_window_size_set(thiz->fwin, w, h);
}

void gegueb_document_draw(Gegueb_Document *thiz, cairo_t *cr,
		Eina_List *damages)
{
	Eina_Rectangle *r;

	if (!thiz->fren)
		return;

	if (!thiz->s)
		return;

	if (!damages)
		return;

	egueb_dom_feature_render_draw_list(thiz->fren, thiz->s,
			ENESIM_ROP_FILL, damages, 0, 0, NULL);
	/* blit */
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_set_source_surface(cr, gegueb_surface_suface_get(thiz->s), 0, 0);
	cairo_paint(cr);
	//cairo_clip(cr);

	EINA_LIST_FREE(damages, r)
		free(r);
}
