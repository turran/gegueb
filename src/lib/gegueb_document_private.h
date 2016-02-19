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
#ifndef _GEGUEB_DOCUMENT_PRIVATE_H_
#define _GEGUEB_DOCUMENT_PRIVATE_H_

typedef struct _Gegueb_Document Gegueb_Document;

typedef void (*Gegueb_Document_Damage_Cb)(GdkRectangle *area,
		gpointer user_data);

Gegueb_Document * gegueb_document_new(void);
void gegueb_document_free(Gegueb_Document *thiz);

void gegueb_document_damage_cb_set(Gegueb_Document *thiz,
		Gegueb_Document_Damage_Cb cb, gpointer user_data);
void gegueb_document_document_set(Gegueb_Document *thiz,
		Egueb_Dom_Node *doc);

void gegueb_document_configure(Gegueb_Document *thiz,
		int w, int h);
void gegueb_document_draw(Gegueb_Document *thiz, cairo_t *cr, Eina_List *damages);

#endif
