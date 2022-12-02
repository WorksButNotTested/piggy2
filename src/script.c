#include "detach.h"
#include "message.h"
#include "script.h"
#include "util.h"

extern unsigned char piggy_js[];
extern unsigned int  piggy_js_len;

FridaScript *
load_script (FridaSession *session, gchar *script_file)
{
  FridaScript        *ret             = NULL;
  gchar              *script_path     = NULL;
  gchar              *script_contents = NULL;
  FridaScript        *script          = NULL;
  FridaScriptOptions *options         = NULL;
  GError             *error           = NULL;

  do
  {
    g_signal_connect (session, "detached", G_CALLBACK (on_detached), NULL);
    if (frida_session_is_detached (session))
    {
      g_printerr ("Session is detached\n");
      break;
    }

    g_print (PIGGY_PREFIX_GREEN "Attached\n");

    options = frida_script_options_new ();

    if (script_file == NULL)
    {
      g_printerr ("No script file specified\n");
      break;
    }

    frida_script_options_set_name (options, script_file);
    frida_script_options_set_runtime (options, FRIDA_SCRIPT_RUNTIME_QJS);

    g_print (PIGGY_PREFIX_GREEN "Created options (external)\n");

    script_path = g_canonicalize_filename (script_file, g_get_current_dir ());

    g_print (PIGGY_PREFIX_GREEN "Script path: %s\n", script_path);

    if (!g_file_get_contents (script_path, &script_contents, NULL, &error))
    {
      g_printerr ("Failed to read script: %s\n", error->message);
      break;
    }

    g_print (PIGGY_PREFIX_GREEN "Read script\n");

    script = frida_session_create_script_sync (
        session, script_contents, options, NULL, &error);
    if (script == NULL)
    {
      g_printerr ("Failed to create script: %s\n", error->message);
      break;
    }

    g_print (PIGGY_PREFIX_GREEN "Created script\n");

    g_signal_connect (script, "message", G_CALLBACK (on_message), NULL);

    g_print (PIGGY_PREFIX_GREEN "Connected message callback\n");

    frida_script_load_sync (script, NULL, &error);
    if (error != NULL)
    {
      g_printerr ("Failed to load script: %s\n", error->message);
      break;
    }

    g_print (PIGGY_PREFIX_GREEN "Loaded script\n");

    ret = script;
  } while (FALSE);

  if (ret != script)
    frida_unref (script);

  if (script_contents != NULL)
    g_free (script_contents);

  if (script_path != NULL)
    g_free (script_path);

  if (options != NULL)
    g_clear_object (&options);

  if (error != NULL)
    g_error_free (error);

  return ret;
}
