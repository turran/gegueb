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
	Egueb_Dom_Node *doc;
};

G_DEFINE_TYPE (GeguebWidget, gegueb_widget, GTK_TYPE_DRAWING_AREA)
/*----------------------------------------------------------------------------*
 *                             GObject interface                              *
 *----------------------------------------------------------------------------*/
static void gegueb_widget_set_property(GObject *object, guint prop_id,
		const GValue *value, GParamSpec *pspec)
{
	GeguebWidget *thiz = GEGUEB_WIDGET(object);

	printf("set property\n");
	switch (prop_id)
	{
		case PROP_DOCUMENT:
		egueb_dom_node_unref(thiz->priv->doc);
		thiz->priv->doc = g_value_get_pointer(value);
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

	printf("get property\n");
	switch (prop_id)
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
static gboolean gegueb_widget_key_press_event(GtkWidget *widget,
		GdkEventKey *event)
{
	printf("key press\n");
	return TRUE;
}

static gint gegueb_widget_expose(GtkWidget *widget, GdkEventExpose    *event)
{
	printf("expose\n");
	return 0;
}

static void gegueb_widget_realize(GtkWidget *widget)
{
	printf("realize\n");
}
/*----------------------------------------------------------------------------*
 *                            GTK Object interface                            *
 *----------------------------------------------------------------------------*/
static void gegueb_widget_destroy(GtkObject *object)
{
	G_OBJECT_CLASS(gegueb_widget_parent_class)->finalize(object);
}
/*----------------------------------------------------------------------------*
 *                              GType interface                               *
 *----------------------------------------------------------------------------*/
static void
gegueb_widget_class_init (GeguebWidgetClass *klass)
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

	widget_class->key_press_event = gegueb_widget_key_press_event;
	widget_class->expose_event = gegueb_widget_expose;
	widget_class->realize = gegueb_widget_realize;
	gtk_object_class->destroy = gegueb_widget_destroy;
}

static void
gegueb_widget_init (GeguebWidget *thiz)
{
	thiz->priv = G_TYPE_INSTANCE_GET_PRIVATE (thiz, GEGUEB_TYPE_WIDGET,
			GeguebWidgetPrivate);
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
GtkWidget *
gegueb_widget_new (Egueb_Dom_Node *doc)
{
	GtkWidget *thiz;

	g_return_val_if_fail (doc != NULL, NULL);

	thiz = g_object_new (GEGUEB_TYPE_WIDGET, NULL);
	return thiz;
}
