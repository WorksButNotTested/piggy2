#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "frida-core.h"

void
on_message (FridaScript *script,
            const gchar *message,
            GBytes      *data,
            gpointer     user_data);

#endif
