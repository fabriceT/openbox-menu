#include <math.h>
#include <stdio.h>

#include <glib.h>

#include "tests.h"


int main (int argc, char **argv)
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/Utils/safename_returns_foobar", test_safename1);
  g_test_add_func ("/Utils/safename_returns_html_entities", test_safename2);
  //g_test_add_func ("/Utils/safename_returns_null", test_safename3);

  g_test_set_nonfatal_assertions ();

  return g_test_run();
}
