#include "message.h"

#include <stdio.h>

#include "io.h"
#include "util.h"

void
on_message (FridaScript *script,
            const gchar *message,
            GBytes      *data,
            gpointer     user_data)
{
  JsonParser  *parser;
  JsonObject  *root;
  const gchar *type;

  parser = json_parser_new ();
  json_parser_load_from_data (parser, message, -1, NULL);
  root = json_node_get_object (json_parser_get_root (parser));

  type = json_object_get_string_member (root, "type");
  if (strcmp (type, "log") == 0)
  {
    const gchar *log_message;

    log_message = json_object_get_string_member (root, "payload");
    g_print (PIGGY_PREFIX_GREEN "%s\n", log_message);
  }
  else if (strcmp (type, "send") == 0)
  {
    JsonArray *payload;
    // g_print (PIGGY_PREFIX_GREEN "on_send: %s\n", message);
    payload = json_object_get_array_member (root, "payload");

    const char *kind = json_array_get_string_element (payload, 0);
    // g_print (PIGGY_PREFIX_GREEN "\tkind: %s\n", kind);
    // const char *uuid = json_array_get_string_element (payload, 1);
    // g_print (PIGGY_PREFIX_GREEN "\tuuid: %s\n", uuid);
    const char *status = json_array_get_string_element (payload, 2);
    // g_print (PIGGY_PREFIX_GREEN "\tstatus: %s\n", status);
    // const char *value = json_array_get_string_element (payload, 3);
    // g_print (PIGGY_PREFIX_GREEN "\tvalue: %s\n", value);

    if (!g_strcmp0 (kind, "frida:rpc"))
    {
      if (!g_strcmp0 (status, "exit"))
      {
        io_reset ();
        exit (0);
      }
      if (!g_strcmp0 (status, "ok") &&
          !json_array_get_null_element (payload, 3))
      {
        const char *value = json_array_get_string_element (payload, 3);
        g_print ("%s", value);
      }
    }
  }
  else
  {
    g_print (PIGGY_PREFIX_GREEN "on_message: %s\n", message);
  }

  g_object_unref (parser);
}
