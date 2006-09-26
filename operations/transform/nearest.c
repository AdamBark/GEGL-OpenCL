/* This file is part of GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Copyright 2006 Philip Lafleur
 */

#include <gegl-plugin.h>
#include <string.h>

#include "matrix.h"

void
affine_nearest (GeglBuffer *dest,
                GeglBuffer *src,
                Matrix3     matrix)
{
  gint     x, y,
           u, v,
           src_w  = src->width,
           src_h  = src->height;
  guint32 *src_buf,
          *dest_buf,
          *dest_ptr,
           abyss = 0;
  Matrix3  inverse;
  gdouble  u_start,
           v_start,
           u_float,
           v_float;

  src_buf  = g_malloc (gegl_buffer_pixels (src) << 2);
  dest_buf = g_malloc (gegl_buffer_pixels (dest) << 2);
  g_assert (src_buf && dest_buf);
  gegl_buffer_get_fmt (src, src_buf, babl_format ("RGBA u8"));

  matrix3_copy (inverse, matrix);
  matrix3_invert (inverse);

  u_start = inverse[0][0] * dest->x + inverse[0][1] * dest->y
            + inverse[0][2] - src->x;
  v_start = inverse[1][0] * dest->x + inverse[1][1] * dest->y
            + inverse[1][2] - src->y;

  /* correct rounding on e.g. negative scaling (is this sound?) */
  if (inverse [0][0] < 0.)
    u_start -= .001;
  if (inverse [1][1] < 0.)
    v_start -= .001;

  for (dest_ptr = dest_buf, y = dest->height; y--;)
    {
      u_float = u_start;
      v_float = v_start;

      for (x = dest->width; x--; dest_ptr++)
        {
          u = u_float;
          v = v_float;

          if (G_LIKELY (u >= 0 &&
                        v >= 0 &&
                        u < src_w &&
                        v < src_h))
            *dest_ptr = src_buf [v * src_w + u];
          else
            *dest_ptr = abyss;

          u_float += inverse [0][0];
          v_float += inverse [1][0];
        }

      u_start += inverse [0][1];
      v_start += inverse [1][1];
    }

  gegl_buffer_set_fmt (dest, dest_buf, babl_format ("RGBA u8"));

  g_free (src_buf);
  g_free (dest_buf);
}

void
scale_nearest (GeglBuffer *dest,
               GeglBuffer *src,
               Matrix3     matrix)
{
  gint     x, y,
           src_w  = src->width,
           src_h  = src->height,
           dest_w = dest->width,
           dest_h = dest->height,
           dest_rowstride = dest->width << 2,
           skip_end   = 0,
           skip_left  = 0,
           skip_right = 0;
  guint32 *src_buf,
          *dest_buf,
          *src_ptr,
          *dest_ptr;
  Matrix3  inverse;
  gdouble  u_start,
           v_start,
           u_float,
           v_float;

  src_buf  = g_malloc (gegl_buffer_pixels (src) << 2);
  dest_buf = g_malloc (gegl_buffer_pixels (dest) << 2);
  g_assert (src_buf && dest_buf);
  gegl_buffer_get_fmt (src, src_buf, babl_format ("RGBA u8"));

  matrix3_copy (inverse, matrix);
  matrix3_invert (inverse);

  u_start = inverse[0][0] * dest->x + inverse[0][2] - src->x;
  v_start = inverse[1][1] * dest->y + inverse[1][2] - src->y;

  /* correct rounding on e.g. negative scaling (is this sound?) */
  if (inverse [0][0] < 0.)
    u_start -= .001;
  if (inverse [1][1] < 0.)
    v_start -= .001;

  /* determine area outside source data
   * (due to rounding errors in rect calculation) */
  dest_ptr = dest_buf;
  for (; ((gint) v_start < 0 || (gint) v_start >= src_h) && dest_h; dest_h--)
    {
      memset (dest_ptr, 0, dest_rowstride);
      dest_ptr += dest->width;
      v_start += inverse [1][1];
    }

  for (; ((gint) u_start < 0 || (gint) u_start >= src_w) && dest_w; dest_w--)
    {
      skip_left++;
      u_start += inverse [0][0];
    }

  v_float = v_start + (dest_h - 1) * inverse [1][1];
  for (; ((gint) v_float < 0 || (gint) v_float >= src_h) && dest_h; dest_h--)
    {
      skip_end++;
      v_float -= inverse [1][1];
    }

  u_float = u_start + (dest_w - 1) * inverse [0][0];
  for (; ((gint) u_float < 0 || (gint) u_float >= src_w) && dest_w; dest_w--)
    {
      skip_right++;
      u_float -= inverse [0][0];
    }

  for (y = dest_h; y--; v_start += inverse [1][1])
    {
      for (x = skip_left; x--;)
        *dest_ptr++ = 0;

      u_float = u_start;
      src_ptr = src_buf + ((gint) v_start) * src_w;

      for (x = dest_w; x--; u_float += inverse [0][0])
        *dest_ptr++ = src_ptr [(gint) u_float];

      for (x = skip_right; x--;)
        *dest_ptr++ = 0;
    }

  while (skip_end--)
    {
      memset (dest_ptr, 0, dest_rowstride);
      dest_ptr += dest->width;
    }

  gegl_buffer_set_fmt (dest, dest_buf, babl_format ("RGBA u8"));

  g_free (src_buf);
  g_free (dest_buf);
}

