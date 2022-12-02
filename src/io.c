#include "io.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <termios.h>

#define INPUT_SIZE 1024

struct termios termios_p   = {0};
struct termios termios_raw = {0};

static void
io_post (FridaScript *script, const char *input, size_t len)
{
  JsonBuilder *builder = NULL;
  JsonBuilder *outer   = NULL;
  gchar       *uuid    = NULL;
  JsonBuilder *inner   = NULL;
  JsonNode    *root    = NULL;
  char        *message = NULL;

  builder = json_builder_new ();

  outer = json_builder_begin_array (builder);
  json_builder_add_string_value (outer, "frida:rpc");
  uuid = g_uuid_string_random ();
  json_builder_add_string_value (outer, uuid);
  json_builder_add_string_value (outer, "call");
  json_builder_add_string_value (outer, "onFridaStdin");
  inner = json_builder_begin_array (outer);
  json_builder_add_string_value (inner, input);
  json_builder_end_array (inner);
  json_builder_end_array (outer);
  root    = json_builder_get_root (builder);
  message = json_to_string (root, FALSE);

  frida_script_post (script, message, NULL);

  g_free (uuid);
  g_free (message);
  json_node_unref (root);
  g_object_unref (builder);
}

static gboolean
io_input (GIOChannel *source, GIOCondition condition, gpointer data)
{
  static char input[INPUT_SIZE + 1] = {0};

  FridaScript *script = (FridaScript *) data;

  size_t len = read (STDIN_FILENO, input, sizeof (input));
  if (len < 0)
  {
    g_printerr ("Failed to read: %u\n", errno);
  }
  else if (len != 0)
  {
    input[len] = 0;
    io_post (script, input, len);
  }

  return TRUE;
}

void
io_reset (gpointer user_data)
{
  if (tcsetattr (STDOUT_FILENO, TCSANOW, &termios_p) < 0)
  {
    g_printerr ("io_reset - Failed to tcsetattr: %u\n", errno);
    return;
  }
}

GIOChannel *
io_init (FridaScript *script)
{
  if (tcgetattr (STDIN_FILENO, &termios_p) < 0)
  {
    g_printerr ("Failed to tcgetattr: %u\n", errno);
    return NULL;
  }

  termios_raw = termios_p;
  cfmakeraw (&termios_raw);
  termios_raw.c_lflag |= ISIG;
  termios_raw.c_oflag |= OPOST;

  if (tcsetattr (STDIN_FILENO, TCSANOW, &termios_raw) < 0)
  {
    g_printerr ("Failed to tcsetattr: %u\n", errno);
    return NULL;
  }

  GIOChannel *channel = g_io_channel_unix_new (STDIN_FILENO);
  g_io_add_watch (channel, G_IO_IN, io_input, script);

  return channel;
}

void
io_fini (GIOChannel *channel)
{
  GError *error = NULL;
  g_io_channel_shutdown (channel, FALSE, &error);

  if (error != NULL)
  {
    g_printerr ("Failed to close io: %s\n", error->message);
    g_error_free (error);
  }

  if (tcsetattr (STDOUT_FILENO, TCSANOW, &termios_p) < 0)
  {
    g_printerr ("io_fini - Failed to tcsetattr: %u\n", errno);
  }
}
