#include "stop.h"
#include "piggy.h"

gboolean
stop (gpointer user_data)
{
  g_main_loop_quit (loop);

  return FALSE;
}
