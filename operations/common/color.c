/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 */
#ifdef GEGL_CHANT_PROPERTIES

gegl_chant_color (value, "Color", "black",
                  "The color to render (defaults to 'black')")
#else

#define GEGL_CHANT_TYPE_POINT_RENDER
#define GEGL_CHANT_C_FILE           "color.c"

#include "gegl-chant.h"

static void
prepare (GeglOperation *operation)
{
  gegl_operation_set_format (operation, "output", babl_format ("RGBA float"));
}

static GeglRectangle
get_bounding_box (GeglOperation *operation)
{
  GeglRectangle result = {-10000000,-10000000,20000000,20000000};
  return result;
}

static GeglNode *
detect (GeglOperation *operation,
        gint           x,
        gint           y)
{
  return NULL;
}

static gboolean
process (GeglOperation *operation,
         void          *out_buf,
         glong          n_pixels,
         GeglRectangle *roi)
{
  GeglChantO *o = GEGL_CHANT_PROPERTIES (operation);
  gfloat     *out_pixel = out_buf;
  gfloat      color[4];

  gegl_color_get_rgba (o->value,
                       &color[0],
                       &color[1],
                       &color[2],
                       &color[3]);

  while (n_pixels--)
    {
      out_pixel[0]=color[0];
      out_pixel[1]=color[1];
      out_pixel[2]=color[2];
      out_pixel[3]=color[3];

      out_pixel += 4;
    }
  return  TRUE;
}


static void
gegl_chant_class_init (GeglChantClass *klass)
{
  GeglOperationClass            *operation_class;
  GeglOperationPointRenderClass *point_render_class;

  operation_class = GEGL_OPERATION_CLASS (klass);
  point_render_class = GEGL_OPERATION_POINT_RENDER_CLASS (klass);

  point_render_class->process = process;
  operation_class->get_bounding_box = get_bounding_box;
  operation_class->prepare = prepare;

  operation_class->name        = "color";
  operation_class->categories  = "render";
  operation_class->description =
        "Generates a buffer entirely filled with the specified color, crop it to get smaller dimensions.";

  operation_class->no_cache = TRUE;
  operation_class->detect = detect;
  operation_class->get_cached_region = NULL;
}

#endif
