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
#ifndef _GEGUEB_WIDGET_H_
#define _GEGUEB_WIDGET_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GEGUEB_TYPE_WIDGET             (gegueb_widget_get_type ())
#define GEGUEB_WIDGET(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEGUEB_TYPE_WIDGET, GeguebWidget))
#define GEGUEB_WIDGET_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GEGUEB_TYPE_WIDGET, GeguebWidgetClass))
#define GEGUEB_IS_WIDGET(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEGUEB_TYPE_WIDGET))
#define GEGUEB_IS_WIDGET_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GEGUEB_TYPE_WIDGET))
#define GEGUEB_WIDGET_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GEGUEB_TYPE_WIDGET, GeguebWidgetClass))

typedef struct _GeguebWidget GeguebWidget;
typedef struct _GeguebWidgetClass GeguebWidgetClass;

typedef struct _GeguebWidgetPrivate GeguebWidgetPrivate;

struct _GeguebWidget
{
	GtkDrawingArea parent;
	GeguebWidgetPrivate *priv;
};

struct _GeguebWidgetClass
{
	GtkDrawingAreaClass parent_class;
};

GType gegueb_widget_get_type (void) G_GNUC_CONST;

/* Constructors */
GtkWidget *gegueb_widget_new(Egueb_Dom_Node *doc);

#endif
