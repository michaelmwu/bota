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
#include "merc.h"
#include "tables.h"
#include "recycle.h"
#include "event.h"

/* command procedures needed */
DECLARE_DO_FUN(do_look		);
DECLARE_DO_FUN(do_recall	);
DECLARE_DO_FUN(do_stand		);
DECLARE_DO_FUN(do_run		);
DECLARE_SPEC_FUN(       spec_quest_vip          );

BUFFER *get_obj(CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container);

/*
 * Local functions.
 */
int	find_door	args( ( CHAR_DATA *ch, char *arg ) );
bool	has_key		args( ( CHAR_DATA *ch, int key ) );
void	enter_exit	args( ( CHAR_DATA *ch, char *arg ) );



void move_char( CHAR_DATA *ch, int door, bool follow, bool quiet )
{
    CHAR_DATA *fch, *vch;
    CHAR_DATA *fch_next, *vch_next;
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    int track;

    if (!IS_NPC(ch))
    {
      /* Uh oh, another drunk Frenchman on the loose! :) */
      if (ch->pcdata->condition[COND_DRUNK] > 10)
      {
        if (ch->pcdata->condition[COND_DRUNK] > number_percent())
        {
          act("You feel a little drunk.. not to mention kind of lost..",
              ch,NULL,NULL,TO_CHAR);
          act("$n looks a little drunk.. not to mention kind of lost..",
              ch,NULL,NULL,TO_ROOM);
          door = number_range(0,5);
        }
        else
        {
          act("You feel a little.. drunk..",ch,NULL,NULL,TO_CHAR);
          act("$n looks a little.. drunk..",ch,NULL,NULL,TO_ROOM);
        }
      }
    }    

    if ( door < 0 || door > 11 )
    {
	bug( "Do_move: bad door %d.", door );
	return;
    }

    /*
     * Exit trigger, if activated, bail out. Only PCs are triggered.
     */
    if ( !IS_NPC(ch) && mp_exit_trigger( ch, door ) )
	return;

    if(ch->in_arena && arena.status != ARENA_STATUS_GAME)
    {
        send_to_char("Wait for the game to begin, please.\n\r",ch);
        return;
    }

    in_room = ch->in_room;
    if ( ( ch->alignment < 0)
    &&   door < 6
    &&   ( pexit   = in_room->exit[door+6] ) != NULL )
    {
	door += 6;
    } else if ( ( pexit = in_room->exit[door] ) == NULL)
    {
	if (!quiet)
	{
	    OBJ_DATA *portal;

	    portal = get_obj_list( ch, dir_name[door],  ch->in_room->contents );
	    if (portal != NULL)
	    {
		enter_exit( ch, dir_name[door] );
		return;
	    }
	}
    }

    if ( ( pexit   = in_room->exit[door] ) == NULL
    ||   ( to_room = pexit->u1.to_room   ) == NULL 
    ||	 !can_see_room(ch,pexit->u1.to_room))
    {
	if (!quiet)
	{
	    if ((IS_NPC(ch) && number_percent() < 10)
	    || (!IS_NPC(ch) && number_percent() < (40 +
	        ch->pcdata->condition[COND_DRUNK])))
	    {
	      if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 5)
	      {
	        act("You drunkenly slam face-first into the 'exit' on your way $T.",ch,NULL,dir_name[door],TO_CHAR);
	        act("$n drunkenly slams face-first into the 'exit' on $s way $T.",ch,NULL,dir_name[door],TO_ROOM);
	        damage_old(ch,ch,10,0,DAM_BASH,FALSE);
	      }
	      else
	      {
                act("You almost go $T, but suddenly realize that there's no exit there.",ch,NULL,dir_name[door],TO_CHAR);
                act("$n looks like $e's about to go $T, but suddenly stops short and looks confused.",ch,NULL,dir_name[door],TO_ROOM);
	      }
	    }
	    else
	    {
	      if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
	      {
	        act("You stumble about aimlessly and fall down drunk.",
	            ch,NULL,dir_name[door],TO_CHAR);
	        act("$n stumbles about aimlessly and falls down drunk.",
	            ch,NULL,dir_name[door],TO_ROOM);
	         ch->position = POS_RESTING;
	      }
	      else
	      {
	        act("You almost go $T, but suddenly realize that there's no exit there.",ch,NULL,dir_name[door],TO_CHAR);
	        act("$n looks like $e's about to go $T, but suddenly stops short and looks confused.",ch,NULL,dir_name[door],TO_ROOM);
	      }
         }
    }
    return;
    }

    if (IS_SET(pexit->exit_info, EX_CLOSED)
    &&  (!IS_AFFECTED(ch, AFF_PASS_DOOR) || IS_SET(pexit->exit_info,EX_NOPASS))
    &&   !IS_TRUSTED(ch,KNIGHT))
    {
	if (!quiet)
	    act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM)
    &&   ch->master != NULL
    &&   in_room == ch->master->in_room )
    {
	if (!quiet)
	    send_to_char( "What?  And leave your beloved master?\n\r", ch );
	return;
    }

    if ( !is_room_owner(ch,to_room) && room_is_private( ch,to_room ) )
    {
	if (!quiet)
	    send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) )
    {
	int iClass, iGuild;
	int move;
	bool ts1;
	bool ts2;

	ts1 = FALSE;
	ts2 = FALSE;
	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	{
	    for ( iGuild = 0; iGuild < MAX_GUILD; iGuild ++)	
	    {
	    	if ( iClass != ch->class
	    	&&   to_room->vnum == class_table[iClass].guild[iGuild] )
	    	{
		    ts1 = TRUE;
		}
	    	if ( iClass == ch->class
	    	&&   to_room->vnum == class_table[iClass].guild[iGuild] )
	    	{
		    ts2 = TRUE;
		}
	    }
	}

	if (ts2)
	{
	    ts1 = FALSE;
	}
	if (ts1 && !IS_IMMORTAL(ch))
	{
	    if (!quiet)
		send_to_char( "You aren't allowed in there.\n\r", ch );
	    return;
	}

	if ( in_room->sector_type == SECT_AIR
	||   to_room->sector_type == SECT_AIR )
	{
	    if ( !IS_AFFECTED(ch, AFF_FLYING) && !IS_IMMORTAL(ch))
	    {
		if (!quiet)
		    send_to_char( "You can't fly.\n\r", ch );
		return;
	    }
	}

	if (( in_room->sector_type == SECT_WATER_NOSWIM
	||    to_room->sector_type == SECT_WATER_NOSWIM )
  	&&    !IS_AFFECTED(ch,AFF_FLYING))
	{
	    OBJ_DATA *obj;
	    bool found;

	    /*
	     * Look for a boat.
	     */
	    found = FALSE;

	    if (IS_IMMORTAL(ch))
		found = TRUE;

	    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
	    {
		if ( obj->item_type == ITEM_BOAT )
		{
		    found = TRUE;
		    break;
		}
	    }
	    if ( !found )
	    {
		if (!quiet)
		    send_to_char( "You need a boat to go there.\n\r", ch );
		return;
	    }
	}

	move = movement_loss[UMIN(SECT_MAX-1, in_room->sector_type)]
	     + movement_loss[UMIN(SECT_MAX-1, to_room->sector_type)]
	     ;

        move /= 2;  /* i.e. the average */

        if(IS_SET(in_room->room_flags, ROOM_ROAD))
            move *= 0.75;

        if(IS_SET(to_room->room_flags, ROOM_ROAD))
            move *= 0.75;

        /* By Thyrr - penalizes junk */
        move += (get_carry_weight(ch) / can_carry_w(ch)) / 25;

        /* Penalize sneaking */
        if(IS_AFFECTED(ch,AFF_SNEAK))
            move += 1;

        if(ch->pcdata->condition[COND_HUNGER] == 0 )
            move += 1;
        if(ch->pcdata->condition[COND_THIRST] == 0 )
            move += 3;

        if ( weather_info.sky == SKY_RAINING ||
             weather_info.sky == SKY_LIGHTNING )
            move += 2;

	/* conditional effects */
	if (IS_AFFECTED(ch,AFF_FLYING) || IS_AFFECTED(ch,AFF_HASTE))
	    move /= 2;

	if (IS_AFFECTED(ch,AFF_SLOW))
	    move *= 2;

	if ( ch->move < move)
	{
	    if (!quiet)
		send_to_char( "You are too exhausted.\n\r", ch );
	    return;
	}

	if ( !IS_NPC(ch) && ch->pcdata->condition[COND_FATIGUE] == 0)
	{
	    if (!quiet)
		send_to_char( "You are out of breath.\n\r", ch );
	    return;
	}

        if(!IS_NPC(ch) && move > 2 && ++(ch->pcdata->moves_this_tick) > 
            get_curr_stat(ch,STAT_CON) * 2.5 && number_bits(3) < 7
            && !ch->in_arena)
            gain_condition(ch, COND_FATIGUE, -1);

	WAIT_STATE( ch, interpolate_r(1, 16, 1, 8, move) );
	ch->move -= move;
    }

    interrupt_char(ch, EVENT_INTERRUPT_ON_MOVE);

    if ( IS_AFFECTED(ch, AFF_HIDE) )
    {
        REMOVE_BIT(ch->affected_by, AFF_HIDE);
    }

    if ( !IS_AFFECTED(ch, AFF_SNEAK)
         && ch->ghost_level == 0
         && ch->invis_level == 0 )
    {
	if (!quiet)
	{
	    if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
	    {
	        act("$n stumbles off drunkenly on $s way $T.",ch,NULL,dir_name[door],TO_ROOM);
	    }
	    else
	    {
	        act_new_2("$n leaves $T.",ch,NULL,dir_name[door],TO_ROOM,POS_RESTING, 1, ACTION_MUST_SEE );
	    }
	}
    }
    else /* Don't see -- sneaking */
    {
	if (!quiet)
	{
	    if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
	    {
	        act("$n stumbles off drunkenly on $s way $T.",ch,NULL,dir_name[door],TO_ROOM);
	    }
	    else
	    {
		act_new_2("$n leaves $T.",ch,NULL,dir_name[door],TO_ROOM,
                POS_RESTING, UMAX(LEVEL_HERO + 1,get_trust(ch)),
                ACTION_MUST_SEE );
	    }
	}
    }

    char_from_room( ch );
    char_to_room( ch, to_room );

    if (IS_NPC(ch) || !IS_IMMORTAL(ch))
    {
	for (track = MAX_TRACK-1; track > 0; track--)
	{
	    ch->track_to[track] = ch->track_to[track-1];
	    ch->track_from[track] = ch->track_from[track-1];
	}

	if (IS_AFFECTED(ch,AFF_FLYING))
	{
	    ch->track_from[0] = 0;
	    ch->track_to[0] = 0;
	}
	else {
	    ch->track_from[0] = in_room->vnum;
	    ch->track_to[0] = to_room->vnum;
	}
    }

    if ( !IS_AFFECTED(ch, AFF_SNEAK)
         && ch->ghost_level == 0
         && ch->invis_level == 0 )
    {
	if (!quiet)
	{
	    if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
	    {
	        act_new_2("$n stumbles in drunkenly, looking all nice and French.",
	            ch,NULL,NULL,TO_ROOM, POS_RESTING, 1, ACTION_MUST_SEE );
	    }
	    else
	    {
		    act_new_2( "$n has arrived.", ch, NULL, NULL, TO_ROOM, POS_RESTING, 1, ACTION_MUST_SEE );
	    }
	}
    
	if (IS_NPC(ch))
	{
	    if( ch->say_descr && ch->say_descr[0] != '\0')
	    {
		if (!quiet)
		    act( "$n says '{a$T{x'", ch, NULL, ch->say_descr, TO_ROOM );
	    }
	}
    }
    else
    {
       if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
          act_new_2("$n stumbles in drunkenly, looking all nice and French.",
             ch,NULL,NULL,TO_ROOM, POS_RESTING, 1, ACTION_MUST_SEE );
       else
          act_new_2( "$n has arrived.", ch, NULL, NULL, TO_ROOM, POS_RESTING,
             UMAX(LEVEL_HERO + 1,get_trust(ch)), ACTION_MUST_SEE );
    }


    if (!quiet && ch->desc)
	do_look( ch, "auto" );

    if (in_room == to_room) /* no circular follows */
	return;

    for ( fch = in_room->people; fch != NULL; fch = fch_next )
    {
	fch_next = fch->next_in_room;

	if ( fch->master == ch && IS_AFFECTED(fch,AFF_CHARM) 
	&&   fch->position < POS_STANDING)
	    do_stand(fch,"");

	if ( fch->master == ch && fch->position == POS_STANDING 
	&&   can_see_room(fch,to_room))
	{

	    if (IS_SET(ch->in_room->room_flags,ROOM_LAW)
	    &&  (IS_NPC(fch) &&
                (IS_SET(fch->act,ACT_AGGRESSIVE)
                || !IS_SET(fch->act, ACT_PET)) ))
	    {
		act("You can't bring $N into the city.",
		    ch,NULL,fch,TO_CHAR);
		act("You aren't allowed in the city.",
		    fch,NULL,NULL,TO_CHAR);
		continue;
	    }

	    act( "You follow $N.", fch, NULL, ch, TO_CHAR );
	    move_char( fch, door, TRUE, FALSE );
	}
    }

    if(IS_NPC(ch))
        script_call(ch->pyobject, "on_arrive", "R", in_room);

    for ( vch = to_room->people; vch != NULL; vch = vch_next )
    {
        vch_next = vch->next_in_room;

        if(vch != ch && can_see(vch,ch) && IS_NPC(vch))
            script_call(vch->pyobject, "on_detectEntry", "C", ch);
    }

    if(IS_NPC(ch) && ch->leader && !IS_NPC(ch->leader) &&
        ch->leader->pcdata->questdata)
    {
        QUEST_DATA *quest = ch->leader->pcdata->questdata;
        if(quest->type == QUEST_ESCORT && quest->target.ch == ch
            && quest->target2.room == to_room)
        {
            send_to_char("You're almost done with your quest!\n\r"
                "Get back to the questmaster before time runs out!\n\r",
                ch->leader);
            quest->status = QUEST_MUST_RETURN;
        }
    }

    /* 
     * If someone is following the char, these triggers get activated
     * for the followers before the char, but it's safer this way...
     */
    if ( IS_NPC( ch ) && HAS_TRIGGER( ch, TRIG_ENTRY ) )
	mp_percent_trigger( ch, NULL, NULL, NULL, TRIG_ENTRY );
    if ( !IS_NPC( ch ) )
    	mp_greet_trigger( ch );

    return;
}

/* RW Enter movable exits */
void enter_exit( CHAR_DATA *ch, char *arg)
{    
    ROOM_INDEX_DATA *location; 
    int track;

    if ( ch->fighting != NULL ) 
	return;

    /* nifty portal stuff */
    if (arg[0] != '\0')
    {
        ROOM_INDEX_DATA *old_room;
	OBJ_DATA *portal;
	CHAR_DATA *fch, *fch_next;

        old_room = ch->in_room;

	portal = get_obj_list( ch, arg,  ch->in_room->contents );
	
	if (portal == NULL)
	{
	    send_to_char("Alas, you cannot go that way.\n\r",ch);
	    return;
	}

	if (portal->item_type != ITEM_EXIT) 
	{
	    send_to_char("Alas, you cannot go that way.\n\r",ch);
	    return;
	}

	location = get_room_index(portal->value[0]);

	if (location == NULL
	||  location == old_room
	||  !can_see_room(ch,location) 
	||  (room_is_private(ch,location) && !IS_TRUSTED(ch,IMPLEMENTOR)))
	{
	    send_to_char("Alas, you cannot go that way.\n\r",ch);
	    return;
	}

	if ( IS_AFFECTED(ch, AFF_CHARM)
	&&   ch->master != NULL
	&&   old_room == ch->master->in_room )
	{
	    send_to_char( "What?  And leave your beloved master?\n\r", ch );
	    return;
	}

        if (IS_NPC(ch) && IS_SET(ch->act,ACT_AGGRESSIVE)
        &&  IS_SET(location->room_flags,ROOM_LAW))
        {
            send_to_char("You aren't allowed in the city.\n\r",ch);
            return;
        }

	if ( !IS_NPC(ch) )
	{
	    int move;

	    if ( old_room->sector_type == SECT_AIR
	    ||   location->sector_type == SECT_AIR )
	    {
		if ( !IS_AFFECTED(ch, AFF_FLYING) && !IS_IMMORTAL(ch))
		{
		    send_to_char( "You can't fly.\n\r", ch );
		    return;
		}
	    }

	    if (( old_room->sector_type == SECT_WATER_NOSWIM
	    ||    location->sector_type == SECT_WATER_NOSWIM )
  	    &&    !IS_AFFECTED(ch,AFF_FLYING))
	    {
		OBJ_DATA *obj;
		bool found;

		/*
		* Look for a boat.
		*/
		found = FALSE;

		if (IS_IMMORTAL(ch))
		    found = TRUE;

		for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
		{
		    if ( obj->item_type == ITEM_BOAT )
		    {
			found = TRUE;
			break;
		    }
		}
		if ( !found )
		{
		    send_to_char( "You need a boat to go there.\n\r", ch );
		    return;
		}
	    }

	    move = movement_loss[UMIN(SECT_MAX-1, old_room->sector_type)]
	     + movement_loss[UMIN(SECT_MAX-1, location->sector_type)]
	     ;

            move /= 2;  /* i.e. the average */


	    /* conditional effects */
	    if (IS_AFFECTED(ch,AFF_FLYING) || IS_AFFECTED(ch,AFF_HASTE))
		move /= 2;

	    if (IS_AFFECTED(ch,AFF_SLOW))
		move *= 2;

	    if ( ch->move < move )
	    {
		send_to_char( "You are too exhausted.\n\r", ch );
		return;
	    }

	    WAIT_STATE( ch, 1 );
	    ch->move -= move;
	}


	if ( !IS_AFFECTED(ch, AFF_SNEAK)
	&&   ch->invis_level <= LEVEL_HERO
	&&   ch->ghost_level <= LEVEL_HERO)
	{
	    act( "$n leaves $p.", ch,portal,NULL,TO_ROOM);
	}

	char_from_room(ch);
	char_to_room(ch, location);
	if (IS_NPC(ch) || !IS_IMMORTAL(ch))
	{
	    for (track = MAX_TRACK-1; track > 0; track--)
	    {
		ch->track_to[track] = ch->track_to[track-1];
		ch->track_from[track] = ch->track_from[track-1];
	    }
	    if (IS_AFFECTED(ch,AFF_FLYING))
	    {
		ch->track_from[0] = 0;
		ch->track_to[0] = 0;
	    } else {
		ch->track_from[0] = old_room->vnum;
		ch->track_to[0] = location->vnum;
	    }
	}

	if ( !IS_AFFECTED(ch, AFF_SNEAK)
	&&   ch->invis_level <= LEVEL_HERO
	&&   ch->ghost_level <= LEVEL_HERO)
	{
	    act("$n has arrived.",ch,NULL,NULL,TO_ROOM);
	    if (IS_NPC(ch))
	    {
		if( ch->say_descr && ch->say_descr[0] != '\0')
		{
		    act( "$n says '{a$T{x'", ch, NULL, ch->say_descr, TO_ROOM );
		}
	    }
	}

        if(ch->desc)
	    do_look(ch,"auto");

	/* protect against circular follows */
	if (old_room == location)
	    return;

    	for ( fch = old_room->people; fch != NULL; fch = fch_next )
    	{
            fch_next = fch->next_in_room;

            if (portal == NULL) 
                continue;
 
            if ( fch->master == ch && IS_AFFECTED(fch,AFF_CHARM)
            &&   fch->position < POS_STANDING)
            	do_stand(fch,"");

            if ( fch->master == ch && fch->position == POS_STANDING
	    &&   can_see_room(fch,location))
            {
 
                if (IS_SET(ch->in_room->room_flags,ROOM_LAW)
                &&  (IS_NPC(fch) && IS_SET(fch->act,ACT_AGGRESSIVE)))
                {
                    act("You can't bring $N into the city.",
                    	ch,NULL,fch,TO_CHAR);
                    act("You aren't allowed in the city.",
                    	fch,NULL,NULL,TO_CHAR);
                    continue;
            	}
 
            	act( "You follow $N.", fch, NULL, ch, TO_CHAR );
		enter_exit(fch,arg);
            }
    	}
	return;
    }

    send_to_char("Alas, you cannot go that way.\n\r",ch);
    return;
}


void do_north( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_NORTH, FALSE, FALSE );
    return;
}



void do_east( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_EAST, FALSE, FALSE );
    return;
}



void do_south( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_SOUTH, FALSE, FALSE );
    return;
}



void do_west( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_WEST, FALSE, FALSE );
    return;
}



void do_up( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_UP, FALSE, FALSE );
    return;
}



void do_down( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_DOWN, FALSE, FALSE );
    return;
}



int find_door( CHAR_DATA *ch, char *arg )
{
    EXIT_DATA *pexit;
    int door;

	 if ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) door = 0;
    else if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) door = 1;
    else if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) door = 2;
    else if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) door = 3;
    else if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) door = 4;
    else if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) door = 5;
    else
    {
	for ( door = 0; door <= 5; door++ )
	{
	    if ( (ch->alignment < 0)
	    &&   ( pexit = ch->in_room->exit[door+6] ) != NULL
            &&   IS_SET(pexit->exit_info, EX_ISDOOR)
            &&   pexit->keyword != NULL
            &&   is_name( arg, pexit->keyword ) )
	    {
                return door+6;
	    }
	    else if ( ( pexit = ch->in_room->exit[door] ) != NULL
	    &&   IS_SET(pexit->exit_info, EX_ISDOOR)
	    &&   pexit->keyword != NULL
	    &&   is_name( arg, pexit->keyword ) )
	    {
		return door;
	    }
	}
	act( "I see no $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if ( (ch->alignment < 0)
    &&   (pexit = ch->in_room->exit[door+6] ) != NULL )
    {
	door += 6;
    }

    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	act( "I see no door $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return -1;
    }

    return door;
}



void do_open( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Open what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
 	/* open portal */
	if (obj->item_type == ITEM_PORTAL)
	{
	    if (!IS_SET(obj->value[1], EX_ISDOOR))
	    {
		send_to_char("You can't do that.\n\r",ch);
		return;
	    }

	    if (!IS_SET(obj->value[1], EX_CLOSED))
	    {
		send_to_char("It's already open.\n\r",ch);
		return;
	    }

	    if (IS_SET(obj->value[1], EX_LOCKED))
	    {
		send_to_char("It's locked.\n\r",ch);
		return;
	    }

	    REMOVE_BIT(obj->value[1], EX_CLOSED);
	    act("You open $p.",ch,obj,NULL,TO_CHAR);
	    act("$n opens $p.",ch,obj,NULL,TO_ROOM);
	    return;
 	}

	/* 'open object' */
	if ( ( obj->item_type != ITEM_CONTAINER )
	&&   ( obj->item_type != ITEM_PIT ) )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_CLOSED);
	act("You open $p.",ch,obj,NULL,TO_CHAR);
	act( "$n opens $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'open door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if (  IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_CLOSED);
	act( "$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );

	/* open the other side */
	if ( ( to_room   = pexit->u1.to_room            ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->u1.to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
		act( "The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
    }

    return;
}



void do_close( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Close what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* portal stuff */
	if (obj->item_type == ITEM_PORTAL)
	{

	    if (!IS_SET(obj->value[1],EX_ISDOOR)
	    ||   IS_SET(obj->value[1],EX_NOCLOSE))
	    {
		send_to_char("You can't do that.\n\r",ch);
		return;
	    }

	    if (IS_SET(obj->value[1],EX_CLOSED))
	    {
		send_to_char("It's already closed.\n\r",ch);
		return;
	    }

	    SET_BIT(obj->value[1],EX_CLOSED);
	    act("You close $p.",ch,obj,NULL,TO_CHAR);
	    act("$n closes $p.",ch,obj,NULL,TO_ROOM);
	    return;
	}

	/* 'close object' */
	if ( ( obj->item_type != ITEM_CONTAINER )
	&&   ( obj->item_type != ITEM_PIT ) )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }

	SET_BIT(obj->value[1], CONT_CLOSED);
	act("You close $p.",ch,obj,NULL,TO_CHAR);
	act( "$n closes $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'close door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit	= ch->in_room->exit[door];
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }

	SET_BIT(pexit->exit_info, EX_CLOSED);
	act( "$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );

	/* close the other side */
	if ( ( to_room   = pexit->u1.to_room            ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
	&&   pexit_rev->u1.to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    SET_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
		act( "The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
    }

    return;
}



bool has_key( CHAR_DATA *ch, int key )
{
    OBJ_DATA *obj;

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( obj->pIndexData->vnum == key )
	    return TRUE;
    }

    return FALSE;
}



void do_lock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Lock what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* portal stuff */
	if (obj->item_type == ITEM_PORTAL)
	{
	    if (!IS_SET(obj->value[1],EX_ISDOOR)
	    ||  IS_SET(obj->value[1],EX_NOCLOSE))
	    {
		send_to_char("You can't do that.\n\r",ch);
		return;
	    }
	    if (!IS_SET(obj->value[1],EX_CLOSED))
	    {
		send_to_char("It's not closed.\n\r",ch);
	 	return;
	    }

	    if (obj->value[4] < 0 || IS_SET(obj->value[1],EX_NOLOCK))
	    {
		send_to_char("It can't be locked.\n\r",ch);
		return;
	    }

	    if (!has_key(ch,obj->value[4]))
	    {
		send_to_char("You lack the key.\n\r",ch);
		return;
	    }

	    if (IS_SET(obj->value[1],EX_LOCKED))
	    {
		send_to_char("It's already locked.\n\r",ch);
		return;
	    }

	    SET_BIT(obj->value[1],EX_LOCKED);
	    act("You lock $p.",ch,obj,NULL,TO_CHAR);
	    act("$n locks $p.",ch,obj,NULL,TO_ROOM);
	    return;
	}

	/* 'lock object' */
	if ( ( obj->item_type != ITEM_CONTAINER )
	&&   ( obj->item_type != ITEM_PIT ) )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT(obj->value[1], CONT_LOCKED);
	act("You lock $p.",ch,obj,NULL,TO_CHAR);
	act( "$n locks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'lock door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit	= ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, pexit->key) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* lock the other side */
	if ( ( to_room   = pexit->u1.to_room            ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
	&&   pexit_rev->u1.to_room == ch->in_room )
	{
	    SET_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}



void do_unlock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Unlock what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
 	/* portal stuff */
	if (obj->item_type == ITEM_PORTAL)
	{
	    if (!IS_SET(obj->value[1],EX_ISDOOR))
	    {
		send_to_char("You can't do that.\n\r",ch);
		return;
	    }

	    if (!IS_SET(obj->value[1],EX_CLOSED))
	    {
		send_to_char("It's not closed.\n\r",ch);
		return;
	    }

	    if (obj->value[4] < 0)
	    {
		send_to_char("It can't be unlocked.\n\r",ch);
		return;
	    }

	    if (!has_key(ch,obj->value[4]))
	    {
		send_to_char("You lack the key.\n\r",ch);
		return;
	    }

	    if (!IS_SET(obj->value[1],EX_LOCKED))
	    {
		send_to_char("It's already unlocked.\n\r",ch);
		return;
	    }

	    REMOVE_BIT(obj->value[1],EX_LOCKED);
	    act("You unlock $p.",ch,obj,NULL,TO_CHAR);
	    act("$n unlocks $p.",ch,obj,NULL,TO_ROOM);
	    return;
	}

	/* 'unlock object' */
	if ( ( obj->item_type != ITEM_CONTAINER )
	&&   ( obj->item_type != ITEM_PIT ) )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_LOCKED);
	act("You unlock $p.",ch,obj,NULL,TO_CHAR);
	act( "$n unlocks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'unlock door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, pexit->key) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* unlock the other side */
	if ( ( to_room   = pexit->u1.to_room            ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->u1.to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}



void do_pick( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Pick what?\n\r", ch );
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_pick_lock].beats );

    /* look for guards */
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( IS_NPC(gch) && IS_AWAKE(gch) && ch->level + 5 < gch->level )
	{
	    act( "$N is standing too close to the lock.",
		ch, NULL, gch, TO_CHAR );
	    return;
	}
    }

    if ( !IS_NPC(ch) && number_percent( ) > get_skill(ch,gsn_pick_lock))
    {
	send_to_char( "You failed.\n\r", ch);
	check_improve(ch,gsn_pick_lock,FALSE,2);
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* portal stuff */
	if (obj->item_type == ITEM_PORTAL)
	{
	    if (!IS_SET(obj->value[1],EX_ISDOOR))
	    {	
		send_to_char("You can't do that.\n\r",ch);
		return;
	    }

	    if (!IS_SET(obj->value[1],EX_CLOSED))
	    {
		send_to_char("It's not closed.\n\r",ch);
		return;
	    }

	    if (obj->value[4] < 0)
	    {
		send_to_char("It can't be unlocked.\n\r",ch);
		return;
	    }

	    if (IS_SET(obj->value[1],EX_PICKPROOF))
	    {
		send_to_char("You failed.\n\r",ch);
		return;
	    }

	    REMOVE_BIT(obj->value[1],EX_LOCKED);
	    act("You pick the lock on $p.",ch,obj,NULL,TO_CHAR);
	    act("$n picks the lock on $p.",ch,obj,NULL,TO_ROOM);
	    check_improve(ch,gsn_pick_lock,TRUE,2);
	    return;
	}

	    


	
	/* 'pick object' */
	if ( ( obj->item_type != ITEM_CONTAINER )
	&&   ( obj->item_type != ITEM_PIT ) )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if ( IS_SET(obj->value[1], CONT_PICKPROOF) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_LOCKED);
        act("You pick the lock on $p.",ch,obj,NULL,TO_CHAR);
        act("$n picks the lock on $p.",ch,obj,NULL,TO_ROOM);
	check_improve(ch,gsn_pick_lock,TRUE,2);
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'pick door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_IMMORTAL(ch))
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 && !IS_IMMORTAL(ch))
	    { send_to_char( "It can't be picked.\n\r",     ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_PICKPROOF) && !IS_IMMORTAL(ch))
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	check_improve(ch,gsn_pick_lock,TRUE,2);

	/* pick the other side */
	if ( ( to_room   = pexit->u1.to_room            ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->u1.to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}




void do_stand( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj = NULL;

    if (argument[0] != '\0')
    {
	if (ch->position == POS_FIGHTING)
	{
	    send_to_char("Maybe you should finish fighting first?\n\r",ch);
	    return;
	}
	obj = get_obj_list(ch,argument,ch->in_room->contents);
	if (obj == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}
	if (obj->item_type != ITEM_FURNITURE
	||  (!IS_SET(obj->value[2],STAND_AT)
	&&   !IS_SET(obj->value[2],STAND_ON)
	&&   !IS_SET(obj->value[2],STAND_IN)))
	{
	    send_to_char("You can't seem to find a place to stand.\n\r",ch);
	    return;
	}
	if (ch->on != obj && count_users(obj) >= obj->value[0])
	{
	    act_new("There's no room to stand on $p.",
		ch,obj,NULL,TO_CHAR,POS_DEAD);
	    return;
	}
	ch->on = obj;
    }
    
    switch ( ch->position )
    {
    case POS_SLEEPING:
	if ( IS_AFFECTED(ch, AFF_SLEEP) )
	    { send_to_char( "You can't wake up!\n\r", ch ); return; }
	
	if (obj == NULL)
	{
	    send_to_char( "You wake and stand up.\n\r", ch );
	    act( "$n wakes and stands up.", ch, NULL, NULL, TO_ROOM );

            if(ch->on && IS_SET(ch->on->value[2],WAKE_TAKE) &&
               can_see_obj(ch,ch->on))
            {
                BUFFER *output;
                OBJ_DATA *furn;

                furn = ch->on;
                ch->on = NULL;

                output = get_obj(ch, furn, NULL);
                send_to_char( buf_string(output) , ch );
                free_buf(output);
                send_to_char("\n\r",ch);

                REMOVE_BIT(furn->value[2],WAKE_TAKE);
            }
	    ch->on = NULL;
	}
	else if (IS_SET(obj->value[2],STAND_AT))
	{
	   act_new("You wake and stand at $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
	   act("$n wakes and stands at $p.",ch,obj,NULL,TO_ROOM);
	}
	else if (IS_SET(obj->value[2],STAND_ON))
	{
	    act_new("You wake and stand on $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
	    act("$n wakes and stands on $p.",ch,obj,NULL,TO_ROOM);
	}
	else 
	{
	    act_new("You wake and stand in $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
	    act("$n wakes and stands in $p.",ch,obj,NULL,TO_ROOM);
	}
	ch->position = POS_STANDING;
	do_look(ch,"auto");
	break;

    case POS_RESTING: case POS_SITTING:
	if (obj == NULL)
	{
	    send_to_char( "You stand up.\n\r", ch );
	    act( "$n stands up.", ch, NULL, NULL, TO_ROOM );

            if(ch->on && IS_SET(ch->on->value[2],WAKE_TAKE) &&
               can_see_obj(ch,ch->on))
            {
                BUFFER *output;
                OBJ_DATA *furn;

                furn = ch->on;
                ch->on = NULL;

                output = get_obj(ch, furn, NULL);
                send_to_char( buf_string(output) , ch );
                free_buf(output);
                send_to_char("\n\r",ch);

                REMOVE_BIT(furn->value[2],WAKE_TAKE);
            }

	    ch->on = NULL;
	}
	else if (IS_SET(obj->value[2],STAND_AT))
	{
	    act("You stand at $p.",ch,obj,NULL,TO_CHAR);
	    act("$n stands at $p.",ch,obj,NULL,TO_ROOM);
	}
	else if (IS_SET(obj->value[2],STAND_ON))
	{
	    act("You stand on $p.",ch,obj,NULL,TO_CHAR);
	    act("$n stands on $p.",ch,obj,NULL,TO_ROOM);
	}
	else
	{
	    act("You stand in $p.",ch,obj,NULL,TO_CHAR);
	    act("$n stands on $p.",ch,obj,NULL,TO_ROOM);
	}
	ch->position = POS_STANDING;
	break;

    case POS_STANDING:
	send_to_char( "You are already standing.\n\r", ch );
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}



void do_rest( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj = NULL;

    if (ch->position == POS_FIGHTING)
    {
	send_to_char("You are already fighting!\n\r",ch);
	return;
    }

    if ( IS_AFFECTED(ch, AFF_SLEEP) )
    {
        act( "You can't wake up!",   ch, NULL, ch, TO_CHAR );
        return;
    }


    /* okay, now that we know we can rest, find an object to rest on */
    if (argument[0] != '\0')
    {
	obj = get_obj_list(ch,argument,ch->in_room->contents);
	if (obj == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}
    }
    else obj = ch->on;

    if (obj != NULL)
    {
        if (!IS_SET(obj->item_type,ITEM_FURNITURE) 
    	||  (!IS_SET(obj->value[2],REST_ON)
    	&&   !IS_SET(obj->value[2],REST_IN)
    	&&   !IS_SET(obj->value[2],REST_AT)))
    	{
	    send_to_char("You can't rest on that.\n\r",ch);
	    return;
    	}

        if (obj != NULL && ch->on != obj && count_users(obj) >= obj->value[0])
        {
	    act_new("There's no more room on $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
	    return;
    	}
	
	ch->on = obj;
    }

    switch ( ch->position )
    {
    case POS_SLEEPING:
	if (obj == NULL)
	{
	    send_to_char( "You wake up and start resting.\n\r", ch );
	    act ("$n wakes up and starts resting.",ch,NULL,NULL,TO_ROOM);
	}
	else if (IS_SET(obj->value[2],REST_AT))
	{
	    act_new("You wake up and rest at $p.",
		    ch,obj,NULL,TO_CHAR,POS_SLEEPING);
	    act("$n wakes up and rests at $p.",ch,obj,NULL,TO_ROOM);
	}
        else if (IS_SET(obj->value[2],REST_ON))
        {
            act_new("You wake up and rest on $p.",
                    ch,obj,NULL,TO_CHAR,POS_SLEEPING);
            act("$n wakes up and rests on $p.",ch,obj,NULL,TO_ROOM);
        }
        else
        {
            act_new("You wake up and rest in $p.",
                    ch,obj,NULL,TO_CHAR,POS_SLEEPING);
            act("$n wakes up and rests in $p.",ch,obj,NULL,TO_ROOM);
        }
	ch->position = POS_RESTING;
	break;

    case POS_RESTING:
	send_to_char( "You are already resting.\n\r", ch );
	break;

    case POS_STANDING:
	if (obj == NULL)
	{
	    send_to_char( "You rest.\n\r", ch );
	    act( "$n sits down and rests.", ch, NULL, NULL, TO_ROOM );
	}
        else if (IS_SET(obj->value[2],REST_AT))
        {
	    act("You sit down at $p and rest.",ch,obj,NULL,TO_CHAR);
	    act("$n sits down at $p and rests.",ch,obj,NULL,TO_ROOM);
        }
        else if (IS_SET(obj->value[2],REST_ON))
        {
	    act("You sit on $p and rest.",ch,obj,NULL,TO_CHAR);
	    act("$n sits on $p and rests.",ch,obj,NULL,TO_ROOM);
        }
        else
        {
	    act("You rest in $p.",ch,obj,NULL,TO_CHAR);
	    act("$n rests in $p.",ch,obj,NULL,TO_ROOM);
        }
	ch->position = POS_RESTING;
	break;

    case POS_SITTING:
	if (obj == NULL)
	{
	    send_to_char("You rest.\n\r",ch);
	    act("$n rests.",ch,NULL,NULL,TO_ROOM);
	}
        else if (IS_SET(obj->value[2],REST_AT))
        {
	    act("You rest at $p.",ch,obj,NULL,TO_CHAR);
	    act("$n rests at $p.",ch,obj,NULL,TO_ROOM);
        }
        else if (IS_SET(obj->value[2],REST_ON))
        {
	    act("You rest on $p.",ch,obj,NULL,TO_CHAR);
	    act("$n rests on $p.",ch,obj,NULL,TO_ROOM);
        }
        else
        {
	    act("You rest in $p.",ch,obj,NULL,TO_CHAR);
	    act("$n rests in $p.",ch,obj,NULL,TO_ROOM);
	}
	ch->position = POS_RESTING;
	break;
    }


    return;
}


void do_sit (CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj = NULL;

    if (ch->position == POS_FIGHTING)
    {
	send_to_char("Maybe you should finish this fight first?\n\r",ch);
	return;
    }

    /* okay, now that we know we can sit, find an object to sit on */
    if (argument[0] != '\0')
    {
	obj = get_obj_list(ch,argument,ch->in_room->contents);
	if (obj == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}
    }
    else obj = ch->on;

    if (obj != NULL)                                                              
    {
	if (!IS_SET(obj->item_type,ITEM_FURNITURE)
	||  (!IS_SET(obj->value[2],SIT_ON)
	&&   !IS_SET(obj->value[2],SIT_IN)
	&&   !IS_SET(obj->value[2],SIT_AT)))
	{
	    send_to_char("You can't sit on that.\n\r",ch);
	    return;
	}

	if (obj != NULL && ch->on != obj && count_users(obj) >= obj->value[0])
	{
	    act_new("There's no more room on $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
	    return;
	}

	ch->on = obj;
    }
    switch (ch->position)
    {
	case POS_SLEEPING:
            if (obj == NULL)
            {
            	send_to_char( "You wake and sit up.\n\r", ch );
            	act( "$n wakes and sits up.", ch, NULL, NULL, TO_ROOM );
            }
            else if (IS_SET(obj->value[2],SIT_AT))
            {
            	act_new("You wake and sit at $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
            	act("$n wakes and sits at $p.",ch,obj,NULL,TO_ROOM);
            }
            else if (IS_SET(obj->value[2],SIT_ON))
            {
            	act_new("You wake and sit on $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
            	act("$n wakes and sits at $p.",ch,obj,NULL,TO_ROOM);
            }
            else
            {
            	act_new("You wake and sit in $p.",ch,obj,NULL,TO_CHAR,POS_DEAD);
            	act("$n wakes and sits in $p.",ch,obj,NULL,TO_ROOM);
            }

	    ch->position = POS_SITTING;
	    break;
	case POS_RESTING:
	    if (obj == NULL)
		send_to_char("You stop resting.\n\r",ch);
	    else if (IS_SET(obj->value[2],SIT_AT))
	    {
		act("You sit at $p.",ch,obj,NULL,TO_CHAR);
		act("$n sits at $p.",ch,obj,NULL,TO_ROOM);
	    }

	    else if (IS_SET(obj->value[2],SIT_ON))
	    {
		act("You sit on $p.",ch,obj,NULL,TO_CHAR);
		act("$n sits on $p.",ch,obj,NULL,TO_ROOM);
	    }
	    ch->position = POS_SITTING;
	    break;
	case POS_SITTING:
	    send_to_char("You are already sitting down.\n\r",ch);
	    break;
	case POS_STANDING:
	    if (obj == NULL)
    	    {
		send_to_char("You sit down.\n\r",ch);
    	        act("$n sits down on the ground.",ch,NULL,NULL,TO_ROOM);
	    }
	    else if (IS_SET(obj->value[2],SIT_AT))
	    {
		act("You sit down at $p.",ch,obj,NULL,TO_CHAR);
		act("$n sits down at $p.",ch,obj,NULL,TO_ROOM);
	    }
	    else if (IS_SET(obj->value[2],SIT_ON))
	    {
		act("You sit on $p.",ch,obj,NULL,TO_CHAR);
		act("$n sits on $p.",ch,obj,NULL,TO_ROOM);
	    }
	    else
	    {
		act("You sit down in $p.",ch,obj,NULL,TO_CHAR);
		act("$n sits down in $p.",ch,obj,NULL,TO_ROOM);
	    }
    	    ch->position = POS_SITTING;
    	    break;
    }
    return;
}


void do_sleep( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj = NULL;

    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;

    case POS_RESTING:
    case POS_SITTING:
    case POS_STANDING: 
	if (argument[0] == '\0' && ch->on == NULL)
	{
	    send_to_char( "You go to sleep.\n\r", ch );
	    act( "$n goes to sleep.", ch, NULL, NULL, TO_ROOM );
	    ch->position = POS_SLEEPING;
	}
	else  /* find an object and sleep on it */
	{
	    if (argument[0] == '\0')
		obj = ch->on;
	    else
            {
	    	obj = get_obj_here( ch, argument);
            }

	    if (obj == NULL)
	    {
		send_to_char("You don't see that here.\n\r",ch);
		return;
	    }

	    if (obj->item_type != ITEM_FURNITURE
	    ||  (!IS_SET(obj->value[2],SLEEP_ON) 
	    &&   !IS_SET(obj->value[2],SLEEP_IN)
	    &&	 !IS_SET(obj->value[2],SLEEP_AT)))
	    {
		send_to_char("You can't sleep on that!\n\r",ch);
		return;
	    }


    if(obj->carried_by)
    {
        if ( !can_drop_obj( ch, obj ) )
        {
            send_to_char( "You can't let go of it.\n\r", ch );
            return;
        }

        obj_from_char( obj );
        obj_to_room( obj, ch->in_room );
        act( "$n drops $p.", ch, obj, NULL, TO_ROOM );
        act( "You drop $p.", ch, obj, NULL, TO_CHAR );
        if (IS_OBJ_STAT(obj,ITEM_MELT_DROP))
        {
            act("$p dissolves into smoke.",ch,obj,NULL,TO_ROOM);
            act("$p dissolves into smoke.",ch,obj,NULL,TO_CHAR);
            extract_obj(obj);
            return;
        }
        SET_BIT(obj->value[2],WAKE_TAKE);
    }


	    if (ch->on != obj && count_users(obj) >= obj->value[0])
	    {
		act_new("There is no room on $p for you.",
		    ch,obj,NULL,TO_CHAR,POS_DEAD);
		return;
	    }

	    ch->on = obj;
	    if (IS_SET(obj->value[2],SLEEP_AT))
	    {
		act("You go to sleep at $p.",ch,obj,NULL,TO_CHAR);
		act("$n goes to sleep at $p.",ch,obj,NULL,TO_ROOM);
	    }
	    else if (IS_SET(obj->value[2],SLEEP_ON))
	    {
	        act("You go to sleep on $p.",ch,obj,NULL,TO_CHAR);
	        act("$n goes to sleep on $p.",ch,obj,NULL,TO_ROOM);
	    }
	    else
	    {
		act("You go to sleep in $p.",ch,obj,NULL,TO_CHAR);
		act("$n goes to sleep in $p.",ch,obj,NULL,TO_ROOM);
	    }
	    ch->position = POS_SLEEPING;
	}
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}



void do_wake( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim, *rch;
    int pos;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
	{ do_stand( ch, argument ); return; }

    if ( !IS_AWAKE(ch) )
	{ send_to_char( "You are asleep yourself!\n\r",       ch ); return; }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{ send_to_char( "They aren't here.\n\r",              ch ); return; }

    if ( IS_AWAKE(victim) )
	{ act( "$N is already awake.", ch, NULL, victim, TO_CHAR ); return; }

    if ( IS_AFFECTED(victim, AFF_SLEEP) && !IS_IMMORTAL(ch))
	{ act( "You can't wake $M!",   ch, NULL, victim, TO_CHAR );  return; }

    rch = ch;
    if(!IS_NPC(victim) || (rch = ch->master) != NULL )
    {
        for (pos = 0; pos < MAX_FORGET; pos++)
        {
            if (victim->pcdata->forget[pos] == NULL)
                break;
            if (!str_cmp(rch->name,victim->pcdata->forget[pos]))
            {
                act("$M ignores your attempts to wake $m", ch, NULL, 
                    victim,  TO_CHAR);
                return;
            }
        }
    }


    act_new( "$n wakes you.", ch, NULL, victim, TO_VICT,POS_SLEEPING );
    do_stand(victim,"");
    return;
}



void do_sneak( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    send_to_char( "You attempt to move silently.\n\r", ch );
    affect_strip( ch, gsn_sneak );

    if (IS_AFFECTED(ch,AFF_SNEAK))
	return;

    if ( number_percent( ) < get_skill(ch,gsn_sneak))
    {
	check_improve(ch,gsn_sneak,TRUE,3);
	af.where     = TO_AFFECTS;
	af.type      = gsn_sneak;
	af.level     = ch->level; 
	af.duration  = ch->level;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_SNEAK;
	affect_to_char( ch, &af );
    }
    else
	check_improve(ch,gsn_sneak,FALSE,3);

    return;
}



void do_hide( CHAR_DATA *ch, char *argument )
{
    send_to_char( "You attempt to hide.\n\r", ch );

    if ( IS_AFFECTED(ch, AFF_HIDE) )
	REMOVE_BIT(ch->affected_by, AFF_HIDE);

    if ( number_percent( ) < get_skill(ch,gsn_hide))
    {
	SET_BIT(ch->affected_by, AFF_HIDE);
	check_improve(ch,gsn_hide,TRUE,3);
    }
    else
	check_improve(ch,gsn_hide,FALSE,3);

    return;
}


/*
 * Contributed by Alander.
 */
void do_visible( CHAR_DATA *ch, char *argument )
{
    affect_strip ( ch, gsn_invis			);
    affect_strip ( ch, gsn_mass_invis			);
    affect_strip ( ch, gsn_sneak			);
    REMOVE_BIT   ( ch->affected_by, AFF_HIDE		);
    REMOVE_BIT   ( ch->shielded_by, SHD_INVISIBLE	);
    REMOVE_BIT   ( ch->affected_by, AFF_SNEAK		);
    send_to_char( "You make yourself visible.\n\r", ch );
    return;
}



void do_recall( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *location;
    int track;

    if (IS_NPC(ch) && !IS_SET(ch->act,ACT_PET))
    {
	send_to_char("Only players can recall.\n\r",ch);
	return;
    }
  
    act( "$n prays for transportation!", ch, 0, 0, TO_ROOM );

    if ( ch->in_arena )
    {
        location = arena.teams[ch->arena_team].hq;
        return; /* no */
    }
    else if ( ch->alignment < 0 )
    {
	if ( ( location = get_room_index( ROOM_VNUM_TEMPLEB ) ) == NULL )
	{
	    send_to_char( "You are completely lost.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( location = get_room_index( ROOM_VNUM_TEMPLE ) ) == NULL )
	{
	    send_to_char( "You are completely lost.\n\r", ch );
	    return;
	}
    }

    if (!IS_NPC(ch))
    {
        if(is_clan(ch) && clan_table[ch->clan].room[0])
            location = get_room_index( clan_table[ch->clan].room[0] );
        else
            location = get_room_index(
              ch->alignment < 0 ? ROOM_VNUM_TEMPLEB:ROOM_VNUM_TEMPLE );
    }

    else if (IS_NPC(ch) && IS_SET(ch->act,ACT_PET))
    {
        if(is_clan(ch->master) && clan_table[ch->master->clan].room[0])
            location = get_room_index( clan_table[ch->master->clan].room[0] );
        else
            location = get_room_index(
              ch->master->alignment < 0 ?
              ROOM_VNUM_TEMPLEB:ROOM_VNUM_TEMPLE );
    }
    else location = get_room_index(ROOM_VNUM_LIMBO);

    if ( ch->in_room == location )
    {
        log_string(LOG_COMMAND,"%s recalled to same room %d",ch->name, ch->in_room->vnum);
	return;
    }

    if ( ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_AFFECTED(ch, AFF_CURSE))
    &&   (ch->level <= HERO) )
    {
	act( "$G has forsaken you.", ch, NULL, NULL, TO_CHAR );
	return;
    }

    if ( ( victim = ch->fighting ) != NULL )
    {
	int lose,skill;

	skill = get_skill(ch,gsn_recall);

	if ( number_percent() < 80 * skill / 100 )
	{
	    check_improve(ch,gsn_recall,FALSE,6);
	    WAIT_STATE( ch, 4 );
	    sprintf( buf, "You failed!.\n\r");
	    send_to_char( buf, ch );
	    return;
	}

	lose = (ch->desc != NULL) ? 25 : 50;
	gain_exp( ch, 0 - lose );
	check_improve(ch,gsn_recall,TRUE,4);
	sprintf( buf, "You recall from combat!  You lose %d exps.\n\r", lose );
	send_to_char( buf, ch );
        if ( !IS_NPC(ch) )
        {
	    if ( !IS_NPC(victim) && ch->attacker == TRUE)
	    {
		send_to_char( "The {RWrath of Thoth {YZAPS{x your butt on the way out!\n\r", ch);
		act( "$n is {Yzapped{x in the butt by a lightning bolt from above!", ch, NULL, NULL, TO_ROOM );
		ch->hit -= (ch->hit/4);
	    }
        }
	stop_fighting( ch, TRUE );
    }

    ch->move *= interpolate(0, 100, 90, 50,
        ((float)ch->move/ch->max_move)*100) * 0.01;

    act( "$n disappears.", ch, NULL, NULL, TO_ROOM );
    if (IS_NPC(ch) || !IS_IMMORTAL(ch))
    {
	for (track = MAX_TRACK-1; track > 0; track--)
	{
	    ch->track_to[track] = ch->track_to[track-1];
	    ch->track_from[track] = ch->track_from[track-1];
	}
	ch->track_from[0] = ch->in_room->vnum;
	ch->track_to[0] = 0;
    }
    char_from_room( ch );
    char_to_room( ch, location );
    act( "$n appears in the room.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );
    
    if (ch->pet != NULL)
	do_recall(ch->pet,"");

    return;
}



void do_train( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *mob;
    sh_int stat = - 1;
    char *pOutput = NULL;
    int cost;

    if ( IS_NPC(ch) )
	return;

    /*
     * Check for trainer.
     */
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
	if ( IS_NPC(mob) && IS_SET(mob->act, ACT_TRAIN) )
	    break;
    }

    if ( mob == NULL )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	sprintf( buf, "You have %d training sessions.\n\r", ch->train );
	send_to_char( buf, ch );
	argument = "foo";
    }

    cost = 2;

    if ( !str_cmp( argument, "str" ) )
    {
	if ( class_table[ch->class].attr_prime == STAT_STR )
	    cost    = 1;
	stat        = STAT_STR;
	pOutput     = "strength";
    }

    else if ( !str_cmp( argument, "int" ) )
    {
	if ( class_table[ch->class].attr_prime == STAT_INT )
	    cost    = 1;
	stat	    = STAT_INT;
	pOutput     = "intelligence";
    }

    else if ( !str_cmp( argument, "wis" ) )
    {
	if ( class_table[ch->class].attr_prime == STAT_WIS )
	    cost    = 1;
	stat	    = STAT_WIS;
	pOutput     = "wisdom";
    }

    else if ( !str_cmp( argument, "dex" ) )
    {
	if ( class_table[ch->class].attr_prime == STAT_DEX )
	    cost    = 1;
	stat  	    = STAT_DEX;
	pOutput     = "dexterity";
    }

    else if ( !str_cmp( argument, "con" ) )
    {
	if ( class_table[ch->class].attr_prime == STAT_CON )
	    cost    = 1;
	stat	    = STAT_CON;
	pOutput     = "constitution";
    }

    else if ( !str_cmp(argument, "hp" ) )
	cost = 1;

    else if ( !str_cmp(argument, "mana" ) )
	cost = 1;

    else if ( !str_cmp(argument, "move" ) )
        cost = 1;
 
    else
    {
	strcpy( buf, "You can train:" );
	if ( ch->perm_stat[STAT_STR] < get_max_train(ch,STAT_STR)) 
	    strcat( buf, " str" );
	if ( ch->perm_stat[STAT_INT] < get_max_train(ch,STAT_INT))  
	    strcat( buf, " int" );
	if ( ch->perm_stat[STAT_WIS] < get_max_train(ch,STAT_WIS)) 
	    strcat( buf, " wis" );
	if ( ch->perm_stat[STAT_DEX] < get_max_train(ch,STAT_DEX))  
	    strcat( buf, " dex" );
	if ( ch->perm_stat[STAT_CON] < get_max_train(ch,STAT_CON))  
	    strcat( buf, " con" );
	strcat( buf, " hp mana move");

	if ( buf[strlen(buf)-1] != ':' )
	{
	    strcat( buf, ".\n\r" );
	    send_to_char( buf, ch );
	}
	else
	{
	    /*
	     * This message dedicated to Jordan ... you big stud!
	     */
	    act( "You have nothing left to train, you $T!",
		ch, NULL,
		ch->sex == SEX_MALE   ? "big stud" :
		ch->sex == SEX_FEMALE ? "hot babe" :
					"wild thing",
		TO_CHAR );
	}

	return;
    }

    if (!str_cmp("hp",argument))
    {
    	if ( cost > ch->train )
    	{
       	    send_to_char( "You don't have enough training sessions.\n\r", ch );
            return;
        }
 
	ch->train -= cost;
        ch->pcdata->perm_hit += 10;
        ch->max_hit += 10;
        ch->hit +=10;
        act( "Your durability increases!",ch,NULL,NULL,TO_CHAR);
        act( "$n's durability increases!",ch,NULL,NULL,TO_ROOM);
        return;
    }
 
    if (!str_cmp("mana",argument))
    {
        if ( cost > ch->train )
        {
            send_to_char( "You don't have enough training sessions.\n\r", ch );
            return;
        }

	ch->train -= cost;
        ch->pcdata->perm_mana += 10;
        ch->max_mana += 10;
        ch->mana += 10;
        act( "Your power increases!",ch,NULL,NULL,TO_CHAR);
        act( "$n's power increases!",ch,NULL,NULL,TO_ROOM);
        return;
    }

    if (!str_cmp("move",argument))
    {
        if ( cost > ch->train )
        {
            send_to_char( "You don't have enough training sessions.\n\r", ch );
            return;
        }
 
        ch->train -= cost;
        ch->pcdata->perm_move += 10;
        ch->max_move += 10;
        ch->move += 10;
        act( "Your endurance increases!",ch,NULL,NULL,TO_CHAR);
        act( "$n's endurance increases!",ch,NULL,NULL,TO_ROOM);
        return;
    }

    if ( ch->perm_stat[stat]  >= get_max_train(ch,stat) )
    {
	act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
	return;
    }

    if ( cost > ch->train )
    {
	send_to_char( "You don't have enough training sessions.\n\r", ch );
	return;
    }

    ch->train		-= cost;
  
    ch->perm_stat[stat]		+= 1;
    act( "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
    act( "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
    return;
}

EVENT_FUN(event_search_room);
EVENT_FUN(interrupt_search_room);

void do_search(CHAR_DATA *ch, char *argument)
{
    if(!can_see_room(ch,ch->in_room)
        || IS_AFFECTED(ch,AFF_BLIND))
    {
        send_to_char("You're having enough trouble finding your way "
                     "around the room!\n\r", ch);
        return;
    }

    if(IS_AFFECTED(ch, AFF_SNEAK))
    {
        send_to_char("You stop sneaking around.\n\r",ch);
        REMOVE_BIT(ch->affected_by, AFF_SNEAK);
    }

    if(argument[0] == '\0' || !str_cmp(argument,"room"))
    {
        EVENT *e;
        e = new_event();

        e->when = current_pulse + 2 * PPS;
        e->actor.ch = ch;
        e->actor_type = EVENT_ACTOR_CHAR;
        e->call = event_search_room;
        e->interrupt_func = interrupt_search_room;

        e->flags = EVENT_REPEAT | EVENT_PLAYER_ACTION
               | EVENT_INTERRUPT_ON_ACTION;

        event_add_char(e,ch);

        act_new_2( "You begin a cursory search of the room.", ch, NULL,
            NULL, TO_CHAR, POS_STANDING, 1, 0);

	act_new_2( "$n begins to glance intently around the room.", ch, 
            NULL, NULL, TO_ROOM, POS_RESTING, 1, ACTION_MUST_SEE );

        return;
    }

    /* in objects? -- add this later*/
}

void event_search_room(EVENT *e)
{
    CHAR_DATA *ch = e->actor.ch;
    CHAR_DATA *vch;
    char *char_msg = NULL;
    char *room_msg = NULL;

    e->when = current_pulse + number_range(3 * PPS, 3.5 * PPS);

    if(e->executions == 6)
    {
        act_new_2( "You stop searching.", ch, NULL,
            NULL, TO_CHAR, POS_STANDING, 1, 0);

        act_new_2( "$n stops hunting around the room.", ch, 
            NULL, NULL, TO_ROOM, POS_RESTING, 1, ACTION_MUST_SEE );

        SET_BIT(e->flags, EVENT_DELETED);
        return;
    }

    switch(e->executions)    
    {
        case 5:
            char_msg = "You begin to wrap up your search.";
            room_msg = "$n slowly hunts around the room.";
            break;
        case 1:
            char_msg = "You start walking around, searching the room.";
            room_msg = "$n walks around, examining everything carefully.";
            break;

        case 2: case 4:
            char_msg = "You continue to search the room.";
            room_msg = "$n walks around, examining everything carefully.";
            break;
    }

    if(char_msg)
        act_new_2(char_msg, ch, NULL,
            NULL, TO_CHAR, POS_STANDING, 1, 0);

    if(room_msg)
        act_new_2( room_msg, ch, 
            NULL, NULL, TO_ROOM, POS_RESTING, 1, ACTION_MUST_SEE );

    /* okay, now search */
    if(number_bits(1))
    { /* player */
        for(vch = ch->in_room->people; vch; vch = vch->next_in_room)
        {
            if(vch == ch) continue;
            if(vch->ghost_level > ch->level) continue;
            if(vch->invis_level > ch->level) continue;
            if(IS_SHIELDED(vch, SHD_INVISIBLE)) continue;

            if(IS_AFFECTED(vch, AFF_HIDE))
            {
                if(number_percent() < get_skill(vch,gsn_hide)/2)
                    continue;
                if(number_percent() > 30 + 5 *
                    (get_skill(vch,gsn_hide) - get_skill(ch,gsn_search)))
                { /* ah hah, found ya */
                    affect_strip(ch, gsn_sneak);
                    affect_strip(ch, gsn_hide);

                    act_new_2("You find $N cowering in hiding!",
                        ch, NULL, vch, TO_CHAR, POS_STANDING, 1, 0);
                    act_new_2("$n finds your hiding spot!",
                        ch, NULL, vch, TO_VICT, POS_RESTING, 1, 0);
                    act_new_2("$n yanks $N out of hiding!",
                        ch, NULL, vch, TO_NOTVICT, POS_RESTING, 1, ACTION_MUST_SEE);

                    SET_BIT(e->flags, EVENT_DELETED);
                    return;
                }
            }
        }
    }

    /* prepare for next */
    event_add(e);
    return;
}

void interrupt_search_room(EVENT *e)
{
    send_to_char("You stop searching.\n\r\n\r",e->actor.ch);

    act_new_2( "$n stops hunting around the room.", e->actor.ch, 
        NULL, NULL, TO_ROOM, POS_RESTING, 1, ACTION_MUST_SEE );
}

void do_push(CHAR_DATA *ch, char *argument)
{
    ;
    return;
}

void do_run( CHAR_DATA *ch, char * argument) {
    char arg1[MSL], arg2[MSL];
    int value, door = 0, i;

    argument = one_argument (argument, arg1);
    strcpy (arg2, argument);

    if (!strstr(arg1, "north") && !strstr(arg1, "south") && !strstr(arg1, "east")
            && !strstr(arg1, "west") && !strstr(arg1, "n") && !strstr(arg1, "s")
            && !strstr(arg1, "e") && !strstr(arg1, "w")) {
        send_to_char("run <direction> <amount>\n\r", ch);
        return;
    }

    value = atoi(arg2);

    if (value < 1 || value > 20) {
        send_to_char("You can run up to twenty and no less then one.", ch);
        return;
    }

    if (!str_cmp (arg1, "n") || !str_cmp (arg1, "north"))
        door = 0;
    else if (!str_cmp (arg1, "e") || !str_cmp (arg1, "east"))
        door = 1;
    else if (!str_cmp (arg1, "s") || !str_cmp (arg1, "south"))
        door = 2;
    else if (!str_cmp (arg1, "w") || !str_cmp (arg1, "west"))
        door = 3;
    else if (!str_cmp (arg1, "u") || !str_cmp (arg1, "up"))
        door = 4;
    else if (!str_cmp (arg1, "d") || !str_cmp (arg1, "down"))
        door = 5;

    for (i = 1; i <= value; ++i)
        if (i != value)
            move_char( ch, door, FALSE, TRUE);
        else
            move_char( ch, door, FALSE, FALSE);
}

