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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"

/* local procedures */
void parse_sign(CHAR_DATA *ch, char *argument, int type);

void do_sign(CHAR_DATA *ch,char *argument)
{
    parse_sign(ch,argument,NOTE_SIGN);
}

void sign_attach( CHAR_DATA *ch, int type )
{
    NOTE_DATA *pnote;

    if ( ch->pnote != NULL )
	return;

    pnote = new_note();

    pnote->next		= NULL;
    pnote->sender	= str_dup( ch->name );
    pnote->date		= str_dup( "" );
    pnote->to_list	= str_dup( "" );
    pnote->subject	= str_dup( "" );
    pnote->text		= str_dup( "" );
    pnote->type		= type;
    ch->pnote		= pnote;
    return;
}

void parse_sign( CHAR_DATA *ch, char *argument, int type )
{
    BUFFER *buffer;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;

    if ( IS_NPC(ch) )
	return;

    argument = one_argument( argument, arg );
    smash_tilde( argument );

    if ( arg[0] == '\0' )
    {
        return;
    }

    if ( !str_cmp( arg, "+" ) )
    {
	sign_attach( ch,type );
	if (ch->pnote->type != type)
	{
	    send_to_char(
		"You already have a different note in progress.\n\r",ch);
	    return;
	}

	if (strlen(ch->pnote->text)+strlen(argument) >= 4096)
	{
	    send_to_char( "Sign too long.\n\r", ch );
	    return;
	}

 	buffer = new_buf();

	add_buf(buffer,ch->pnote->text);
	add_buf(buffer,argument);
	add_buf(buffer,"\n\r");
	free_string( ch->pnote->text );
	ch->pnote->text = str_dup( buf_string(buffer) );
	free_buf(buffer);
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if (!str_cmp(arg,"-"))
    {
 	int len;
	bool found = FALSE;

	sign_attach(ch,type);
        if (ch->pnote->type != type)
        {
            send_to_char(
                "You already have a different note in progress.\n\r",ch);
            return;
        }

	if (ch->pnote->text == NULL || ch->pnote->text[0] == '\0')
	{
	    send_to_char("No lines left to remove.\n\r",ch);
	    return;
	}

	strcpy(buf,ch->pnote->text);

	for (len = strlen(buf); len > 0; len--)
 	{
	    if (buf[len] == '\r')
	    {
		if (!found)  /* back it up */
		{
		    if (len > 0)
			len--;
		    found = TRUE;
		}
		else /* found the second one */
		{
		    buf[len + 1] = '\0';
		    free_string(ch->pnote->text);
		    ch->pnote->text = str_dup(buf);
		    return;
		}
	    }
	}
	buf[0] = '\0';
	free_string(ch->pnote->text);
	ch->pnote->text = str_dup(buf);
	return;
    }

    if ( !str_prefix( arg, "make" ) )
    {
	sign_attach( ch,type );
        if (ch->pnote->type != type)
        {
            send_to_char(
                "You already have a different note in progress.\n\r",ch);
            return;
        }
	free_string( ch->pnote->to_list );
	free_string( ch->pnote->subject );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg, "clear" ) )
    {
	if ( ch->pnote != NULL )
	{
	    free_note(ch->pnote);
	    ch->pnote = NULL;
	}

	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg, "show" ) )
    {
	if ( ch->pnote == NULL )
	{
	    send_to_char( "You have no sign in progress.\n\r", ch );
	    return;
	}

	if (ch->pnote->type != type)
	{
	    send_to_char("You aren't working on that kind of note.\n\r",ch);
	    return;
	}

	send_to_char( ch->pnote->text, ch );
	return;
    }

    if ( !str_prefix( arg, "post" ) || !str_prefix(arg, "send")
    || !str_prefix(arg, "save"))
    {
	EXTRA_DESCR_DATA *ed;

	if ( ch->pnote == NULL )
	{
	    send_to_char( "You have no sign in progress.\n\r", ch );
	    return;
	}

        if (ch->pnote->type != type)
        {
            send_to_char("You aren't working on that kind of note.\n\r",ch);
            return;
        }

	pObjIndex = get_obj_index(OBJ_VNUM_QUEST_SIGN);
	obj = create_object( pObjIndex, ch->level );
	obj_to_room( obj, ch->in_room );

	ed = new_extra_descr();

	ed->keyword = str_dup( "sign" );

	buffer = new_buf();
        add_buf(buffer,ch->pnote->text);
	ed->description = str_dup(buf_string(buffer));

	ed->next = NULL;
	obj->extra_descr = ed;
	ch->pnote = NULL;

        send_to_char( "A sign now floats before you.\n\r", ch );
	return;
    }

    send_to_char( "You can't do that.\n\r", ch );
    return;
}

