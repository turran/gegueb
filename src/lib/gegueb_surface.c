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
#include "gegueb_surface_private.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _gegueb_surface_free(void *buffer_data, void *data)
{
	cairo_surface_t *cs = data;
	cairo_surface_destroy(cs);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Enesim_Surface * gegueb_surface_sw_new(int w, int h)
{
	Enesim_Surface *s;
	cairo_surface_t *cs;

	cs = cairo_image_surface_create(CAIRO_FORMAT_RGB24, w, h);
	s = enesim_surface_new_data_from(ENESIM_FORMAT_ARGB8888, w, h,
			EINA_FALSE,
			cairo_image_surface_get_data(cs),
			cairo_image_surface_get_stride(cs),
			_gegueb_surface_free, cs);
	enesim_surface_private_set(s, cs);
	return s;
}

cairo_surface_t * gegueb_surface_suface_get(Enesim_Surface *s)
{
	return enesim_surface_private_get(s);
}
