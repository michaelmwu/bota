
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
#include "music.h"
#include "recycle.h"
#include "event.h"

/* command procedures needed */
DECLARE_DO_FUN(do_quit		);
DECLARE_DO_FUN(do_grats		);
DECLARE_DO_FUN(do_open		);
DECLARE_DO_FUN(do_wake		);
DECLARE_DO_FUN(do_sleep		);

/*
 * Local functions.
 */
int	hit_gain	args( ( CHAR_DATA *ch, bool micro ) );
int	mana_gain	args( ( CHAR_DATA *ch ) );
int	move_gain	args( ( CHAR_DATA *ch ) );
void	mobile_update	args( ( void ) );
void	weather_update	args( ( void ) );
void	char_update	args( ( void ) );
void	obj_update	args( ( void ) );
void	aggr_update	args( ( void ) );

void	quest_update	args(( void ));
void	html_who	args(( void ));
void	end_arena	args(( bool normal ));
void	generate_items	args(( void ));
void	generate_mobs	args(( void ));

extern bool microheal_test;

/* used for saving */

int	save_number = 0;

RANDOM_ITEM random_items[MAX_RANDOM_ITEMS] =
{
	{3386,2},
	{1236,1}
};

RANDOM_MOB random_mobs[MAX_RANDOM_MOBS] =
{
	{19900,	1,	{19901,19902,19903,19905,19917,19918} },
	{1203,	1,	{1209} },
	{1204,	1,	{1237} },
	{1205,	1,	{1218} }
};

/*
 * Advancement stuff.
 */
void advance_level( CHAR_DATA *ch, bool quiet )
{
    char buf[MAX_STRING_LENGTH];
    int loc, i;
    int add_hp;
    int add_mana;
    int add_move;
    int add_prac;
    OBJ_DATA *obj;

    ch->pcdata->last_level = 
	( ch->played + (int) (current_time - ch->logon) ) / 3600;

    if (!IS_SET(ch->comm2, COMM_TITLELOCK))
    {
      sprintf( buf, "the %s",
	title_table [ch->class] [ch->level] [ch->sex == SEX_FEMALE ? 1 : 0] );
      set_title( ch, buf );
    }

    add_hp	= con_app[get_curr_stat(ch,STAT_CON)].hitp + number_range(
		    class_table[ch->class].hp_min,
		    class_table[ch->class].hp_max );
    add_mana 	= number_range(4,(5*get_curr_stat(ch,STAT_INT)
				  + get_curr_stat(ch,STAT_WIS))/2);

    add_mana *= class_table[ch->class].fMana;

    if (!class_table[ch->class].fMana)
	add_mana /= 8;
    add_move	= number_range( 1, (get_curr_stat(ch,STAT_CON)
				  + get_curr_stat(ch,STAT_DEX))/5 );
    add_prac	= wis_app[get_curr_stat(ch,STAT_WIS)].practice;

    add_hp = add_hp * 63/40;
    add_mana = add_mana * 63/40;
    add_move = add_move * 9/10;

    add_hp	= UMAX(  4, add_hp   );
    add_mana	= UMAX(  4, add_mana );
    add_move	= UMAX(  8, add_move );

    ch->max_hit 	+= add_hp;
    ch->max_mana	+= add_mana;
    ch->max_move	+= add_move;
    ch->practice	+= add_prac;
    ch->train		+= 1;
    if(number_percent() < get_curr_stat(ch,STAT_WIS))
	ch->train	+= 1;

    ch->hit		= ch->max_hit;
    ch->mana		= ch->max_mana;
    ch->move		= ch->max_move;

    ch->pcdata->perm_hit	+= add_hp;
    ch->pcdata->perm_mana	+= add_mana;
    ch->pcdata->perm_move	+= add_move;

    sprintf( buf,
	"Your gain is: %d/%d hp, %d/%d m, %d/%d mv %d/%d prac.\n\r",
	add_hp,		ch->max_hit,
	add_mana,	ch->max_mana,
	add_move,	ch->max_move,
	add_prac,	ch->practice
	);
    if(!quiet)
        send_to_char( buf, ch );

    if(ch->level == 8 && !quiet)
    {
        obj = create_object( get_obj_index( 14516 ), 0 );
        obj->level = 8;

        if( ch->carry_number + get_obj_number( obj ) < can_carry_n( ch ) )
        {
            obj_to_char( obj, ch );
            act( "A sprite appears and hands $n $p as a gift.", ch, obj,
                  NULL, TO_ROOM );

            act( "A sprite appears and gives you $p as a gift.", ch, obj, NULL, TO_CHAR );
        }
        else
        {
            obj_to_room( obj, ch->in_room );
            act( "A sprite appears before $n and drops $p.", ch, obj,
                  NULL, TO_ROOM );

            act( "A sprite appears before you and drops $p.",
                  ch, obj, NULL, TO_CHAR );

        }

        obj = NULL;
    }


    for (loc = 0; loc < MAX_WEAR; loc++)
    {
	obj = get_eq_char(ch,loc);
	if (obj == NULL)
	    continue;
	if (obj->clan)
	{
	    for (i = 0; i < 4; i++)
	    {
		ch->armor[i] += apply_ac( ch->level-1, obj, loc, i );
		ch->armor[i] -= apply_ac( ch->level, obj, loc, i );
	    }
	}
    }
    return;
}   

void advance_level_quiet( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    int loc, i;
    int add_hp;
    int add_mana;
    int add_move;
    int add_prac;
    OBJ_DATA *obj;

    ch->pcdata->last_level = 
	( ch->played + (int) (current_time - ch->logon) ) / 3600;

    if(!IS_SET(ch->comm2, COMM_TITLELOCK))
    {
      sprintf( buf, "the %s",
	title_table [ch->class] [ch->level] [ch->sex == SEX_FEMALE ? 1 : 0] );
      set_title( ch, buf );
    }

    add_hp	= con_app[get_curr_stat(ch,STAT_CON)].hitp + number_range(
		    class_table[ch->class].hp_min,
		    class_table[ch->class].hp_max );
    add_mana 	= number_range(4,(4*get_curr_stat(ch,STAT_INT)
				  + get_curr_stat(ch,STAT_WIS))/2);
    if (!class_table[ch->class].fMana)
	add_mana /= 2;
    add_move	= number_range( 1, (get_curr_stat(ch,STAT_CON)
				  + get_curr_stat(ch,STAT_DEX))/5 );
    add_prac	= wis_app[get_curr_stat(ch,STAT_WIS)].practice;

    add_hp = add_hp * 9/10;
    add_hp = add_hp * 7/4;
    add_mana = add_mana * 9/10;
    add_mana = add_mana * 7/4;
    add_move = add_move * 9/10;
    add_move = add_move * 2;

    add_hp	= UMAX(  4, add_hp   );
    add_mana	= UMAX(  4, add_mana );
    add_move	= UMAX(  12, add_move );

    ch->max_hit 	+= add_hp;
    ch->max_mana	+= add_mana;
    ch->max_move	+= add_move;
    ch->practice	+= add_prac;
    ch->train		+= 1;
    if(number_percent() < get_curr_stat(ch,STAT_WIS))
        ch->train       += 1;

    ch->pcdata->perm_hit	+= add_hp;
    ch->pcdata->perm_mana	+= add_mana;
    ch->pcdata->perm_move	+= add_move;

    for (loc = 0; loc < MAX_WEAR; loc++)
    {
	obj = get_eq_char(ch,loc);
	if (obj == NULL)
	    continue;
	if (obj->clan)
	{
	    for (i = 0; i < 4; i++)
	    {
		ch->armor[i] += apply_ac( ch->level-1, obj, loc, i );
		ch->armor[i] -= apply_ac( ch->level, obj, loc, i );
	    }
	}
    }
    return;
}   


void gain_exp( CHAR_DATA *ch, int gain )
{
    char buf[MAX_STRING_LENGTH];
    int lastlevel;

    if ( IS_NPC(ch) || ch->level >= LEVEL_HERO )
	return;

    lastlevel = ch->level;

    if(ch->pcdata->points < 0) ch->pcdata->points = 40;

    ch->exp = UMAX( exp_per_level(ch,ch->pcdata->points), ch->exp + gain );
    while ( ch->level < LEVEL_HERO && ch->exp >= 
	exp_per_level(ch,ch->pcdata->points) * (ch->level+1) )
    {
        if(ch->level - lastlevel > 10)
        {
            bug_f("%s went up more than 10 levels!", ch->name);
        }
	send_to_char( "{RYou raise a level!!{x  ", ch );
	ch->level += 1;
	sprintf(buf,"$N has attained level %d!",ch->level);
	wiznet(buf,ch,NULL,WIZ_LEVELS,0,0);
	advance_level( ch, FALSE );
	save_char_obj(ch);
    }

    if(ch->level - lastlevel == 1)
    {
        if(ch->level % 10 == 0)
            sprintf(buf,
            "{B@@@ {RCongratz to {G%s{R on your {M%dth{x level! {B@@@{x",
            ch->name,ch->level);
        if(ch->level == 101)
            sprintf(buf,
            "{B{z@@@ {RCongratz on becoming a {WHERO{R, {G%s{R! {B{z@@@{x",
            ch->name);
        else
            sprintf(buf,
            "{RCongratz to {G%s{R on level {M%d{W!{x",
            ch->name,ch->level);
        do_grats(NULL,buf);

    }
    else if(ch->level - lastlevel > 1)
    {
        if(ch->level % 10 == 0)
            sprintf(buf,
            "{B@@@ {RCongratz, {G%s{R on your {M%dth{x level! {B@@@{x",
            ch->name,ch->level);
        if(ch->level == 101)
            sprintf(buf,
            "{B{z@@@{x {RCongratz on becoming a {WHERO{R, {G%s{R! {B{z@@@{x",
            ch->name);
        else
            sprintf(buf,
     "{B@@@ {RSpecial congratulations %d{Yx{R for {G%s{R on level {M%d{x! {B@@@{x",
                ch->level - lastlevel,ch->name,ch->level);
        do_grats(NULL,buf);

    }

    return;
}



/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch, bool micro )
{
    int gain;
    int number;

    if (ch->in_room == NULL)
	return 0;

    if ( IS_NPC(ch) )
    {
	gain =  5 + ch->level;
 	if (IS_AFFECTED(ch,AFF_REGENERATION))
	    gain *= 2;

	switch(ch->position)
	{
	    default : 		gain /= 2;			break;
	    case POS_SLEEPING: 	gain = 3 * gain/2;		break;
	    case POS_RESTING:  					break;
	    case POS_FIGHTING:	gain /= 3;		 	break;
 	}

	
    }
    else
    {
	gain = UMAX(3,get_curr_stat(ch,STAT_CON) - 3 + ch->level/2); 
	gain += class_table[ch->class].hp_max - 10;
 	number = number_percent();
	if (number < get_skill(ch,gsn_fast_healing))
	{
	    gain += number * gain / 100;
	    if (ch->hit < ch->max_hit)
		check_improve(ch,gsn_fast_healing,TRUE,8);
	}

	switch ( ch->position )
	{
	    default:	   	gain /= 4;			break;
	    case POS_SLEEPING: 					break;
	    case POS_RESTING:  	gain /= 2;			break;
	    case POS_FIGHTING: 	gain /= 6;			break;
	}

	if ( ch->pcdata->condition[COND_HUNGER]   == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;

    }

    gain = gain * ch->in_room->heal_rate / 90;
    
    if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
	gain = gain * ch->on->value[3] / 90;

    if ( IS_AFFECTED(ch, AFF_POISON) )
	gain /= 4;

    if (IS_AFFECTED(ch, AFF_PLAGUE))
	gain /= 8;

    if (IS_AFFECTED(ch,AFF_HASTE) || IS_AFFECTED(ch,AFF_SLOW))
	gain /=2 ;

    return UMIN(gain, ch->max_hit - ch->hit);
}



int mana_gain( CHAR_DATA *ch )
{
    int gain;
    int number;

    if (ch->in_room == NULL)
	return 0;

    if ( IS_NPC(ch) )
    {
	gain = 5 + ch->level;
	switch (ch->position)
	{
	    default:		gain /= 2;		break;
	    case POS_SLEEPING:	gain = 3 * gain/2;	break;
   	    case POS_RESTING:				break;
	    case POS_FIGHTING:	gain /= 3;		break;
    	}
    }
    else
    {
	gain = ((get_curr_stat(ch,STAT_WIS) 
	      + get_curr_stat(ch,STAT_INT) + ch->level) / 3) * 2;
	number = number_percent();
	if (number < get_skill(ch,gsn_meditation))
	{
	    gain += number * gain / 100;
	    if (ch->mana < ch->max_mana)
	        check_improve(ch,gsn_meditation,TRUE,8);
	}
	if (!class_table[ch->class].fMana)
	    gain /= 2;

	switch ( ch->position )
	{
	    default:		gain /= 4;			break;
	    case POS_SLEEPING: 					break;
	    case POS_RESTING:	gain *= 0.8;			break;
	    case POS_FIGHTING:	gain /= 6;			break;
	}

	if ( ch->pcdata->condition[COND_HUNGER]   == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;

    }

    gain = gain * ch->in_room->mana_rate / 60;

    if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
	gain = gain * ch->on->value[4] / 90;

    if ( IS_AFFECTED( ch, AFF_POISON ) )
	gain /= 4;

    if (IS_AFFECTED(ch, AFF_PLAGUE))
        gain /= 8;

    if (IS_AFFECTED(ch,AFF_HASTE) || IS_AFFECTED(ch,AFF_SLOW))
        gain /=2 ;

    return UMIN(gain, ch->max_mana - ch->mana);
}



int move_gain( CHAR_DATA *ch )
{
    int gain;

    if (ch->in_room == NULL)
	return 0;

    if ( IS_NPC(ch) )
    {
	gain = ch->level;
    }
    else
    {
	gain = UMAX( 15, ch->level );

	switch ( ch->position )
	{
	case POS_SLEEPING: gain += get_curr_stat(ch,STAT_DEX) *1.5;	break;
	case POS_RESTING:  gain += get_curr_stat(ch,STAT_DEX) / 2;	break;
	}

	if ( ch->pcdata->condition[COND_HUNGER]   == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;
    }

    gain = gain * ch->in_room->heal_rate/100;

    if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
	gain = gain * ch->on->value[3] / 90;

    if ( IS_AFFECTED(ch, AFF_POISON) )
	gain /= 4;

    if (IS_AFFECTED(ch, AFF_PLAGUE))
        gain /= 8;

    if (IS_AFFECTED(ch,AFF_HASTE) || IS_AFFECTED(ch,AFF_SLOW))
        gain /=2 ;

    return UMIN(gain, ch->max_move - ch->move);
}


static const char *hunger_messages[] =
{
"You begin to wonder how long it's been since you ate.\n\r",
"You feel uncomfortable as your stomach growls.\n\r",
"Nourishment is essential -- and you haven't eaten yet.\n\r",
"You snap into and out of a daydream about food.\n\r",
"The hunger pangs, the hunger pains!\n\r",
"You are hungry.\n\r"
};

static const char *thirst_messages[] =
{
"Your throat feels so parched right now.\n\r",
"A drink would be nice, would it not?\n\r",
"Dehydration is dangerous ... drink something!\n\r",
"You are thirsty.\n\r"
};

static const char *fatigue_messages[] =
{
"Your limbs feel sore from moving around so much.\n\r",
"You feel out of breath.\n\r",
"You are tired.\n\r"
};

void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
    int condition;

    if ( value == 0 || IS_NPC(ch) || ch->level >= LEVEL_IMMORTAL)
	return;

    condition				= ch->pcdata->condition[iCond];
    if (condition == -1)
        return;
    ch->pcdata->condition[iCond]	= URANGE( 0, condition + value, 48 );

    if ( ch->pcdata->condition[iCond] == 0 )
    {
	switch ( iCond )
	{
	case COND_HUNGER:
	    send_to_char( RANDOM_STRING(hunger_messages),  ch );
	    break;

	case COND_THIRST:
	    send_to_char( RANDOM_STRING(thirst_messages), ch );
	    break;

	case COND_FATIGUE:
	    send_to_char( RANDOM_STRING(fatigue_messages), ch );
	    break;

	case COND_DRUNK:
	    if ( condition != 0 )
		send_to_char( "You are sober.\n\r", ch );
	    break;
	}
    }

    return;
}



/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    EXIT_DATA *pexit;
    int door;

    /* Examine all mobs. */
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	ch_next = ch->next;

	if ( !IS_NPC(ch) || ch->in_room == NULL || IS_AFFECTED(ch,AFF_CHARM))
	    continue;

	if (ch->in_room->area->empty && !IS_SET(ch->act,ACT_UPDATE_ALWAYS))
	    continue;

	/* Examine call for special procedure */
	if ( ch->spec_fun != 0 )
	{
	    if ( (*ch->spec_fun) ( ch ) )
		continue;
	}

	if (ch->pIndexData->pShop != NULL) /* give him some platinum */
	    if ((ch->platinum * 100 + ch->gold) < ch->pIndexData->wealth)
	    {
		ch->platinum += ch->pIndexData->wealth * number_range(1,20)/5000000;
		ch->gold += ch->pIndexData->wealth * number_range(1,20)/50000;
	    }
	 
	/*
	 * Check triggers only if mobile still in default position
	 */
	if ( ch->position == ch->pIndexData->default_pos )
	{
            if(ch->pyobject)
                script_call_noarg(ch->pyobject, "on_think");

	    /* Delay */
	    if ( HAS_TRIGGER( ch, TRIG_DELAY) 
	    &&   ch->mprog_delay > 0 )
	    {
		if ( --ch->mprog_delay <= 0 )
		{
		    mp_percent_trigger( ch, NULL, NULL, NULL, TRIG_DELAY );
		    continue;
		}
	    } 
	    if ( HAS_TRIGGER( ch, TRIG_RANDOM) )
	    {
		if( mp_percent_trigger( ch, NULL, NULL, NULL, TRIG_RANDOM ) )
		continue;
	    }
	}

	/* That's all for sleeping / busy monster, and empty zones */
	if ( ch->position != POS_STANDING )
	    continue;

	/* Scavenge */
	if ( IS_SET(ch->act, ACT_SCAVENGER)
	&&   ch->in_room->contents != NULL
	&&   number_bits( 6 ) == 0 )
	{
	    OBJ_DATA *obj;
	    OBJ_DATA *obj_best;
	    int max;

	    max         = 1;
	    obj_best    = 0;
	    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	    {
		if ( CAN_WEAR(obj, ITEM_TAKE) && can_loot(ch, obj)
		     && obj->cost > max  && obj->cost > 0
		     && !CAN_WEAR(obj, ITEM_NOTAKE) )
		{
                    if(IS_SET(ch->in_room->room_flags, ROOM_SAFE)
                       && obj->cost > 1000)
                        continue;

		    obj_best    = obj;
		    max         = obj->cost;
		}
	    }

	    if ( obj_best )
	    {
		obj_from_room( obj_best );
		obj_to_char( obj_best, ch );
		act( "$n gets $p.", ch, obj_best, NULL, TO_ROOM );
	    }
	}

        if(!ch->path)
            bug_f("mobile_update: ch->path does not exist!!!");

        if(ch->path[0] != '\0')
        {

            int door = 0;
            char buf[MAX_STRING_LENGTH];

            if(!ch->path_pos) ch->path_pos = ch->path;
            
            switch(*(ch->path_pos))
            {
              case 'n': door = DIR_NORTH; break;
              case 's': door = DIR_SOUTH; break;
              case 'e': door = DIR_EAST;  break;
              case 'w': door = DIR_WEST;  break;
              case 'u': door = DIR_UP;  break;
              case 'd': door = DIR_DOWN;  break;
              default: bug_f("mobile_update: Bad path");
                       continue;
            }

            if( (pexit = ch->in_room->exit[door] ) == NULL
                    || pexit->u1.to_room == NULL)
            {
                bug_f("mobile_update: path to nonexistant room");
                continue;
            }

            if( IS_SET(pexit->exit_info, EX_CLOSED) )
            {
                sprintf(buf,"%s", dir_name[door]);
                do_open(ch, buf);
            }

            if( IS_SET(pexit->exit_info, EX_CLOSED) )
            {
                free_string(ch->path);
                ch->path = &str_empty[0];
                ch->path_pos = NULL;
                continue;
            }

            move_char(ch, door, FALSE, FALSE);

            ch->path_pos++;

            if(*(ch->path_pos) == '\0')
            {
                free_string(ch->path);
                ch->path = &str_empty[0];
                ch->path_pos = NULL;

                script_call_noarg(ch->pyobject, "on_endOfPath");
            }

            continue;
        }

	/* Wander */
	if ( !IS_SET(ch->act, ACT_SENTINEL) 
	&& number_bits(3) == 0
	&& ( door = number_bits( 5 ) ) <= 5
	&& ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->u1.to_room != NULL
	&&   !IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB)
	&& ( !IS_SET(ch->act, ACT_STAY_AREA)
	||   pexit->u1.to_room->area == ch->in_room->area ) 
	&& ( !IS_SET(ch->act, ACT_OUTDOORS)
	||   !IS_SET(pexit->u1.to_room->room_flags,ROOM_INDOORS)) 
	&& ( !IS_SET(ch->act, ACT_INDOORS)
	||   IS_SET(pexit->u1.to_room->room_flags,ROOM_INDOORS)))
	{
	    move_char( ch, door, FALSE, FALSE );
	}

    }

    return;
}



/*
 * Update the weather.
 */
void weather_update( void )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int diff;

    buf[0] = '\0';

    switch ( ++time_info.hour )
    {
    case  5:
	weather_info.sunlight = SUN_LIGHT;
	strcat( buf, "The day has begun.\n\r" );
	break;

    case  6:
	weather_info.sunlight = SUN_RISE;
	strcat( buf, "The sun rises in the east.\n\r" );
	break;

    case 19:
	weather_info.sunlight = SUN_SET;
	strcat( buf, "The sun slowly disappears in the west.\n\r" );
	break;

    case 20:
	weather_info.sunlight = SUN_DARK;
	strcat( buf, "The night has begun.\n\r" );
	break;

    case 24:
	time_info.hour = 0;
	time_info.day++;
	break;
    }

    if ( time_info.day   >= 35 )
    {
	time_info.day = 0;
	time_info.month++;
    }

    if ( time_info.month >= 17 )
    {
	time_info.month = 0;
	time_info.year++;
    }

    /*
     * Weather change.
     */
    if ( time_info.month >= 9 && time_info.month <= 16 )
	diff = weather_info.mmhg >  985 ? -2 : 2;
    else
	diff = weather_info.mmhg > 1015 ? -2 : 2;

    weather_info.change   += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
    weather_info.change    = UMAX(weather_info.change, -12);
    weather_info.change    = UMIN(weather_info.change,  12);

    weather_info.mmhg += weather_info.change;
    weather_info.mmhg  = UMAX(weather_info.mmhg,  960);
    weather_info.mmhg  = UMIN(weather_info.mmhg, 1040);

    switch ( weather_info.sky )
    {
    default: 
	bug( "Weather_update: bad sky %d.", weather_info.sky );
	weather_info.sky = SKY_CLOUDLESS;
	break;

    case SKY_CLOUDLESS:
	if ( weather_info.mmhg <  990
	|| ( weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The sky is getting cloudy.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_CLOUDY:
	if ( weather_info.mmhg <  970
	|| ( weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "Rain begins to pour down.\n\r" );
	    weather_info.sky = SKY_RAINING;
	}

	if ( weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "The clouds disappear.\n\r" );
	    weather_info.sky = SKY_CLOUDLESS;
	}
	break;

    case SKY_RAINING:
	if ( weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "{YLightning{x flashes in the sky.\n\r" );
	    weather_info.sky = SKY_LIGHTNING;
	}

	if ( weather_info.mmhg > 1030
	|| ( weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The rain dies down.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_LIGHTNING:
	if ( weather_info.mmhg > 1010
	|| ( weather_info.mmhg >  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The lightning has stopped.\n\r" );
	    weather_info.sky = SKY_RAINING;
	    break;
	}
	break;
    }

    if ( buf[0] != '\0' )
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&   IS_OUTSIDE(d->character)
	    &&   IS_AWAKE(d->character) )
		send_to_char( buf, d->character );
	}
    }

    return;
}



/*
 * Update all chars, including mobs.
*/
void char_update( void )
{   
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *ch_quit;
    int dream;
    OBJ_DATA *munchie;

    ch_quit	= NULL;

    /* update save counter */
    save_number++;

    if (save_number > 4)
	save_number = 0;

    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;

	ch_next = ch->next;

        if ( ch->timer > ((!ch->desc) ? 3 : 8) && !IS_IMMORTAL(ch) )
            ch_quit = ch;

        if ( !IS_NPC(ch) && ch->pcdata->auto_away > 0
             && ch->timer == ch->pcdata->auto_away
             && !IS_SET(ch->comm, COMM_AFK))
        {
            send_to_char("{WAutomatically going AFK ...{x\n\r",ch);
            SET_BIT(ch->comm, COMM_AFK);
        }

	if ( IS_NPC(ch) && ch->hastimer )
	{
	    if ( ++ch->timer > 5 )
	    {
            	act("$n decays into dust.",ch,NULL,NULL,TO_ROOM);
            	extract_char(ch,TRUE);
	    }
	}

        if ( ch->desc && ch->desc->connected != CON_PLAYING )
            continue;

        if (!IS_NPC(ch)) ch->pcdata->moves_this_tick = 0;

/* add in char_update */

    dream = number_range(1,460);

if (!IS_NPC(ch) && ch->position == POS_SLEEPING)
{

if (dream <=186)
{
send_to_char("zzzzzzzzzzzzzzzzzzzzzzzzz.\n\r",ch);
}

else if (dream <= 188)
{
send_to_char("Frantically looking around, your arms hit the bars of a cold iron cage..\n\r",ch);
send_to_char("Panicking, you rattle the bars hoping to escape.. finding the door\n\r",ch);
send_to_char("unlocked you quickly leap from the cage.. when suddenly a seemingly\n\r",ch);
send_to_char("giant roar echoes in your ears.. Looking above you see a common yet\n\r",ch);
send_to_char("titanic tabby cat licking its chops, waiting for a delicious meal. Before\n\r",ch);
send_to_char("you can react it pounces upon you...\n\r",ch);
}

else if (dream <= 189)
{
send_to_char("Sitting before a vast table, you and a hoard of guests begin to eat..\n\r",ch);
send_to_char("Course after fine course flow across the table... and toast after toast\n\r",ch);
send_to_char("of fine wine flows throughout the hall!  Minstrels begin to sing songs\n\r",ch);
send_to_char("of valient bravery and lost love.. as people get up from their host's   \n\r",ch);
send_to_char("table to dance...\n\r",ch);
}

else if (dream <= 190)
{
send_to_char("Moving your hands through the air, you summon powerful magic!\n\r",ch);
send_to_char("Creating and destoying worlds, setting ablaze all of time!\n\r",ch);
send_to_char("Cackling with your god-like power, you crush whole races between\n\r",ch);
send_to_char("your fingertips.. and create new ones with a single thought,\n\r",ch);
send_to_char("to worship your awe-inspiring power..\n\r",ch);
}

else if (dream <= 191)
{
send_to_char("At the head of a vast and powerful army, you lead your troops toward\n\r",ch);
send_to_char("a secluded village.. Unsuspecting and innocent, the townspeople see your\n\r",ch);
send_to_char("approach and begin to flee in terror! You give orders to your archers\n\r",ch);
send_to_char("and pikemen rush toward them! Flaming arrows are sent into the sky, fires\n\r",ch);
send_to_char("begin to flame around the defenseless village.. slaughter and blood fill\n\r",ch);
send_to_char("the scene.. as innocent lives scream for mercy..\n\r",ch);
}

else if (dream <= 192)
{
send_to_char("Soaring through the sky you fly, diving and careening through the\n\r",ch);
send_to_char("gusty wind.. Looking below you, the ground slowly dissapears, leaving\n\r",ch);
send_to_char("you flying through an empty and endless blue sky.. As the sun sets,\n\r",ch);
send_to_char("sending golden rays streaming acoss the air you glance at back and find\n\r",ch);
send_to_char("you have no wings! Gasping in terror you begin to fallll.....\n\r",ch);
}

else if (dream <= 193)
{
send_to_char("Feeling the cooling surf wash over your feet and the wet sand creep\n\r",ch);
send_to_char("its way between your toes, you smile briefly.  Looking across the\n\r",ch);
send_to_char("horizon you cannot tell where crystal blue ocean ends and sky begins.\n\r",ch);
send_to_char("Above you the rustling of palm trees are your only companion on this\n\r",ch);
send_to_char("lone beach...\n\r",ch);
}

else if (dream <= 194)
{
send_to_char("A pair of gleaming red eyes shine from the pitch black darkness,\n\r",ch);
send_to_char("hearing the creatures breath, you freeze.  Closer and closer it\n\r",ch);
send_to_char("creeps, scraping its claws against the dungeons' stone walls.\n\r",ch);
send_to_char("Panicking you begin to run through the darkness blindly. Out of\n\r",ch);
send_to_char("breath you stop and listen.. all is silent in the pitch blackness...\n\r",ch);
send_to_char("Suddenly you feel some icy claws bore into your flesh!\n\r",ch);
}

else if (dream <= 202)
{
send_to_char("Strange colors swirl around you, blinding you.\n\r",ch); 
act( "$n mumbles in $s sleep.", ch, NULL, NULL, TO_ROOM);
}

else if ( dream <= 204)
{
send_to_char("Horses thunder past you, bearing naked riders to \n\r",ch);
send_to_char("who knows where. The sound of horns echo in the distance.\n\r",ch);
act( "$n tosses restlessly in $s sleep.", ch, NULL, NULL, TO_ROOM);
}

else if (dream <= 206)
{
send_to_char("You awake in a soft, warm bed. As you bask in warm glow of firelight you\n\r",ch);
send_to_char("wonder how long it's been since last time you've had such comfort. The\n\r",ch);
send_to_char("firelight fades and turns from red-orange to blue to white to a\n\r",ch);
send_to_char("spectrum. The colors surround you. You hear soft music, filling your\n\r",ch);
send_to_char("ears and your soul. You are swept away on bittersweet melancholic\n\r",ch);
send_to_char("chords, your spirit lifted and your heart twisted.. You hear faint\n\r",ch);
send_to_char("laughter, a woman's voice, a child whimpering softly.. You begin to cry \n\r",ch); 
send_to_char("suddenly and are not sure why.\n\r",ch);
act( "$n sobs quietly in $s sleep for a moment then is still once again.", ch, NULL, NULL, TO_ROOM);
}

else if (dream <= 208)
{
send_to_char("You stand high on a mountain top, surrounded by azure skies and whitest\n\r",ch);
send_to_char("clouds There is a blinding flash of light, and the clouds part as does\n\r",ch);
send_to_char("the very sky as if it were a fabric that could be ripped. From this\n\r",ch);
send_to_char("opening fly hundreds of winged beings, majestic and terrible in\n\r",ch);
send_to_char("their beauty. They seem to emanate light and heat.. their bodies\n\r",ch);
send_to_char("are so bright that it hurts your eyes to look at them. You avert your\n\r",ch);
send_to_char("eyes, and when you dare to look up the beings are gone, the sky as it\n\r",ch);
send_to_char("had been..\n\r",ch);
send_to_char("You notice a single golden feather at your feet and pick it up\n\r",ch);
send_to_char("thoughtfully.\n\r",ch);
munchie = create_object( get_obj_index( 1291 ), 0 );
obj_to_char( munchie, ch );
act( "$n smiles slightly in $s sleep.", ch, NULL, NULL, TO_ROOM);
}

else if (dream <= 210)
{
send_to_char("You push through a crowd of people, repulsed by the smell of bodies, of\n\r",ch);
send_to_char("heat and of greed. A thousand booths stretch out along each side of the\n\r",ch);
send_to_char("street each selling a different ware or service. A particular booth\n\r",ch);
send_to_char("catches your attention, the one in which you catch a glimpse of white \n\r",ch);
send_to_char("and gold.. As you get closer you see a caged unicorn, pure white with a \n\r",ch);
send_to_char("gold horn. Next to the grubby man selling tickets to touch its horn it looks all the\n\r",ch);
send_to_char("more majestic. It turns its sad, pleading doelike eyes on you, and you can\n\r",ch);
send_to_char("hear its thoughts, its anguish, its utter loneliness and longing to be \n\r",ch); 
send_to_char("free..\n\r",ch);
}   

else if (dream <= 212)
{
send_to_char("You move but hardly notice the feeling of the floor....\n\r",ch);
send_to_char("You grab a 20 by 30 foot canvas and vainly attempt to set it up....\n\r",ch);
send_to_char("Three or four passerbyers stop to inspect your work....\n\r",ch);
send_to_char("Everything you needed is now available but you feel as if soon it \n\r",ch);
send_to_char("will be gone... The walls close in and a glowing presense alerts you.\n\r",ch);
send_to_char("You dig at the walls with your fingertips, trying to get through\n\r",ch);
send_to_char("The cinder block comes loose! You are free! You squeeze through the hole\n\r",ch);
send_to_char("And the next room...it is exactly the same....\n\r",ch);
}

else if (dream <= 214)
{
send_to_char("You are high above the earth...plummeting towards the ground.\n\r",ch);
send_to_char("You realize its a dream and think, OK I'll wake up in a minute.\n\r",ch);
send_to_char("As the ground draws near you begin to scream in terror of death.\n\r",ch);
send_to_char("You hit the ground with a sickening crunch and lay there until you \n\r",ch);
send_to_char("bleed to death.\n\r",ch);
act( "$n screams in $s sleep.", ch, NULL, NULL, TO_ROOM);
}

else if (dream <= 216)
{
send_to_char("the shadows around you swirl and gire into the form of a dark\n\r",ch);
send_to_char("steed...it whispers something in a strange tongue and melts\n\r",ch);
send_to_char("into the background.....\n\r",ch);
}

else if (dream <= 218)
{
send_to_char("Lights streak past you leaving long trails.\n\r",ch);
send_to_char("{RRed{0, {Bblue{0, {Yyellow{0, {Wwhite{0 trails.\n\r",ch);
send_to_char("A coldness unlike any you have ever felt before creeps through your \n\r",ch);
send_to_char("bones. The only sound you hear is the slow thud of your heart.\n\r",ch);
}

else if (dream <= 220)
{
send_to_char("You find yourself standing at your old bus stop *sigh*\n\r",ch);
send_to_char("A dreary morning only 1/3 into the school year\n\r",ch);
send_to_char("All of a sudden you are soaring high above the street!\n\r",ch);
send_to_char("You swoop down and topple over a couple trash cans!\n\r",ch);
send_to_char("All your fellow students turn green with envy\n\r",ch);
send_to_char("You begin to doubt your abilities! LOOK OUT!!\n\r",ch);
send_to_char("You are falling into a deep, black hole.....\n\r",ch);
}

else if (dream <= 222)
{
send_to_char("Ahhhhh you are in perhaps THE most comfortable bed ever.\n\r",ch);
send_to_char("The blankets arranged just so comfy and very warm...\n\r",ch);
send_to_char("and your arm hangs over the edge of the bed...Ahhhhhh.\n\r",ch);
send_to_char("then a hand comes from under the bed and GRABS your hand!\n\r",ch);
send_to_char("You wake up in a cold sweat!\n\r",ch);
send_to_char("Something STILL has your hand!\n\r",ch);
send_to_char("You wake up in a cold sweat...whew your hand is ok.\n\r",ch);
do_wake(ch, "");
}

else if (dream == 224)
{
send_to_char("A warm incredible feeling of happiness washes over you.\n\r",ch);
send_to_char("It feels like you are floating on a bed of air and a \n\r",ch);
send_to_char("thousand butterflies are fluttering over you gently cooling\n\r",ch);
send_to_char("you with the beating of their wings.  A far off harp\n\r",ch);
send_to_char("peacefully fills the air with soul-stirring music.  As you\n\r",ch);
send_to_char("open your eyes, you realize the sweet aroma filling the air comes\n\r",ch);
send_to_char("from the hundreds of flowers on and around you with still\n\r",ch);
send_to_char("more floating down from the sky above.  You close your eyes\n\r",ch);
send_to_char("hoping to retain these feelings forever.\n\r",ch);
act( "The air is filled with sweetness as hundreds of flower petals float", ch, NULL, NULL, TO_ROOM);
munchie = create_object( get_obj_index( 1294 ), 0 ); 
	munchie->timer=number_range(1,6);
        obj_to_room( munchie, ch->in_room );
act( "down from the sky to cover $n as $e lies sleeping.", ch, NULL, NULL, TO_ROOM);
}

else if (dream <= 226)
{
send_to_char("Right, left, onward.\n\r",ch);
send_to_char("The hall seems to stretch endlessly.\n\r",ch);
send_to_char("Is this the door? No.\n\r",ch);
send_to_char("Is this the door? No.\n\r",ch);
send_to_char("Onward...there seems to be no way out.\n\r",ch);
send_to_char("Ahead you see a figure crumbled on the ground.  As you turn him over\n\r",ch);
send_to_char("he crumbles into dust.  All that remains is a large black opal\n\r",ch);
send_to_char("strung on a gold chain.\n\r",ch);
munchie = create_object( get_obj_index( 1295 ), 0 ); 
	munchie->timer=number_range(1,6);
        obj_to_room( munchie, ch->in_room );
}

else if (dream <= 228)
{
send_to_char("You hear a slow, pulsing beat, rushing liquid.. A bird cries out in the \n\r",ch);
send_to_char("distance. You crane your neck to see what is around you, but \n\r",ch);
send_to_char("everything remains hazy.\n\r",ch);
send_to_char("You try to stand up, to walk towards the sound, but you cannot move\n\r",ch);
send_to_char("You legs do not respond, your arms are pinned to your chest..\n\r",ch);
send_to_char("You struggle and try to cry out, but no sound comes..\n\r",ch);
}

else if (dream <= 230)
{
send_to_char("A pair of ginger-haired twins, pale, thin and beautifully androgynous\n\r",ch);
send_to_char("stand before you, smiling cryptically. They look at you and say, in \n\r",ch);
send_to_char("unison,  How nice of you to visit! We've been waiting for you...\n\r",ch);
send_to_char("The taller one looks at his brother lovingly and says Oh but I have \n\r",ch);
send_to_char("been hungry... isn't this just lovely? \n\r",ch);
send_to_char("He opens his mouth in a wide grin and you realize.. he has fangs.\n\r",ch);
}

else if (dream <= 232)
{
send_to_char("Breathing heavily, you duck behind a stone wall.\n\r",ch);
send_to_char("You can hear them behind you, dozens of them.\n\r",ch);
send_to_char("It had been a long and glorius battle.  One by one\n\r",ch);
send_to_char("you comrades had fallen.  Now there was only you left and\n\r",ch);
send_to_char("your quiver is empty.  Breathing deeply, you calm yourself.\n\r",ch);
send_to_char("Sword left in the chest of a battle dragon, arrows expended,\n\r",ch);
send_to_char("all you have left is the small charm that an old woman gave you.\n\r",ch); 
munchie = create_object( get_obj_index( 1296 ), 0 ); 
        obj_to_char( munchie, ch );
}

else if (dream <= 234)
{
send_to_char("Tick Tick Tick Tick ...\n\r",ch);
send_to_char("The sound of the clock echos though the house.\n\r",ch);
send_to_char("Somewhere between sleep and wake you drift ...\n\r",ch);
send_to_char("Something big is going to happen, you can sense it.\n\r",ch); 
send_to_char("TICK TICK TICK TICK ...\n\r",ch);
send_to_char("Somehow the clock seems louder ... more ominous ...\n\r",ch);
send_to_char("You struggle to awake, your heart quickening.\n\r",ch); 
send_to_char("TICK TICK TICK TICK ...\n\r",ch);
}

else if (dream <= 236)
{
send_to_char("A flash of spinning silver\n\r",ch);
send_to_char("trapped in a whirling sphere of dimpled gleaming light\n\r",ch);
send_to_char("Friction a fiction, the body slides without feeling, presses\n\r",ch);
send_to_char("without resistance\n\r",ch);
send_to_char("splayed like a starfish scrabbling for purchase\n\r",ch);
send_to_char("Inevitably funneled into a chrome trumpet of doomsounding thunder\n\r",ch);
send_to_char("spun into a slick wet thread of bare bony wire.\n\r",ch);
}

else if (dream <= 238)
{
send_to_char("The horse and it's rider comes trampling out of the fog called\n\r",ch);
send_to_char("sleep. As you peer at it's indistinguishable form, it's name\n\r",ch);
send_to_char("escapes you but for a second.  The lance is pointed straight \n\r",ch);
send_to_char("for your forehead.  It's thundering hooves jolt your memory.\n\r",ch);
send_to_char("HARK!  You remember the name if the horse and rider, it's\n\r",ch);
send_to_char("Knight-Mare.\n\r",ch);
}

else if (dream <= 240)
{
send_to_char("Someone wakes you up.\n\r",ch);
do_wake(ch, "");
send_to_char("Someone's backstab does UNSPEAKABLE things to you!\n\r",ch);
send_to_char("You have been killed!!!\n\r",ch);
send_to_char("\n\r",ch);
send_to_char("\n\r",ch);
send_to_char("\n\r",ch);
do_sleep(ch, "");
send_to_char("Oh, You're dreaming again.  *sigh*\n\r",ch);
}

else if (dream <= 242)
{
send_to_char("The abomination points at you with a crocked and gnawed finger, \n\r",ch);
send_to_char("whispering your name over and over.  Chanting your death wish \n\r",ch);
send_to_char("like a mantra.  It glides towards where you stand frozen.  Your \n\r",ch);
send_to_char("mind begins to scream for your legs to run... to move... to do \n\r",ch);
send_to_char("anything but buckle and fall.  You stare in horror, as it's mouth \n\r",ch);
send_to_char("open sup and... \n\r",ch);
send_to_char("\n\r",ch);
send_to_char("That's when you realize with disgust your bedroll has somehow been soaked. \n\r",ch);
}

else if (dream <= 244)
{
send_to_char("The cat grabs the dog by the nose and bonks him on the head.  \n\r",ch);
send_to_char("The dog, growling, pokes the mouse in it's eyes. The mouse, \n\r",ch);
send_to_char("with one stroke, slaps both the cat and the dog. The cat gets \n\r",ch);
send_to_char("mad, swings his fist and kicks the mouse in the stomach.  The \n\r",ch);
send_to_char("dog, seeing the mouse being abused, bonks the cat\n\r",ch);
send_to_char("\n\r",ch);
send_to_char("Oh gawd, the 3 Stoogies meet Tom and Jerry.\n\r",ch);
}

else if (dream <= 246)
{
send_to_char("You feel a tickling on your arm, you try to reach over and \n\r",ch);
send_to_char("scratch it but you find yourself frozen.  You open your eyes to \n\r",ch);
send_to_char("see a bright light shining at you. Shadows move in the \n\r",ch);
send_to_char("background and you hear a throbbing sound\n\r",ch);
send_to_char("A sting from your arm feels like a needle and you realize that \n\r",ch); 
send_to_char("your very marrow is being drawn from your bones.\n\r",ch);
send_to_char("As you are just about to pass out you hear alien voices buzzing \n\r",ch);
send_to_char("quietly...\n\r",ch);
munchie = create_object( get_obj_index( 1297 ), 0 ); 
	munchie->timer=1;
        obj_to_room( munchie, ch->in_room );
}

else if (dream <= 248)
{
send_to_char("You're sitting at the table, eating your steak and potatoes, \n\r",ch);
send_to_char("when the meat you're just about to devour begins to cough and \n\r",ch);
send_to_char("spasm.  The potatoes start to roll around on your plate, and \n\r",ch);
send_to_char("some even jump up and down, making a huge mess of your \n\r",ch);
send_to_char("vegetables, and a loud ruckus to boot.  Hey buddy, says the \n\r",ch);
send_to_char("slab of flesh you called dinner.  Why don't you pick on someone \n\r",ch);
send_to_char("your OWN size? \n\r",ch);
}

else if (dream <= 250)
{
send_to_char("You walk through a valley of daisys.. \n\r",ch);
send_to_char("over a ways you see people from different races spread out sleeping \n\r",ch); 
send_to_char("over the field.. \n\r",ch);
send_to_char("In the distance you hear the faint sound of music.\n\r",ch);
send_to_char("your eyes get heavy....\n\r",ch);
send_to_char("you collapse to the ground and sleep....\n\r",ch);
}

else if (dream <= 252)
{
send_to_char("Darkness surrounds you.. cold and silent.. feeling around you, you \n\r",ch);
send_to_char("catch ahold of a candle, and magically it lights itself.. burning \n\r",ch);
send_to_char("slowly sending melted wax down your hand.. Looking around you find \n\r",ch);
send_to_char("you are surrounded..by wooden walls?.. you begin to recognize \n\r",ch);
send_to_char("the shape.. of a coffin.. panicing you begin to claw at the lid, \n\r",ch);
send_to_char("trying in vain to push it open.. but it is impossible.. as you \n\r",ch);
send_to_char("struggle alittle dirt tumbles in from a crack in the coffin \n\r",ch);
send_to_char("wall.. and as the candle slowly is extinguished you realize you are \n\r",ch);
send_to_char("doomed...\n\r",ch);
}

else if (dream <= 254)
{
send_to_char("Standing in front of a large man-sized mirror.. you gaze into its \n\r",ch);
send_to_char("depths looking into your reflection.. when suddenly you think you \n\r",ch);
send_to_char("see the reflection wink at you.. slightly startled you realize \n\r",ch);
send_to_char("it was just a trick of your mind.. curious you place your hand \n\r",ch);
send_to_char("against the mirror.. and the reflection mimics your actions \n\r",ch);
send_to_char("exactally.. drawing your hand back away from the mirror your \n\r",ch);
send_to_char("reflection gains a demonic appearance.. reaching through the \n\r",ch); 
send_to_char("mirror it grabs your hand and pulls you in..\n\r",ch);
}

else if (dream <= 256)
{
send_to_char("Plucking a apple from a tree along the trail, you lift it to your \n\r",ch); 
send_to_char("mouth and begin to take a bite.. a faint whisper is heard..\n\r",ch);
send_to_char("pleeeease... pleeeease slightly puzzled you ignore the faint voice\n\r",ch);
send_to_char("and commence to take a satisfying bite from the ripe apple, and \n\r",ch);
send_to_char("in responce a scream of mortal pain is heard..looking down at the \n\r",ch);
send_to_char("apple in your palm.. a twisted and painfilled face glares at you \n\r",ch); 
send_to_char("from the plush ripe apple..\n\r",ch);
munchie = create_object( get_obj_index( 1298 ), 0 ); 
	munchie->timer=number_range(1, 3);
        obj_to_room( munchie, ch->in_room );
}

else if (dream <= 258)
{
send_to_char("Sitting upon a plush throne of gold and frilly pillows you feel a \n\r",ch);
send_to_char("breeze.. looking to your right and left you see a pair of beautiful \n\r",ch);
send_to_char("servants waving peacock feather fans through the air.  Stepping \n\r",ch);
send_to_char("towards you a equally handsome servant approaches with a plate of\n\r",ch); 
send_to_char("ripe grapes...\n\r",ch);
}

else if (dream <= 260)
{
send_to_char("Being led through a crowd of hissing and booing peasants.. two guards\n\r",ch);
send_to_char("push you along towards a strange platform in the center of the square.\n\r",ch);
send_to_char("shackles bear their heavy burden upon your chained arms and legs..\n\r",ch);
send_to_char("Rotten fruit and eggs sail through the air at you.. stepping upon\n\r",ch);
send_to_char("the wooden platform, you are forced to kneel and a strange wooden bar\n\r",ch); 
send_to_char("secures your head from moving... looking upwards you see a pair of straight\n\r",ch);
send_to_char("wooden poles and at the top a gleaming harbringer of death.. a sharp blade..\n\r",ch);
send_to_char("Hearing a voice accost you from behind the words are barely heard as the\n\r",ch);
send_to_char("crowds yells..  you have been accused of murder.. and found guilty by\n\r",ch);
send_to_char("the court.. executioner let the blade fall..   and the last thing you hear\n\r",ch);
send_to_char("is the grinding sound of metal against wood.. pain.. and then oblivion\n\r",ch);
}

else if (dream <= 262)
{
send_to_char("Walking through the twisting corridors of the endless dungeon.. you trip\n\r",ch);
send_to_char("over a rock.. trying to get up you find your arms and legs stuck.. from\n\r",ch);
send_to_char("the last faint flickers of your torch you see a gigantic web or sticky\n\r",ch);
send_to_char("fibers... struggling against the tight pull of the web.. you hear a\n\r",ch);
send_to_char("strange clicking sound come from above.. and as your torch goes out\n\r",ch);
send_to_char("a enormous black spider floats down upon you..\n\r",ch);
}

else if (dream <= 264)
{
send_to_char("Soft breezes blow against your skin, the salty air bitter on your tongue.\n\r",ch); 
send_to_char("You see them... so soft... so unreal...\n\r",ch);
send_to_char("You walk towards them, and hold out your hand for them to take.\n\r",ch);
send_to_char("As your hand clasps theirs, they swirl and slowly fade into nothing.\n\r",ch);
send_to_char("You sink to the damp, white sand... they are gone. And you are alone.\n\r",ch);
}

else if (dream <= 266)
{
send_to_char("You feel someone breathing down the back of your neck.. you open your eyes to\n\r",ch);
send_to_char("see a huge cat-like creature growling behind you..it smiles and you see a\n\r",ch);
send_to_char("row of hideous sharp teeth..before a scream can pass your lips it sinks \n\r",ch); 
send_to_char("it's fangs into your neck. Everything goes black.\n\r",ch);
}

else if (dream <= 268)
{
send_to_char("Creak.\n\r",ch);
send_to_char("Someone ... or something is approaching.\n\r",ch);
send_to_char("You struggle to awaken as you hear the whisper of a sword being drawn.\n\r",ch);
send_to_char("Red eyes gleem evilly at you as you frantically roll out of bed reaching \n\r",ch); 
send_to_char("in vain for your weapon.\n\r",ch);
send_to_char("Swish.\n\r",ch);
send_to_char("Darkness.\n\r",ch);
}

else if (dream <= 270)
{
send_to_char("You look around at a land youve never seen before wondering why it seems so \n\r",ch); 
send_to_char("familar.. you hear the slithy toes giring nearby\n\r",ch);
send_to_char("you feel something moving around your feet and look down to see the mome raths \n\r",ch); 
send_to_char("swarming around you....\n\r",ch);
send_to_char("a flock of jubjub birds flap towards the sky from some near by bushes \n\r",ch); 
send_to_char("startled by something ..\n\r",ch);
send_to_char("You hear a sound coming nearer something about it makes you shiver...\n\r",ch);
send_to_char("You look up into a pair of glowing eyes and realize it must be ....it cant be \n\r",ch); 
send_to_char("..oh no it is the creature your mother told you to beware of...\n\r",ch);
send_to_char("the Jabberwock stands there its jaws snapping\n\r",ch);
send_to_char("you turn to run but can go no where...\n\r",ch);
send_to_char("you scream.....it fades into silence\n\r",ch);
}

else if (dream <= 272)
{
send_to_char("You stand alone on a moonlit sandy beach, a sudden breeze blows agaainst you \n\r",ch);
send_to_char("face, you shiver for some unknown reason.\n\r",ch);
send_to_char("An eerie fog swirls around you.\n\r",ch);
send_to_char("You hear a cackle from somewhere behind you but before you can turn an arm \n\r",ch);
send_to_char("grabs you by the neck. \n\r",ch);
send_to_char("A dagger presses against your throat, pierceing the skin.\n\r",ch);
send_to_char("A low voice growls in your ear 'fear the ghost pirate Acheron'\n\r",ch);
send_to_char("The fog swirls again and is gone. You are alone and trembling..fearing the day\n\r",ch);
send_to_char("that he might return...\n\r",ch);
act( "$n fidgets and mumbles strange words in $s sleep.", ch, NULL, NULL, TO_ROOM); 
}

else if (dream <= 274)
{
send_to_char("You are walking down the street on a bright and sunny morning.\n\r",ch);
send_to_char("People are laughing and smiling happily as you walk by and you smile in\n\r",ch);
send_to_char("return.  It sure is nice to see people happy for once.\n\r",ch);
send_to_char("You continue your walk down the street and stop to look in a store window\n\r",ch);
send_to_char("at beautiful dishes. Looking more closely in the window you gasp in horror.\n\r",ch);
send_to_char("YOU FORGOT TO PUT YOUR CLOTHES ON THIS MORNING!!!!\n\r",ch);
send_to_char("You turn deep red from embrassment and hide behind a bush.\n\r",ch);
}

else if (dream <= 276)
{
send_to_char("You are surrounded by {Ygolden{0 flames.\n\r",ch);
send_to_char("You frantically look for a way of escape, but to no avail.\n\r",ch);
send_to_char("You smell the armoma of burning flesh.\n\r",ch);
send_to_char("{RYOUR FLESH!{0\n\r",ch);
send_to_char("The flames around you seem to be getting hotter.\n\r",ch);
send_to_char("You scream but no one hears your voice or is it that no one cares.\n\r",ch);
send_to_char("You run aimlessly but the flames seem to follow your every move.\n\r",ch);
send_to_char("THERE IS NO ESCAPE!\n\r",ch);
send_to_char("\n\r",ch);
send_to_char("As the flames are about to engulf you...\n\r",ch);
send_to_char("You realize....its only a dream....its only a dream...\n\r",ch);
send_to_char("Or is it........\n\r",ch);
}

else if (dream <= 278)
{
send_to_char("Ahhhh...  Asleep at last, it's been a long time since you've had a good\n\r",ch);
send_to_char("nights sleep, and you figure that  tonight should be different.. Or should\n\r",ch);
send_to_char("it!   All of a sudden you feel a burst of energy run thru your body, and\n\r",ch);
send_to_char("you break out in a cold sweat!  YOu feel as if someone or some THING is\n\r",ch);
send_to_char("pulling your body, down, down, down, your pulled until your at the center\n\r",ch);
send_to_char("of the earth.  Then a deep voice speaks,   I have been waiting your arrival\n\r",ch);
send_to_char("it booms. Looking around franticlly you see a dark figure with horns dressed\n\r",ch);
send_to_char("in a blood-stained cloak. 'You have been very bad...' it continues 'for this\n\r",ch);
send_to_char("I will punish you!' With that you feel a draining of strength and you fall\n\r",ch);
send_to_char("to the ground.  Trying to get up you realise that you can't move! Then the\n\r",ch);
send_to_char("voice booms 'And now, you DIE!'  ZZZZZZZZZZaaaaaaaaaaaaappppppp! a bolt \n\r",ch);
send_to_char("lightning hits your body...\n\r",ch);
send_to_char("\n\r",ch);
send_to_char("\n\r",ch);
send_to_char("                                  Everything falls silent.\n\r",ch);
ch->hit = ch->hit * 7 / 8;
if (ch->hit < 0)
  ch->hit = 1;
munchie = create_object( get_obj_index( 1293 ), 0 );
	munchie->timer = number_range(1, 10);
        obj_to_room( munchie, ch->in_room );
act( "A bolt of {WLIGHTNING{0 falls from the sky and stikes $n as $e lies sleeping!", ch, NULL, NULL, TO_ROOM);
}

else if (dream <= 280)
{
send_to_char("As you start to drift off to sleep you see sheep  jumping over a fence..\n\r",ch);
send_to_char("One Sheep, Two sheep, Three Sheep, Four Sheep...\n\r",ch);
send_to_char("Slowly as you fall into  a deeper sslleep the sheep start morphing into wolves.\n\r",ch);
send_to_char("Even farther into your slumber the wolves again start morphing. into corpses..\n\r",ch); 
send_to_char("Eight Corpses, Nine Corpses...\n\r",ch);
send_to_char("You then  realise that the corpses are YOUR corpses!\n\r",ch);
send_to_char("You wake up bathed in a blood-red sweat....\n\r",ch);
do_wake(ch, "");
}

else if (dream <= 282)
{
send_to_char("'They're coming' shrieked the old woman.\n\r",ch);
send_to_char("Sudden blurs of movement fog your already hazy eyes.\n\r",ch);
send_to_char("Praying for strength you struggle to rise to your feet.\n\r",ch);
send_to_char("You fall to your knees, too dizzy to stand. You realize everyone else\n\r",ch);
send_to_char("has escaped through the passage and you are left alone...\n\r",ch);
send_to_char("The door bursts open and you lift your head to see.\n\r",ch);
send_to_char("Laughing in hysteria you know it's over and there is no escape.\n\r",ch);
send_to_char("The dark figures roughly pull you to your feet \n\r",ch);
send_to_char("sneering in disgust at your hideous appearance, the leader snaps your neck\n\r",ch);
send_to_char("and all is silent in the  church yard as the 7 nuns dispose of your body.\n\r",ch);
}

else if (dream <= 284)
{
send_to_char("You look into their eyes, unable to break the gaze because of its\n\r",ch);
send_to_char("intensity.  She reaches for your hand and places it in hers.  Holding it\n\r",ch);
send_to_char("there for a moment of sadness, then kissing your forehead lovingly she goes.\n\r",ch);
send_to_char("You watch her leave, and a single tear falls to the ground, you know she\n\r",ch);
send_to_char("will never return.  Realizing she left something in your hand you open\n\r",ch);
send_to_char("it to find a small ring with a beautiful amethyst  set in it.\n\r",ch);
send_to_char("You put the ring on your own finger and swear to never forget the most\n\r",ch);
send_to_char("important person in your life. Once again you cry for the loss of\n\r",ch);
send_to_char("your mother.\n\r",ch);
}

else if (dream <= 286)
{
send_to_char("You hear fighting all around you, looking around you realize your in the\n\r",ch);
send_to_char("middle of a gang fight. You notice that you are holding a 6 inch knife, and \n\r",ch); 
send_to_char("there's a man in front of youo pointing a gun at your head.\n\r",ch);
send_to_char("All of a sudden you hear strange noises from behind you and the\n\r",ch);
send_to_char("man with the gun whirls around to get a better look. As he does you lunge\n\r",ch);
send_to_char("the knife into  him and knock the gun ou of his hand.\n\r",ch);
send_to_char("reallizing what you just did you take off running, and keep running,\n\r",ch);
send_to_char("leaping over a 8' fence no  problem, running, running, running until you\n\r",ch);
send_to_char("can't run any more and you collapse under a tree. Finding yourself exhausted\n\r",ch); 
send_to_char("you pass out without knowing it.\n\r",ch);
send_to_char("Your vision goes blood-red and the man you stabbed is  burned into\n\r",ch); 
send_to_char("your mind until you wake up trembling  in horror.\n\r",ch);
do_wake(ch, "");
}

else if (dream == 289)
{
send_to_char("You are in a large cell with mattresses completly covering every wall.. \n\r",ch);
send_to_char("You try to move but find that\n\r",ch);
send_to_char("it is nearly immpossible since you are in a straight jacket.\n\r",ch);
send_to_char("You scream as loud as you can, hoping that someone will come, but nobody\n\r",ch);
send_to_char("does..\n\r",ch);
send_to_char("You're doomed to stay here for the rest of eternity, there is no escape..\n\r",ch);
}

else if (dream <= 292)
{
send_to_char("Running through the forrest, your heart almost pounds out of your\n\r",ch);
send_to_char("chest. The cold sweat pouring down your cheeks blurs your vision\n\r",ch);
send_to_char("and the burning is almost unbearable.  You can hear the rapid\n\r",ch);
send_to_char("hoofbeats close behind you and the howling of dogs on your scent.\n\r",ch);
send_to_char("There!! Up ahead you see a light!!! Happy tears stream down your\n\r",ch);
send_to_char("face as you run to the flames. As you are almost upon the flames your laughter\n\r",ch); 
send_to_char("stops and is replaced with piercing shrieks. Where are\n\r",ch);
send_to_char("the horrible screams coming from? You turn and see 2 charred bodies\n\r",ch);
send_to_char("running madly around, still covered with flickering fire. You cover\n\r",ch);
send_to_char("your mouth as the sickening feeling over comes you and you \n\r",ch);
send_to_char("collapse  into the flames and join the poor people behind you.\n\r",ch);
}

else if (dream <=300)
{
send_to_char("A hand grabs your shoulder and a grizzled old man\n\r",ch);
send_to_char("peers at your wounds.\n\r",ch);
send_to_char("'Hmmmm, let me see.'\n\r",ch);
send_to_char("He digs in his pouch and pulls out a smelly potion.\n\r",ch);
send_to_char("'Here, drink this.'\n\r",ch);
send_to_char("As you raise yourself from your slumbers, he disappears.\n\r",ch);
send_to_char("All that remains is a vial that reeks even from arms length.\n\r",ch);
munchie = create_object( get_obj_index( 1292 ), 0 ); 
        obj_to_char( munchie, ch );
do_wake(ch, "");
}

else if (dream <=302)
{
send_to_char("The bright beam catches you on the shoulder and you scream in pain as\n\r",ch);
send_to_char("your flesh rips open. You fumble with your gun and try to unjam the trigger.\n\r",ch);
send_to_char("Still unable to get the laser to work, you throw it to the ground in\n\r",ch);
send_to_char("disgust. Holding your injured shoulder with one hand, you reach with the\n\r",ch);
send_to_char("other into your pack, pulling out a small blinking sphere.\n\r",ch);
send_to_char("You close your eyes and say a silent good-bye to your fleet, only\n\r",ch);
send_to_char("praying they made it out in time.  You can hear the strange foreign\n\r",ch);
send_to_char("shouts of the Krakians coming up quickly, and you shudder in fear.\n\r",ch);
send_to_char("Soon they are nearing the gate you're in and you know it is time. You\n\r",ch);
send_to_char("push a glowing button on the sphere and it begins to beep loudly.\n\r",ch);
send_to_char("'10 seconds until utter destruction' a computerized voice intones.\n\r",ch);
send_to_char("You clip the sphere onto the wall as the Krakians enter screaming.\n\r",ch);
send_to_char("There is a sudden white light and then nothing....\n\r",ch);
}

else if (dream <= 304)
{
send_to_char("You hear the slow flap of wings.\n\r",ch);
send_to_char("It draws closer and closer.  Looking up\n\r",ch);
send_to_char("you can see a large silver dragon outlined against the sun.\n\r",ch);
send_to_char("As it draws nearer, you see a  golden lyre dangling from a strap\n\r",ch);
send_to_char("around its neck.  Suddenly you let out a scream and run for cover.\n\r",ch);
send_to_char("You fall.  splat.  Heartbeats later.  SPLAT!!\n\r",ch);
}

else if (dream <= 306)
{
send_to_char("You sit on the green grass beneath a broad large oak tree staring out at the\n\r",ch);
send_to_char("sunlit fields while a soft warm breeze touchs you lightly.  Staring up\n\r",ch);
send_to_char("through the leaves you can see motes of sunlight peeking through and you\n\r",ch);
send_to_char("can only think of how wonderful it is to be alive...\n\r",ch);
send_to_char("\n\r",ch);
send_to_char("Suddenly, violently the weather changes, wind whipping almost through you,\n\r",ch);
send_to_char("chilling your soul.  The plains turn a drab grey and they sun clouds over.\n\r",ch);
send_to_char("Something caresses your shoulder and you restist temptation to look even \n\r",ch);
send_to_char("though it frightens and freezes your blood....\n\r",ch);
send_to_char("\n\r",ch);
send_to_char("Like a snake striking a tendril wraps around your neck choking you.  Fighting\n\r",ch);
send_to_char("to scream, for even a sound to escape your throat, you claw at your unkown\n\r",ch);
send_to_char("foe.\n\r",ch);
}

else if (dream <= 308)
{
send_to_char("Aaaaaahhhh, what a good nights sleep you had.  You stand up and stretch\n\r",ch);
send_to_char("letting everybone in your body snap and pop.. Looking at the clock you\n\r",ch);
send_to_char("realise it's only 6:30AM and that mabye it would be a good time to go\n\r",ch);
send_to_char("check out the mud.  As you start walking to the computer room you feel a\n\r",ch);
send_to_char("cold chill run thru  your body but just shrug it off.  As you enter the room\n\r",ch);
send_to_char("and look over to where your computer is, you realize IT'S GONE!  Thinkinng\n\r",ch);
send_to_char("you've been robbed you run to the phone, but find it isn't there either!\n\r",ch);
send_to_char("Feeling scared, and invaded you look around to see what else is missing\n\r",ch);
send_to_char("when you notice the door, and none of the windows have been broken into.\n\r",ch);
send_to_char("Another cold chill runs thru your body and you hear a voice off in the\n\r",ch);
send_to_char("background...\n\r",ch);
send_to_char("'You are entering another dimension...'\n\r",ch);
}

else if (dream <= 310)
{
send_to_char("Looking back you see hundreds of people running your way, all screaming your\n\r",ch);
send_to_char("name and waving weapons in the air.  You have no idea why there chasing you\n\r",ch);
send_to_char("but decide it would be a good idea to get out of there! Starting to run\n\r",ch);
send_to_char(", you run down many streets and alleys slowly winding your way out of the\n\r",ch);
send_to_char("town only stopping occassionaly to fight someone that jumped out in front\n\r",ch);
send_to_char("of you. After what seems like forever you come to a small inn and decide to stay for the night.\n\r",ch); 
send_to_char("after paying the innkeeper extra to not tell anyone your there you go up to\n\r",ch);
send_to_char("your room and tend to your wounds. Shortly after you hear a horde of people come in and\n\r",ch);
send_to_char("question him, then go racing off...\n\r",ch);
send_to_char("The next morning you thank the man and go off looking for\n\r",ch);
send_to_char("a new place to  hide, fearing they will eventually catch up to you and\n\r",ch);
send_to_char("wonder what they will do then...\n\r",ch);
}

else if (dream <= 312)
{
send_to_char("You feel a soft brush at your neck, and ignore it.\n\r",ch);
send_to_char("Then you feel the fangs PIERCE your skin and your lifeblood being taken!\n\r",ch);
send_to_char("\n\r",ch);
send_to_char("You sit bolt upright, your heart pounding!\n\r",ch);
send_to_char("You look about but see no one near you.\n\r",ch);
send_to_char("You calm yourself and drift back to sleep, realizing it was only a dream.\n\r",ch);
send_to_char("Or was it...........?\n\r",ch);
}

else if (dream <= 314)
{
send_to_char("You hear flutes in the distance. As the fog clears you realize that you\n\r",ch);
send_to_char("are standing in a quaint village filled with small happy fauns. A small\n\r",ch);
send_to_char("male approaches you, smiles, puts a pipe to his lips, and begins to play\n\r",ch);
send_to_char("a tune. He walks away - and you begin to follow. At first you are enchanted\n\r",ch);
send_to_char("...but you slowly come to the realization you can't stop! You struggle\n\r",ch);
send_to_char("from the sound...throwing your hands over your ears...but it solves\n\r",ch);
send_to_char("nothing. He leads you away from the village into a dark, ominous forest.\n\r",ch);
send_to_char("The sound begins to drill into your eardrums...then your brain...\n\r",ch);
send_to_char("You awake with a start!\n\r",ch);
do_wake(ch, "");
}

else if (dream <= 316)
{
send_to_char("You are being led down a corridor.\n\r",ch);
send_to_char("You are completely shackled and each arm is fiercely gripped by a decaying\n\r",ch);
send_to_char("zombie...They force you into a large dark throne room. Before you sits\n\r",ch);
send_to_char("Egil on a throne of chipped bone and patchworked flesh. He is in deep\n\r",ch);
send_to_char("conversation with Withers, who stands to his left. Withers leans over and\n\r",ch);
send_to_char("whispers something into Egil's ear, and Egil turns to glare at you. He\n\r",ch);
send_to_char("extends his arm, makes a fist, and points his thumb down to the floor.\n\r",ch);
send_to_char("\n\r",ch);
send_to_char("Excruciating pain fills your entire body, and you look down to see your\n\r",ch);
send_to_char("skin dissolve into dust. You open your mouth, and a scream of hopelessness\n\r",ch);
send_to_char("escapes........\n\r",ch);
send_to_char("\n\r",ch);
send_to_char("All goes dark.......\n\r",ch);
}

else if (dream <= 318)
{
send_to_char("You're resting peacefully, dreaming about beautiful green fields, full of colorful flowers.. the \n\r",ch); 
send_to_char("sun shines brightly overhead, when an odd sensation awakes you.\n\r",ch);
send_to_char("You wake up to feel an odd burning sensation on your left hand, and look down to\n\r",ch);
send_to_char("find your hand literally on fire! Bright flames and greasy orange smoke rise from the palm of your \n\r",ch); 
send_to_char("hand, as you sit helplessly and watch the skin drip and crackle,\n\r",ch);
send_to_char("peeling away from the bones as you begin to scream in agony..\n\r",ch);
send_to_char("You sit bolt upright in bed, rocked to the core of your being as you realize this was only.. a \n\r",ch);
send_to_char("dream..? \n\r",ch);
do_wake(ch, "");
}

else if (dream <= 321)
{
send_to_char("Your walking down a grey road, and the scenes all about\n\r",ch); 
send_to_char("you are blue, thats all you see, greish blue for as far as \n\r",ch);
send_to_char("the eye can see.\n\r",ch);
send_to_char("You keep walking down the road, and you come upon a bench, \n\r",ch);
send_to_char("with 2 people sitting on it reading news papers, you \n\r",ch);
send_to_char("notice the papers are close to there faces.\n\r",ch);
send_to_char("All of a sudden there faces morph into the news paper, and \n\r",ch);
send_to_char("they put their hands down. They look at you then start \n\r",ch);
send_to_char("talking to each other.\n\r",ch);
send_to_char("You run as fast as you can down the road, till you see a \n\r",ch);
send_to_char("woman, made of bones. She is taking a walk, with her baby in the cart\n\r",ch); 
send_to_char("she is pushing along, you decide to look closer, so you \n\r",ch);
send_to_char("walk over to the cart made of bones...\n\r",ch);
send_to_char("You look in side and the baby screams so loud it knocks \n\r",ch);
send_to_char("you back a few feet. \n\r",ch);
send_to_char("You run as fast as you can, you come to a cliff.\n\r",ch);
}

else if (dream <= 324)
{
send_to_char("you hear them comming\n\r",ch);
send_to_char("They're comming, there's too many of them. What will you do?\n\r",ch);
send_to_char("Suddenly, they crest the ridge, and you are bowled over by \n\r",ch);
send_to_char("2000 puppies who want to lick your face.\n\r",ch);
send_to_char("They're all over. Well, at least they're housebroken.\n\r",ch);
send_to_char("Aren't they?\n\r",ch);
}

else if (dream <= 326)
{
send_to_char("You find yourself in a labyrinth of pipes and gears. All around you, you\n\r",ch);
send_to_char("hear clanks, bangs, and the rattle of chains. You smell the oil used to\n\r",ch);
send_to_char("lubricate the gears.  As you walk through this place, you find it\n\r",ch);
send_to_char("looks familiar. You wonder why. Then you realize, you've\n\r",ch);
send_to_char("seen it before! A gnome had been working on the plans a while back!\n\r",ch);
}

else if (dream <= 328)
{
send_to_char("Wait, is that pipe shaking?  Yes, it is! All of the sudden\n\r",ch); 
send_to_char("you, hear the shrill scream of alarms! You run as fast as you\n\r",ch);
send_to_char("can, trying to get out, but it's just too big! Oh, no!\n\r",ch); 
send_to_char("BOOOOOOOOMMMMMMMMMM!!!!!!!!!!!!!!!\n\r",ch);
send_to_char("You wake up, with grease in your hair.\n\r",ch);
do_wake(ch, "");
}

else if (dream == 465) 
{ 
send_to_char("You open your eyes, it is still dark.\n\r",ch);
send_to_char("The fire has gone out.  Where are the matchs?\n\r",ch);
send_to_char("You reach around in the dark, your hand strikes something hard.\n\r",ch);
send_to_char("Ouch!!!!  Blood flows from your hand as you nick it on the blade of a \n\r",ch);
send_to_char("sword. The room lights up as your blood drips onto the blade. \n\r",ch);
send_to_char("The image of a powerful sword is burned into your mind!\n\r",ch);
munchie = create_object( get_obj_index( 1289 ), 0 ); 
	munchie->value[1]=number_range(1,3);
	munchie->value[2]=number_range(1,8);
	munchie->timer=number_range(1,25);
        obj_to_room( munchie, ch->in_room );
act( "$n lets out a cry and a bit of blood drips from $s hand!", ch, NULL, NULL, TO_ROOM);
act( "Mumbling strange words, $n passes into a deeper sleep.", ch, NULL, NULL, TO_ROOM);
}

else if (dream == 466)
{
send_to_char("You shudder from the cold as you see a scene of a vast frozen wasteland\n\r",ch);
send_to_char("you begin to stumble around in the cold snow drifts and frozen ponds\n\r",ch);
send_to_char("the cold begins to reach your fingers and toes as you begin to loose\n\r",ch);
send_to_char("feeling in them, all of a sudden a man dressed in black appears before you\n\r",ch);
send_to_char("in one hand he is holding a dagger and in the other...\n\r",ch);
send_to_char("YOUR HEAD!?!! As you begin to scream it slowly fades away as you feel the frost\n\r",ch);
send_to_char("from a short blade against your neck.\n\r",ch);
munchie = create_object( get_obj_index( 1289 ), 0 );
        munchie->value[1]=number_range(1,3);
	munchie->value[2]=number_range(1,8);
	munchie->timer=number_range(1,25);
        obj_to_room( munchie, ch->in_room );
act( "Mumbling strange words, $n passes into a deeper sleep.", ch, NULL, NULL, TO_ROOM);
}

else if (dream == 467)
{
send_to_char("You think about vampires and one appears.\n\r",ch);
send_to_char("It reaches for you and you scream in terror as\n\r",ch);
send_to_char("it's fangs bite into your neck!\n\r",ch);
}

else
{
send_to_char("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz.\n\r",ch);
}
}


	if ( ch->position >= POS_STUNNED )
	{
            int number = 5;

            /* check to see if we need to go home */
            if (IS_NPC(ch) && ch->zone != NULL && ch->zone != ch->in_room->area
            && ch->desc == NULL &&  ch->fighting == NULL 
	    && !IS_AFFECTED(ch,AFF_CHARM) )
            {
                if(IS_SET(ch->in_room->room_flags,ROOM_LAW))
                    number += 30;
                if(IS_SET(ch->act, ACT_AGGRESSIVE))
                    number += 40;
	        if(IS_SET(ch->in_room->room_flags,ROOM_SAFE))
                    number += 30;

                if(number_percent() < number)
                {
                    act("$n wanders on home.",ch,NULL,NULL,TO_ROOM);
                    extract_char(ch,TRUE);
                }
            	continue;
            }

            if(microheal_test && !IS_NPC(ch) && start_hit_gain(ch))
                ;
	    else if ( ch->hit  < ch->max_hit )
		ch->hit  += hit_gain(ch, FALSE);
	    else
		ch->hit = ch->max_hit;

            if(microheal_test && !IS_NPC(ch) && start_mana_move_gain(ch))
                ;
            else
            {
                if ( ch->mana < ch->max_mana )
                    ch->mana += mana_gain(ch);
                else
                    ch->mana = ch->max_mana;

                if ( ch->move < ch->max_move )
                    ch->move += move_gain(ch);
                else
                    ch->move = ch->max_move;
            }
	}

	if ( ch->position == POS_STUNNED )
	    update_pos( ch );

	if ( !IS_NPC(ch) && ch->level < LEVEL_IMMORTAL )
	{
	    OBJ_DATA *obj;

	    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
	    &&   obj->item_type == ITEM_LIGHT
	    &&   obj->value[2] > 0 )
	    {
		if ( --obj->value[2] == 0 && ch->in_room != NULL )
		{
		    --ch->in_room->light;
		    act( "$p goes out.", ch, obj, NULL, TO_ROOM );
		    act( "$p flickers and goes out.", ch, obj, NULL, TO_CHAR );
		    extract_obj( obj );
		}
	 	else if ( obj->value[2] <= 5 && ch->in_room != NULL)
		    act("$p flickers.",ch,obj,NULL,TO_CHAR);
	    }

	    if ( ++ch->timer >= 12 )
	    {
		if ( ch->was_in_room == NULL && ch->in_room != NULL )
		{
		    ch->was_in_room = ch->in_room;
		    if ( ch->fighting != NULL )
			stop_fighting( ch, TRUE );
		    act( "$n disappears into the void.",
			ch, NULL, NULL, TO_ROOM );
		    send_to_char( "You disappear into the void.\n\r", ch );
		    if (ch->level > 1)
		        save_char_obj( ch );
		    char_from_room( ch );
		    char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
		}
	    }

          if(!ch->in_arena)
          {
	    gain_condition( ch, COND_DRUNK,  -1 );
	    gain_condition( ch, COND_FULL, ch->size > SIZE_MEDIUM ? -4 : -2 );
	    gain_condition( ch, COND_THIRST, -1 );

            if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
	        gain_condition( ch, COND_FATIGUE, 6 );
            else if(ch->position <= POS_SITTING)
	        gain_condition( ch, COND_FATIGUE, 4 );
            else
	        gain_condition( ch, COND_FATIGUE, 2 );

	    gain_condition( ch, COND_HUNGER, ch->size > SIZE_MEDIUM ? -2 : -1);
          }
	}

	for ( paf = ch->affected; paf != NULL; paf = paf_next )
	{
	    paf_next	= paf->next;
	    if ( paf->duration > 0 )
	    {
		paf->duration--;
		if (number_range(0,4) == 0 && paf->level > 0)
		  paf->level--;  /* spell strength fades with time */
            }
	    else if ( paf->duration < 0 )
		;
	    else
	    {
		if ( paf_next == NULL
		||   paf_next->type != paf->type
		||   paf_next->duration > 0 )
		{
		    if ( paf->type > 0 && skill_table[paf->type].msg_off )
		    {
			send_to_char( skill_table[paf->type].msg_off, ch );
			send_to_char( "\n\r", ch );
		    }
		}
	  
		affect_remove( ch, paf );
	    }
	}

        if( IS_SHIELDED(ch, SHD_MANA) && ( ch->max_mana < 1 || 100 * ch->mana / ch->max_mana < 40 ) )
        {
                send_to_char("Your manashield *{Rbreaks{x*!\n\r", ch);
                act("$n's manashield *{Rbreaks{x*!",ch, NULL,NULL,TO_ROOM);
                affect_strip(ch,gsn_manashield);
        }

         if (!IS_NPC(ch) && ch->pcdata->granted != NULL)
         {
           GRANT_DATA *gran,*gran_next, *gran_prev;
           char buf[MAX_STRING_LENGTH];
 
           gran_prev=ch->pcdata->granted;
 
           for (gran=ch->pcdata->granted; gran != NULL; gran=gran_next)
           {
             gran_next = gran->next;
             if (gran->duration > 0) gran->duration--;
             if (gran->duration==0)
             {
               if (gran==ch->pcdata->granted)
                 ch->pcdata->granted=gran_next;
               else
                 gran_prev->next = gran->next;
               sprintf(buf,
                 "Your time runs out on the %s command.\n\r",gran->name);
               send_to_char(buf,ch);
               free_string(gran->name);
               free_mem(gran,sizeof(*gran));
               gran = NULL;
             }
             if (gran != NULL) gran_prev=gran;
           }
         }

	/*
	 * Careful with the damages here,
	 *   MUST NOT refer to ch after damage taken,
	 *   as it may be lethal damage (on NPC).
	 */

        if (is_affected(ch, gsn_plague) && ch != NULL)
        {
            AFFECT_DATA *af, plague;
            CHAR_DATA *vch;
            int dam;

	    if (ch->in_room == NULL)
		return;
            
	    act("$n writhes in agony as plague sores erupt from $s skin.",
		ch,NULL,NULL,TO_ROOM);
	    send_to_char("You writhe in agony from the plague.\n\r",ch);
            for ( af = ch->affected; af != NULL; af = af->next )
            {
            	if (af->type == gsn_plague)
                    break;
            }
        
            if (af == NULL)
            {
            	REMOVE_BIT(ch->affected_by,AFF_PLAGUE);
            	return;
            }
        
            if (af->level == 1)
            	return;
        
	    plague.where		= TO_AFFECTS;
            plague.type 		= gsn_plague;
            plague.level 		= af->level - 5; 
            plague.duration 	= number_range(1,plague.level);
            plague.location		= APPLY_STR;
            plague.modifier 	= -5;
            plague.bitvector 	= AFF_PLAGUE;
        
            for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
            {
                if (!saves_spell(plague.level - 8,vch,DAM_DISEASE) 
		&&  !IS_IMMORTAL(vch)
                &&  plague.level - vch->level < 50
                &&  !IS_SET(ch->in_room->room_flags,ROOM_SAFE)
            	&&  !IS_AFFECTED(vch,AFF_PLAGUE) && number_bits(4) == 0)
            	{
            	    send_to_char("You feel hot and feverish.\n\r",vch);
            	    act("$n shivers and looks very ill.",vch,NULL,NULL,TO_ROOM);
            	    affect_join(vch,&plague);
            	}
            }

	    dam = UMIN(ch->level,af->level/5+1);
	    ch->mana -= dam;
	    ch->move -= dam;
	    damage_old( ch, ch, dam, gsn_plague,DAM_DISEASE,FALSE);
        }
	else if ( IS_AFFECTED(ch, AFF_POISON) && ch != NULL
	     &&   !IS_AFFECTED(ch,AFF_SLOW))
	{
	    AFFECT_DATA *poison;

	    poison = affect_find(ch->affected,gsn_poison);

	    if (poison != NULL)
	    {
	        act( "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
	        send_to_char( "You shiver and suffer.\n\r", ch );
	        damage_old(ch,ch,poison->level/5 * 4,gsn_poison,
		    DAM_POISON,FALSE);
	    }
            else
            {
	      poison = affect_find(ch->affected,gsn_accelerate_poison);
	      if (poison != NULL)
	      {
	        act( "$n shivers convulsively and suffers.", ch, NULL, NULL, TO_ROOM );
	        send_to_char( "You shiver convulsively and suffer.\n\r", ch );
	        damage_old(ch,ch,poison->level/4 * 20,gsn_poison,
		    DAM_POISON,FALSE);
	      }
            }
	}

	else if ( ch->position == POS_INCAP && number_range(0,1) == 0)
	{
	    damage( ch, ch, 3, TYPE_UNDEFINED, DAM_NONE,FALSE);
	}
	else if ( ch->position == POS_MORTAL )
	{
	    damage( ch, ch, 6, TYPE_UNDEFINED, DAM_NONE,FALSE);
	}
    }

    /*
     * Autosave and autoquit.
     * Check that these chars still exist.
     */
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
        ch_next = ch->next;

	if (ch->desc != NULL && ch->desc->descriptor % 5 == save_number)
	    save_char_obj(ch);

        if ( ch == ch_quit )
            do_quit( ch, "" );
    }

    return;
}




/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update( void )
{   
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA *paf, *paf_next;

    for ( obj = object_list; obj != NULL; obj = obj_next )
    {
	CHAR_DATA *rch;
	char *message;

	obj_next = obj->next;

	/* go through affects and decrement */
        for ( paf = obj->affected; paf != NULL; paf = paf_next )
        {
            paf_next    = paf->next;
            if ( paf->duration > 0 )
            {
                paf->duration--;
                if (number_range(0,4) == 0 && paf->level > 0)
                  paf->level--;  /* spell strength fades with time */
            }
            else if ( paf->duration < 0 )
                ;
            else
            {
                if ( paf_next == NULL
                ||   paf_next->type != paf->type
                ||   paf_next->duration > 0 )
                {
                    if ( paf->type > 0 && skill_table[paf->type].msg_obj )
                    {
			if (obj->carried_by != NULL)
			{
			    rch = obj->carried_by;
			    act(skill_table[paf->type].msg_obj,
				rch,obj,NULL,TO_CHAR);
			}
			if (obj->in_room != NULL 
			&& obj->in_room->people != NULL)
			{
			    rch = obj->in_room->people;
			    act(skill_table[paf->type].msg_obj,
				rch,obj,NULL,TO_ALL);
			}
                    }
                }

                affect_remove_obj( obj, paf );
            }
        }


	if ( obj->timer <= 0 || --obj->timer > 0 )
	    continue;

	switch ( obj->item_type )
	{
	default:              message = "$p crumbles into dust.";  break;
	case ITEM_FOUNTAIN:   message = "$p dries up.";         break;
	case ITEM_CORPSE_NPC: message = "$p decays into dust."; break;
	case ITEM_CORPSE_PC:  message = "$p decays into dust."; break;
	case ITEM_FOOD:       message = "$p decomposes.";	break;
	case ITEM_POTION:     message = "$p has evaporated from disuse.";	
								break;
	case ITEM_PORTAL:     message = "$p fades out of existence."; break;
	case ITEM_CONTAINER: 
	case ITEM_PIT: 
	    if (CAN_WEAR(obj,ITEM_WEAR_FLOAT))
		if (obj->contains)
		    message = 
		"$p flickers and vanishes, spilling its contents on the floor.";
		else
		    message = "$p flickers and vanishes.";
	    else
		message = "$p crumbles into dust.";
	    break;
	}

	if ( obj->carried_by != NULL )
	{
	    if (IS_NPC(obj->carried_by) 
	    &&  obj->carried_by->pIndexData->pShop != NULL)
		obj->carried_by->silver += obj->cost/5;
	    else
	    {
	    	act( message, obj->carried_by, obj, NULL, TO_CHAR );
		if ( obj->wear_loc == WEAR_FLOAT)
		    act(message,obj->carried_by,obj,NULL,TO_ROOM);
	    }
	}
	else if ( obj->in_room != NULL
	&&      ( rch = obj->in_room->people ) != NULL )
	{
	    if (! (obj->in_obj && obj->in_obj->pIndexData->item_type == ITEM_PIT
	           && !CAN_WEAR(obj->in_obj,ITEM_TAKE)))
	    {
	    	act( message, rch, obj, NULL, TO_ROOM );
	    	act( message, rch, obj, NULL, TO_CHAR );
	    }
	}

        if ((obj->item_type == ITEM_CORPSE_PC || obj->wear_loc == WEAR_FLOAT)
	&&  obj->contains)
	{   /* save the contents */
     	    OBJ_DATA *t_obj, *next_obj;

	    for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
	    {
		next_obj = t_obj->next_content;
		obj_from_obj(t_obj);

		if (obj->in_obj) /* in another object */
		    obj_to_obj(t_obj,obj->in_obj);

		else if (obj->carried_by)  /* carried */
		    if (obj->wear_loc == WEAR_FLOAT)
			if (obj->carried_by->in_room == NULL)
			    extract_obj(t_obj);
			else
			    obj_to_room(t_obj,obj->carried_by->in_room);
		    else
		    	obj_to_char(t_obj,obj->carried_by);

		else if (obj->in_room == NULL)  /* destroy it */
		    extract_obj(t_obj);

		else /* to a room */
		    obj_to_room(t_obj,obj->in_room);
	    }
	}

	extract_obj( obj );
    }

    return;
}



/*
 * Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function takes 25% to 35% of ALL Merc cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't the mob to just attack the first PC
 *   who leads the party into the room.
 *
 * -- Furey
 */
void aggr_update( void )
{
    CHAR_DATA *wch;
    CHAR_DATA *wch_next;
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    CHAR_DATA *victim;

    for ( wch = plr_list; wch != NULL; wch = wch_next )
    {
	wch_next = wch->plr_next;
	if ( IS_NPC(wch)
	||   wch->level >= LEVEL_IMMORTAL
	||   wch->in_room == NULL 
	||   wch->in_room->area->empty
        ||   !wch->desc
        ||   wch->desc->connected != CON_PLAYING
	||   IS_SET(wch->in_room->room_flags,ROOM_SAFE) )

	    continue;

	for ( ch = wch->in_room->people; ch != NULL; ch = ch_next )
	{
	    int count;

	    ch_next	= ch->next_in_room;

	    if ( !IS_NPC(ch)
	    ||   !IS_SET(ch->act, ACT_AGGRESSIVE)
	    ||   IS_SET(ch->in_room->room_flags,ROOM_SAFE)
	    ||   IS_AFFECTED(ch,AFF_CALM)
	    ||   ch->fighting != NULL
	    ||   IS_AFFECTED(ch, AFF_CHARM)
	    ||   !IS_AWAKE(ch)
	    ||   ( IS_SET(ch->act, ACT_WIMPY) && IS_AWAKE(wch) )
	    ||   !can_see( ch, wch ) 
	    ||   number_bits(1) == 0)
		continue;

	    /*
	     * Ok we have a 'wch' player character and a 'ch' npc aggressor.
	     * Now make the aggressor fight a RANDOM pc victim in the room,
	     *   giving each 'vch' an equal chance of selection.
	     */
	    count	= 0;
	    victim	= NULL;
	    for ( vch = wch->in_room->people; vch != NULL; vch = vch_next )
	    {
		vch_next = vch->next_in_room;

		if ( !IS_NPC(vch)
		&&   vch->level < LEVEL_IMMORTAL
		&&   (ch->in_arena || ch->level >= vch->level - 5)
                &&   !is_friend(ch,vch)
		&&   ( !IS_SET(ch->act, ACT_WIMPY) || !IS_AWAKE(vch) )
		&&   can_see( ch, vch ) )
		{
		    if ( number_range( 0, count ) == 0 )
			victim = vch;
		    count++;
		}
	    }

	    if ( victim == NULL )
		continue;

            if(IS_SET(ch->off_flags, OFF_BACKSTAB))
	        multi_hit( ch, victim, gsn_backstab );
            else
	        multi_hit( ch, victim, TYPE_UNDEFINED );
	}
    }

    return;
}



/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */

void update_handler( void )
{
    static int	pulse_area;
    static int	pulse_mobile;
    static int	pulse_violence;
    static int	pulse_point;
    static int	pulse_music;
    static int	pulse_item;
    static int	pulse_mob;

    if ( --pulse_area     <= 0 )
    {
	pulse_area	= PULSE_AREA;
	/* number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 ); */
	area_update	( );
    }

    if ( --pulse_music	  <= 0 )
    {
	pulse_music	= PULSE_MUSIC;
	song_update();
    }

    if ( --pulse_mobile   <= 0 )
    {
	pulse_mobile	= PULSE_MOBILE;
	mobile_update	( );
    }

    if ( --pulse_violence <= 0 )
    {
	pulse_violence	= PULSE_VIOLENCE;
	violence_update	( );
    }

    if ( --pulse_point    <= 0 )
    {
	wiznet("TICK!",NULL,NULL,WIZ_TICKS,0,0);
	pulse_point     = PULSE_TICK;
/* number_range( PULSE_TICK / 2, 3 * PULSE_TICK / 2 ); */
	weather_update	( );
	char_update	( );
	obj_update	( );
        quest_update    ( );
        html_who	( );
/*	randomize_entrances ( ROOM_VNUM_CHAIN );*/

        if(arena.timer > 0 && --arena.timer <= 0)
            arena.status = ARENA_STATUS_FINISHING;

    }

    if ( --pulse_item <= 0 )
    {
        pulse_item	= PULSE_ITEM;
	generate_items	( );
    }

    if ( --pulse_mob <= 0 )
    {
	pulse_mob	= PULSE_MOB;
	generate_mobs	( );
    }

    aggr_update( );


    if(arena.status == ARENA_STATUS_FINISHING)
        end_arena(TRUE);

    tail_chain( );
    return;
}

/*  */
int micro_hit_gain( CHAR_DATA *ch )
{
    int gain;
    int number;

    if (ch->in_room == NULL)
	return 0;

    if ( IS_NPC(ch) )
    {
	gain =  5 + ch->level/2;

	switch(ch->position)
	{
	    default : 		gain /= 2;			break;
	    case POS_SLEEPING: 	gain = 3 * gain/2;		break;
	    case POS_RESTING:  					break;
	    case POS_FIGHTING:	gain /= 3;		 	break;
 	}

	
    }
    else
    {
	gain = UMAX(5,get_curr_stat(ch,STAT_CON) - 3 + ch->level/2); 
	gain += class_table[ch->class].hp_max * 1.5 - 10;

        gain /= 2; // for microhealing - default every 10 sec

 	number = number_percent();

	switch ( ch->position )
	{
	    default:	   	gain /= 4;			break;
	    case POS_SLEEPING: 					break;
	    case POS_RESTING:  	gain /= 2;			break;
	    case POS_FIGHTING: 	gain /= 6;			break;
	}

	if ( ch->pcdata->condition[COND_FULL]   > 40 )
	    gain *= 1.2;

	if ( ch->pcdata->condition[COND_HUNGER]   == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 3;

    }

    if(!IS_NPC(ch) && ch->desc == NULL && ch->timer > 2)
        gain = gain * UMAX(ch->in_room->heal_rate / 360, 1);
    else
        gain = gain * ch->in_room->heal_rate / 90;
    
    if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
	gain = gain * ch->on->value[3] / 85;

    if ( IS_AFFECTED(ch, AFF_POISON) )
	gain /= 4;

    if (IS_AFFECTED(ch, AFF_PLAGUE))
	gain /= 8;

    if (IS_AFFECTED(ch,AFF_HASTE) || IS_AFFECTED(ch,AFF_SLOW))
	gain /= 2;

    return UMIN(gain, ch->max_hit - ch->hit);
}


int micro_mana_gain( CHAR_DATA *ch )
{
    int gain;

    if (ch->in_room == NULL)
	return 0;

    if ( IS_NPC(ch) )
    {
	gain = 5 + ch->level;
	switch (ch->position)
	{
	    default:		gain /= 2;		break;
	    case POS_SLEEPING:	gain = 3 * gain/2;	break;
   	    case POS_RESTING:				break;
	    case POS_FIGHTING:	gain /= 3;		break;
    	}
    }
    else
    {
	gain = ((get_curr_stat(ch,STAT_WIS) 
	      + get_curr_stat(ch,STAT_INT) + ch->level) / 3) * 2;

        gain *= 0.7;

	if (!class_table[ch->class].fMana)
	    gain /= 2;

	switch ( ch->position )
	{
	    default:		gain /= 4;			break;
	    case POS_SLEEPING: 					break;
	    case POS_RESTING:	gain /= 2;			break;
	    case POS_FIGHTING:	gain /= 6;			break;
	}

	if ( ch->pcdata->condition[COND_FULL]   > 40 )
	    gain *= 0.9;

	if ( ch->pcdata->condition[COND_HUNGER] == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 2;

    }

    gain = gain * ch->in_room->mana_rate / 90;

    if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
	gain = gain * ch->on->value[4] / 90;

    if ( IS_AFFECTED( ch, AFF_POISON ) )
	gain /= 4;

    if (IS_AFFECTED(ch, AFF_PLAGUE))
        gain /= 8;

    if (IS_AFFECTED(ch,AFF_HASTE) || IS_AFFECTED(ch,AFF_SLOW))
        gain /=2 ;

    return UMIN(gain, ch->max_mana - ch->mana);
}


int micro_move_gain( CHAR_DATA *ch )
{
    int gain;

    if (ch->in_room == NULL)
	return 0;

    if ( IS_NPC(ch) )
    {
	gain = ch->level;
    }
    else
    {
	gain = UMAX( 15, ch->level )/6;

	switch ( ch->position )
	{
	case POS_SLEEPING: gain += get_curr_stat(ch,STAT_DEX) * 2;
                           break;
	case POS_RESTING:  gain += get_curr_stat(ch,STAT_DEX) / 1.5;	
                           break;
	}

	if ( ch->pcdata->condition[COND_HUNGER]   == 0 )
	    gain /= 2;

	if ( ch->pcdata->condition[COND_THIRST] == 0 )
	    gain /= 3;
    }

    gain = gain * ch->in_room->heal_rate/90;

    if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
	gain = gain * ch->on->value[3] / 70;

    if ( IS_AFFECTED(ch, AFF_POISON) )
	gain /= 4;

    if (IS_AFFECTED(ch, AFF_PLAGUE))
        gain /= 8;

    if (IS_AFFECTED(ch,AFF_HASTE) || IS_AFFECTED(ch,AFF_SLOW))
        gain /=2 ;

    gain = UMAX(gain, 3);

    return UMIN(gain, ch->max_move - ch->move);
}


bool start_hit_gain(CHAR_DATA *ch)
{
    EVENT *e;

    if(!microheal_test) return FALSE;

    if( ch->hit >= ch->max_hit )
        return FALSE;

    if( find_event_type(ch->event_list, event_hit_gain) )
        return FALSE;

    e = new_event();
    e->when = current_pulse + 1 * PPS;
    e->actor.ch = ch; 
    e->actor_type = EVENT_ACTOR_CHAR;
    SET_BIT(e->flags,EVENT_REPEAT);
    e->call = event_hit_gain;
            
    event_add_char(e,ch);

    return TRUE;
}

bool start_mana_move_gain(CHAR_DATA *ch)
{
    EVENT *e;

    if(!microheal_test) return FALSE;

    if( ch->mana >= ch->max_mana && ch->move >= ch->max_move )
        return FALSE;

    if( find_event_type(ch->event_list, event_mana_move_gain) )
        return FALSE;

    e = new_event();
    e->when = current_pulse + 1 * PPS;
    e->actor.ch = ch; 
    e->actor_type = EVENT_ACTOR_CHAR;
    SET_BIT(e->flags,EVENT_REPEAT);
    e->call = event_mana_move_gain;
            
    event_add_char(e,ch);
    
    return TRUE;
}

void generate_items( void )
{
    int			i;
    int			j;
    ROOM_INDEX_DATA *	room;
    OBJ_DATA *		object;
    int			room_vnum;

    for (i = 0; i < MAX_RANDOM_ITEMS; i++ )
    {
	for(j = count_obj_vnum_ground(random_items[i].vnum); j > 0; j-- )
	{
	    object = get_obj_vnum_ground(random_items[i].vnum);
	    extract_obj(object);
	}
	
	for(j = 0; j < random_items[i].number; )
	{
            room_vnum = number_range(50, 32000);
	    if( room_vnum >= 2900 && room_vnum < 3400 )
		continue;

            if ( (room = get_room_index(room_vnum) ) != NULL )
            {
		object = create_object( get_obj_index(random_items[i].vnum), GENERATE_LEVEL );
		obj_to_room( object, room );
		j++;
            }
	}
    }
}

void generate_mobs( void )
{
    int                 i;
    int                 j;
    int			k;
    ROOM_INDEX_DATA *   room;
    CHAR_DATA *		mob;
    OBJ_DATA *		object;
    int                 room_vnum;

    for (i = 0; i < MAX_RANDOM_MOBS; i++ )
    {
        for(j = count_char_vnum(random_mobs[i].vnum); j > 0; j-- )
        {
            mob = get_char_vnum(random_mobs[i].vnum);
	    if( mob->fighting == NULL )
		extract_char(mob,TRUE);
        }

	if( number_percent() < 50 )
	    continue;


        for(j = count_char_vnum(random_mobs[i].vnum); j < random_mobs[i].number; )
        {
            room_vnum = number_range(50, 32000);
            if( room_vnum >= 2900 && room_vnum < 3400 )
		continue;

            if ( (room = get_room_index(room_vnum) ) != NULL )
            {
		if( IS_SET(room->area->area_flags, AREA_NOPLAYERS)
                    || IS_SET(room->room_flags,ROOM_SAFE) )
		    continue;

                mob = create_mobile( get_mob_index(random_mobs[i].vnum) );
                char_to_room( mob, room );
                j++;
		for(k=0; random_mobs[i].inventory[k]; k++)
		{
			object = create_object( get_obj_index(random_mobs[i].inventory[k]), GENERATE_LEVEL );
			obj_to_char( object, mob );
		}
            }
        }
    }
}

