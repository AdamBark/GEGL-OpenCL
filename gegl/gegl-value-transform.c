/* GObject - GLib Type, Object, Parameter and Signal Library
 * Copyright (C) 2001 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include        <string.h>

#include	"gvalue.h"
#include	"genums.h"


/* same type transforms
 */
static void
value_transform_memcpy_data0 (const GValue *src_value,
			      GValue       *dest_value)
{
  memcpy (&dest_value->data[0], &src_value->data[0], sizeof (src_value->data[0]));
}
#define	value_transform_int_int		value_transform_memcpy_data0
#define	value_transform_uint_uint	value_transform_memcpy_data0
#define	value_transform_long_long	value_transform_memcpy_data0
#define	value_transform_ulong_ulong	value_transform_memcpy_data0
#define	value_transform_float_float	value_transform_memcpy_data0
#define	value_transform_double_double	value_transform_memcpy_data0


/* numeric casts
 */
#define	DEFINE_CAST(func_name, from_member, ctype, to_member)			\
    static void									\
    value_transform_##func_name (const GValue *src_value,			\
			         GValue       *dest_value)			\
    {										\
      ctype c_value = src_value->data[0].from_member;				\
      dest_value->data[0].to_member = c_value;					\
    }
DEFINE_CAST (int_s8,		v_int,    gint8,   v_int);
DEFINE_CAST (int_u8,		v_int,    guint8,  v_uint);
DEFINE_CAST (int_uint,		v_int,    guint,   v_uint);
DEFINE_CAST (int_long,		v_int,    glong,   v_long);
DEFINE_CAST (int_ulong,		v_int,    gulong,  v_ulong);
DEFINE_CAST (int_float,		v_int,    gfloat,  v_float);
DEFINE_CAST (int_double,	v_int,    gdouble, v_double);
DEFINE_CAST (uint_s8,		v_uint,   gint8,   v_int);
DEFINE_CAST (uint_u8,		v_uint,   guint8,  v_uint);
DEFINE_CAST (uint_int,		v_uint,   gint,    v_int);
DEFINE_CAST (uint_long,		v_uint,   glong,   v_long);
DEFINE_CAST (uint_ulong,	v_uint,   gulong,  v_ulong);
DEFINE_CAST (uint_float,	v_uint,   gfloat,  v_float);
DEFINE_CAST (uint_double,	v_uint,   gdouble, v_double);
DEFINE_CAST (long_s8,		v_long,   gint8,   v_int);
DEFINE_CAST (long_u8,		v_long,   guint8,  v_uint);
DEFINE_CAST (long_int,		v_long,   gint,    v_int);
DEFINE_CAST (long_uint,		v_long,   guint,   v_uint);
DEFINE_CAST (long_ulong,	v_long,   gulong,  v_ulong);
DEFINE_CAST (long_float,	v_long,   gfloat,  v_float);
DEFINE_CAST (long_double,	v_long,   gdouble, v_double);
DEFINE_CAST (ulong_s8,		v_ulong,  gint8,   v_int);
DEFINE_CAST (ulong_u8,		v_ulong,  guint8,  v_uint);
DEFINE_CAST (ulong_int,		v_ulong,  gint,    v_int);
DEFINE_CAST (ulong_uint,	v_ulong,  guint,   v_uint);
DEFINE_CAST (ulong_long,	v_ulong,  glong,   v_long);
DEFINE_CAST (ulong_float,	v_ulong,  gfloat,  v_float);
DEFINE_CAST (ulong_double,	v_ulong,  gdouble, v_double);
DEFINE_CAST (float_s8,		v_float,  gint8,   v_int);
DEFINE_CAST (float_u8,		v_float,  guint8,  v_uint);
DEFINE_CAST (float_int,		v_float,  gint,    v_int);
DEFINE_CAST (float_uint,	v_float,  guint,   v_uint);
DEFINE_CAST (float_long,	v_float,  glong,   v_long);
DEFINE_CAST (float_ulong,	v_float,  gulong,  v_ulong);
DEFINE_CAST (float_double,	v_float,  gdouble, v_double);
DEFINE_CAST (double_s8,		v_double, gint8,   v_int);
DEFINE_CAST (double_u8,		v_double, guint8,  v_uint);
DEFINE_CAST (double_int,	v_double, gint,    v_int);
DEFINE_CAST (double_uint,	v_double, guint,   v_uint);
DEFINE_CAST (double_long,	v_double, glong,   v_long);
DEFINE_CAST (double_ulong,	v_double, gulong,  v_ulong);
DEFINE_CAST (double_float,	v_double, gfloat,  v_float);


/* boolean assignments
 */
#define	DEFINE_BOOL_CHECK(func_name, from_member)				\
    static void									\
    value_transform_##func_name (const GValue *src_value,			\
			         GValue       *dest_value)			\
    {										\
      dest_value->data[0].v_int = src_value->data[0].from_member != 0;	\
    }
DEFINE_BOOL_CHECK (int_bool,	v_int);
DEFINE_BOOL_CHECK (uint_bool,	v_uint);
DEFINE_BOOL_CHECK (long_bool,	v_long);
DEFINE_BOOL_CHECK (ulong_bool,	v_uint);


/* string printouts
 */
#define	DEFINE_SPRINTF(func_name, from_member, format)				\
    static void									\
    value_transform_##func_name (const GValue *src_value,			\
			         GValue       *dest_value)			\
    {										\
      dest_value->data[0].v_pointer =						\
        g_strdup_printf ((format), src_value->data[0].from_member);		\
    }
DEFINE_SPRINTF (int_string,	v_int,	  "%d");
DEFINE_SPRINTF (uint_string,	v_uint,	  "%u");
DEFINE_SPRINTF (long_string,	v_long,	  "%ld");
DEFINE_SPRINTF (ulong_string,	v_ulong,  "%lu");
DEFINE_SPRINTF (float_string,	v_float,  "%f");
DEFINE_SPRINTF (double_string,	v_double, "%f");


/* special cases
 */
static void
value_transform_bool_string (const GValue *src_value,
			     GValue       *dest_value)
{
  dest_value->data[0].v_pointer = g_strdup_printf ("%s",
						   src_value->data[0].v_int ?
						   "TRUE" : "FALSE");
}
static void
value_transform_string_string (const GValue *src_value,
			       GValue       *dest_value)
{
  dest_value->data[0].v_pointer = g_strdup (src_value->data[0].v_pointer);
}
static void
value_transform_enum_string (const GValue *src_value,
			     GValue       *dest_value)
{
  GEnumClass *class = g_type_class_ref (G_VALUE_TYPE (src_value));
  GEnumValue *enum_value = g_enum_get_value (class, src_value->data[0].v_int);

  if (enum_value)
    dest_value->data[0].v_pointer = g_strdup (enum_value->value_name);
  else
    dest_value->data[0].v_pointer = g_strdup_printf ("%d", src_value->data[0].v_int);

  g_type_class_unref (class);
}
static void
value_transform_flags_string (const GValue *src_value,
			      GValue       *dest_value)
{
  GFlagsClass *class = g_type_class_ref (G_VALUE_TYPE (src_value));
  GFlagsValue *flags_value = g_flags_get_first_value (class, src_value->data[0].v_uint);

  if (flags_value)
    {
      GString *gstring = g_string_new ("");
      guint v_flags = src_value->data[0].v_uint;
      
      do
	{
	  v_flags &= ~flags_value->value;
	  
	  if (gstring->str[0])
	    g_string_append (gstring, " | ");
	  g_string_append (gstring, flags_value->value_name);
	  flags_value = g_flags_get_first_value (class, v_flags);
	}
      while (flags_value);

      if (v_flags)
	dest_value->data[0].v_pointer = g_strdup_printf ("%s | %u",
							 gstring->str,
							 v_flags);
      else
	dest_value->data[0].v_pointer = g_strdup (gstring->str);
      g_string_free (gstring, TRUE);
    }
  else
    dest_value->data[0].v_pointer = g_strdup_printf ("%u", src_value->data[0].v_uint);

  g_type_class_unref (class);
}


/* registration
 */
void
g_value_transforms_init (void)		/* sync with gtype.c */
{
  /* some transformations are a bit questionable,
   * we currently skip those
   */
#define	SKIP____register_transform_func(type1,type2,transform_func)	/* skip questionable transforms */

  /* numeric types (plus to string) */
  g_value_register_transform_func (G_TYPE_CHAR,		G_TYPE_CHAR,		value_transform_int_int);
  g_value_register_transform_func (G_TYPE_CHAR,		G_TYPE_UCHAR,		value_transform_int_u8);
  g_value_register_transform_func (G_TYPE_CHAR,		G_TYPE_BOOLEAN,		value_transform_int_bool);
  g_value_register_transform_func (G_TYPE_CHAR,		G_TYPE_INT,		value_transform_int_int);
  g_value_register_transform_func (G_TYPE_CHAR,		G_TYPE_UINT,		value_transform_int_uint);
  g_value_register_transform_func (G_TYPE_CHAR,		G_TYPE_LONG,		value_transform_int_long);
  g_value_register_transform_func (G_TYPE_CHAR,		G_TYPE_ULONG,		value_transform_int_ulong);
  g_value_register_transform_func (G_TYPE_CHAR,		G_TYPE_ENUM,		value_transform_int_int);
  g_value_register_transform_func (G_TYPE_CHAR,		G_TYPE_FLAGS,		value_transform_int_uint);
  g_value_register_transform_func (G_TYPE_CHAR,		G_TYPE_FLOAT,		value_transform_int_float);
  g_value_register_transform_func (G_TYPE_CHAR,		G_TYPE_DOUBLE,		value_transform_int_double);
  g_value_register_transform_func (G_TYPE_CHAR,		G_TYPE_STRING,		value_transform_int_string);
  g_value_register_transform_func (G_TYPE_UCHAR,	G_TYPE_CHAR,		value_transform_uint_s8);
  g_value_register_transform_func (G_TYPE_UCHAR,	G_TYPE_UCHAR,		value_transform_uint_uint);
  g_value_register_transform_func (G_TYPE_UCHAR,	G_TYPE_BOOLEAN,		value_transform_uint_bool);
  g_value_register_transform_func (G_TYPE_UCHAR,	G_TYPE_INT,		value_transform_uint_int);
  g_value_register_transform_func (G_TYPE_UCHAR,	G_TYPE_UINT,		value_transform_uint_uint);
  g_value_register_transform_func (G_TYPE_UCHAR,	G_TYPE_LONG,		value_transform_uint_long);
  g_value_register_transform_func (G_TYPE_UCHAR,	G_TYPE_ULONG,		value_transform_uint_ulong);
  g_value_register_transform_func (G_TYPE_UCHAR,	G_TYPE_ENUM,		value_transform_uint_int);
  g_value_register_transform_func (G_TYPE_UCHAR,	G_TYPE_FLAGS,		value_transform_uint_uint);
  g_value_register_transform_func (G_TYPE_UCHAR,	G_TYPE_FLOAT,		value_transform_uint_float);
  g_value_register_transform_func (G_TYPE_UCHAR,	G_TYPE_DOUBLE,		value_transform_uint_double);
  g_value_register_transform_func (G_TYPE_UCHAR,	G_TYPE_STRING,		value_transform_uint_string);
  g_value_register_transform_func (G_TYPE_BOOLEAN,	G_TYPE_CHAR,		value_transform_int_s8);
  g_value_register_transform_func (G_TYPE_BOOLEAN,	G_TYPE_UCHAR,		value_transform_int_u8);
  g_value_register_transform_func (G_TYPE_BOOLEAN,	G_TYPE_BOOLEAN,		value_transform_int_int);
  g_value_register_transform_func (G_TYPE_BOOLEAN,	G_TYPE_INT,		value_transform_int_int);
  g_value_register_transform_func (G_TYPE_BOOLEAN,	G_TYPE_UINT,		value_transform_int_uint);
  g_value_register_transform_func (G_TYPE_BOOLEAN,	G_TYPE_LONG,		value_transform_int_long);
  g_value_register_transform_func (G_TYPE_BOOLEAN,	G_TYPE_ULONG,		value_transform_int_ulong);
  g_value_register_transform_func (G_TYPE_BOOLEAN,	G_TYPE_ENUM,		value_transform_int_int);
  g_value_register_transform_func (G_TYPE_BOOLEAN,	G_TYPE_FLAGS,		value_transform_int_uint);
  SKIP____register_transform_func (G_TYPE_BOOLEAN,	G_TYPE_FLOAT,		value_transform_int_float);
  SKIP____register_transform_func (G_TYPE_BOOLEAN,	G_TYPE_DOUBLE,		value_transform_int_double);
  g_value_register_transform_func (G_TYPE_BOOLEAN,	G_TYPE_STRING,		value_transform_bool_string);
  g_value_register_transform_func (G_TYPE_INT,		G_TYPE_CHAR,		value_transform_int_s8);
  g_value_register_transform_func (G_TYPE_INT,		G_TYPE_UCHAR,		value_transform_int_u8);
  g_value_register_transform_func (G_TYPE_INT,		G_TYPE_BOOLEAN,		value_transform_int_bool);
  g_value_register_transform_func (G_TYPE_INT,		G_TYPE_INT,		value_transform_int_int);
  g_value_register_transform_func (G_TYPE_INT,		G_TYPE_UINT,		value_transform_int_uint);
  g_value_register_transform_func (G_TYPE_INT,		G_TYPE_LONG,		value_transform_int_long);
  g_value_register_transform_func (G_TYPE_INT,		G_TYPE_ULONG,		value_transform_int_ulong);
  g_value_register_transform_func (G_TYPE_INT,		G_TYPE_ENUM,		value_transform_int_int);
  g_value_register_transform_func (G_TYPE_INT,		G_TYPE_FLAGS,		value_transform_int_uint);
  g_value_register_transform_func (G_TYPE_INT,		G_TYPE_FLOAT,		value_transform_int_float);
  g_value_register_transform_func (G_TYPE_INT,		G_TYPE_DOUBLE,		value_transform_int_double);
  g_value_register_transform_func (G_TYPE_INT,		G_TYPE_STRING,		value_transform_int_string);
  g_value_register_transform_func (G_TYPE_UINT,		G_TYPE_CHAR,		value_transform_uint_s8);
  g_value_register_transform_func (G_TYPE_UINT,		G_TYPE_UCHAR,		value_transform_uint_u8);
  g_value_register_transform_func (G_TYPE_UINT,		G_TYPE_BOOLEAN,		value_transform_uint_bool);
  g_value_register_transform_func (G_TYPE_UINT,		G_TYPE_INT,		value_transform_uint_int);
  g_value_register_transform_func (G_TYPE_UINT,		G_TYPE_UINT,		value_transform_uint_uint);
  g_value_register_transform_func (G_TYPE_UINT,		G_TYPE_LONG,		value_transform_uint_long);
  g_value_register_transform_func (G_TYPE_UINT,		G_TYPE_ULONG,		value_transform_uint_ulong);
  g_value_register_transform_func (G_TYPE_UINT,		G_TYPE_ENUM,		value_transform_uint_int);
  g_value_register_transform_func (G_TYPE_UINT,		G_TYPE_FLAGS,		value_transform_uint_uint);
  g_value_register_transform_func (G_TYPE_UINT,		G_TYPE_FLOAT,		value_transform_uint_float);
  g_value_register_transform_func (G_TYPE_UINT,		G_TYPE_DOUBLE,		value_transform_uint_double);
  g_value_register_transform_func (G_TYPE_UINT,		G_TYPE_STRING,		value_transform_uint_string);
  g_value_register_transform_func (G_TYPE_LONG,		G_TYPE_CHAR,		value_transform_long_s8);
  g_value_register_transform_func (G_TYPE_LONG,		G_TYPE_UCHAR,		value_transform_long_u8);
  g_value_register_transform_func (G_TYPE_LONG,		G_TYPE_BOOLEAN,		value_transform_long_bool);
  g_value_register_transform_func (G_TYPE_LONG,		G_TYPE_INT,		value_transform_long_int);
  g_value_register_transform_func (G_TYPE_LONG,		G_TYPE_UINT,		value_transform_long_uint);
  g_value_register_transform_func (G_TYPE_LONG,		G_TYPE_LONG,		value_transform_long_long);
  g_value_register_transform_func (G_TYPE_LONG,		G_TYPE_ULONG,		value_transform_long_ulong);
  g_value_register_transform_func (G_TYPE_LONG,		G_TYPE_ENUM,		value_transform_long_int);
  g_value_register_transform_func (G_TYPE_LONG,		G_TYPE_FLAGS,		value_transform_long_uint);
  g_value_register_transform_func (G_TYPE_LONG,		G_TYPE_FLOAT,		value_transform_long_float);
  g_value_register_transform_func (G_TYPE_LONG,		G_TYPE_DOUBLE,		value_transform_long_double);
  g_value_register_transform_func (G_TYPE_LONG,		G_TYPE_STRING,		value_transform_long_string);
  g_value_register_transform_func (G_TYPE_ULONG,	G_TYPE_CHAR,		value_transform_ulong_s8);
  g_value_register_transform_func (G_TYPE_ULONG,	G_TYPE_UCHAR,		value_transform_ulong_u8);
  g_value_register_transform_func (G_TYPE_ULONG,	G_TYPE_BOOLEAN,		value_transform_ulong_bool);
  g_value_register_transform_func (G_TYPE_ULONG,	G_TYPE_INT,		value_transform_ulong_int);
  g_value_register_transform_func (G_TYPE_ULONG,	G_TYPE_UINT,		value_transform_ulong_uint);
  g_value_register_transform_func (G_TYPE_ULONG,	G_TYPE_LONG,		value_transform_ulong_long);
  g_value_register_transform_func (G_TYPE_ULONG,	G_TYPE_ULONG,		value_transform_ulong_ulong);
  g_value_register_transform_func (G_TYPE_ULONG,	G_TYPE_ENUM,		value_transform_ulong_int);
  g_value_register_transform_func (G_TYPE_ULONG,	G_TYPE_FLAGS,		value_transform_ulong_uint);
  g_value_register_transform_func (G_TYPE_ULONG,	G_TYPE_FLOAT,		value_transform_ulong_float);
  g_value_register_transform_func (G_TYPE_ULONG,	G_TYPE_DOUBLE,		value_transform_ulong_double);
  g_value_register_transform_func (G_TYPE_ULONG,	G_TYPE_STRING,		value_transform_ulong_string);
  g_value_register_transform_func (G_TYPE_ENUM,		G_TYPE_CHAR,		value_transform_int_s8);
  g_value_register_transform_func (G_TYPE_ENUM,		G_TYPE_UCHAR,		value_transform_int_u8);
  SKIP____register_transform_func (G_TYPE_ENUM,		G_TYPE_BOOLEAN,		value_transform_int_bool);
  g_value_register_transform_func (G_TYPE_ENUM,		G_TYPE_INT,		value_transform_int_int);
  g_value_register_transform_func (G_TYPE_ENUM,		G_TYPE_UINT,		value_transform_int_uint);
  g_value_register_transform_func (G_TYPE_ENUM,		G_TYPE_LONG,		value_transform_int_long);
  g_value_register_transform_func (G_TYPE_ENUM,		G_TYPE_ULONG,		value_transform_int_ulong);
  g_value_register_transform_func (G_TYPE_ENUM,		G_TYPE_ENUM,		value_transform_int_int);
  g_value_register_transform_func (G_TYPE_ENUM,		G_TYPE_FLAGS,		value_transform_int_uint);
  SKIP____register_transform_func (G_TYPE_ENUM,		G_TYPE_FLOAT,		value_transform_int_float);
  SKIP____register_transform_func (G_TYPE_ENUM,		G_TYPE_DOUBLE,		value_transform_int_double);
  g_value_register_transform_func (G_TYPE_ENUM,		G_TYPE_STRING,		value_transform_enum_string);
  g_value_register_transform_func (G_TYPE_FLAGS,	G_TYPE_CHAR,		value_transform_uint_s8);
  g_value_register_transform_func (G_TYPE_FLAGS,	G_TYPE_UCHAR,		value_transform_uint_u8);
  SKIP____register_transform_func (G_TYPE_FLAGS,	G_TYPE_BOOLEAN,		value_transform_uint_bool);
  g_value_register_transform_func (G_TYPE_FLAGS,	G_TYPE_INT,		value_transform_uint_int);
  g_value_register_transform_func (G_TYPE_FLAGS,	G_TYPE_UINT,		value_transform_uint_uint);
  g_value_register_transform_func (G_TYPE_FLAGS,	G_TYPE_LONG,		value_transform_uint_long);
  g_value_register_transform_func (G_TYPE_FLAGS,	G_TYPE_ULONG,		value_transform_uint_ulong);
  SKIP____register_transform_func (G_TYPE_FLAGS,	G_TYPE_ENUM,		value_transform_uint_int);
  g_value_register_transform_func (G_TYPE_FLAGS,	G_TYPE_FLAGS,		value_transform_uint_uint);
  SKIP____register_transform_func (G_TYPE_FLAGS,	G_TYPE_FLOAT,		value_transform_uint_float);
  SKIP____register_transform_func (G_TYPE_FLAGS,	G_TYPE_DOUBLE,		value_transform_uint_double);
  g_value_register_transform_func (G_TYPE_FLAGS,	G_TYPE_STRING,		value_transform_flags_string);
  g_value_register_transform_func (G_TYPE_FLOAT,	G_TYPE_CHAR,		value_transform_float_s8);
  g_value_register_transform_func (G_TYPE_FLOAT,	G_TYPE_UCHAR,		value_transform_float_u8);
  SKIP____register_transform_func (G_TYPE_FLOAT,	G_TYPE_BOOLEAN,		value_transform_float_bool);
  g_value_register_transform_func (G_TYPE_FLOAT,	G_TYPE_INT,		value_transform_float_int);
  g_value_register_transform_func (G_TYPE_FLOAT,	G_TYPE_UINT,		value_transform_float_uint);
  g_value_register_transform_func (G_TYPE_FLOAT,	G_TYPE_LONG,		value_transform_float_long);
  g_value_register_transform_func (G_TYPE_FLOAT,	G_TYPE_ULONG,		value_transform_float_ulong);
  SKIP____register_transform_func (G_TYPE_FLOAT,	G_TYPE_ENUM,		value_transform_float_int);
  SKIP____register_transform_func (G_TYPE_FLOAT,	G_TYPE_FLAGS,		value_transform_float_uint);
  g_value_register_transform_func (G_TYPE_FLOAT,	G_TYPE_FLOAT,		value_transform_float_float);
  g_value_register_transform_func (G_TYPE_FLOAT,	G_TYPE_DOUBLE,		value_transform_float_double);
  g_value_register_transform_func (G_TYPE_FLOAT,	G_TYPE_STRING,		value_transform_float_string);
  g_value_register_transform_func (G_TYPE_DOUBLE,	G_TYPE_CHAR,		value_transform_double_s8);
  g_value_register_transform_func (G_TYPE_DOUBLE,	G_TYPE_UCHAR,		value_transform_double_u8);
  SKIP____register_transform_func (G_TYPE_DOUBLE,	G_TYPE_BOOLEAN,		value_transform_double_bool);
  g_value_register_transform_func (G_TYPE_DOUBLE,	G_TYPE_INT,		value_transform_double_int);
  g_value_register_transform_func (G_TYPE_DOUBLE,	G_TYPE_UINT,		value_transform_double_uint);
  g_value_register_transform_func (G_TYPE_DOUBLE,	G_TYPE_LONG,		value_transform_double_long);
  g_value_register_transform_func (G_TYPE_DOUBLE,	G_TYPE_ULONG,		value_transform_double_ulong);
  SKIP____register_transform_func (G_TYPE_DOUBLE,	G_TYPE_ENUM,		value_transform_double_int);
  SKIP____register_transform_func (G_TYPE_DOUBLE,	G_TYPE_FLAGS,		value_transform_double_uint);
  g_value_register_transform_func (G_TYPE_DOUBLE,	G_TYPE_FLOAT,		value_transform_double_float);
  g_value_register_transform_func (G_TYPE_DOUBLE,	G_TYPE_DOUBLE,		value_transform_double_double);
  g_value_register_transform_func (G_TYPE_DOUBLE,	G_TYPE_STRING,		value_transform_double_string);
  /* string types */
  g_value_register_transform_func (G_TYPE_STRING,	G_TYPE_STRING,		value_transform_string_string);
}
