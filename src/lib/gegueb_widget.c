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
	GtkWidget *view;

	g_return_val_if_fail (doc != NULL);

	view = g_object_new (GEGUEB_TYPE_WIDGET, NULL);
	return view;
}
