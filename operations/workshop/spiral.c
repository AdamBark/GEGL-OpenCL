/* This file is an image processing operation for GEGL
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2017 Ell
 *
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

property_double (x, _("X"), 0.5)
  description (_("Spiral origin X coordinate"))
  ui_range    (0.0, 1.0)
  ui_meta     ("unit", "relative-coordinate")
  ui_meta     ("axis", "x")

property_double (y, _("Y"), 0.5)
  description (_("Spiral origin Y coordinate"))
  ui_range    (0.0, 1.0)
  ui_meta     ("unit", "relative-coordinate")
  ui_meta     ("axis", "y")

property_double (radius, _("Radius"), 100.0)
  description (_("Spiral radius"))
  value_range (1.0, G_MAXDOUBLE)
  ui_range    (1.0, 400.0)
  ui_meta     ("unit", "pixel-distance")

property_double (thickness, _("Thickness"), 0.5)
  description (_("Spiral thickness, as a fraction of the radius"))
  value_range (0.0, 1.0)

property_double (angle, _("Angle"), 0.0)
  description (_("Spiral start angle"))
  value_range (0.0, 360.0)
  ui_meta     ("unit", "degree")

enum_start (gegl_spiral_direction)
  enum_value (GEGL_SPIRAL_DIRECTION_CLOCKWISE,        "cw",  N_("Clockwise"))
  enum_value (GEGL_SPIRAL_DIRECTION_COUNTERCLOCKWISE, "ccw", N_("Counter-clockwise"))
enum_end (GeglSpiralDirection)

property_enum (direction, _("Direction"),
               GeglSpiralDirection, gegl_spiral_direction,
               GEGL_SPIRAL_DIRECTION_CLOCKWISE)
  description (_("Spiral swirl direction"))

property_color  (color1, _("Color 1"), "black")
  ui_meta     ("role", "color-primary")

property_color  (color2, _("Color 2"), "white")
  ui_meta     ("role", "color-secondary")

property_int    (width, _("Width"), 1024)
  description (_("Width of the generated buffer"))
  value_range (0, G_MAXINT)
  ui_range    (0, 4096)
  ui_meta     ("unit", "pixel-distance")
  ui_meta     ("axis", "x")
  ui_meta     ("role", "output-extent")

property_int    (height, _("Height"), 768)
  description (_("Height of the generated buffer"))
  value_range (0, G_MAXINT)
  ui_range    (0, 4096)
  ui_meta     ("unit", "pixel-distance")
  ui_meta     ("axis", "y")
  ui_meta     ("role", "output-extent")

#else

#define GEGL_OP_POINT_RENDER
#define GEGL_OP_NAME     spiral
#define GEGL_OP_C_SOURCE spiral.c

#include "gegl-op.h"
#include <math.h>

static void
prepare (GeglOperation *operation)
{
  gegl_operation_set_format (operation, "output", babl_format ("R'G'B'A float"));
}

static GeglRectangle
get_bounding_box (GeglOperation *operation)
{
  return gegl_rectangle_infinite_plane ();
}

static inline void
blend (const gfloat *color1,
       const gfloat *color2,
       gfloat        a,
       gfloat       *result)
{
  if (a == 0.0)
    {
      memcpy (result, color1, 4 * sizeof (gfloat));
    }
  else if (a == 1.0)
    {
      memcpy (result, color2, 4 * sizeof (gfloat));
    }
  else
    {
      gfloat alpha = color1[3] + a * (color2[3] - color1[3]);
      gint   c;

      if (alpha)
        {
          gfloat ratio = a * color2[3] / alpha;

          for (c = 0; c < 3; c++)
            result[c] = color1[c] + ratio * (color2[c] - color1[c]);
        }
      else
        {
          memcpy (result, color1, 3 * sizeof (gfloat));
        }

      result[3] = alpha;
    }
}

static gboolean
process (GeglOperation       *operation,
         void                *out_buf,
         glong                n_pixels,
         const GeglRectangle *roi,
         gint                 level)
{
  GeglProperties      *o         = GEGL_PROPERTIES (operation);
  const Babl          *format    = babl_format ("R'G'B'A float");
  gfloat              *dest      = out_buf;
  gdouble              radius    = o->radius;
  gdouble              thickness = o->thickness;
  gdouble              angle     = o->angle / 360.0;
  gboolean             clockwise = o->direction == GEGL_SPIRAL_DIRECTION_CLOCKWISE;
  gdouble              lim;
  gfloat               color1[4];
  gfloat               color2[4];
  gdouble              x0;
  gdouble              x;
  gdouble              y;
  gint                 i;
  gint                 j;

  if (thickness <= 0.5)
    {
      gegl_color_get_pixel (o->color1, format, color1);
      gegl_color_get_pixel (o->color2, format, color2);
    }
  else
    {
      thickness  = 1.0 - thickness;
      angle     -= 0.5;

      gegl_color_get_pixel (o->color2, format, color1);
      gegl_color_get_pixel (o->color1, format, color2);
    }

  if (radius == 1.0 || thickness == 0.0)
    {
      gfloat color[4];

      blend (color2, color1, thickness, color);

      gegl_memset_pattern (dest, color, sizeof (color), n_pixels);

      return TRUE;
    }

  lim = thickness * radius;

  if (clockwise)
    angle = 1.0 - angle;

  angle += thickness / 2.0;

  x0 = roi->x - gegl_coordinate_relative_to_pixel (o->x, o->width);
  y  = roi->y - gegl_coordinate_relative_to_pixel (o->y, o->height);

  for (j = roi->height; j; j--, y++)
    {
      gdouble y2 = y * y;

      x = x0;

      for (i = roi->width; i; i--, x++)
        {
          gdouble x2 = x * x;
          gdouble r;
          gdouble t;
          gdouble s;
          gdouble a;

          r = sqrt (x2 + y2);
          t = atan2 (clockwise ? y : -y, x) / (2.0 * G_PI) - angle;

          t = fmod (t + 2.0, 1.0);
          s = t * radius;

          if (r >= s)
            {
              r = fmod (r - s, radius);

              if (r < 0.5)
                a = MIN (lim, r + 0.5);
              else if (r > radius - 0.5)
                a = MAX (lim - (r - 0.5), 0.0) + MIN (lim, (r + 0.5) - radius);
              else
                a = CLAMP (lim - (r - 0.5), 0.0, 1.0);
            }
          else
            {
              gdouble l = lim - (radius - s);

              if (t <= 0.5)
                {
                  if (r < 0.5)
                    a = CLAMP (l + radius / 2.0, 0.0, 0.5 - r);
                  else
                    a = 0.0;
                }
              else
                {
                  if (r < 0.5)
                    a = CLAMP (l, 0.0, r + 0.5);
                  else
                    a = CLAMP (l - (r - 0.5), 0.0, 1.0);
                }

              if (r > s - 0.5)
                a += MIN (lim, (r + 0.5) - s);
            }

          blend (color2, color1, a, dest);

          dest += 4;
        }
    }

  return TRUE;
}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass            *operation_class;
  GeglOperationPointRenderClass *point_render_class;

  operation_class    = GEGL_OPERATION_CLASS (klass);
  point_render_class = GEGL_OPERATION_POINT_RENDER_CLASS (klass);

  point_render_class->process = process;

  operation_class->get_bounding_box = get_bounding_box;
  operation_class->prepare = prepare;
  operation_class->opencl_support = FALSE;

  gegl_operation_class_set_keys (operation_class,
    "name",               "gegl:spiral",
    "title",              _("Spiral"),
    "categories",         "render",
    "position-dependent", "true",
    "license",            "GPL3+",
    "description",        _("Spiral renderer"),
    NULL);
}

#endif
