#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "merc.h"
#include "tables.h"

/*
**  long2roman() - Convert a long integer into roman numerals
**
**  Public domain, by Jim Walsh, Dann Corbit, Bob Stout, and others
**
**  Arguments: 1 - Value to convert
**             2 - Buffer to receive the converted roman numeral string
**             3 - Length of the string buffer
**
**  Returns: Pointer to the buffer, else NULL if error or buffer overflow
*/

static struct
{
      long  PostValue;
      char *PostLetter;
      long  PreValue;
      char *PreLetter;
} RomanConvert[] = {
    {1000L, "M", 900L, "CM"},
    {500L, "D", 400L, "CD"},
    {100L, "C", 90L, "XC"},
    {50L, "L", 40L, "XL"},
    {10L, "X", 9L, "IX"},
    {5L, "V", 4L, "IV"},
    {1L, "I", 1L, "I"}
};

char *long2roman(long val)
{
      static char buf[MAX_STRING_LENGTH];
      size_t posn = 0;
      size_t place = 0;

      if (val > 3999L)
            return NULL;
      do
      {
            while (val >= RomanConvert[place].PostValue)
            {
                  posn += sprintf(&buf[posn], "%s",
                                  RomanConvert[place].PostLetter);
                  val -= RomanConvert[place].PostValue;
            }
            if (val >= RomanConvert[place].PreValue)
            {
                  posn += sprintf(&buf[posn], "%s",
                                  RomanConvert[place].PreLetter);
                  val -= RomanConvert[place].PreValue;
            }
            place++;
      } while (val > 0);

      return buf;
}

/***************************************************************
 *
 * Fuzzy string searching subroutines
 *
 * Author:    John Rex
 * Date:      August, 1988
 * References: (1) Computer Algorithms, by Sara Baase
 *                 Addison-Wesley, 1988, pp 242-4.
 *             (2) Hall PAV, Dowling GR: "Approximate string matching",
 *                 ACM Computing Surveys, 12:381-402, 1980.
 *
 * Freely usable according to Snippets.org
 *
 **************************************************************/

/* local, static data */

static char *Text, *Pattern; /* pointers to search strings       */
static int Textloc;          /* current search position in Text  */
static int Plen;             /* length of Pattern                */
static int Degree;           /* max degree of allowed mismatch   */
static int *Ldiff, *Rdiff;   /* dynamic difference arrays        */
static int *Loff,  *Roff;    /* used to calculate start of match */

void App_init(char *pattern, char *text, int degree);
void App_next(char **start, char **end, int *howclose);

void App_init(char *pattern, char *text, int degree)
{
      int i;

      /* save parameters */

      Text = text;
      Pattern = pattern;
      Degree = degree;

      /* initialize */

      Plen = strlen(pattern);
      Ldiff  = (int *) malloc(sizeof(int) * (Plen + 1) * 4);
      Rdiff  = Ldiff + Plen + 1;
      Loff   = Rdiff + Plen + 1;
      Roff   = Loff +  Plen + 1;
      for (i = 0; i <= Plen; i++)
      {
            Rdiff[i] = i;   /* initial values for right-hand column */
            Roff[i]  = 1;
      }

      Textloc = -1; /* current offset into Text */
}

void App_next(char **start, char **end, int *howclose)
{
      int *temp, a, b, c, i;

      *start = NULL;
      while (*start == NULL)  /* start computing columns */
      {
            if (Text[++Textloc] == '\0') /* out of text to search! */
                  break;

            temp = Rdiff;   /* move right-hand column to left ... */
            Rdiff = Ldiff;  /* ... so that we can compute new ... */
            Ldiff = temp;   /* ... right-hand column */
            Rdiff[0] = 0;   /* top (boundary) row */

            temp = Roff;    /* and swap offset arrays, too */
            Roff = Loff;
            Loff = temp;
            Roff[1] = 0;

            for (i = 0; i < Plen; i++)    /* run through pattern */
            {
                  /* compute a, b, & c as the three adjacent cells ... */

                  if (Pattern[i] == Text[Textloc])
                        a = Ldiff[i];
                  else  a = Ldiff[i] + 1;
                  b = Ldiff[i+1] + 1;
                  c = Rdiff[i] + 1;

                  /* ... now pick minimum ... */

                  if (b < a)
                        a = b;
                  if (c < a)
                        a = c;

                  /* ... and store */

                  Rdiff[i+1] = a;
            }

            /* now update offset array */
            /* the values in the offset arrays are added to the
               current location to determine the beginning of the
               mismatched substring. (see text for details) */

            if (Plen > 1) for (i=2; i<=Plen; i++)
            {
                  if (Ldiff[i-1] < Rdiff[i])
                        Roff[i] = Loff[i-1] - 1;
                  else if (Rdiff[i-1] < Rdiff[i])
                        Roff[i] = Roff[i-1];
                  else if (Ldiff[i] < Rdiff[i])
                        Roff[i] = Loff[i] - 1;
                  else /* Ldiff[i-1] == Rdiff[i] */
                        Roff[i] = Loff[i-1] - 1;
            }

            /* now, do we have an approximate match? */

            if (Rdiff[Plen] <= Degree)    /* indeed so! */
            {
                  *end = Text + Textloc;
                  *start = *end + Roff[Plen];
                  *howclose = Rdiff[Plen];
            }
      }

      if (start == NULL) /* all done - free dynamic arrays */
            free(Ldiff);
}

void do_oops(CHAR_DATA *ch, char *argument)
{
}
