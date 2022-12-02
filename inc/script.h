#ifndef _SCRIPT_H
#define _SCRIPT_H

#include "frida-core.h"

FridaScript *
load_script (FridaSession *session, gchar *script_file);

#endif
