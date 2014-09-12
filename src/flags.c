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
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "tables.h"

int flag_lookup args( ( const char *name, const struct flag_type *flag_table) );
int ft_bitpos_lookup   args( (int bit, const struct flag_type *flag_table) );


void do_flag(CHAR_DATA *ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH],arg3[MAX_INPUT_LENGTH];
    char word[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *location;
    long *flag, old = 0, new = 0, marked = 0, pos;
    char type;
    char buf[100];
    int sn, col;
    const struct flag_type *flag_table;

    argument = one_argument(argument,arg1);
    argument = one_argument(argument,arg2);
    argument = one_argument(argument,arg3);

    type = argument[0];

    if (type == '=' || type == '-' || type == '+')
        argument = one_argument(argument,word);

    if (arg1[0] == '\0')
    {
	send_to_char("Syntax:\n\r",ch);
	send_to_char("  flag mob  <name> <field> <flags>\n\r",ch);
	send_to_char("  flag char <name> <field> <flags>\n\r",ch);
	send_to_char("  flag obj  <name> <field> <flags>\n\r",ch);
	send_to_char("  flag room <room> <field> <flags>\n\r",ch);
	send_to_char("  mob  flags: act,aff,shd,off,imm,res,vuln,form,part\n\r",ch);
	send_to_char("  char flags: plr,comm,aff,shd,imm,res,vuln\n\r",ch);
	send_to_char("  obj  flags: extra,wear,weap\n\r",ch);
	send_to_char("  room flags: room\n\r",ch);
	send_to_char("  +: add flag, -: remove flag, = set equal to\n\r",ch);
	send_to_char("  otherwise flag toggles the flags listed.\n\r",ch);
	return;
    }

    if (arg2[0] == '\0')
    {
	if (!str_prefix(arg1,"mob"))
	{
	    send_to_char("Syntax:\n\r",ch);
	    send_to_char("  flag mob <name> <field> <flags>\n\r",ch);
	    send_to_char("  mob flags: act,aff,shd,off,imm,res,vuln,form,part\n\r",ch);
	    return;
	}
	if (!str_prefix(arg1,"char"))
	{
	    send_to_char("Syntax:\n\r",ch);
	    send_to_char("  flag char <name> <field> <flags>\n\r",ch);
	    send_to_char("  char flags: plr,comm,aff,shd,imm,res,vuln\n\r",ch);
	    return;
	}
	if (!str_prefix(arg1,"obj"))
	{
	    send_to_char("Syntax:\n\r",ch);
	    send_to_char("  flag obj <name> <field> <flags>\n\r",ch);
	    send_to_char("  obj flags: extra,wear,weap\n\r",ch);
	    return;
	}
	if (!str_prefix(arg1,"room"))
	{
	    send_to_char("Syntax:\n\r",ch);
	    send_to_char("  flag room <room> <field> <flags>\n\r",ch);
	    send_to_char("  room flags: room\n\r",ch);
	    return;
	}
	send_to_char("Syntax:\n\r",ch);
	send_to_char("  flag mob  <name> <field> <flags>\n\r",ch);
	send_to_char("  flag char <name> <field> <flags>\n\r",ch);
	send_to_char("  flag obj  <name> <field> <flags>\n\r",ch);
	send_to_char("  flag room <room> <field> <flags>\n\r",ch);
	return;
    }

    if (arg3[0] == '\0')
    {
	send_to_char("You need to specify a flag to set.\n\r",ch);
	if (!str_prefix(arg1,"mob"))
	{
	    send_to_char("  mob flags: act,aff,shd,off,imm,res,vuln,form,part\n\r",ch);
	}
	if (!str_prefix(arg1,"char"))
	{
	    send_to_char("  char flags: plr,comm,aff,shd,imm,res,vuln\n\r",ch);
	}
	if (!str_prefix(arg1,"obj"))
	{
	    send_to_char("  obj flags: extra,wear,weap\n\r",ch);
	}
	if (!str_prefix(arg1,"room"))
	{
	    send_to_char("  room flags: room\n\r",ch);
	}
	return;
    }

    if (argument[0] == '\0')
    {
	if (!str_prefix(arg1,"mob") || !str_prefix(arg1,"char"))
	{
	    victim = get_char_world(ch,arg2);
	    if (victim == NULL)
	    {
		send_to_char("You can't find them.\n\r",ch);
		return;
	    }
	    if (!str_prefix(arg3,"act"))
	    {
		if (!IS_NPC(victim))
		{
		    send_to_char("Use plr for PCs.\n\r",ch);
		    return;
		}
		flag_table = act_flags;
	    }
	    else if (!str_prefix(arg3,"plr"))
	    {
		if (IS_NPC(victim))
		{
		    send_to_char("Use act for NPCs.\n\r",ch);
		    return;
		}
		flag_table = plr_flags;
	    }
	    else if (!str_prefix(arg3,"aff"))
	    {
		flag_table = affect_flags;
	    }
	    else if (!str_prefix(arg3,"shd"))
	    {
		flag_table = shield_flags;
	    }
 	    else if (!str_prefix(arg3,"shield"))
	    {
		flag_table = shield_flags;
	    }
  	    else if (!str_prefix(arg3,"immunity"))
	    {
		flag_table = imm_flags;
	    }
	    else if (!str_prefix(arg3,"resist"))
	    {
		flag_table = imm_flags;
	    }
	    else if (!str_prefix(arg3,"vuln"))
	    {
		flag_table = imm_flags;
	    }
	    else if (!str_prefix(arg3,"form"))
	    {
		if (!IS_NPC(victim))
		{
		    send_to_char("Form can't be set on PCs.\n\r",ch);
		    return;
		}
		flag_table = form_flags;
	    }
	    else if (!str_prefix(arg3,"parts"))
	    {
		if (!IS_NPC(victim))
		{
		    send_to_char("Parts can't be set on PCs.\n\r",ch);
		    return;
		}
		flag_table = part_flags;
	    }
	    else if (!str_prefix(arg3,"comm"))
	    {
		if (IS_NPC(victim))
		{
		    send_to_char("Comm can't be set on NPCs.\n\r",ch);
		    return;
		}
		flag_table = comm_flags;
	    }
	    else 
	    {
		send_to_char("That's not an acceptable flag.\n\r",ch);
		return;
	    }
	}
	else if (!str_prefix(arg1,"obj"))
	{
	    obj = get_obj_world(ch,arg2);
	    if (obj == NULL)
	    {
		send_to_char("You can't find that.\n\r",ch);
		return;
	    }
	    if (!str_prefix(arg3,"extra"))
	    {
		flag_table = item_extra;
	    }
	    else if (!str_prefix(arg3,"wear"))
	    {
		flag_table = item_wear;
	    }
	    else if (!str_prefix(arg3,"weapon"))
	    {
		flag_table = item_weapon;
	    }
	    else 
	    {
		send_to_char("That's not an acceptable flag.\n\r",ch);
		return;
	    }
	}
	else if (!str_prefix(arg1,"room"))
	{
	    location = find_location(ch,arg2);
	    if ( location == NULL )
	    {
		send_to_char( "No such location.\n\r", ch );
		return;
	    }
	    if (!str_prefix(arg3,"room"))
	    {
		flag_table = area_room;
	    }
	    else 
	    {
		send_to_char("That's not an acceptable flag.\n\r",ch);
		return;
	    }
        }
	else
	{
	    send_to_char("Syntax:\n\r",ch);
	    send_to_char("  flag mob  <name> <field> <flags>\n\r",ch);
	    send_to_char("  flag char <name> <field> <flags>\n\r",ch);
	    send_to_char("  flag obj  <name> <field> <flags>\n\r",ch);
	    send_to_char("  flag room <room> <field> <flags>\n\r",ch);
	    send_to_char("  mob  flags: act,aff,shd,off,imm,res,vuln,form,part\n\r",ch);
	    send_to_char("  char flags: plr,comm,aff,shd,imm,res,vuln\n\r",ch);
	    send_to_char("  obj  flags: extra,wear,weap\n\r",ch);
	    send_to_char("  room flags: room\n\r",ch);
	    send_to_char("  +: add flag, -: remove flag, = set equal to\n\r",ch);
	    send_to_char("  otherwise flag toggles the flags listed.\n\r",ch);
	    return;
	}
	send_to_char("Which flags do you wish to change?\n\r",ch);
	send_to_char("  {Bchangeable  {Runchangeable{x\n\r",ch);
	col = 0;
	for (sn = 0; sn < 30; sn++)
	{
	    if (flag_table[sn].name == NULL)
		break;
	    if (flag_table[sn].settable)
		sprintf(buf,"{B%-20s ", flag_table[sn].name);
	    else
		sprintf(buf,"{R%-20s ", flag_table[sn].name);
	    send_to_char(buf,ch);
	    if (++col % 3 == 0)
		send_to_char("{x\n\r",ch);
	}
	if ( col % 3 != 0 )
	    send_to_char( "{x\n\r", ch );
	return;
    }


    if (!str_prefix(arg1,"mob") || !str_prefix(arg1,"char"))
    {

/* Mobiles and Characters */

	victim = get_char_world(ch,arg2);
	if (victim == NULL)
	{
	    send_to_char("You can't find them.\n\r",ch);
	    return;
	}

        /* select a flag to set */
	if (!str_prefix(arg3,"act"))
	{
	    if (!IS_NPC(victim))
	    {
		send_to_char("Use plr for PCs.\n\r",ch);
		return;
	    }

	    flag = &victim->act;
	    flag_table = act_flags;
	}

	else if (!str_prefix(arg3,"plr"))
	{
	    if (IS_NPC(victim))
	    {
		send_to_char("Use act for NPCs.\n\r",ch);
		return;
	    }

	    flag = &victim->act;
	    flag_table = plr_flags;
	}

 	else if (!str_prefix(arg3,"aff"))
	{
	    flag = &victim->affected_by;
	    flag_table = affect_flags;
	}

 	else if (!str_prefix(arg3,"shd"))
	{
	    flag = &victim->shielded_by;
	    flag_table = shield_flags;
	}

 	else if (!str_prefix(arg3,"shield"))
	{
	    flag = &victim->shielded_by;
	    flag_table = shield_flags;
	}

  	else if (!str_prefix(arg3,"immunity"))
	{
	    flag = &victim->imm_flags;
	    flag_table = imm_flags;
	}

	else if (!str_prefix(arg3,"resist"))
	{
	    flag = &victim->res_flags;
	    flag_table = imm_flags;
	}

	else if (!str_prefix(arg3,"vuln"))
	{
	    flag = &victim->vuln_flags;
	    flag_table = imm_flags;
	}

	else if (!str_prefix(arg3,"form"))
	{
	    if (!IS_NPC(victim))
	    {
	 	send_to_char("Form can't be set on PCs.\n\r",ch);
		return;
	    }

	    flag = &victim->form;
	    flag_table = form_flags;
	}

	else if (!str_prefix(arg3,"parts"))
	{
	    if (!IS_NPC(victim))
	    {
		send_to_char("Parts can't be set on PCs.\n\r",ch);
		return;
	    }

	    flag = &victim->parts;
	    flag_table = part_flags;
	}

	else if (!str_prefix(arg3,"comm"))
	{
	    if (IS_NPC(victim))
	    {
		send_to_char("Comm can't be set on NPCs.\n\r",ch);
		return;
	    }

	    flag = &victim->comm;
	    flag_table = comm_flags;
	}

	else 
	{
	    send_to_char("That's not an acceptable flag.\n\r",ch);
	    return;
	}

	old = *flag;
	victim->zone = NULL;

	if (type != '=')
	    new = old;

        /* mark the words */
        for (; ;)
        {
	    argument = one_argument(argument,word);

	    if (word[0] == '\0')
		break;

	    pos = flag_lookup(word,flag_table);
	    if (pos == NO_FLAG)
	    {
		send_to_char("That flag doesn't exist!\n\r",ch);
		return;
	    }
	    else if(!flag_table[ft_bitpos_lookup(pos,flag_table)].settable)
	    {
	        send_to_char("That flag is not settable.\n\r",ch);
	        return;
            }
	    else
		SET_BIT(marked,pos);
	}

	for (pos = 0; flag_table[pos].name != NULL; pos++)
	{
	    if (!flag_table[pos].settable && IS_SET(old,flag_table[pos].bit))
	    {
		SET_BIT(new,flag_table[pos].bit);
		continue;
	    }

	    if (IS_SET(marked,flag_table[pos].bit))
	    {
		switch(type)
		{
		    case '=':
		    case '+':
			SET_BIT(new,flag_table[pos].bit);
			break;
		    case '-':
			REMOVE_BIT(new,flag_table[pos].bit);
			break;
		    default:
			if (IS_SET(new,flag_table[pos].bit))
			    REMOVE_BIT(new,flag_table[pos].bit);
			else
			    SET_BIT(new,flag_table[pos].bit);
		}
	    }
	}
	*flag = new;
	return;
    }
    else if (!str_prefix(arg1,"obj"))
    {

/* Objects */

	obj = get_obj_world(ch,arg2);
	if (obj == NULL)
	{
	    send_to_char("You can't find that.\n\r",ch);
	    return;
	}
	if (obj->item_type == ITEM_EXIT)
	{
	    send_to_char("You can not flag exit objects.\n\r",ch);
	    return;
	}
 	if (!str_prefix(arg3,"extra"))
	{
	    flag = (long *)&obj->extra_flags;
	    flag_table = item_extra;
	}

  	else if (!str_prefix(arg3,"wear"))
	{
	    flag = (long *)&obj->wear_flags;
	    flag_table = item_wear;
	}

	else if (!str_prefix(arg3,"weapon"))
	{
	    flag = (long *)&obj->value[4];
	    flag_table = item_weapon;
	}
	else 
	{
	    send_to_char("That's not an acceptable flag.\n\r",ch);
	    return;
	}

	old = *flag;

	if (type != '=')
	    new = old;

        /* mark the words */
        for (; ;)
        {
	    argument = one_argument(argument,word);

	    if (word[0] == '\0')
		break;

	    pos = flag_lookup(word,flag_table);
	    if (pos == NO_FLAG)
	    {
		send_to_char("That flag doesn't exist!\n\r",ch);
		return;
	    }
            else if(!flag_table[ft_bitpos_lookup(pos,flag_table)].settable)
            {
                send_to_char("That flag is not settable.\n\r",ch);
                return;
            }
	    else
		SET_BIT(marked,pos);
	}

	for (pos = 0; flag_table[pos].name != NULL; pos++)
	{
	    if (!flag_table[pos].settable && IS_SET(old,flag_table[pos].bit))
	    {
		SET_BIT(new,flag_table[pos].bit);
		continue;
	    }

	    if (IS_SET(marked,flag_table[pos].bit))
	    {
		switch(type)
		{
		    case '=':
		    case '+':
			SET_BIT(new,flag_table[pos].bit);
			break;
		    case '-':
			REMOVE_BIT(new,flag_table[pos].bit);
			break;
		    default:
			if (IS_SET(new,flag_table[pos].bit))
			    REMOVE_BIT(new,flag_table[pos].bit);
			else
			    SET_BIT(new,flag_table[pos].bit);
		}
	    }
	}
	*flag = new;
	return;
    }
    else if (!str_prefix(arg1,"room"))
    {

/* Rooms */

	location = find_location(ch,arg2);
	if ( location == NULL )
	{
	    send_to_char( "No such location.\n\r", ch );
	    return;
	}
	if (!str_prefix(arg3,"room"))
	{
	    flag = (long *)&location->room_flags;
	    flag_table = area_room;
	}
	else 
	{
	    send_to_char("That's not an acceptable flag.\n\r",ch);
	    return;
	}

	old = *flag;

	if (type != '=')
	    new = old;

        /* mark the words */
        for (; ;)
        {
	    argument = one_argument(argument,word);

	    if (word[0] == '\0')
		break;

	    pos = flag_lookup(word,flag_table);
	    if (pos == NO_FLAG)
	    {
		send_to_char("That flag doesn't exist!\n\r",ch);
		return;
	    }
            else if(!flag_table[ft_bitpos_lookup(pos,flag_table)].settable)
            {
                send_to_char("That flag is not settable.\n\r",ch);
                return;
            }
	    else
		SET_BIT(marked,pos);
	}

	for (pos = 0; flag_table[pos].name != NULL; pos++)
	{
	    if (!flag_table[pos].settable && IS_SET(old,flag_table[pos].bit))
	    {
		SET_BIT(new,flag_table[pos].bit);
		continue;
	    }

	    if (IS_SET(marked,flag_table[pos].bit))
	    {
		switch(type)
		{
		    case '=':
		    case '+':
			SET_BIT(new,flag_table[pos].bit);
			break;
		    case '-':
			REMOVE_BIT(new,flag_table[pos].bit);
			break;
		    default:
			if (IS_SET(new,flag_table[pos].bit))
			    REMOVE_BIT(new,flag_table[pos].bit);
			else
			    SET_BIT(new,flag_table[pos].bit);
		}
	    }
	}
	*flag = new;
	return;
    }

}



    
