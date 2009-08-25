/* t-stringhelp.c - Regression tests for stringhelp.c
 * Copyright (C) 2007 Free Software Foundation, Inc.
 *
 * This file is part of JNLIB.
 *
 * JNLIB is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * JNLIB is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "stringhelp.h"

#include "t-support.h"



static void
test_percent_escape (void)
{
  char *result;
  static struct {
    const char *extra; 
    const char *value; 
    const char *expected;
  } tests[] = 
    {
      { NULL, "", "" },
      { NULL, "%", "%25" },
      { NULL, "%%", "%25%25" },
      { NULL, " %", " %25" },
      { NULL, ":", "%3a" },
      { NULL, " :", " %3a" },
      { NULL, ": ", "%3a " },
      { NULL, " : ", " %3a " },
      { NULL, "::", "%3a%3a" },
      { NULL, ": :", "%3a %3a" },
      { NULL, "%:", "%25%3a" },
      { NULL, ":%", "%3a%25" },
      { "\\\n:", ":%", "%3a%25" },
      { "\\\n:", "\\:%", "%5c%3a%25" },
      { "\\\n:", "\n:%", "%0a%3a%25" },
      { "\\\n:", "\xff:%", "\xff%3a%25" },
      { "\\\n:", "\xfe:%", "\xfe%3a%25" },
      { "\\\n:", "\x01:%", "\x01%3a%25" },
      { "\x01",  "\x01:%", "%01%3a%25" },
      { "\xfe",  "\xfe:%", "%fe%3a%25" },
      { "\xfe",  "\xff:%", "\xff%3a%25" },

      { NULL, NULL, NULL }
    };
  int testno;

  result = percent_escape (NULL, NULL);
  if (result)
    fail (0);
  for (testno=0; tests[testno].value; testno++)
    {
      result = percent_escape (tests[testno].value, tests[testno].extra);
      if (!result)
        fail (testno);
      if (strcmp (result, tests[testno].expected))
        fail (testno);
      xfree (result);
    }

}


static void
test_compare_filenames (void)
{
  struct {
    const char *a;
    const char *b;
    int result;
  } tests[] = {
    { "", "", 0 },
    { "", "a", -1 },
    { "a", "", 1 },
    { "a", "a", 0 },
    { "a", "aa", -1 },
    { "aa", "a", 1 },
    { "a",  "b", -1  },

#ifdef HAVE_W32_SYSTEM
    { "a", "A", 0 },
    { "A", "a", 0 },
    { "foo/bar", "foo\\bar", 0 },
    { "foo\\bar", "foo/bar", 0 },
    { "foo\\", "foo/", 0 },
    { "foo/", "foo\\", 0 },
#endif /*HAVE_W32_SYSTEM*/
    { NULL, NULL, 0}
  };
  int testno, result;

  for (testno=0; tests[testno].a; testno++)
    {
      result = compare_filenames (tests[testno].a, tests[testno].b);
      result = result < 0? -1 : result > 0? 1 : 0;
      if (result != tests[testno].result)
        fail (testno);
    }
}


static void
test_strconcat (void)
{
  char *out;

  out = strconcat ("1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                   "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                   "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                   "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                   "1", "2", "3", "4", "5", "6", "7", NULL);
  if (!out)
    fail (0);
  else
    xfree (out);
  out = strconcat ("1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                   "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                   "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                   "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                   "1", "2", "3", "4", "5", "6", "7", "8", NULL);
  if (out)
    fail (0);
  else if (errno != EINVAL)
    fail (0);

  out = strconcat ("1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                   "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                   "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                   "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                   "1", "2", "3", "4", "5", "6", "7", "8", "9", NULL);
  if (out)
    fail (0);
  else if (errno != EINVAL)
    fail (0);
  
#if __GNUC__ < 4 /* gcc 4.0 has a sentinel attribute.  */
  out = strconcat (NULL);
  if (!out || *out)
    fail (1);
#endif
  out = strconcat (NULL, NULL);
  if (!out || *out)
    fail (1);
  out = strconcat ("", NULL);
  if (!out || *out)
    fail (1);
  xfree (out);

  out = strconcat ("", "", NULL);
  if (!out || *out)
    fail (2);
  xfree (out);

  out = strconcat ("a", "b", NULL);
  if (!out || strcmp (out, "ab"))
    fail (3);
  xfree (out);
  out = strconcat ("a", "b", "c", NULL);
  if (!out || strcmp (out, "abc"))
    fail (3);
  xfree (out);

  out = strconcat ("a", "b", "cc", NULL);
  if (!out || strcmp (out, "abcc"))
    fail (4);
  xfree (out);
  out = strconcat ("a1", "b1", "c1", NULL);
  if (!out || strcmp (out, "a1b1c1"))
    fail (4);
  xfree (out);

  out = strconcat ("", " long b ", "", "--even-longer--", NULL);
  if (!out || strcmp (out, " long b --even-longer--"))
    fail (5);
  xfree (out);

  out = strconcat ("", " long b ", "", "--even-longer--", NULL);
  if (!out || strcmp (out, " long b --even-longer--"))
    fail (5);
  xfree (out);
}

static void
test_xstrconcat (void)
{
  char *out;

  out = xstrconcat ("1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                   "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                   "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                   "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                   "1", "2", "3", "4", "5", "6", "7", NULL);
  if (!out)
    fail (0);

#if __GNUC__ < 4 /* gcc 4.0 has a sentinel attribute.  */
  out = xstrconcat (NULL);
  if (!out)
    fail (1);
#endif
  out = xstrconcat (NULL, NULL);
  if (!out)
    fail (1);
  out = xstrconcat ("", NULL);
  if (!out || *out)
    fail (1);
  xfree (out);

  out = xstrconcat ("", "", NULL);
  if (!out || *out)
    fail (2);
  xfree (out);

  out = xstrconcat ("a", "b", NULL);
  if (!out || strcmp (out, "ab"))
    fail (3);
  xfree (out);
  out = xstrconcat ("a", "b", "c", NULL);
  if (!out || strcmp (out, "abc"))
    fail (3);
  xfree (out);

  out = xstrconcat ("a", "b", "cc", NULL);
  if (!out || strcmp (out, "abcc"))
    fail (4);
  xfree (out);
  out = xstrconcat ("a1", "b1", "c1", NULL);
  if (!out || strcmp (out, "a1b1c1"))
    fail (4);
  xfree (out);

  out = xstrconcat ("", " long b ", "", "--even-longer--", NULL);
  if (!out || strcmp (out, " long b --even-longer--"))
    fail (5);
  xfree (out);

  out = xstrconcat ("", " long b ", "", "--even-longer--", NULL);
  if (!out || strcmp (out, " long b --even-longer--"))
    fail (5);
  xfree (out);
}


int
main (int argc, char **argv)
{
  (void)argc;
  (void)argv;

  test_percent_escape ();
  test_compare_filenames ();
  test_strconcat ();
  test_xstrconcat ();

  return 0;
}

