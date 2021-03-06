/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 2 -*- */
/* nautilus-gst-shares.h: this file is part of shares-admin, a gnome-system-tool frontend 
 * for shared folders administration.
 * 
 * Copyright (C) 2004 Carlos Garnacho
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 * Authors: Carlos Garnacho Parro <carlosg@gnome.org>.
 */

#ifndef NAUTILUS_SHARES_H
#define NAUTILUS_SHARES_H

#include <glib-object.h>
#include <oobs/oobs.h>
#include <libnautilus-extension/nautilus-file-info.h>

G_BEGIN_DECLS

#define NAUTILUS_TYPE_SHARES    (nautilus_shares_get_type ())
#define NAUTILUS_SHARES(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), NAUTILUS_TYPE_SHARES, NautilusShares))
#define NAUTILUS_IS_SHARES(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NAUTILUS_TYPE_SHARES))

typedef struct _NautilusShares      NautilusShares;
typedef struct _NautilusSharesClass NautilusSharesClass;

struct _NautilusShares {
  GObject parent;

  OobsSession *session;
  OobsObject *smb_config;
  OobsObject *nfs_config;

  GHashTable *paths;

  gint objects_updating;

  GPid pid;

  /* current file info */
  NautilusFileInfo *file_info;
};

struct _NautilusSharesClass {
  GObjectClass parent_class;
};

GType nautilus_shares_get_type      (void);
void  nautilus_shares_register_type (GTypeModule *module);

G_END_DECLS

#endif /* NAUTILUS_SHARES_H */
