#ifndef _IO_H
#define _IO_H

#include "frida-core.h"

GIOChannel *
io_init (FridaScript *script);
void
io_fini (GIOChannel *channel);
void
io_reset ();
#endif
