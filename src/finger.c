/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*	ROM 2.4 is copyright 1993-1995 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@pacinfo.com)				   *
*	    Gabrielle Taylor (gtaylor@pacinfo.com)			   *
*	    Brian Moore (rom@rom.efn.org)				   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

/*************************************************************************** 
*       ROT 1.4 is copyright 1996-1997 by Russ Walsh                       * 
*       By using this code, you have agreed to follow the terms of the     * 
*       ROT license, in the file doc/rot.license                           * 
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"

#define MAX_NEST	100
static	OBJ_DATA *	rgObjNest	[MAX_NEST];

extern  void	fread_char	args( ( CHAR_DATA *ch,  FILE *fp ) );
bool    check_valid_name        args( ( char *name ) );

void do_finger( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    FILE *fp;
    bool fOld;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Finger whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) != NULL)
    {
	if (!IS_NPC(victim))
	{
	    act( "$N is on right now!", ch, NULL, victim, TO_CHAR );
	    return;
	}
    }

    if(!check_valid_name(arg) && !IS_IMMORTAL(ch))
    {
        send_to_char("That's not a valid name.\n\r",ch);
    }

    victim = new_char();
    victim->pcdata = new_pcdata();
    fOld = FALSE;

    fclose( fpReserve );
    
    sprintf( buf, "%s%s", PLAYER_DIR, capitalize( arg ) );
    if ( ( fp = fopen( buf, "r" ) ) != NULL )
    {
	int iNest;

	for ( iNest = 0; iNest < MAX_NEST; iNest++ )
	    rgObjNest[iNest] = NULL;

	fOld = TRUE;
	for ( ; ; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "do_finger: Load_char_obj: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if      ( !str_cmp( word, "PLAYER" ) ) fread_char( victim, fp );
	    else if ( !str_cmp( word, "OBJECT" ) ) break;
	    else if ( !str_cmp( word, "O"      ) ) break;
	    else if ( !str_cmp( word, "PET"    ) ) break;
	    else if ( !str_cmp( word, "BANK-ACCOUNT"    ) ) break;
	    else if ( !str_cmp( word, "END"    ) ) break;
	    else
	    {
		bug( "Load_char_obj: bad section.", 0 );
		break;
	    }
	}
	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );

    if ( !fOld )
    {
	send_to_char("No player by that name exists.\n\r",ch);
	free_pcdata(victim->pcdata);
	free_char(victim);
	return;
    }
    if ( (victim->level > LEVEL_HERO) && (victim->level > ch->level) )
    {
	send_to_char("The gods wouldn't like that.\n\r",ch);
	free_pcdata(victim->pcdata);
	free_char(victim);
	return;
    }
    sprintf(buf,"%s last logged off on %s\n\r",
	victim->name, (char *) ctime(&victim->llogoff));
    send_to_char(buf,ch);

    if(get_trust(ch) == MAX_LEVEL)
    {
        sprintf(buf, "Last IP: %s\n\r", victim->pcdata->last_ip);
        send_to_char(buf,ch);
    }

    free_pcdata(victim->pcdata);
    free_char(victim);
    return;
}

