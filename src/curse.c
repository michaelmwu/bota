/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,	   *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *									   *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael	   *
 *  Chastain, Michael Quan, and Mitchell Tse.				   *
 *									   *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc	   *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.						   *
 *									   *
 *  Much time and thought has gone into this software and you are	   *
 *  benefitting.  We hope that you share your changes too.  What goes	   *
 *  around, comes around.						   *
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
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "magic.h"

void do_curse(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *mob;
    char arg[MAX_INPUT_LENGTH];
    int cost,sn;
    SPELL_FUN *spell;
    char *words;	

    /* check for priest */
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
        if ( IS_NPC(mob) && IS_SET(mob->act, ACT_IS_PRIEST)
	   && !IS_EVIL(mob) )
            break;
    }
 
    if ( mob == NULL )
    {
        send_to_char( "You can't do that here.\n\r", ch );
        return;
    }

    one_argument(argument,arg);

    if (arg[0] == '\0')
    {
        /* display price list */
	act("$N says '{aI offer the following services:{x'",ch,NULL,mob,TO_CHAR);
	send_to_char("  align: Belan's favor          20 gold\n\r",ch);
	send_to_char("  sanctuary: Belan's protection 35 gold\n\r",ch);
	send_to_char("  voodoo: Remove voodoo curses   1 platinum\n\r",ch);
	send_to_char(" Type curse <type> to be cursed.\n\r",ch);
	return;
    }

    if (!str_prefix(arg,"sanctuary"))
    {
	spell = spell_sanctuary;
	sn    = skill_lookup("sanctuary");
	words = "judicandus unam";
	cost  = 3500;
    }

    else if (!str_prefix(arg,"align"))
    {
	spell = NULL;
	sn    = -1;
	words = "judicandus belan";
	cost  = 2000;
    }

    else if (!str_prefix(arg,"voodoo"))
    {
	spell = NULL;
	sn    = -1;
	words = "judicandus mojo";
	cost  = 10000;
    }

    else 
    {
	act("$N says '{aType 'curse' for a list of spells.{x'",
	    ch,NULL,mob,TO_CHAR);
	return;
    }

    if (cost > (ch->gold * 100 + ch->silver))
    {
	act("$N says '{aYou do not have enough gold for my services.{x'",
	    ch,NULL,mob,TO_CHAR);
	return;
    }

    if (spell != NULL && ch->alignment >= 0)
    {
	act("$N says '{aBelan does not protect the pure of heart!{x'",
	    ch,NULL,mob,TO_CHAR);
	return;
    }

    WAIT_STATE(ch,PULSE_VIOLENCE);

    if (!str_prefix(arg,"voodoo"))
    {
	if (remove_voodoo(ch))
	{
	    deduct_cost(ch,cost,VALUE_SILVER);
	    act("$n utters the words '{a$T{x'.",mob,NULL,words,TO_ROOM);
	    act("$n tells you '{aThe voodoo curses on you will soon be destroyed.{x'",mob,NULL,ch,TO_VICT);
	    return;
	}
	deduct_cost(ch,cost/5,VALUE_SILVER);
	act("$n utters the words '{a$T{x'.",mob,NULL,words,TO_ROOM);
	act("$n tells you '{aI couldn't find any voodoo dolls with your name.{x'",mob,NULL,ch,TO_VICT);
	return;
    }

    deduct_cost(ch,cost,VALUE_SILVER);
    act("$n utters the words '{a$T{x'.",mob,NULL,words,TO_ROOM);
  
    if (spell == NULL)  /* Decrease alignment toward evil */
    {
	ch->alignment -= 200;
	ch->alignment = UMAX(ch->alignment, -1000);
	if ( ch->pet != NULL )
	    ch->pet->alignment = ch->alignment;
	send_to_char("You feel Thoth's anger toward your actions!\n\r",ch);
	return;
     }

     if (sn == -1)
	return;
    
     spell(sn,mob->level,mob,ch,TARGET_CHAR);
}
