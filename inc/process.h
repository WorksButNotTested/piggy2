#ifndef _PROCESS_H
#define _PROCESS_H

#include "frida-core.h"

guint
get_process (FridaDevice *local_device, gchar *target);

#endif
