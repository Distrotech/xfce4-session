/* $Id$ */
/*-
 * Copyright (c) 2003-2004 Benedikt Meurer <benny@xfce.org>
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *                                                                              
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *                                                                              
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef __XFSM_SPLASH_RC_H__
#define __XFSM_SPLASH_RC_H__

#include <libxfce4util/libxfce4util.h>


G_BEGIN_DECLS;

typedef struct _XfsmSplashRc XfsmSplashRc;


XfsmSplashRc *xfsm_splash_rc_new              (XfceRc       *rc,
                                               const gchar  *group);
const gchar  *xfsm_splash_rc_read_entry       (XfsmSplashRc *splash_rc,
                                               const gchar  *key,
                                               const gchar  *fallback);
gint          xfsm_splash_rc_read_int_entry   (XfsmSplashRc *splash_rc,
                                               const gchar  *key,
                                               gint          fallback);
gboolean      xfsm_splash_rc_read_bool_entry  (XfsmSplashRc *splash_rc,
                                               const gchar  *key,
                                               gboolean      fallback);
gchar       **xfsm_splash_rc_read_list_entry  (XfsmSplashRc *splash_rc,
                                               const gchar  *key,
                                               const gchar  *delimiter);
void          xfsm_splash_rc_write_entry      (XfsmSplashRc *splash_rc,
                                               const gchar  *key,
                                               const gchar  *value);
void          xfsm_splash_rc_write_int_entry  (XfsmSplashRc *splash_rc,
                                               const gchar  *key,
                                               gint          value);
void          xfsm_splash_rc_write_bool_entry (XfsmSplashRc *splash_rc,
                                               const gchar  *key,
                                               gboolean      value);
void          xfsm_splash_rc_write_list_entry (XfsmSplashRc *splash_rc,
                                               const gchar  *key,
                                               gchar       **value,
                                               const gchar  *delimiter);
void          xfsm_splash_rc_free             (XfsmSplashRc *splash_rc);

G_END_DECLS;


#endif /* !__XFSM_SPLASH_RC_H__ */
