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
struct _GeguebWidgetPrivate
{

};

G_DEFINE_TYPE (GeguebWidget, gegueb_widget, GTK_TYPE_DRAWING_AREA)
/*----------------------------------------------------------------------------*
 *                             GObject interface                              *
 *----------------------------------------------------------------------------*/
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
	g_type_class_add_private(object_class, sizeof(GeguebWidgetPrivate));
	/* properties */
	/* TODO the doc */

	widget_class->key_press_event = gegueb_widget_key_press_event;
	widget_class->expose_event = gegueb_widget_expose;
	widget_class->realize = gegueb_widget_realize;
	gtk_object_class->destroy = gegueb_widget_destroy;
}

static void
gegueb_widget_init (GeguebWidget *thiz)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

/**
 * gegueb_widget_new:
 * @buffer: a #GtkSourceBuffer.
 *
 * Creates a new #GeguebWidget widget displaying the buffer
 * @buffer. One buffer can be shared among many widgets.
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
