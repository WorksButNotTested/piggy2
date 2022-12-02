#include "process.h"
#include "util.h"

guint
get_process (FridaDevice *local_device, gchar *target)
{
  guint              ret           = 0;
  guint              target_pid    = 0;
  FridaSpawnOptions *spawn_options = NULL;
  GError            *error         = NULL;

  do
  {
    target_pid = atoi (target);
    if (target_pid != 0)
    {
      ret = target_pid;
      break;
    }

    spawn_options = frida_spawn_options_new ();

    g_print (PIGGY_PREFIX_GREEN "spawning: %s\n", target);
    target_pid = frida_device_spawn_sync (
        local_device, target, spawn_options, NULL, &error);
    if (error != NULL)
    {
      g_printerr ("Failed to spawn %s: %s\n", target, error->message);
      break;
    }

    ret = target_pid;

  } while (FALSE);

  if (spawn_options != NULL)
    g_object_unref (spawn_options);

  if (error != NULL)
    g_error_free (error);

  return ret;
}
