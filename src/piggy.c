#include <stdlib.h>
#include <string.h>

#include "detach.h"
#include "device.h"
#include "io.h"
#include "message.h"
#include "piggy.h"
#include "process.h"
#include "script.h"
#include "sig.h"
#include "stop.h"
#include "util.h"

GMainLoop *loop = NULL;

int
main (int argc, char *argv[])
{
  gchar              *basename     = NULL;
  gchar              *target       = NULL;
  gchar              *script_file  = NULL;
  FridaDeviceManager *manager      = NULL;
  GError             *error        = NULL;
  FridaDevice        *local_device = NULL;
  guint               target_pid   = 0;
  FridaSession       *session      = NULL;
  FridaScript        *script       = NULL;
  GIOChannel         *channel      = NULL;

  frida_init ();

  do
  {
    basename = g_path_get_basename (argv[0]);

    if (argc != 3)
    {
      g_printerr ("Usage: %s (<pid> | <name>) <script>\n", argv[0]);
      break;
    }
    target      = argv[1];
    script_file = argv[2];

    loop = g_main_loop_new (NULL, TRUE);

    signal (SIGINT, on_signal);
    signal (SIGTERM, on_signal);

    manager = frida_device_manager_new ();
    if (manager == NULL)
    {
      g_printerr ("Failed to find device manager");
      break;
    }

    local_device = get_device (manager);
    if (local_device == NULL)
    {
      g_printerr ("Failed to find local device");
      break;
    }

    target_pid = get_process (local_device, target);
    if (target_pid == 0)
    {
      g_printerr ("Failed to find target process");
      break;
    }

    g_print (PIGGY_PREFIX_GREEN "target_pid: %u\n", target_pid);

    session =
        frida_device_attach_sync (local_device, target_pid, NULL, NULL, &error);
    if (session == NULL)
    {
      g_printerr ("Failed to attach to device: %s\n", error->message);
      g_warning ("Ensure that your /tmp is not mounted noexec (mount -o "
                 "remount,exec /tmp)");
      break;
    }

    g_print (PIGGY_PREFIX_GREEN "Attached device\n");

    script = load_script (session, script_file);
    if (target_pid == 0)
    {
      g_printerr ("Failed to load script");
      break;
    }

    g_print (PIGGY_PREFIX_GREEN "Script loaded\n");

    channel = io_init (script);
    if (channel == NULL)
    {
      g_printerr ("Failed to initialize io");
      break;
    }

    if (g_main_loop_is_running (loop))
      g_main_loop_run (loop);

    g_print (PIGGY_PREFIX_GREEN "Stopped\n");

  } while (FALSE);

  if (channel != NULL)
  {
    io_fini (channel);
  }

  if (script != NULL)
  {
    frida_script_unload_sync (script, NULL, NULL);
    frida_unref (script);
  }

  if (session != NULL)
  {
    frida_session_detach_sync (session, NULL, NULL);
    frida_unref (session);
  }

  if (local_device != NULL)
    frida_unref (local_device);

  if (manager != NULL)
  {
    frida_device_manager_close_sync (manager, NULL, NULL);
    frida_unref (manager);
  }

  if (loop != NULL)
    g_main_loop_unref (loop);

  if (error != NULL)
    g_error_free (error);

  if (basename != NULL)
  {
    g_free (basename);
  }

  return 0;
}
