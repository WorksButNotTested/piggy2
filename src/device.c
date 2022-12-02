#include "device.h"
#include "util.h"

FridaDevice *
get_device (FridaDeviceManager *manager)
{
  FridaDeviceList *devices      = NULL;
  GError          *error        = NULL;
  FridaDevice     *local_device = NULL;
  gint             num_devices  = 0;
  gint             i            = 0;

  do
  {
    devices =
        frida_device_manager_enumerate_devices_sync (manager, NULL, &error);
    if (error != NULL)
    {
      g_printerr ("Failed to enumerate devices: %s\n", error->message);
      break;
    }

    num_devices = frida_device_list_size (devices);
    for (i = 0; i != num_devices; i++)
    {
      FridaDevice *device = frida_device_list_get (devices, i);

      g_print (PIGGY_PREFIX_GREEN "Found device: \"%s\"\n",
               frida_device_get_name (device));

      if (frida_device_get_dtype (device) == FRIDA_DEVICE_TYPE_LOCAL)
      {
        local_device = g_object_ref (device);
        break;
      }

      g_object_unref (device);
    }
  } while (FALSE);

  frida_unref (devices);

  if (error != NULL)
    g_error_free (error);

  return local_device;
}
