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
#include "gegueb_widget.h"

#include "gegueb_document_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/* Properties */
enum
{
	PROP_0,
	PROP_DOCUMENT,
};

struct _GeguebWidgetPrivate
{
	Gegueb_Document *gdoc;
	Egueb_Dom_Node *doc;
	GdkRegion *regions;
	Eina_List *damages;
};

G_DEFINE_TYPE(GeguebWidget, gegueb_widget, GTK_TYPE_WIDGET)

static void gegueb_widget_damage_cb(GdkRectangle *area,
		gpointer user_data)
{
	GeguebWidget *thiz = GEGUEB_WIDGET(user_data);
	gtk_widget_queue_draw_area(GTK_WIDGET(thiz), area->x, area->y,
			area->width, area->height);
	printf("damaged\n");
}

static void gegueb_widget_draw(GeguebWidget *thiz)
{
	cairo_t *cr;

	cr = gdk_cairo_create(GTK_WIDGET(thiz)->window);
	gdk_cairo_region(cr, thiz->priv->regions);
	gegueb_document_draw(thiz->priv->gdoc, cr, thiz->priv->damages);
	gdk_region_empty(thiz->priv->regions);
	cairo_destroy(cr);
	thiz->priv->damages = NULL;
}
/*----------------------------------------------------------------------------*
 *                             GObject interface                              *
 *----------------------------------------------------------------------------*/
static void gegueb_widget_set_property(GObject *object, guint prop_id,
		const GValue *value, GParamSpec *pspec)
{
	GeguebWidget *thiz = GEGUEB_WIDGET(object);

	switch(prop_id)
	{
		case PROP_DOCUMENT:
		egueb_dom_node_unref(thiz->priv->doc);
		thiz->priv->doc = g_value_get_pointer(value);
		gegueb_document_document_set(thiz->priv->gdoc,
				egueb_dom_node_ref(thiz->priv->doc));
		break;

		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void gegueb_widget_get_property(GObject *object, guint prop_id,
		GValue *value, GParamSpec *pspec)
{
	GeguebWidget *thiz = GEGUEB_WIDGET(object);

	switch(prop_id)
	{
		case PROP_DOCUMENT:
		g_value_set_pointer(value, egueb_dom_node_ref(thiz->priv->doc));
		break;

		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void gegueb_widget_finalize(GObject *object)
{
	printf("finalize\n");
}
/*----------------------------------------------------------------------------*
 *                            GTK Widget interface                            *
 *----------------------------------------------------------------------------*/
static void
gegueb_widget_realize(GtkWidget *widget)
{
	GeguebWidget *thiz;
	GdkWindowAttr attributes;
	gint attributes_mask;
	
	thiz = GEGUEB_WIDGET(widget);

	gtk_widget_set_realized(widget, TRUE);

	/* Make the main, clipping window */
	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.x = widget->allocation.x;
	attributes.y = widget->allocation.y;
	attributes.width = widget->allocation.width;
	attributes.height = widget->allocation.height;
	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.visual = gtk_widget_get_visual(widget);
	attributes.colormap = gtk_widget_get_colormap(widget);
	attributes.event_mask = gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK;

	attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

	widget->window = gdk_window_new(gtk_widget_get_parent_window(widget),
					 &attributes, attributes_mask);
	gdk_window_set_back_pixmap(widget->window, NULL, FALSE);
	gdk_window_set_user_data(widget->window, widget);
	widget->style = gtk_style_attach(widget->style, widget->window);
}

static void gegueb_widget_style_set(GtkWidget *widget,
		GtkStyle *previous_style)
{
	GeguebWidget *thiz = GEGUEB_WIDGET(widget);

	if (gtk_widget_get_realized(widget))
	{
		gdk_window_set_background(widget->window, &widget->style->base[widget->state]);
	}
	gtk_widget_queue_resize(widget);
}

static void gegueb_widget_size_request(GtkWidget *widget,
		GtkRequisition *requisition)
{
	GeguebWidget *thiz = GEGUEB_WIDGET(widget);
	printf("size request\n");
	/* TODO check the window feature */
}

static void gegueb_widget_size_allocate(GtkWidget *widget,
		GtkAllocation *allocation)
{
	GeguebWidget *thiz = GEGUEB_WIDGET(widget);

	printf("size allocate %d %d %d %d\n", allocation->x, allocation->y, allocation->width, allocation->height);
	widget->allocation = *allocation;
	if (gtk_widget_get_realized(widget))
	{
		gdk_window_move_resize(widget->window,
				allocation->x, allocation->y,
				allocation->width, allocation->height);
	}
	gegueb_document_configure(thiz->priv->gdoc, allocation->width,
			allocation->height);
}

static gboolean gegueb_widget_expose(GtkWidget *widget, GdkEventExpose *expose)
{
	GeguebWidget *thiz;
	GdkRectangle *rects;
	Eina_Rectangle *r;
	int nrects;
	int i;

	thiz = GEGUEB_WIDGET(widget);

	printf("expose %d\n", expose->count);
	gdk_region_union(thiz->priv->regions, expose->region);
	gdk_region_get_rectangles(expose->region, &rects, &nrects);
	for (i = 0; i < nrects; i++)
	{
		r = malloc(sizeof(Eina_Rectangle));
		r->x = rects[i].x;
		r->y = rects[i].y;
		r->w = rects[i].width;
		r->h = rects[i].height;
		thiz->priv->damages = eina_list_append(thiz->priv->damages, r);
	}

	if (!expose->count)
	{
		gegueb_widget_draw(thiz);
	}

	return TRUE;
}

static gboolean gegueb_widget_key_press(GtkWidget *widget,
		GdkEventKey *event)
{
	printf("key press\n");
	return TRUE;
}

static gboolean gegueb_widget_key_release(GtkWidget *widget,
		GdkEventKey *event)
{
	printf("key release\n");
	return TRUE;
}

static gboolean gegueb_widget_button_press(GtkWidget *widget,
		GdkEventButton *event)
{
	return TRUE;
}

static gboolean gegueb_widget_button_release(GtkWidget *widget,
		GdkEventButton *event)
{
	return TRUE;
}
/*----------------------------------------------------------------------------*
 *                            GTK Object interface                            *
 *----------------------------------------------------------------------------*/
static void gegueb_widget_destroy(GtkObject *object)
{
	GeguebWidget *thiz = GEGUEB_WIDGET(object);

	printf("destroy\n");
	if (thiz->priv->doc)
	{
		egueb_dom_node_unref(thiz->priv->doc);
		thiz->priv->doc = NULL;
	}
	if (thiz->priv->gdoc)
	{
		gegueb_document_free(thiz->priv->gdoc);
		thiz->priv->gdoc = NULL;
	}
	if (thiz->priv->regions)
	{
		gdk_region_destroy(thiz->priv->regions);
		thiz->priv->regions = NULL;
	}
	if (thiz->priv->damages)
	{
		Eina_Rectangle *r;
		EINA_LIST_FREE(thiz->priv->damages, r)
			free(r);
		thiz->priv->damages = NULL;
	}
	G_OBJECT_CLASS(gegueb_widget_parent_class)->finalize(G_OBJECT(object));
}
/*----------------------------------------------------------------------------*
 *                              GType interface                               *
 *----------------------------------------------------------------------------*/
static void
gegueb_widget_class_init(GeguebWidgetClass *klass)
{
	GObjectClass *object_class;
	GtkWidgetClass *widget_class;
	GtkObjectClass *gtk_object_class;

	object_class = G_OBJECT_CLASS(klass);
	widget_class = GTK_WIDGET_CLASS(klass);
	gtk_object_class = GTK_OBJECT_CLASS(klass);

	/* override parents */
	object_class->finalize = gegueb_widget_finalize;
	object_class->set_property = gegueb_widget_set_property;
	object_class->get_property = gegueb_widget_get_property;
	g_type_class_add_private(object_class, sizeof(GeguebWidgetPrivate));
	/* properties */
	g_object_class_install_property(object_class, PROP_DOCUMENT,
			g_param_spec_pointer("document", "document", "document",
			G_PARAM_READWRITE));

	widget_class->realize = gegueb_widget_realize;
	widget_class->style_set = gegueb_widget_style_set;
	widget_class->size_request = gegueb_widget_size_request;
	widget_class->size_allocate = gegueb_widget_size_allocate;
	widget_class->expose_event = gegueb_widget_expose;
	widget_class->button_press_event = gegueb_widget_button_press;
	widget_class->button_release_event = gegueb_widget_button_release;
	widget_class->key_press_event = gegueb_widget_key_press;
	widget_class->key_release_event = gegueb_widget_key_release;
	widget_class->realize = gegueb_widget_realize;
	gtk_object_class->destroy = gegueb_widget_destroy;
}

static void
gegueb_widget_init(GeguebWidget *thiz)
{
	thiz->priv = G_TYPE_INSTANCE_GET_PRIVATE(thiz, GEGUEB_TYPE_WIDGET,
			GeguebWidgetPrivate);
	thiz->priv->gdoc = gegueb_document_new();
	thiz->priv->regions = gdk_region_new();
	gegueb_document_damage_cb_set(thiz->priv->gdoc,
			gegueb_widget_damage_cb, thiz);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

/**
 * gegueb_widget_new:
 *
 * Creates a new #GeguebWidget
 *
 * Return value: a new #GeguebWidget.
 **/
GtkWidget * gegueb_widget_new(void)
{
	GtkWidget *thiz;
	thiz = g_object_new(GEGUEB_TYPE_WIDGET, NULL);
	return thiz;
}
