/*-
 * Copyright (C) 2012 Christian Hesse
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 */

#include <config.h>

#include <gio/gio.h>
#include <polkit/polkit.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>

#include <xfce4-session/xfsm-systemd.h>



#define SYSTEMD_DBUS_NAME               "org.freedesktop.login1"
#define SYSTEMD_DBUS_PATH               "/org/freedesktop/login1"
#define SYSTEMD_DBUS_INTERFACE          "org.freedesktop.login1.Manager"
#define SYSTEMD_REBOOT_ACTION           "Reboot"
#define SYSTEMD_POWEROFF_ACTION         "PowerOff"
#define SYSTEMD_REBOOT_TEST             "org.freedesktop.login1.reboot"
#define SYSTEMD_POWEROFF_TEST           "org.freedesktop.login1.power-off"



static void     xfsm_systemd_finalize     (GObject         *object);



struct _XfsmSystemdClass
{
  GObjectClass __parent__;
};

struct _XfsmSystemd
{
  GObject __parent__;

  PolkitAuthority *authority;
  PolkitSubject   *subject;
};



G_DEFINE_TYPE (XfsmSystemd, xfsm_systemd, G_TYPE_OBJECT)



static void
xfsm_systemd_class_init (XfsmSystemdClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = xfsm_systemd_finalize;
}



static void
xfsm_systemd_init (XfsmSystemd *systemd)
{
  systemd->authority = polkit_authority_get_sync (NULL, NULL);
  systemd->subject = polkit_unix_process_new (getpid());
}



static void
xfsm_systemd_finalize (GObject *object)
{
  XfsmSystemd *systemd = XFSM_SYSTEMD (object);

  g_object_unref (G_OBJECT (systemd->authority));
  g_object_unref (G_OBJECT (systemd->subject));

  (*G_OBJECT_CLASS (xfsm_systemd_parent_class)->finalize) (object);
}


static gboolean
xfsm_systemd_can_method (XfsmSystemd  *systemd,
                         gboolean     *can_method,
                         const gchar  *method,
                         GError      **error)
{
  PolkitAuthorizationResult *res;
  GError                    *local_error = NULL;

  *can_method = FALSE;

  res = polkit_authority_check_authorization_sync (systemd->authority,
                                                   systemd->subject,
                                                   method,
                                                   NULL,
                                                   POLKIT_CHECK_AUTHORIZATION_FLAGS_NONE,
                                                   NULL,
                                                   &local_error);

  if (res == NULL)
    {
      g_propagate_error (error, local_error);
      return FALSE;
    }

  *can_method = polkit_authorization_result_get_is_authorized (res)
                || polkit_authorization_result_get_is_challenge (res);

  g_object_unref (G_OBJECT (res));

  return TRUE;
}



static gboolean
xfsm_systemd_try_method (XfsmSystemd  *systemd,
                         const gchar  *method,
                         GError      **error)
{
  GDBusConnection *bus;
  GError          *local_error = NULL;

  bus = g_bus_get_sync (G_BUS_TYPE_SYSTEM, NULL, error);
  if (G_UNLIKELY (bus == NULL))
    return FALSE;

  g_dbus_connection_call_sync (bus,
                               SYSTEMD_DBUS_NAME,
                               SYSTEMD_DBUS_PATH,
                               SYSTEMD_DBUS_INTERFACE,
                               method,
                               g_variant_new ("(b)", TRUE),
                               NULL, 0, G_MAXINT, NULL,
                               &local_error);

  g_object_unref (G_OBJECT (bus));

  if (local_error != NULL)
    {
      g_propagate_error (error, local_error);
      return FALSE;
    }

  return TRUE;
}



XfsmSystemd *
xfsm_systemd_get (void)
{
  static XfsmSystemd *object = NULL;

  if (G_LIKELY (object != NULL))
    {
      g_object_ref (G_OBJECT (object));
    }
  else
    {
      object = g_object_new (XFSM_TYPE_SYSTEMD, NULL);
      g_object_add_weak_pointer (G_OBJECT (object), (gpointer) &object);
    }

  return object;
}



gboolean
xfsm_systemd_try_restart (XfsmSystemd  *systemd,
                          GError      **error)
{
  return xfsm_systemd_try_method (systemd,
                                  SYSTEMD_REBOOT_ACTION,
                                  error);
}



gboolean
xfsm_systemd_try_shutdown (XfsmSystemd  *systemd,
                           GError      **error)
{
  return xfsm_systemd_try_method (systemd,
                                  SYSTEMD_POWEROFF_ACTION,
                                  error);
}



gboolean
xfsm_systemd_can_restart (XfsmSystemd  *systemd,
                          gboolean     *can_restart,
                          GError      **error)
{
  return xfsm_systemd_can_method (systemd,
                                  can_restart,
                                  SYSTEMD_REBOOT_TEST,
                                  error);
}



gboolean
xfsm_systemd_can_shutdown (XfsmSystemd  *systemd,
                           gboolean     *can_shutdown,
                           GError      **error)
{
  return xfsm_systemd_can_method (systemd,
                                  can_shutdown,
                                  SYSTEMD_POWEROFF_TEST,
                                  error);
}
