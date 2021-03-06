/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/*
 * Copyright (C) 2016 Red Hat
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include "config.h"

#include "backends/meta-logical-monitor.h"

G_DEFINE_TYPE (MetaLogicalMonitor, meta_logical_monitor, G_TYPE_OBJECT)

MetaLogicalMonitor *
meta_logical_monitor_new (MetaMonitor *monitor,
                          int          x,
                          int          y,
                          int          number)
{
  MetaLogicalMonitor *logical_monitor;
  MetaOutput *main_output;
  GList *outputs;
  GList *l;
  gboolean is_presentation;

  g_assert (meta_monitor_is_active (monitor));

  logical_monitor = g_object_new (META_TYPE_LOGICAL_MONITOR, NULL);

  main_output = meta_monitor_get_main_output (monitor);
  logical_monitor->number = number;
  logical_monitor->winsys_id = main_output->winsys_id;
  logical_monitor->scale = main_output->scale;
  logical_monitor->in_fullscreen = -1;

  logical_monitor->rect.x = x;
  logical_monitor->rect.y = y;
  meta_monitor_get_dimensions (monitor,
                               &logical_monitor->rect.width,
                               &logical_monitor->rect.height);

  is_presentation = TRUE;
  outputs = meta_monitor_get_outputs (monitor);
  for (l = outputs; l; l = l->next)
    {
      MetaOutput *output = l->data;

      output->crtc->logical_monitor = logical_monitor;

      is_presentation = is_presentation && output->is_presentation;
    }

  logical_monitor->is_presentation = is_presentation;

  logical_monitor->monitors = g_list_append (logical_monitor->monitors,
                                             monitor);

  return logical_monitor;
}

void
meta_logical_monitor_add_monitor (MetaLogicalMonitor *logical_monitor,
                                  MetaMonitor        *monitor)
{
  GList *l;
  gboolean is_presentation;

  is_presentation = logical_monitor->is_presentation;
  logical_monitor->monitors = g_list_append (logical_monitor->monitors,
                                             monitor);

  for (l = logical_monitor->monitors; l; l = l->next)
    {
      MetaMonitor *monitor = l->data;
      GList *outputs;
      GList *l_output;

      outputs = meta_monitor_get_outputs (monitor);
      for (l_output = outputs; l_output; l_output = l_output->next)
        {
          MetaOutput *output = l_output->data;

          is_presentation = is_presentation && output->is_presentation;
          output->crtc->logical_monitor = logical_monitor;
        }
    }

  logical_monitor->is_presentation = is_presentation;
}

gboolean
meta_logical_monitor_is_primary (MetaLogicalMonitor *logical_monitor)
{
  return logical_monitor->is_primary;
}

void
meta_logical_monitor_make_primary (MetaLogicalMonitor *logical_monitor)
{
  logical_monitor->is_primary = TRUE;
}

int
meta_logical_monitor_get_scale (MetaLogicalMonitor *logical_monitor)
{
  return logical_monitor->scale;
}

GList *
meta_logical_monitor_get_monitors (MetaLogicalMonitor *logical_monitor)
{
  return logical_monitor->monitors;
}

static void
meta_logical_monitor_init (MetaLogicalMonitor *logical_monitor)
{
}

static void
meta_logical_monitor_class_init (MetaLogicalMonitorClass *klass)
{
}
