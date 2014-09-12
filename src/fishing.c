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
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "tables.h"

void do_fish( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *pole;
    OBJ_DATA *obj;
    char buf[MAX_STRING_LENGTH];

#define MAX_FISH 5
#define OBJ_VNUM_FISH 56
#define OBJ_VNUM_BOOT 619

    const struct
    {
        const char *	name;
        const char *	sdesc;
        float		value;
        bool		isfish;
    } fish [MAX_FISH] =
    {
        {"salmon",	"salmon",	1.5,		TRUE },
        {"bass",	"bass",		0.8,		TRUE },
        {"pike",	"pike",		1.2,		TRUE },
        {"halibut",	"halibut",	1.0,		TRUE },
        {"",		"",		OBJ_VNUM_BOOT,	FALSE }
    };

/*    if(strcmp(argument,"test"))
    {
        send_to_char("Hey! Fishing isn't ready yet!\n\r",ch);
        return;
    }*/


    if ( get_skill(ch,gsn_fishing) < 1)
    {
        send_to_char("You don't have a clue how to fish.\n\r", ch);
        return;
    }

    if ( ch->in_room->sector_type != SECT_WATER_SWIM
         && ch->in_room->sector_type != SECT_WATER_NOSWIM )
    {
        send_to_char("You can't fish here.\n\r", ch);
        return;
    }

    if ( (pole = get_eq_char(ch,WEAR_WIELD)) == NULL
          || pole->item_type != ITEM_FISHING_POLE  )
    {
        send_to_char("You aren't wielding a fishing pole.\n\r", ch);
        return;
    }

    send_to_char( "You attempt to fish.\n\r", ch );
    if ( number_percent( ) + 15 < get_skill(ch,gsn_fishing))
    {
        int type = number_range(0,MAX_FISH - 1);
        int weight = (fish[type].isfish) ?
		(number_range(3,21) * fish[type].value) : 0;

        if(fish[type].isfish)
        {
           obj = create_object(get_obj_index(OBJ_VNUM_FISH),0);
        }
        else
        {
           obj = create_object(get_obj_index(fish[type].value),0);
        }

        sprintf(buf, "%s %s",
           (!fish[type].isfish) ? "smelly":
           (weight > 30) ? "enormous":
           (weight > 20) ? "fat":
           (weight > 10) ? "average" : "thin",
	   (!fish[type].isfish) ? obj->name : fish[type].name );

        free_string(obj->name);
        obj->name = str_dup(buf);

        sprintf(buf, "%s %s",
           (!fish[type].isfish) ? "a smelly":
           (weight > 30) ? "an enormous":
           (weight > 25) ? "a really fat":
           (weight > 20) ? "a fat":
           (weight > 10) ? "an average" :
           (weight >  5) ? "a thin" : "an extremely thin",
	   (!fish[type].isfish) ? obj->short_descr : fish[type].sdesc );

        free_string(obj->short_descr);
        obj->short_descr = str_dup(buf);

        if(fish[type].isfish)
	{
            sprintf(buf, "%s %s is here.",
            (weight > 30) ? "An enormous":
            (weight > 25) ? "A really fat":
            (weight > 20) ? "A fat":
            (weight > 10) ? "An average" :
            (weight >  5) ? "A thin" : "An extremely thin",
	    fish[type].sdesc );

            free_string(obj->description);
            obj->description = str_dup(buf);
	}

        if (weight)
	{
            obj->weight = weight * 8.5;
	    obj->cost = weight * 5;
	}
	else
	{
	    obj->cost = UMAX(0,obj->cost * 0.8 - 10); /* Smelly = yuck */

	}

        if (fish[type].isfish)
        {
            act("You caught $P!", ch, NULL, obj, TO_CHAR);
            act("$n caught $P!", ch, NULL, obj, TO_ROOM);
        }
        else
        {
            act("GASP! You reel in $P!", ch, NULL, obj, TO_CHAR);
            act("$n reels in $P!", ch, NULL, obj, TO_ROOM);
        }

        if ( get_carry_weight(ch) + get_obj_weight( obj ) > can_carry_w( ch ) )
        {
            act("Your inventory is full, so you fling $P on the floor.",
		ch, NULL, obj, TO_CHAR);
            act("Already carrying too much, $n flings the $P on the floor.",
		ch, NULL, obj, TO_ROOM);
            obj_to_room(obj,ch->in_room);
        }
        else
        {
            obj_to_char(obj, ch);
        }

	check_improve(ch,gsn_fishing,TRUE,3);
    }
    else
    {
        send_to_char("You failed to catch anything.\n\r",ch);
	check_improve(ch,gsn_fishing,FALSE,3);
        return;
    }

    return;
}

void do_cook( CHAR_DATA *ch, char *argument )
{
//    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;

    if(argument[0] == '\0')
    {
        send_to_char("Syntax: cook <obj>\n\r",ch);
        send_to_char("(Cooking's not done.)\n\r",ch);
        return;
    }

    if ( get_skill(ch,gsn_fishing) < 1)
    {
        send_to_char("You don't have a clue how to cook.\n\r", ch);
        return;
    }

    if ( (obj = get_obj_carry(ch, argument) ) == NULL )
    {
        send_to_char("You aren't carrying anything like that.\n\r",ch);
        return;
    }

    send_to_char("(Feature under testing)\n\r", ch);

    if( obj->item_type != ITEM_UNCOOKED_FOOD )
    {
        if( obj->item_type == ITEM_FOOD )
            send_to_char("That doesn't need cooking.\n\r",ch);
        else
            send_to_char("That can't be cooked.\n\r",ch);

        return;
    }

    return;

}
