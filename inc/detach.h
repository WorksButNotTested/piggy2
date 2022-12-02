#ifndef _DETACH_H
#define _DETACH_H

#include "frida-core.h"

void
on_detached (FridaSession            *session,
             FridaSessionDetachReason reason,
             FridaCrash              *crash,
             gpointer                 user_data);

#endif
