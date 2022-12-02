#include "signal.h"
#include "stop.h"

void
on_signal (int signo)
{
  g_idle_add (stop, NULL);
}
