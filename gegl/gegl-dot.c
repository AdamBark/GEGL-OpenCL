/* This file is the public GEGL API
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
 * License along with GEGL; if not, see <http://www.gnu.org/licenses>.
 *
 * 2006 © Øyvind Kolås.
 */

/* FIXME: this file should be implemented using public API only */

#include <stdio.h>
#include <string.h>
#include <glib-object.h>

#include "gegl.h"
#include "gegl-types-internal.h"
#include "graph/gegl-node.h"
#include "graph/gegl-pad.h"
#include "graph/gegl-connection.h"
#include "gegl-dot.h"
#include "gegl.h"

static void
gegl_dot_util_add_node (GString  *string,
                        GeglNode *node)
{
  g_string_append_printf (string, "op_%p [label=\"", node);

  /* We build the record from top to bottom */
  g_string_append_printf (string, "{");

  /* The first row is a list of output pads */
  {
    GSList  *pads       = gegl_node_get_pads (node);
    GSList  *entry      = pads;
    gboolean got_output = FALSE;

    g_string_append_printf (string, "{");

    while (entry)
      {
        GeglPad *pad = entry->data;
        if (gegl_pad_is_output (pad))
          {
            if (got_output)
              {
                g_string_append (string, "|");
              }
            got_output = TRUE;
            g_string_append_printf (string, "<%s>%s",
                                    gegl_pad_get_name (pad),
                                    gegl_pad_get_name (pad));
          }
        entry = g_slist_next (entry);
      }

    g_string_append_printf (string, "}|");
  }

  /* The second row is the operation name such as gegl:translate */
  g_string_append_printf (string, "%s %p |", gegl_node_get_debug_name (node), node);

  /* The next rows are property names and their values */
  if (1)
    {
      guint        n_properties;
      GParamSpec **properties = gegl_list_properties (gegl_node_get_operation (node), &n_properties);
      gint         i;
      for (i = 0; i < n_properties; i++)
        {
          const gchar *name   = properties[i]->name;
          GValue       tvalue = { 0, };
          GValue       svalue = { 0, };

          if (properties[i]->value_type == GEGL_TYPE_BUFFER)
            continue;

          g_value_init (&svalue, G_TYPE_STRING);
          g_value_init (&tvalue, properties[i]->value_type);

          gegl_node_get_property (node, name, &tvalue);

          if (g_value_transform (&tvalue, &svalue))
            {
              gchar *sval = g_value_dup_string (&svalue);
              if (sval && strlen (sval) > 30)
                {
                  sval[28] = '.';
                  sval[29] = '.';
                  sval[30] = '\0';
                }
              if (sval)
                {
                  g_string_append_printf (string, "%s=%s | ", name, sval);
                  g_free (sval);
                }
              g_value_unset (&svalue);
            }
          g_value_unset (&tvalue);
        }
      g_free (properties);
    }

  /* The last row is input pads */
  {
    GSList  *pads      = gegl_node_get_pads (node);
    GSList  *entry     = pads;
    gboolean got_input = FALSE;

    g_string_append_printf (string, "{");

    while (entry)
      {
        GeglPad *pad = entry->data;
        if (gegl_pad_is_input (pad))
          {
            if (got_input)
              {
                g_string_append (string, "|");
              }
            got_input = TRUE;
            g_string_append_printf (string, "<%s>%s",
                                    gegl_pad_get_name (pad),
                                    gegl_pad_get_name (pad));
          }
        entry = g_slist_next (entry);
      }

    g_string_append_printf (string, "}");
  }

  g_string_append_printf (string, "}\"");
  g_string_append_printf (string, "shape=\"record\"];\n");
}

static void
gegl_dot_util_add_node_sink_edges (GString  *string,
                                   GeglNode *node)
{
  GSList *connections = gegl_node_get_sinks (node);
  GSList *iter;

  for (iter = connections; iter; iter = g_slist_next (iter))
    {
      GeglConnection *connection = iter->data;
      GeglNode       *source;
      GeglNode       *sink;
      GeglPad        *source_pad;
      GeglPad        *sink_pad;

      source     = gegl_connection_get_source_node (connection);
      sink       = gegl_connection_get_sink_node (connection);
      source_pad = gegl_connection_get_source_pad (connection);
      sink_pad   = gegl_connection_get_sink_pad (connection);

      g_string_append_printf (string, "op_%p:%s -> op_%p:%s;\n",
                              source, gegl_pad_get_name (source_pad),
                              sink,   gegl_pad_get_name (sink_pad));
    }
}

static void
gegl_dot_add_graph (GString     *string,
                    GeglNode    *node,
                    const gchar *label)
{
  GeglNode *graph = node;

  g_string_append_printf (string, "subgraph cluster_%s%p { graph [ label=\"%s %p\" fontsize=\"10\" ranksep=\"0.3\" nodesep=\"0.3\"]; node [ fontsize=\"10\" ];\n", label, node, label, node);

  {
    GSList *nodes = gegl_node_get_children (graph);
    GSList *entry = nodes;

    while (entry)
      {
        GeglNode *node = entry->data;

        if (node->is_graph)
          {
            gchar *name = g_strdup (gegl_node_get_debug_name (node));
            gchar *p    = name;
            while (*p)
              {
                if (*p == ' ' ||
                    *p == '-')
                  *p = '_';
                p++;
              }
            gegl_dot_add_graph (string, node, name);
            g_free (name);
          }

        gegl_dot_util_add_node (string, node);

        entry = g_slist_next (entry);
      }

    g_slist_free (nodes);
  }

  {
    GSList *nodes = gegl_node_get_children (graph);
    GSList *entry = nodes;

    while (entry)
      {
        GeglNode *node = entry->data;

        gegl_dot_util_add_node_sink_edges (string, node);

        entry = g_slist_next (entry);
      }
    g_slist_free (nodes);
  }
  g_string_append_printf (string, "}\n");
}

gchar *
gegl_to_dot (GeglNode *node)
{
  GString *string;

  string = g_string_new ("digraph gegl { graph [ rankdir = \"BT\"];\n");
  gegl_dot_add_graph (string, node, "GEGL");
  g_string_append (string, "}");

  return g_string_free (string, FALSE);
}
