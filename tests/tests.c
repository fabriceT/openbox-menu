#include <math.h>
#include <stdio.h>

#include <glib.h>

#include "tests.h"


int main (int argc, char **argv)
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/Utils/safename_returns_foobar", test_safename1);
  g_test_add_func ("/Utils/safename_returns_html_entities", test_safename2);
  g_test_add_func ("/Utils/safename_returns_null", test_safename3);

  g_test_add_func ("/Utils/application_menu_with_prefix", test_application_menu_with_prefix);
  g_test_add_func ("/Utils/application_menu_without_prefix", test_application_menu_without_prefix);

  g_test_add_func ("/Context/set_terminal_cmd", test_set_terminal_cmd);
  g_test_add_func ("/Context/set_desktop_flag", test_set_desktop_flag);
  g_test_add_func ("/Context/add_desktop_flag", test_add_desktop_flag);

  g_test_add_func ("/Context/set_persistent_true", test_persistent_true);
  g_test_add_func ("/Context/set_persistent_false", test_persistent_false);



  g_test_set_nonfatal_assertions ();

  return g_test_run();
}
