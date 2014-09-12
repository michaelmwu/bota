/*
 * There are a few things you need to do to get this working.
 * Firstly I suggest putting the prototype for the cprintf()'s in
 * your main headerfile. Add something like this.
 *
 *  int cprintf   ( char *buf, char *ptr, ... ) __attribute__ ((format(printf, 2, 3)));
 *  int cnprintf  ( char *buf, int maxlen, char *ptr, ... ) __attribute__ ((format(printf, 3, 4)));
 *
 * Secondly, you may want to add cprintf.c to your Makefile.
 *
 * And finally you may want to edit the ansi tag strings found
 * below, so they fit the ansi tags you allow on your mud.
 *
 * cprintf.c by Brian Graversen
 * ----------------------------
 * web:  http://www.daimi.au.dk/~jobo/dystopia/
 * mail: jobo@daimi.au.dk
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

/*
 * should also include your main headerfile,
 * I assume it's merc.h. We need this for stuff
 * like TRUE, FALSE, MAX_STRING_LENGTH, etc.
 */
#include "merc.h"

#define  _NO_STRING_LIMIT_      -1

/*
 * Just edit these three defines to make sure cprintf()
 * works with your ansi color code. The COLOR_TAG is
 * the pretag that comes before the ansitag, on most muds
 * this is '{' or '#'. Secondly, add all the valid color
 * tags to the ANSI_STRING, if #R is an ansi tag on your
 * mud, then you must add R to ANSI_STRING, just do this
 * for all the valid uses. Now, do you have certain non-ansi
 * tags that are converted into something else? For instance
 * some muds like to use '#-' as a replacement for '~', others,
 * may use '##' as a replacement for '#'. These special conversion
 * tags should be added to REPLACE_STRING.
 *
 * The following settings works with dystopia 1.4/1.4CE.
 */
#define  COLOR_TAG              '{'
#define  ANSI_STRING            "1234567890"
#define  REPLACE_STRING         "-{"

/*
 * local procedures
 */
char *string_restrict  ( char *str, int size );
int   collen           ( const char *str );
int  _csprintf          ( char *buf, int maxlen, char *ptr, va_list ap );

/*
 * Acts like sprintf(), but doesn't break alignment
 * due to colors. It only supports %d and %s. Returns
 * the amount of chars copied.
 */
int csprintf(char *buf, char *ptr, ...)
{
  va_list ap;

  va_start(ap, ptr);

  return _csprintf(buf, _NO_STRING_LIMIT_, ptr, ap);
}

/*
 * Just as csprintf(), but safer, since you can restrict
 * the maximum amount of copied chars. It will return
 * the amount of copied chars, unless the output was
 * truncated due to reaching maxlen before it was done
 * copying the entire string, in which case it will return -1.
 */
int cnprintf(char *buf, int maxlen, char *ptr, ...)
{
  va_list ap;

  va_start(ap, ptr);

  return _csprintf(buf, maxlen, ptr, ap);
}

int _csprintf(char *buf, int maxlen, char *ptr, va_list ap)
{
  char dirty[100];
  char *s;
  int i, copied = 0;
  bool bEnd;

  while(*ptr != '\0')
  {
    bool reverse = FALSE;
    int size = 0, max_size = 0, j = 0;
    bEnd = FALSE;

    switch(*ptr)
    {
      default:
	*buf++ = *ptr++;

	if (++copied == maxlen)
	  goto done_copied;

	break;
      case '%':

	/* should we align this in reverse ? */
	if (*(ptr + 1) == '-')
	{
	  ptr++;
	  reverse = TRUE;
	}

	/* get the size, if any */
	while (isdigit(*(ptr + 1)))
	{
	  size *= 10;
	  size += *(++ptr) - '0';
	}

	/* any size restrictions ? */
	if (*(ptr + 1) == '.')
	{
	  ptr++;
	  while (isdigit(*(ptr + 1)))
          {
	    max_size *= 10;
	    max_size += *(++ptr) - '0';
          }
	}

	switch(*(++ptr))
	{
          default:
	    *buf++ = '%';

            if (++copied == maxlen)
	      goto done_copied;

	    break;
          case 's':
	    s = va_arg(ap, char *);
	    s = string_restrict(s, max_size);

/*	    size -= collen(s);*/
	    size -= nc_strlen(s);

	    if (!reverse)
	    {
	      while(size-- > 0)
	      {
		*buf++ = ' ';

                if (++copied == maxlen)
	          goto done_copied;
	      }
	    }
	    while(*s != '\0')
	    {
	      *buf++ = *s++;

	      if (!reverse && *s == '\0')
		bEnd = TRUE;

              if (++copied == maxlen)
                goto done_copied;
	    }
	    if (reverse)
	    {
	      while(size-- > 0)
	      {
		*buf++ = ' ';

                if (size == 0)
	          bEnd = TRUE;

                if (++copied == maxlen)
	          goto done_copied;
	      }
	    }
	    ptr++;
	    break;
          case 'd':
	    i = va_arg(ap, int);

	    /* a little trick to see how long the number is */
	    sprintf(dirty, "%d", i);
	    size -= strlen(dirty);

	    if (!reverse)
	    {
	      while(size-- > 0)
	      {
		*buf++ = ' ';

                if (++copied == maxlen)
	          goto done_copied;
	      }
	    }

	    while (dirty[j] != '\0')
	    {
	      *buf++ = dirty[j++];

	      if (!reverse && dirty[j] == '\0')
		bEnd = TRUE;

              if (++copied == maxlen)
                goto done_copied;
	    }

	    if (reverse)
	    {
	      while(size-- > 0)
	      {
		*buf++ = ' ';

                if (size == 0)
	          bEnd = TRUE;

                if (++copied == maxlen)
	          goto done_copied;
	      }
	    }
	    ptr++;
	    break;
	}
	break;
    }
  }

  /*
   * this is our jumppoint, we use a goto for cleaner code,
   * some people may argue that one should never use goto's
   * while others will argue that refusing to use goto's no
   * matter what, can result in code that's horrible to read.
   */
 done_copied:
  *buf = '\0';

  /* if the output was truncated, we return -1 */
  if (*ptr != '\0' && (*(++ptr) != '\0' || !bEnd))
    copied = -1;

  /* clean up */
  va_end(ap);

  /* return how much we copied */
  return copied;
}

/*
 * This nifty little function calculates the length of a
 * string without the color tags. If you use other tags
 * than those mentioned here, then you should add them.
 */
int collen(const char *str)
{
  int len = 0;

  while (*str != '\0')
  {
    int i = 0, j = 0;
    bool found = FALSE;

    switch(*str)
    {
      default:
	len++, str++;
	break;
      case COLOR_TAG:
	str++;
	while (ANSI_STRING[i] != '\0' && !found)
	{
	  if (ANSI_STRING[i] == *str)
	  {
	    str++;
            found = TRUE;
	  }
	  i++;
	}
	while (REPLACE_STRING[j] != '\0' && !found)
	{
	  if (REPLACE_STRING[j] == *str)
	  {
	    len++, str++;
            found = TRUE;
	  }
	  j++;
	}
	if (!found)
          len++;
	break;
    }
  }

  return len;
}

/*
 * This nifty little function will return the
 * longest possible prefix of 'str' that can
 * be displayed in 'size' characters on a mud
 * client. (ie. it ignores ansi chars).
 */
char *string_restrict(char *str, int size)
{
  static char buf[MAX_STRING_LENGTH] = { '\0' };
  char *ptr = buf;
  int len = 0;
  bool done = FALSE;

  /* no size restrictions, we just return the string */
  if (size == 0)
    return str;

  while (*str != '\0' && !done)
  {
/*    int i = 0, j = 0;
    bool found = FALSE;*/

    switch(*str)
    {
      default:
	if (++len > size)
	{
	  done = TRUE;
	  break;
	}
	*ptr++ = *str++;
	break;
      case COLOR_TAG:
	str++;

        if(*str == '-' || *str == COLOR_TAG)
        {
            if (++len > size)
            {
              done = TRUE;
	      break;
            }
	    *ptr++ = COLOR_TAG;
	    *ptr++ = *str++;
        }
        else
        {
	    *ptr++ = COLOR_TAG;
	    *ptr++ = *str++;
        }

/*
	while (ANSI_STRING[i] != '\0' && !found)
	{
	  if (ANSI_STRING[i] == *str)
	  {
	    *ptr++ = COLOR_TAG;
	    *ptr++ = *str++;
            found = TRUE;
	  }
	  i++;
	}
	while (REPLACE_STRING[j] != '\0' && !found)
	{
	  if (REPLACE_STRING[j] == *str)
	  {
            if (++len > size)
            {
              done = TRUE;
	      break;
            }
	    *ptr++ = COLOR_TAG;
	    *ptr++ = *str++;
            found = TRUE;
	  }
	  j++;
	}
	if (!found)
	{
          if (++len > size)
          {
            done = TRUE;
	    break;
          }
          *ptr++ = COLOR_TAG;
	}
	break;*/
    }
  }
  *ptr = '\0';

  return buf;
}
