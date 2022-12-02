#include "detach.h"
#include "io.h"
#include "stop.h"
#include "util.h"

void
on_detached (FridaSession            *session,
             FridaSessionDetachReason reason,
             FridaCrash              *crash,
             gpointer                 user_data)
{
  gchar *reason_str;

  reason_str = g_enum_to_string (FRIDA_TYPE_SESSION_DETACH_REASON, reason);
  g_print (PIGGY_PREFIX_GREEN "on_detached: reason=%s crash=%p\n",
           reason_str,
           (void *) crash);
  g_free (reason_str);

  io_reset ();

  g_idle_add (stop, NULL);
}
