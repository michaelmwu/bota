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
#include "event.h"
#include "recycle.h"
#include "math.h"

/* command procedures needed */
DECLARE_DO_FUN(do_backstab	);
DECLARE_DO_FUN(do_emote		);
DECLARE_DO_FUN(do_berserk	);
DECLARE_DO_FUN(do_bash		);
DECLARE_DO_FUN(do_trip		);
DECLARE_DO_FUN(do_dirt		);
DECLARE_DO_FUN(do_flee		);
DECLARE_DO_FUN(do_kick		);
DECLARE_DO_FUN(do_disarm	);
DECLARE_DO_FUN(do_get		);
DECLARE_DO_FUN(do_recall	);
DECLARE_DO_FUN(do_yell		);
DECLARE_DO_FUN(do_shout		);
DECLARE_DO_FUN(do_sacrifice	);
DECLARE_DO_FUN(do_circle	);
DECLARE_DO_FUN(do_feed		);
DECLARE_DO_FUN(do_gouge		);
DECLARE_DO_FUN(do_vdpi		);
DECLARE_DO_FUN(do_vdtr		);
DECLARE_DO_FUN(do_vdth		);
DECLARE_DO_FUN(do_look		);
DECLARE_DO_FUN(do_sit		);


/*
 * Local functions.
 */
void	check_assist	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_dodge	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_phase	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_parry	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_shield_block     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    dam_message 	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
                            int dt, bool immune ) );
void	death_cry	args( ( CHAR_DATA *ch ) );
void	group_gain	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	xp_compute	args( ( CHAR_DATA *gch, CHAR_DATA *victim, 
			    int total_levels ) );
bool    check_critical  args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_safe		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_safe_mock	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_voodood	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	make_corpse	args( ( CHAR_DATA *ch, CHAR_DATA *killer ) );
void	one_hit	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool secondary ) );
void	one_hit_mock args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool secondary ) );
void    mob_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	raw_kill	args( ( CHAR_DATA *victim, CHAR_DATA *killer ) );
void	set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	disarm		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

PK_COUNT_DATA *get_create_pk_count(PK_COUNT_DATA **list, char *name);

void update_pk_count(CHAR_DATA *ch, CHAR_DATA *victim);

void arena_death (CHAR_DATA *ch, CHAR_DATA *killer);

/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *victim;

    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	ch_next	= ch->next;

	if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
	    continue;

	if ( IS_AWAKE(ch) && ch->in_room == victim->in_room )
	    multi_hit( ch, victim, TYPE_UNDEFINED );
	else
	    stop_fighting( ch, FALSE );

        ch->fight_rounds++;

	if ( ( victim = ch->fighting ) == NULL )
	    continue;

	/*
	 * Fun for the whole family!
	 */
	check_assist(ch,victim);

	if ( IS_NPC( ch ) )
	{
	    if ( HAS_TRIGGER( ch, TRIG_FIGHT ) )
		mp_percent_trigger( ch, victim, NULL, NULL, TRIG_FIGHT );
	    if ( HAS_TRIGGER( ch, TRIG_HPCNT ) )
		mp_hprct_trigger( ch, victim );
	}
    }

    return;
}

/* for auto assisting */
void check_assist(CHAR_DATA *ch,CHAR_DATA *victim)
{
    CHAR_DATA *rch, *rch_next;

    for (rch = ch->in_room->people; rch != NULL; rch = rch_next)
    {
	rch_next = rch->next_in_room;
	
	if (IS_AWAKE(rch) && rch->fighting == NULL)
	{
	    /* quick check for ASSIST_PLAYER */
	    if (!IS_NPC(ch) && IS_NPC(rch) 
	    && IS_SET(rch->off_flags,ASSIST_PLAYERS)
            && (!rch->in_arena || rch->arena_team == ch->arena_team)
	    &&  rch->level + 6 > victim->level)
	    {
		do_emote(rch,"{Rscreams and attacks!{x");
		multi_hit(rch,victim,TYPE_UNDEFINED);
		continue;
	    }

	    /* PCs next */
	    if (!IS_NPC(ch) || IS_AFFECTED(ch,AFF_CHARM))
	    {
		if ( ( (!IS_NPC(rch) && IS_SET(rch->act,PLR_AUTOASSIST))
		||     IS_AFFECTED(rch,AFF_CHARM)) 
		&&   (is_same_group(ch,rch)
                  || (ch->in_arena && rch->in_arena
                   && ch->arena_team == rch->arena_team)
                     )
		&&   !is_safe(rch, victim))
		    multi_hit (rch,victim,TYPE_UNDEFINED);
		
		continue;
	    }
  	
	    /* now check the NPC cases */
	    
 	    if (IS_NPC(ch) && !IS_AFFECTED(ch,AFF_CHARM))
	
	    {
		if ( (IS_NPC(rch) && IS_SET(rch->off_flags,ASSIST_ALL))

		||   (IS_NPC(rch) && rch->group && rch->group == ch->group)

		||   (IS_NPC(rch) && rch->race == ch->race 
		   && IS_SET(rch->off_flags,ASSIST_RACE))

		||   (IS_NPC(rch) && IS_SET(rch->off_flags,ASSIST_ALIGN)
		   &&   ((IS_GOOD(rch)    && IS_GOOD(ch))
		     ||  (IS_EVIL(rch)    && IS_EVIL(ch))
		     ||  (IS_NEUTRAL(rch) && IS_NEUTRAL(ch)))) 

		||   (rch->pIndexData == ch->pIndexData 
		   && IS_SET(rch->off_flags,ASSIST_VNUM)))

	   	{
		    CHAR_DATA *vch;
		    CHAR_DATA *target;
		    int number;

		    if (number_bits(1) == 0)
			continue;
		
		    target = NULL;
		    number = 0;
		    for (vch = ch->in_room->people; vch; vch = vch->next)
		    {
			if (can_see(rch,vch)
			&&  is_same_group(vch,victim)
			&&  number_range(0,number) == 0)
			{
			    target = vch;
			    number++;
			}
		    }

		    if (target != NULL)
		    {
			do_emote(rch,"{Rscreams and attacks!{x");
			multi_hit(rch,target,TYPE_UNDEFINED);
		    }
		}	
	    }
	}
    }
}

bool check_critical(CHAR_DATA *ch, CHAR_DATA *victim)
{
        OBJ_DATA *obj;

        obj = get_eq_char(ch,WEAR_WIELD);

        if (
             ( get_eq_char(ch,WEAR_WIELD) == NULL ) || 
             ( get_skill(ch,gsn_critical)  <  1 ) ||
             ( get_weapon_skill(ch,get_weapon_sn(ch))  !=  100 ) ||
             ( number_range(0,100) > get_skill(ch,gsn_critical) )
           )
                return FALSE;


        if ( number_range(0,100) > 25 )
                return FALSE;


        /* Now, if it passed all the tests... */

        check_improve(ch,gsn_critical,TRUE,6);
        return TRUE;
}

/*
 * Do one group of attacks.
 */
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    int     chance;

    /* decrement the wait */
    if (ch->desc == NULL)
	ch->wait = UMAX(0,ch->wait - PULSE_VIOLENCE);

    if (ch->desc == NULL)
	ch->daze = UMAX(0,ch->daze - PULSE_VIOLENCE); 


    /* no attacks for stunnies -- just a check */
    if (ch->position < POS_RESTING)
	return;

    if (ch->stunned)
    {
	ch->stunned--;
	if (!ch->stunned)
	{
	    send_to_char( "You regain your equilibrium.\n\r", ch);
	    act( "$n regains $s equilibrium.", ch, NULL, NULL, TO_ROOM );
	}
        return;
    }

    if (IS_NPC(ch))
    {
	mob_hit(ch,victim,dt);
	return;
    }

    one_hit( ch, victim, dt, FALSE );

    if (get_eq_char (ch, WEAR_SECONDARY))
    {
	chance = (get_skill(ch,gsn_dual_wield)/3)*2;
	chance += 33;
	if ( number_percent( ) < chance )
	{
	    one_hit( ch, victim, dt, TRUE );
	    if ( get_skill(ch,gsn_dual_wield) != 0 && (!IS_NPC(ch)
	    && ch->level >= skill_table[gsn_dual_wield].skill_level[ch->class]))
	    {
		check_improve(ch,gsn_dual_wield,TRUE,1);
	    }
	}
        if ( ch->fighting != victim )
            return;
    }

    if (ch->fighting != victim)
	return;

    if (IS_AFFECTED(ch,AFF_HASTE))
	one_hit(ch,victim,dt, FALSE);

    if ( ch->fighting != victim || dt == gsn_backstab || dt == gsn_circle)
	return;

    chance = get_skill(ch,gsn_second_attack)/2;

    if (IS_AFFECTED(ch,AFF_SLOW))
	chance /= 2;

    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt, FALSE );
	check_improve(ch,gsn_second_attack,TRUE,5);
	if ( ch->fighting != victim )
	    return;
    } else {
	return;
    }

    chance = get_skill(ch,gsn_third_attack)/2;

    if (IS_AFFECTED(ch,AFF_SLOW))
	chance /= 2;

    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt, FALSE );
	check_improve(ch,gsn_third_attack,TRUE,6);
	if ( ch->fighting != victim )
	    return;
    } else {
	return;
    }

    chance = get_skill(ch, gsn_fourth_attack)/2;

    if (IS_AFFECTED(ch, AFF_SLOW))
	chance /= 3;

    if (number_percent( ) < chance)
    {
	one_hit( ch, victim, dt, FALSE );
	check_improve(ch, gsn_fourth_attack, TRUE, 6);
	if (ch->fighting != victim)
	    return;
    } else {
	return;
    }

    chance = get_skill(ch, gsn_fifth_attack)/2;

    if (IS_AFFECTED(ch, AFF_SLOW))
	chance = 0;

    if (number_percent( ) < chance)
    {
	one_hit( ch, victim, dt, FALSE );
	check_improve(ch, gsn_fifth_attack, TRUE, 6);
	if (ch->fighting != victim)
	    return;
    }

    return;
}

/* procedure for all mobile attacks */
void mob_hit (CHAR_DATA *ch, CHAR_DATA *victim, int dt)
{
    int chance,number, i;
    CHAR_DATA *vch, *vch_next;
    bool cantdo = FALSE;

    one_hit(ch,victim,dt, FALSE);

    if (ch->fighting != victim)
	return;

    if (ch->stunned)
	return;

    /* Area attack -- BALLS nasty! */
 
    if (IS_SET(ch->off_flags,OFF_AREA_ATTACK))
    {
	for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
	{
	    vch_next = vch->next;
	    if ((vch != victim && vch->fighting == ch))
		one_hit(ch,vch,dt, FALSE);
	}
    }

    if (ch->fighting != victim)
	return;

    if (get_eq_char (ch, WEAR_SECONDARY))
    {
	chance = (get_skill(ch,gsn_dual_wield)/3)*2;
	chance += 33;
	if ( number_percent( ) < chance )
	{
	    one_hit( ch, victim, dt, TRUE );
	}
        if ( ch->fighting != victim )
            return;
    }

    if (IS_AFFECTED(ch,AFF_HASTE) 
    ||  (IS_SET(ch->off_flags,OFF_FAST) && !IS_AFFECTED(ch,AFF_SLOW)))
	one_hit(ch,victim,dt, FALSE);

    if (ch->fighting != victim || dt == gsn_backstab || dt == gsn_circle)
	return;

    chance = get_skill(ch,gsn_second_attack)/2;

    if (IS_AFFECTED(ch,AFF_SLOW) && !IS_SET(ch->off_flags,OFF_FAST))
	chance /= 2;

    if (number_percent() < chance)
    {
	one_hit(ch,victim,dt, FALSE);
	if (ch->fighting != victim)
	    return;
	chance = get_skill(ch,gsn_third_attack)/2;

	if (IS_AFFECTED(ch,AFF_SLOW) && !IS_SET(ch->off_flags,OFF_FAST))
	    chance /= 2;

	if (number_percent() < chance)
	{
	    one_hit(ch,victim,dt, FALSE);
	    if (ch->fighting != victim)
		return;

	    chance = get_skill(ch, gsn_fourth_attack)/2;

	    if (IS_AFFECTED(ch, AFF_SLOW) && !IS_SET(ch->off_flags,OFF_FAST))
		chance /= 3;

	    if (number_percent( ) < chance)
	    {
		one_hit(ch,victim,dt, FALSE);
		if (ch->fighting != victim)
		    return;

		chance = get_skill(ch, gsn_fifth_attack)/2;

		if (IS_AFFECTED(ch, AFF_SLOW) && !IS_SET(ch->off_flags,OFF_FAST))
		    chance = 0;

		if (number_percent( ) < chance)
		{
		    one_hit(ch,victim,dt, FALSE);
		    if (ch->fighting != victim)
			return;
		}
	    }
	}
    }

    /* oh boy!  Fun stuff! */

    if (ch->wait > 0)
	return;

    number = number_range(0,2);

    if (number == 1 && IS_SET(ch->act,ACT_MAGE))
    {
	/*  { mob_cast_mage(ch,victim); return; } */ ;
    }

    if (number == 2 && IS_SET(ch->act,ACT_CLERIC))
    {	
	/* { mob_cast_cleric(ch,victim); return; } */ ;
    }

    /* now for the skills */

  for(i = 0; i < 3; i++)
  {
    cantdo = FALSE;
    number = number_range(0,10);

    switch(number) 
    {
    case (0) :
	if (IS_SET(ch->off_flags,OFF_BASH))
	    do_bash(ch,"");
	break;

    case (1) :
	if (IS_SET(ch->off_flags,OFF_BERSERK) && !IS_AFFECTED(ch,AFF_BERSERK))
	    do_berserk(ch,"");
        else cantdo = TRUE;
	break;


    case (2) :
	if (IS_SET(ch->off_flags,OFF_DISARM) 
	|| (get_weapon_sn(ch) != gsn_hand_to_hand 
	&& (IS_SET(ch->act,ACT_WARRIOR)
	||  IS_SET(ch->act,ACT_VAMPIRE)
   	||  IS_SET(ch->act,ACT_THIEF))))
	    do_disarm(ch,"");
        else cantdo = TRUE;

	break;

    case (3) :
	if (IS_SET(ch->off_flags,OFF_KICK))
	    do_kick(ch,"");
        else cantdo = TRUE;

	break;

    case (4) :
	if (IS_SET(ch->off_flags,OFF_KICK_DIRT))
	    do_dirt(ch,"");
        else cantdo = TRUE;

	break;

    case (5) :
	if (IS_SET(ch->off_flags,OFF_TAIL))
	{
	    /* do_tail(ch,"") */ ;
	}
	break; 

    case (6) :
	if (IS_SET(ch->off_flags,OFF_TRIP))
	    do_trip(ch,"");
        else cantdo = TRUE;

	break;

    case (7) :
/*	if((IS_SET(ch->off_flags,OFF_CRUSH))
	{
	    do_crush(ch,"");
	}*/

	if (IS_SET(ch->off_flags,OFF_BACKSTAB))
	{
	    do_circle(ch,"");
	}
        else cantdo = TRUE;

	break;
    case (8) :
        break;
    }

    if(!cantdo) return;

  }

}
	

/*
 * Hit one guy once.
 */
void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool secondary )
{
    OBJ_DATA *wield;
    int victim_ac;
    int thac0;
    int thac0_00;
    int thac0_32;
    int dam;
    int diceroll;
    int sn,skill;
    int dam_type;
    bool result;
    bool critical;

    sn = -1;


    /* just in case */
    if (victim == ch || ch == NULL || victim == NULL)
	return;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;

    /*
     * Figure out the type of damage message.
     * if secondary == true, use the second weapon.
     */
    if (!secondary)
        wield = get_eq_char( ch, WEAR_WIELD );
    else
        wield = get_eq_char( ch, WEAR_SECONDARY );
    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
	else 
	    dt += ch->dam_type;
    }

    if (dt < TYPE_HIT)
    	if (wield != NULL)
    	    dam_type = attack_table[wield->value[3]].damage;
    	else
    	    dam_type = attack_table[ch->dam_type].damage;
    else
    	dam_type = attack_table[dt - TYPE_HIT].damage;

    if (dam_type == -1)
	dam_type = DAM_BASH;

    /* get the weapon skill */
    sn = get_weapon_sn(ch);
    skill = 20 + get_weapon_skill(ch,sn);

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if ( IS_NPC(ch) )
    {
	thac0_00 = 20;
	thac0_32 = -4;   /* as good as a thief */ 
	if (IS_SET(ch->act,ACT_VAMPIRE))
	    thac0_32 = -30;
        else if (IS_SET(ch->act,ACT_DRUID))
            thac0_32 = 0;
        else if (IS_SET(ch->act,ACT_RANGER))
            thac0_32 = -4;
	else if (IS_SET(ch->act,ACT_WARRIOR))
	    thac0_32 = -10;
	else if (IS_SET(ch->act,ACT_THIEF))
	    thac0_32 = -4;
	else if (IS_SET(ch->act,ACT_CLERIC))
	    thac0_32 = 2;
	else if (IS_SET(ch->act,ACT_MAGE))
	    thac0_32 = 6;
    }
    else
    {
	thac0_00 = class_table[ch->class].thac0_00;
	thac0_32 = class_table[ch->class].thac0_32;
    }
    thac0  = interpolate32( ch->level, thac0_00, thac0_32 );

    if (thac0 < 0)
        thac0 = thac0/2;

    if (thac0 < -5)
        thac0 = -5 + (thac0 + 5) / 2;

    thac0 -= GET_HITROLL(ch) * skill/100;
    thac0 += 5 * (100 - skill) / 100;

    if (dt == gsn_backstab && !IS_NPC(ch))
	thac0 -= 10 * (100 - get_skill(ch,gsn_backstab));

    if (dt == gsn_circle)
	thac0 -= 8 * (100 - get_skill(ch,gsn_circle));

    if ( IS_SHIELDED(victim, SHD_VACILLATING) && dt == gsn_backstab)
        thac0 += !IS_NPC(ch) ?
           10 * get_skill(victim,gsn_vacillating_aura) + 30 :
            UMIN(victim->level,100);
    
    switch(dam_type)
    {
	case(DAM_PIERCE):victim_ac = GET_AC(victim,AC_PIERCE)/10;	break;
	case(DAM_BASH):	 victim_ac = GET_AC(victim,AC_BASH)/10;		break;
	case(DAM_SLASH): victim_ac = GET_AC(victim,AC_SLASH)/10;	break;
	default:	 victim_ac = GET_AC(victim,AC_EXOTIC)/10;	break;
    }; 
	
    if (victim_ac < -15)
	victim_ac = (victim_ac + 15) / 5 - 15;
     
    if ( !can_see( ch, victim ) )
	victim_ac -= 4;


    if ( victim->position < POS_FIGHTING)
	victim_ac += 4;
 
    if (victim->position < POS_RESTING)
	victim_ac += 6;

    /*
     * The moment of excitement!
     */
    while ( ( diceroll = number_bits( 5 ) ) >= 20 )
	;

    if ( diceroll == 0
    || ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
    {
	/* Miss. */
	damage( ch, victim, 0, dt, dam_type, TRUE );
	tail_chain( );
	return;
    }

    /*
     * Hit.
     * Calc damage.
     */


    if ( IS_NPC(ch) && (!ch->pIndexData->new_format || wield == NULL))
	if (!ch->pIndexData->new_format)
	{
	    dam = number_range( ch->level / 2, ch->level * 3 / 2 );
	    if ( wield != NULL )
	    	dam += dam / 2;
	}
	else
	    dam = dice(ch->damage[DICE_NUMBER],ch->damage[DICE_TYPE]);
	
    else
    {
	if (sn != -1)
	    check_improve(ch,sn,TRUE,5);
	if ( wield != NULL )
	{
	    if (wield->clan)
	    {
		dam = dice(ch->level/3,3) * skill/100;
	    } else
	    {
		if (wield->pIndexData->new_format)
		    dam = dice(wield->value[1],wield->value[2]) * skill/100;
		else
	    	    dam = number_range( wield->value[1] * skill/100, 
				wield->value[2] * skill/100);
	    }

	    if (get_eq_char(ch,WEAR_SHIELD) == NULL)  /* no shield = more */
		dam = dam * 11/10;

	    /* sharpness! */
	    if (IS_WEAPON_STAT(wield,WEAPON_SHARP))
	    {
		int percent;

		if ((percent = number_percent()) <= (skill / 8))
		    dam = 2 * dam + (dam * 2 * percent / 100);
	    }
	}
	else
	    dam = number_range( 1 + 4 * skill/100, 2 * ch->level/3 * skill/100);
    }

    /*
     * Bonuses.
     */
    if ( get_skill(ch,gsn_enhanced_damage) > 0 )
    {
        diceroll = number_percent();
        if (diceroll <= get_skill(ch,gsn_enhanced_damage))
        {
            check_improve(ch,gsn_enhanced_damage,TRUE,6);
            dam += 2 * ( dam * diceroll/300);
        }
    }

    if ( !IS_AWAKE(victim) )
	dam *= 2;
     else if (victim->position < POS_FIGHTING)
	dam = dam * 3 / 2;

    if ( dt == gsn_backstab && wield != NULL) 
    {
    	if ( wield->value[0] != 2 )
	    dam *= 1.9 + (ch->level / 16); 
	else 
	    dam *= 2 + (ch->level / 13);
    }

    if ( dt == gsn_circle && wield != NULL) 
    {
    	if ( wield->value[0] != 2 )
	    dam *= 2 + (ch->level / 20);  /* was 15 */
	else 
	    dam *= 2 + (ch->level / 18); /* was 12 */
    }

    if ( dt == gsn_furor )
    {
        if(can_see(ch,victim))
            dam = (dam * 7)/6;
        else
            dam = (dam * 5)/10;
    }


    dam += GET_DAMROLL(ch) * UMIN(100,skill) /100;

    if ( dam <= 0 )
	dam = 1;

    if(IS_SHIELDED(ch, SHD_VACILLATING))
      dam *= 0.7;

    if ( ( critical = check_critical(ch,victim) ) )
        dam *= 1.40;

    result = damage( ch, victim, dam, dt, dam_type, TRUE );

    if ( critical && result )
    {
        act("$p CRITICALLY STRIKES $n!",victim,wield,NULL,TO_NOTVICT);
        act("CRITICAL STRIKE!",ch,NULL,victim,TO_VICT);
    }
    
    /* but do we have a funky weapon? */
    if (result && wield != NULL)
    { 
	int dam;

	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_POISON))
	{
	    int level;
	    AFFECT_DATA *poison, af;

	    if ((poison = affect_find(wield->affected,gsn_poison)) == NULL 
	     || (poison = affect_find(wield->affected,gsn_accelerate_poison))
                == NULL)
		level = wield->level;
	    else
		level = poison->level;
	
	    if (!saves_spell(level / 2,victim,DAM_POISON)) 
	    {
		send_to_char("{cYou feel {ypoison{c coursing through your veins.{x",
		    victim);
		act("$n is {ypoisoned{x by the venom on $p.",
		    victim,wield,NULL,TO_ROOM);

    		af.where     = TO_AFFECTS;
    		af.type      = gsn_poison;
    		af.level     = level * 3/4;
    		af.duration  = level / 2;
    		af.location  = APPLY_STR;
    		af.modifier  = -1;
    		af.bitvector = AFF_POISON;
    		affect_join( victim, &af );
	    }

	    /* weaken the poison if it's temporary */
	    if (poison != NULL)
	    {
	    	poison->level = UMAX(0,poison->level - 2);
	    	poison->duration = UMAX(0,poison->duration - 1);
	
	    	if (poison->level == 0 || poison->duration == 0)
		    act("The {ypoison{x on $p has worn off.",ch,wield,NULL,TO_CHAR);
	    }
 	}

        if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_HOLY))
        {
            dam = number_range(1, wield->level / 5 + 1);
            act("{k$p draws life from $n.{x",victim,wield,NULL,TO_ROOM);
            act("{iYou feel $p drawing your life away.{x",
                victim,wield,NULL,TO_CHAR);
            damage_old(ch,victim,dam,0,DAM_HOLY,FALSE);
            ch->alignment = UMIN(1000,ch->alignment + 2);
            if ( ch->pet != NULL )
                ch->pet->alignment = ch->alignment;
            ch->hit += dam/2;
        }

    	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_VAMPIRIC))
	{
	    dam = number_range(1, wield->level / 5 + 1);
	    act("{k$p draws life from $n.{x",victim,wield,NULL,TO_ROOM);
	    act("{iYou feel $p drawing your life away.{x",
		victim,wield,NULL,TO_CHAR);
	    damage_old(ch,victim,dam,0,DAM_NEGATIVE,FALSE);
	    ch->alignment = UMAX(-1000,ch->alignment - 2);
	    if ( ch->pet != NULL )
		ch->pet->alignment = ch->alignment;
	    ch->hit += dam/2;
	}

	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FLAMING))
	{
	    dam = number_range(1,wield->level / 5 + 1);
	    act("{k$n is {rburned{k by $p.{x",victim,wield,NULL,TO_ROOM);
	    act("{i$p {rsears{i your flesh.{x",victim,wield,NULL,TO_CHAR);
	    fire_effect( (void *) victim,
                IS_NPC(ch) ? wield->level/6 : wield->level/12,dam,TARGET_CHAR);
	    damage(ch,victim,dam,0,DAM_FIRE,FALSE);
	}

	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FROST))
	{
	    dam = number_range(1,wield->level / 6 + 2);
	    act("{k$p {cfreezes{k $n.{x",victim,wield,NULL,TO_ROOM);
	    act("{iThe {Ccold{i touch of $p surrounds you with {Cice.{x",
		victim,wield,NULL,TO_CHAR);
	    cold_effect(victim,wield->level/2,dam,TARGET_CHAR);
	    damage(ch,victim,dam,0,DAM_COLD,FALSE);
	}

	if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_SHOCKING))
	{
	    dam = number_range(1,wield->level/5 + 2);
	    act("{k$n is struck by {Ylightning{k from $p.{x",victim,wield,NULL,TO_ROOM);
	    act("{iYou are {Yshocked{i by $p.{x",victim,wield,NULL,TO_CHAR);
	    shock_effect(victim,wield->level/2,dam,TARGET_CHAR);
	    damage(ch,victim,dam,0,DAM_LIGHTNING,FALSE);
	}
    }
    if (ch->fighting == victim);
    {
	if (result)
	{
	    if (IS_SHIELDED(victim, SHD_ICE))
	    {
		if (!IS_SHIELDED(ch, SHD_ICE))
		{
		    dt = skill_lookup("iceshield");
		    dam = number_range(5, 15);
		    damage_old(victim, ch, dam, dt, DAM_COLD, TRUE);
		}
	    }
	    if (IS_SHIELDED(victim, SHD_FIRE))
	    {
		if (!IS_SHIELDED(ch, SHD_FIRE))
		{
		    dt = skill_lookup("fireshield");
		    dam = number_range(10, 20);
		    damage_old(victim, ch, dam, dt, DAM_FIRE, TRUE);
		}
	    }
	    if (IS_SHIELDED(victim, SHD_SHOCK))
	    {
		if (!IS_SHIELDED(ch, SHD_SHOCK))
		{
		    dt = skill_lookup("shockshield");
		    dam = number_range(15, 25);
		    damage_old(victim, ch, dam, dt, DAM_LIGHTNING, TRUE);
		}
	    }
	}

    }

    if( IS_SHIELDED( victim, SHD_VACILLATING ) )
    {

      if(dt != gsn_backstab)
      {
        send_to_char( "{yYour {Bfluctuating aura{y has been disrupted.{x\n\r", victim);
        act( "{y$n's {Bfluctuating aura{y dissapates.{x", victim, NULL, NULL, TO_ROOM );
        affect_strip( victim, gsn_vacillating_aura );
        REMOVE_BIT(ch->shielded_by,SHD_VACILLATING);
      }
      else
      {
        send_to_char( "{yYour {Bfluctuating aura{y sparks and wavers.{x\n\r", victim);
        act( "{y$n's {Bfluctuating aura{y sparks and wavers.{x", victim, NULL, NULL, TO_ROOM );
      }

    }


    tail_chain( );
    return;
}

/*
 * Mock hit one guy once.
 */
void one_hit_mock( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool secondary )
{
    OBJ_DATA *wield;
    int victim_ac;
    int thac0;
    int thac0_00;
    int thac0_32;
    int dam;
    int diceroll;
    int sn,skill;
    int dam_type;
    bool result;

    sn = -1;


    /* just in case */
    if (ch == NULL || victim == NULL)
	return;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;

    /*
     * Figure out the type of damage message.
     * if secondary == true, use the second weapon.
     */
    if (!secondary)
        wield = get_eq_char( ch, WEAR_WIELD );
    else
        wield = get_eq_char( ch, WEAR_SECONDARY );
    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
	else 
	    dt += ch->dam_type;
    }

    if (dt < TYPE_HIT)
    	if (wield != NULL)
    	    dam_type = attack_table[wield->value[3]].damage;
    	else
    	    dam_type = attack_table[ch->dam_type].damage;
    else
    	dam_type = attack_table[dt - TYPE_HIT].damage;

    if (dam_type == -1)
	dam_type = DAM_BASH;

    /* get the weapon skill */
    sn = get_weapon_sn(ch);
    skill = 20 + get_weapon_skill(ch,sn);

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if ( IS_NPC(ch) )
    {
	thac0_00 = 20;
	thac0_32 = -4;   /* as good as a thief */ 
        if (IS_SET(ch->act,ACT_VAMPIRE))
            thac0_32 = -30;
        else if (IS_SET(ch->act,ACT_DRUID))
            thac0_32 = 0;
        else if (IS_SET(ch->act,ACT_RANGER))
            thac0_32 = -4;
        else if (IS_SET(ch->act,ACT_WARRIOR))
	    thac0_32 = -10;
	else if (IS_SET(ch->act,ACT_THIEF))
	    thac0_32 = -4;
	else if (IS_SET(ch->act,ACT_CLERIC))
	    thac0_32 = 2;
	else if (IS_SET(ch->act,ACT_MAGE))
	    thac0_32 = 6;
    }
    else
    {
	thac0_00 = class_table[ch->class].thac0_00;
	thac0_32 = class_table[ch->class].thac0_32;
    }
    thac0  = interpolate32( ch->level, thac0_00, thac0_32 );

    if (thac0 < 0)
        thac0 = thac0/2;

    if (thac0 < -5)
        thac0 = -5 + (thac0 + 5) / 2;

    thac0 -= GET_HITROLL(ch) * skill/100;
    thac0 += 5 * (100 - skill) / 100;

    if (dt == gsn_backstab)
	thac0 -= 10 * (100 - get_skill(ch,gsn_backstab));

    if (dt == gsn_circle)
	thac0 -= 10 * (100 - get_skill(ch,gsn_circle));

    switch(dam_type)
    {
	case(DAM_PIERCE):victim_ac = GET_AC(victim,AC_PIERCE)/10;	break;
	case(DAM_BASH):	 victim_ac = GET_AC(victim,AC_BASH)/10;		break;
	case(DAM_SLASH): victim_ac = GET_AC(victim,AC_SLASH)/10;	break;
	default:	 victim_ac = GET_AC(victim,AC_EXOTIC)/10;	break;
    }; 
	
    if (victim_ac < -15)
	victim_ac = (victim_ac + 15) / 5 - 15;
     
    if ( !can_see( ch, victim ) )
	victim_ac -= 10;

    if ( victim->position < POS_FIGHTING)
	victim_ac += 4;
 
    if (victim->position < POS_RESTING)
	victim_ac += 6;

    /*
     * The moment of excitement!
     */
    while ( ( diceroll = number_bits( 5 ) ) >= 20 )
	;

    if ( diceroll == 0
    || ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
    {
	/* Miss. */
	damage_mock( ch, victim, 0, dt, dam_type, TRUE );
	tail_chain( );
	return;
    }

    /*
     * Hit.
     * Calc damage.
     */
    if ( IS_NPC(ch) && (!ch->pIndexData->new_format || wield == NULL))
	if (!ch->pIndexData->new_format)
	{
	    dam = number_range( ch->level / 2, ch->level * 3 / 2 );
	    if ( wield != NULL )
	    	dam += dam / 2;
	}
	else
	    dam = dice(ch->damage[DICE_NUMBER],ch->damage[DICE_TYPE]);
	
    else
    {
	if (sn != -1)
	    check_improve(ch,sn,TRUE,5);
	if ( wield != NULL )
	{
	    if (wield->clan)
	    {
		dam = dice(ch->level/3,3) * skill/100;
	    } else
	    {
		if (wield->pIndexData->new_format)
		    dam = dice(wield->value[1],wield->value[2]) * skill/100;
		else
	    	    dam = number_range( wield->value[1] * skill/100, 
				wield->value[2] * skill/100);
	    }

	    if (get_eq_char(ch,WEAR_SHIELD) == NULL)  /* no shield = more */
		dam = dam * 11/10;

	    /* sharpness! */
	    if (IS_WEAPON_STAT(wield,WEAPON_SHARP))
	    {
		int percent;

		if ((percent = number_percent()) <= (skill / 8))
		    dam = 2 * dam + (dam * 2 * percent / 100);
	    }
	}
	else
	    dam = number_range( 1 + 4 * skill/100, 2 * ch->level/3 * skill/100);
    }

    /*
     * Bonuses.
     */
    if ( get_skill(ch,gsn_enhanced_damage) > 0 )
    {
        diceroll = number_percent();
        if (diceroll <= get_skill(ch,gsn_enhanced_damage))
        {
            check_improve(ch,gsn_enhanced_damage,TRUE,6);
            dam += 2 * ( dam * diceroll/300);
        }
    }

    if ( !IS_AWAKE(victim) )
	dam *= 2;
     else if (victim->position < POS_FIGHTING)
	dam = dam * 3 / 2;

    if ( dt == gsn_backstab && wield != NULL) 
    {
    	if ( wield->value[0] != 2 )
	    dam *= 1.9 + (ch->level / 10); 
	else 
	    dam *= 2 + (ch->level / 8);
    }

    if ( dt == gsn_circle && wield != NULL) 
    {
    	if ( wield->value[0] != 2 )
	    dam *= 2 + (ch->level / 17); 
	else 
	    dam *= 2 + (ch->level / 13);
    }

    dam += GET_DAMROLL(ch) * UMIN(100,skill) /100;

    if ( dam <= 0 )
	dam = 1;

    result = damage_mock( ch, victim, dam, dt, dam_type, TRUE );
    
    tail_chain( );
    return;
}

/*
 * Inflict damage from a hit.
 */
bool damage(CHAR_DATA *ch,CHAR_DATA *victim,int dam,int dt,int dam_type,
	    bool show) 
{
    int mana_lost;

    OBJ_DATA *corpse;
    bool immune;

    if ( victim->position == POS_DEAD )
	return FALSE;

    /*
     * Stop up any residual loopholes.
     */
    if ( dam > 2500 && dt >= TYPE_HIT && !IS_IMMORTAL(ch) && !IS_NPC(ch))
    {
	log_string(LOG_GAME, "Damage: %s: %d: more than 2500 points!",ch->name, dam );
	dam = 2500;
	if (!IS_IMMORTAL(ch) && 0)
	{
	    OBJ_DATA *obj;
	    obj = get_eq_char( ch, WEAR_WIELD );
	    send_to_char("{cYou {z{Breally{x{c shouldn't cheat.{x\n\r",ch);
	    if (obj != NULL)
	    	extract_obj(obj);
	}

    }

    if ( dam > 3000 ) /* Anything else... */
    {
/*        wiznet("$N did over 3000 damage!",ch,NULL,WIZ_BUGS,0,0);*/
	        dam = 3000 - number_range(400,800);
    }

    
    /* damage reduction */
    if ( dam > 65)
	dam = (dam - 35)/2 + 35;
    if ( dam > 120)
	dam = (dam - 80)/2 + 80; 



   
    if ( victim != ch )
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if ( is_safe( ch, victim ) )
	    return FALSE;

	if ( victim->position > POS_STUNNED )
	{
	    if ( victim->fighting == NULL )
	    {
		set_fighting( victim, ch );
		if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_KILL ) )
		    mp_percent_trigger( victim, ch, NULL, NULL, TRIG_KILL );
	    }
	    if (victim->timer <= 4)
	    	victim->position = POS_FIGHTING;
	}

	if ( victim->position > POS_STUNNED )
	{
	    if ( ch->fighting == NULL )
		set_fighting( ch, victim );

	    /*
	     * If victim is charmed, ch might attack victim's master.
	     taken out by Russ! */
/*
	    if ( IS_NPC(ch)
	    &&   IS_NPC(victim)
	    &&   IS_AFFECTED(victim, AFF_CHARM)
	    &&   victim->master != NULL
	    &&   victim->master->in_room == ch->in_room
	    &&   number_bits( 3 ) == 0 )
	    {
		stop_fighting( ch, FALSE );
		multi_hit( ch, victim->master, TYPE_UNDEFINED );
		return FALSE;
	    }
*/
	}

	/*
	 * More charm stuff.
	 */
	if ( victim->master == ch )
	    stop_follower( victim );
    }

    /*
     * Inviso attacks ... not.
     */
    if ( IS_SHIELDED(ch, SHD_INVISIBLE) )
    {
	affect_strip( ch, gsn_invis );
	affect_strip( ch, gsn_mass_invis );
	REMOVE_BIT( ch->shielded_by, SHD_INVISIBLE );
	act( "$n fades into existence.", ch, NULL, NULL, TO_ROOM );
    }

    if ( IS_SHIELDED(ch, SHD_PEACE) )
    {
	affect_strip( ch, gsn_peace );
	REMOVE_BIT( ch->shielded_by, SHD_PEACE );
    }

    /*
     * Damage modifiers.
     */

    if ( dam > 1 && !IS_NPC(victim) 
    &&   victim->pcdata->condition[COND_DRUNK]  > 6 )
	dam = 9 * dam / 10;

    if ( dam > 1 && !IS_NPC(ch)
    &&   ch->pcdata->condition[COND_DRUNK]  > 6 )
        dam = 12 * dam / 10;

    if ( dam > 1 && IS_SHIELDED(victim, SHD_SANCTUARY) )
	dam /= 2;

    if ( dam > 1 && ((IS_SHIELDED(victim, SHD_PROTECT_EVIL) && IS_EVIL(ch) )
    ||		     (IS_SHIELDED(victim, SHD_PROTECT_GOOD) && IS_GOOD(ch) )))
	dam -= dam / 4;

    immune = FALSE;


    /*
     * Check for parry, and dodge.
     */
    if ( dt >= TYPE_HIT && ch != victim)
    {
        if ( check_parry( ch, victim ) )
	    return FALSE;
	if ( check_dodge( ch, victim ) )
	    return FALSE;
	if ( check_phase( ch, victim ) )
	    return FALSE;
	if ( check_shield_block(ch,victim))
	    return FALSE;

    }

    switch(check_immune(victim,dam_type))
    {
	case(IS_IMMUNE):
	    immune = TRUE;
	    dam = 0;
	    break;
	case(IS_RESISTANT):	
	    dam -= dam/3;
	    break;
	case(IS_VULNERABLE):
	    dam += dam/2;
	    break;
    }

    if(IS_SHIELDED(victim, SHD_MANA))
    {
        mana_lost = mana_damage( victim, 1.4 * dam );

        dam = UMAX(dam * 3 / 10, dam - mana_lost / 2) ;
    }

    if (show)
    	dam_message( ch, victim, dam, dt, immune );

    if (dam == 0)
	return FALSE;

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;
    if ( !IS_NPC(victim)
    &&   victim->level >= LEVEL_IMMORTAL
    &&   victim->hit < 1 )
	victim->hit = 1;

    update_pos( victim );

    ch->fight_damage += dam;

    if (dt == gsn_feed)
    {
	ch->hit = UMIN(ch->hit+((dam/3)*2), ch->max_hit);
	ch->mana = UMIN(ch->mana + ((dam/3)*4), ch->max_mana);
	update_pos( ch );
    }

    switch( victim->position )
    {
    case POS_MORTAL:
	act( "{c$n is mortally wounded, and will die soon, if not aided.{x",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char( 
	    "{cYou are mortally wounded, and will die soon, if not aided.{x\n\r",
	    victim );
	break;

    case POS_INCAP:
	act( "{c$n is incapacitated and will slowly die, if not aided.{x",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char(
	    "{cYou are incapacitated and will slowly {z{Rdie{x{c, if not aided.{x\n\r",
	    victim );
	break;

    case POS_STUNNED:
	act( "{c$n is stunned, but will probably recover.{x",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char("{cYou are stunned, but will probably recover.{x\n\r",
	    victim );
	break;

    case POS_DEAD:
        if ((IS_NPC(victim)) && ( victim->die_descr && 
		victim->die_descr[0] != '\0'))
	{
	    act( "{c$n $T{x", victim, 0, victim->die_descr, TO_ROOM );
	}
	else
	{
	    act( "{c$n is {CDEAD!!{x", victim, 0, 0, TO_ROOM );
	}
	send_to_char( "{cYou have been {RKILLED!!{x\n\r\n\r", victim );
	break;

    default:
	if ( dam > victim->max_hit / 4 )
	    send_to_char( "{cThat really did {RHURT!{x\n\r", victim );
	if ( victim->hit < victim->max_hit / 4 )
	    send_to_char( "{cYou sure are {z{RBLEEDING!{x\n\r", victim );
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE(victim) )
	stop_fighting( victim, FALSE );

    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
	group_gain( ch, victim );

	if ( !IS_NPC(victim) )
	{
	    sprintf( log_buf, "%s killed by %s at %d",
		victim->name,
		(IS_NPC(ch) ? ch->short_descr : ch->name),
		ch->in_room->vnum );
	    log_string(LOG_GAME, log_buf );

	    /*
	     * Dying penalty:
	     * 2/3 way back to previous level.
	     */
	    if ( victim->exp > exp_per_level(victim,victim->pcdata->points) 
			       * victim->level )
	gain_exp( victim, (5 * (exp_per_level(victim,victim->pcdata->points)
			         * victim->level - victim->exp)/6) + 50 );
	}

        sprintf( log_buf, "%s got toasted by %s at %s [room %d]",
            (IS_NPC(victim) ? victim->short_descr : victim->name),
            (IS_NPC(ch) ? ch->short_descr : ch->name),
            ch->in_room->name, ch->in_room->vnum);
 
        if (IS_NPC(victim))
            wiznet(log_buf,NULL,NULL,WIZ_MOBDEATHS,0,0);
        else
            wiznet(log_buf,NULL,NULL,WIZ_DEATHS,0,0);

        /*
         * Death trigger
         */
        if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_DEATH) )
        {
            victim->position = POS_STANDING;
            mp_percent_trigger( victim, ch, NULL, NULL, TRIG_DEATH );
        }

	raw_kill( victim, ch );
        /* dump the flags */
        if (ch != victim && !IS_NPC(ch) && (!is_same_clan(ch,victim)
	|| is_independent(victim)))
        {
            if (IS_SET(victim->act,PLR_TWIT))
                REMOVE_BIT(victim->act,PLR_TWIT);
        }

        /* RT new auto commands */

	if ( !IS_NPC(ch) && IS_NPC(victim) )
	{
	    OBJ_DATA *coins;

	    corpse = get_obj_list( ch, "corpse", ch->in_room->contents ); 

	    if ( IS_SET(ch->act, PLR_AUTOLOOT) &&
		 corpse && corpse->contains) /* exists and not empty */
		do_get( ch, "all corpse" );

 	    if (IS_SET(ch->act,PLR_AUTOGOLD) &&
	        corpse && corpse->contains  && /* exists and not empty */
		!IS_SET(ch->act,PLR_AUTOLOOT))
		if ((coins = get_obj_list(ch,"gcash",corpse->contains))
		     != NULL)
	      	    do_get(ch, "all.gcash corpse");
            
	    if ( IS_SET(ch->act, PLR_AUTOSAC) )
            {
       	      if ( IS_SET(ch->act,PLR_AUTOLOOT) && corpse && corpse->contains)
		return TRUE;  /* leave if corpse has treasure */
	      else
		do_sacrifice( ch, "corpse" );
            }
	}

	return TRUE;
    }

    if ( victim == ch )
	return TRUE;

    /*
     * Take care of link dead people.
     */
    if ( !IS_NPC(victim) && victim->desc == NULL )
    {
	if ( number_range( 0, victim->wait ) == 0 )
	{
	    do_recall( victim, "" );
	    return TRUE;
	}
    }

    /*
     * Wimp out?
     */
    if ( IS_NPC(victim) && dam > 0 && victim->wait < PULSE_VIOLENCE / 2)
    {
	if ( ( IS_SET(victim->act, ACT_WIMPY) && number_bits( 2 ) == 0
	&&   victim->hit < victim->max_hit / 5) 
	||   ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
	&&     victim->master->in_room != victim->in_room ) )
	    do_flee( victim, "" );
    }

    if ( !IS_NPC(victim)
    &&   victim->hit > 0
    &&   victim->hit <= victim->wimpy
    &&   victim->wait < PULSE_VIOLENCE / 2 )
	do_flee( victim, "" );

    tail_chain( );
    return TRUE;
}

/*
 * Show damage from a mock hit.
 */
bool damage_mock(CHAR_DATA *ch,CHAR_DATA *victim,int dam,int dt,int dam_type,
	    bool show) 
{
    long immdam;
    bool immune;
    char buf1[256], buf2[256], buf3[256];
    const char *attack;

    if ( victim->position == POS_DEAD )
	return FALSE;

    if( dam > 30000) dam = 30000;

    if ( dam > 35)
	dam = (dam - 35)/2 + 35;
    if ( dam > 80)
	dam = (dam - 80)/2 + 80; 
    if ( is_safe_mock( ch, victim ) )
	    return FALSE;
    /*
     * Damage modifiers.
     */

    if ( dam > 1 && !IS_NPC(victim) 
    &&   victim->pcdata->condition[COND_DRUNK]  > 10 )
	dam = 9 * dam / 10;

    if ( dam > 1 && IS_SHIELDED(victim, SHD_SANCTUARY) )
	dam /= 2;

    if ( dam > 1 && ((IS_SHIELDED(victim, SHD_PROTECT_EVIL) && IS_EVIL(ch) )
    ||		     (IS_SHIELDED(victim, SHD_PROTECT_GOOD) && IS_GOOD(ch) )))
	dam -= dam / 4;

    if ( dam > 1 && IS_SHIELDED(victim, SHD_MANA) )
	dam /= 5;

    immune = FALSE;


    switch(check_immune(victim,dam_type))
    {
	case(IS_IMMUNE):
	    immune = TRUE;
	    dam = 0;
	    break;
	case(IS_RESISTANT):	
	    dam -= dam/3;
	    break;
	case(IS_VULNERABLE):
	    dam += dam/2;
	    break;
    }

    if ( dt >= 0 && dt < MAX_SKILL )
        attack      = skill_table[dt].noun_damage;
    else if ( dt >= TYPE_HIT
    && dt <= TYPE_HIT + MAX_DAMAGE_MESSAGE)
        attack      = attack_table[dt - TYPE_HIT].noun;
    else
    {
        bug( "Dam_message: bad dt %d.", dt );
         dt  = TYPE_HIT;
        attack  = attack_table[0].name;
    }
    immdam = 0;
    if (ch->level == MAX_LEVEL)
    {
	immdam = dam*63;
    }
    if (ch == victim)
    {
	sprintf( buf1, "{y$n's {gmock {B%s{g would have done {R%d hp{g damage to {y$mself{g.{x",  attack, dam );
	sprintf( buf2, "{yYour {gmock {B%s{g would have done {R%d hp{g damage to {yyourself{g.{x",  attack, dam );
	act(buf1,ch,NULL,NULL,TO_ROOM);
	act(buf2,ch,NULL,NULL,TO_CHAR);
    }
    else if (ch->level < MAX_LEVEL)
    {
	sprintf( buf1, "{y$n's {gmock {B%s{g would have done {R%d hp{g damage to {y$N{g.{x",  attack, dam );
	sprintf( buf2, "{yYour {gmock {B%s{g would have done {R%d hp{g damage to {y$N{g.{x",  attack, dam );
	sprintf( buf3, "{y$n's {gmock {B%s{g would have done {R%d hp{g damage to {yyou{g.{x", attack, dam );
	act( buf1, ch, NULL, victim, TO_NOTVICT );
	act( buf2, ch, NULL, victim, TO_CHAR );
	act( buf3, ch, NULL, victim, TO_VICT );
    }
    else
    {
        sprintf( buf1, "{y$n's {gmock {B%s{g would have done {R%lu hp{g damage to {y$N{g.{x",  attack, immdam );
        sprintf( buf2, "{yYour {gmock {B%s{g would have done {R%lu hp{g damage to {y$N{g.{x",  attack, immdam );
        sprintf( buf3, "{y$n's {gmock {B%s{g would have done {R%lu hp{g damage to {yyou{g.{x", attack, immdam );
        act( buf1, ch, NULL, victim, TO_NOTVICT );
        act( buf2, ch, NULL, victim, TO_CHAR );
        act( buf3, ch, NULL, victim, TO_VICT );
    }

    tail_chain( );
    return TRUE;
}

/*
 * Inflict damage from a hit.
 */
bool damage_old( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt,
                 int dam_type, bool show )
{
    OBJ_DATA *corpse;
    bool immune;

    int mana_lost;

    if ( victim->position == POS_DEAD )
	return FALSE;

    /*
     * Stop up any residual loopholes.
     */

    if ( dam > 2501 && dt >= TYPE_HIT && !IS_IMMORTAL(ch))
    {
        bug( "Damage_old: %d: more than 2501 points!", dam );
        dam = 2501;
        if (!IS_IMMORTAL(ch) && 0)
        {
            OBJ_DATA *obj;
            obj = get_eq_char( ch, WEAR_WIELD );
            send_to_char("{cYou {z{Greally{x{c shouldn't cheat.{x\n\r",ch);
            if (obj != NULL)
                extract_obj(obj);
        }
 
    }

    
    /* damage reduction */
    if ( dam > 35)
	dam = (dam - 35)/2 + 35;
    if ( dam > 80)
	dam = (dam - 80)/2 + 80; 



   
    if ( victim != ch )
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if ( is_safe( ch, victim ) )
	    return FALSE;

	if ( victim->position > POS_STUNNED )
	{
	    if ( victim->fighting == NULL )
		set_fighting( victim, ch );
	    if (victim->timer <= 4)
	    	victim->position = POS_FIGHTING;
	}

	if ( victim->position > POS_STUNNED )
	{
	    if ( ch->fighting == NULL )
		set_fighting( ch, victim );

	    /*
	     * If victim is charmed, ch might attack victim's master.
	     */
	    if ( IS_NPC(ch)
	    &&   IS_NPC(victim)
	    &&   IS_AFFECTED(victim, AFF_CHARM)
	    &&   victim->master != NULL
	    &&   victim->master->in_room == ch->in_room
	    &&   number_bits( 3 ) == 0 )
	    {
		stop_fighting( ch, FALSE );
		multi_hit( ch, victim->master, TYPE_UNDEFINED );
		return FALSE;
	    }
	}

	/*
	 * More charm stuff.
	 */
	if ( victim->master == ch )
	    stop_follower( victim );
    }

    /*
     * Inviso attacks ... not.
     */
    if ( IS_SHIELDED(ch, SHD_INVISIBLE) )
    {
	affect_strip( ch, gsn_invis );
	affect_strip( ch, gsn_mass_invis );
	REMOVE_BIT( ch->shielded_by, SHD_INVISIBLE );
	act( "$n fades into existence.", ch, NULL, NULL, TO_ROOM );
    }

    if ( IS_SHIELDED(ch, SHD_PEACE) )
    {
	affect_strip( ch, gsn_peace );
	REMOVE_BIT( ch->shielded_by, SHD_PEACE );
    }

    /*
     * Damage modifiers.
     */

    if ( dam > 1 && !IS_NPC(victim)
    &&   victim->pcdata->condition[COND_DRUNK]  > 10 )
        dam = 9 * dam / 10;

    if ( dam > 1 && !IS_NPC(ch)
    &&   ch->pcdata->condition[COND_DRUNK]  > 6 )
        dam = 12 * dam / 10;
 
    if ( dam > 1 && IS_SHIELDED(victim, SHD_SANCTUARY) )
        dam /= 2;
 
    if ( dam > 1 && ((IS_SHIELDED(victim, SHD_PROTECT_EVIL) && IS_EVIL(ch) )
    ||               (IS_SHIELDED(victim, SHD_PROTECT_GOOD) && IS_GOOD(ch) )))
        dam -= dam / 4;

    immune = FALSE;


    /*
     * Check for parry, and dodge.
     */
    if ( dt >= TYPE_HIT && ch != victim)
    {
        if ( check_parry( ch, victim ) )
	    return FALSE;
	if ( check_dodge( ch, victim ) )
	    return FALSE;
	if ( check_phase( ch, victim ) )
	    return FALSE;
	if ( check_shield_block(ch,victim))
	    return FALSE;

    }

    switch(check_immune(victim,dam_type))
    {
	case(IS_IMMUNE):
	    immune = TRUE;
	    dam = 0;
	    break;
	case(IS_RESISTANT):	
	    dam -= dam/3;
	    break;
	case(IS_VULNERABLE):
	    dam += dam/2;
	    break;
    }

    if(IS_SHIELDED(victim, SHD_MANA))
    {
        mana_lost = mana_damage( victim, 1.4 * dam );

        dam = UMAX(dam * 3 / 10, dam - mana_lost / 2) ;
    }

    if (show)
    	dam_message( ch, victim, dam, dt, immune );

    if (dam == 0)
	return FALSE;

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;
    if ( !IS_NPC(victim)
    &&   victim->level >= LEVEL_IMMORTAL
    &&   victim->hit < 1 )
	victim->hit = 1;
    update_pos( victim );
    if (dt == gsn_feed)
    {
	ch->hit = UMIN(ch->hit+((dam/3)*2), ch->max_hit);
        ch->mana = UMIN(ch->mana + ((dam/3)*4), ch->max_mana);
	update_pos( ch );
    }

    ch->fight_damage += dam;


    switch( victim->position )
    {
    case POS_MORTAL:
	act( "{c$n is mortally wounded, and will die soon, if not aided.{x",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char( 
	    "{cYou are mortally wounded, and will die soon, if not aided.{x\n\r",
	    victim );
	break;

    case POS_INCAP:
	act( "{c$n is incapacitated and will slowly die, if not aided.{x",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char(
	    "{cYou are incapacitated and will slowly {Rdie{c, if not aided.{x\n\r",
	    victim );
	break;

    case POS_STUNNED:
	act( "{c$n is stunned, but will probably recover.{x",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char("{cYou are stunned, but will probably recover.{x\n\r",
	    victim );
	break;

    case POS_DEAD:
        if ((IS_NPC(victim)) && ( victim->die_descr && victim->die_descr[0] != '\0'))
        { 
            act( "{c$n $T{x", victim, 0, victim->die_descr, TO_ROOM ); 
        } 
        else 
        {  
	    act( "{c$n is {CDEAD!!{x", victim, 0, 0, TO_ROOM );
	}
	send_to_char( "{cYou have been {RKILLED!!{x\n\r\n\r", victim );
	break;

    default:
	if ( dam > victim->max_hit / 4 )
	    send_to_char( "{cThat really did {RHURT!{x\n\r", victim );
	if ( victim->hit < victim->max_hit / 4 )
	    send_to_char( "{cYou sure are {z{RBLEEDING!{x\n\r", victim );
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE(victim) )
	stop_fighting( victim, FALSE );

    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
	group_gain( ch, victim );

	if ( !IS_NPC(victim) )
	{
	    sprintf( log_buf, "%s killed by %s at %d",
		victim->name,
		(IS_NPC(ch) ? ch->short_descr : ch->name),
		victim->in_room->vnum );
	    log_string(LOG_GAME, log_buf );

	    /*
	     * Dying penalty:
	     * 2/3 way back to previous level.
	     */
	    if ( victim->exp > exp_per_level(victim,victim->pcdata->points) 
			       * victim->level )
	gain_exp( victim, (2 * (exp_per_level(victim,victim->pcdata->points)
			         * victim->level - victim->exp)/3) + 50 );
	}

        sprintf( log_buf, "%s got toasted by %s at %s [room %d]",
            (IS_NPC(victim) ? victim->short_descr : victim->name),
            (IS_NPC(ch) ? ch->short_descr : ch->name),
            ch->in_room->name, ch->in_room->vnum);
 
        if (IS_NPC(victim))
            wiznet(log_buf,NULL,NULL,WIZ_MOBDEATHS,0,0);
        else
            wiznet(log_buf,NULL,NULL,WIZ_DEATHS,0,0);

        /*
         * Death trigger
         */
        if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_DEATH) )
        {
            victim->position = POS_STANDING;
            mp_percent_trigger( victim, ch, NULL, NULL, TRIG_DEATH );
        }

	raw_kill( victim, ch );
	/* dump the flags */
	if (ch != victim && !IS_NPC(ch) && (!is_same_clan(ch,victim)
	|| is_independent(victim)))
	{
	    if (IS_SET(victim->act,PLR_TWIT))
	    	REMOVE_BIT(victim->act,PLR_TWIT);
	}
        /* RT new auto commands */

	if ( !IS_NPC(ch) && IS_NPC(victim) )
	{
	    corpse = get_obj_list( ch, "corpse", ch->in_room->contents ); 

	    if ( IS_SET(ch->act, PLR_AUTOLOOT) &&
		 corpse && corpse->contains) /* exists and not empty */
		do_get( ch, "all corpse" );

 	    if (IS_SET(ch->act,PLR_AUTOGOLD) &&
	        corpse && corpse->contains  && /* exists and not empty */
		!IS_SET(ch->act,PLR_AUTOLOOT))
	      do_get(ch, "gold corpse");
            
	    if ( IS_SET(ch->act, PLR_AUTOSAC) )
            {
       	      if ( IS_SET(ch->act,PLR_AUTOLOOT) && corpse && corpse->contains)
		return TRUE;  /* leave if corpse has treasure */
	      else
		do_sacrifice( ch, "corpse" );
            }
	}

	return TRUE;
    }

    if ( victim == ch )
	return TRUE;

    /*
     * Take care of link dead people.
     */
    if ( !IS_NPC(victim) && victim->desc == NULL )
    {
	if ( number_range( 0, victim->wait ) == 0 )
	{
	    do_recall( victim, "" );
	    return TRUE;
	}
    }

    /*
     * Wimp out?
     */
    if ( IS_NPC(victim) && dam > 0 && victim->wait < PULSE_VIOLENCE / 2)
    {
	if ( ( IS_SET(victim->act, ACT_WIMPY) && number_bits( 2 ) == 0
	&&   victim->hit < victim->max_hit / 5) 
	||   ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
	&&     victim->master->in_room != victim->in_room ) )
	    do_flee( victim, "" );
    }

    if ( !IS_NPC(victim)
    &&   victim->hit > 0
    &&   victim->hit <= victim->wimpy
    &&   victim->wait < PULSE_VIOLENCE / 2 )
	do_flee( victim, "" );

    tail_chain( );
    return TRUE;
}

int mana_damage( CHAR_DATA *ch, int mana)
{
	int mana_damage;

	if( ch->mana < mana )
	{
		mana_damage = ch->mana;
		ch->mana = 0;
	}
	else
	{
		mana_damage = mana;
		ch->mana -= mana;
	}

        if( IS_SHIELDED(ch, SHD_MANA) && ( ch->max_mana < 1 || 100 * ch->mana / ch->max_mana < 40 ) )
        {
		send_to_char("Your manashield *{Rbreaks{x*!\n\r", ch);
		act("$n's manashield *{Rbreaks{x*!",ch, NULL,NULL,TO_ROOM);
		affect_strip(ch,gsn_manashield);
        }

	return mana_damage;
}

bool is_safe(CHAR_DATA *ch, CHAR_DATA *victim)
{
    if (victim->in_room == NULL || ch->in_room == NULL)
	return TRUE;

    if (victim->fighting == ch || victim == ch)
	return FALSE;

    if (!IS_NPC(ch) && IS_IMMORTAL(ch))
	return FALSE;

    /* killing mobiles */
    if (IS_NPC(victim))
    {

	/* safe room? */
	if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
	{
	    send_to_char("Not in this room.\n\r",ch);
	    return TRUE;
	}

	if (victim->pIndexData->pShop != NULL)
	{
	    send_to_char("The shopkeeper wouldn't like that.\n\r",ch);
	    return TRUE;
	}

	/* no killing healers, trainers, etc */
	if (IS_SET(victim->act,ACT_TRAIN)
	||  IS_SET(victim->act,ACT_PRACTICE)
	||  IS_SET(victim->act,ACT_IS_HEALER)
	||  IS_SET(victim->act,ACT_IS_SPELLWIZ)
	||  IS_SET(victim->act,ACT_IS_PRIEST))
	{
	    act("I don't think $G would approve.",ch,NULL,NULL,TO_CHAR);
	    return TRUE;
	}

	if (!IS_NPC(ch))
	{
	    /* no pets */
	    if (IS_SET(victim->act,ACT_PET))
	    {
		act("But $N looks so cute and cuddly...",
		    ch,NULL,victim,TO_CHAR);
		return TRUE;
	    }

	    /* no charmed creatures unless owner, or owned by pk-able */
	    if (IS_AFFECTED(victim,AFF_CHARM)
                && ch != victim->master)
	    {
                if(victim->master && !IS_NPC(victim->master)
                    && !is_safe(ch, victim->master))
                {
                    return TRUE;
                }

		send_to_char("You don't own that monster.\n\r",ch);
		return TRUE;
	    }
	}
    }
    /* killing players */
    else
    {
	/* NPC doing the killing */
	if (IS_NPC(ch))
	{
	    /* safe room check */
	    if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
	    {
		send_to_char("Not in this room.\n\r",ch);
		return TRUE;
	    }

            if(ch->in_arena && victim->in_arena) return FALSE;

	    /* charmed mobs and pets cannot attack players while owned */
	    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master != NULL
	    &&  ch->master->fighting != victim)
	    {
		send_to_char("Players are your friends!\n\r",ch);
		return TRUE;
	    }
	}
	/* player doing the killing */
	else
	{

            if(IS_SHIELDED(victim, SHD_PEACE))
            {
                send_to_char("They may not be attacked at this time.\n\r",ch);
                return TRUE;
            }

            if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
            {
                send_to_char("Not in this room.\n\r",ch);
                return TRUE;
            }

            if (IS_SET(victim->in_room->room_flags,ROOM_LIMIT_PK))
            {
		if (ch->level > victim->level + 5)
		{
		    send_to_char("PK is limited to within 5 levels in this room.\n\r",ch);
		    return TRUE;
		}
		if (ch->level < victim->level - 5)
		{
		    send_to_char("PK is limited to within 5 levels in this room.\n\r",ch);
		    return TRUE;
		}

            }

            if(ch->in_arena && victim->in_arena) return FALSE;

	    if (IS_SET(victim->act,PLR_TWIT) )
		return FALSE;

	    if (((victim->level >19)
	    || ((victim->class >= MAX_CLASS/2)
	    && (victim->level > 14)))
	    && (is_voodood(ch, victim) ) )
		return FALSE;

	    if (ch->on_quest)
	    {
		send_to_char("Not while you are on a quest.\n\r",ch);
		return TRUE;
	    }
	    if (victim->on_quest)
	    {
		send_to_char("They are on a quest, leave them alone.\n\r",ch);
		return TRUE;
	    }
	    if (!is_clan(ch))
	    {
		send_to_char("Join a clan if you want to fight players.\n\r",ch);
		return TRUE;
	    }

	    if (!is_pkill(ch))
	    {
		send_to_char("Your clan does not allow player fighting.\n\r",ch);
		return TRUE;
	    }

	    if (!is_clan(victim))
	    {
		send_to_char("They aren't in a clan, leave them alone.\n\r",ch);
		return TRUE;
	    }

	    if (!is_pkill(victim))
	    {
		send_to_char("They are in a no pkill clan, leave them alone.\n\r",ch);
		return TRUE;
	    }

	    if (is_same_clan(ch,victim))
	    {
		send_to_char("You can't fight your own clan members.\n\r",ch);
		return TRUE;
	    }

	    if ( ( ( ch->class < MAX_CLASS/2 )
	    && ( victim->class < MAX_CLASS/2 ) )
	    || ( ( ch->class >= MAX_CLASS/2 )
	    && ( victim->class >= MAX_CLASS/2 ) ) )
	    {
		if (ch->level > victim->level + 15)
		{
		    send_to_char("Pick on someone your own size.\n\r",ch);
		    return TRUE;
		}
		if (ch->level < victim->level - 15)
		{
		    send_to_char("Pick on someone your own size.\n\r",ch);
		    return TRUE;
		}
	    } else {
		if (ch->level > victim->level + 10)
		{
		    send_to_char("Pick on someone your own size.\n\r",ch);
		    return TRUE;
		}
		if (ch->level < victim->level - 10)
		{
		    send_to_char("Pick on someone your own size.\n\r",ch);
		    return TRUE;
		}
	    }
	}
    }
    return FALSE;
}

bool is_safe_mock(CHAR_DATA *ch, CHAR_DATA *victim)
{
    if (victim->in_room == NULL || ch->in_room == NULL)
	return TRUE;
    if (!IS_NPC(ch) && IS_IMMORTAL(ch))
	return FALSE;
    if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
    {
        send_to_char("Not in this room.\n\r",ch);
        return TRUE;
    }
    if (IS_NPC(victim))
    {
	send_to_char("{RYou can only use this on a player.{x\n\r",ch);
	return TRUE;
    }
    return FALSE;
}

bool is_voodood(CHAR_DATA *ch, CHAR_DATA *victim)
{
    OBJ_DATA *object;
    bool found;

    if ( ch->level > HERO )
	return FALSE;

    found = FALSE;
    for ( object = victim->carrying; object != NULL; object = object->next_content )
    {
	if (object->pIndexData->vnum == OBJ_VNUM_VOODOO)
	{
	    char arg[MAX_INPUT_LENGTH];

	    one_argument(object->name, arg);
	    if (!str_cmp(arg, ch->name))
	    {
		return TRUE;
	    }
	}
    }
    return FALSE;
}

bool is_safe_spell(CHAR_DATA *ch, CHAR_DATA *victim, bool area )
{
    if (victim->in_room == NULL || ch->in_room == NULL)
        return TRUE;

    if (victim == ch && area)
	return TRUE;

    if (victim->fighting == ch || victim == ch)
	return FALSE;

    if (!IS_NPC(ch) && IS_IMMORTAL(ch))
	return FALSE;

    /* killing mobiles */
    if (IS_NPC(victim))
    {
	/* safe room? */
	if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
	    return TRUE;

        if(ch->in_arena && victim->in_arena) return FALSE;

	if (victim->pIndexData->pShop != NULL)
	    return TRUE;

	/* no killing healers, trainers, etc */
	if (IS_SET(victim->act,ACT_TRAIN)
	||  IS_SET(victim->act,ACT_PRACTICE)
	||  IS_SET(victim->act,ACT_IS_HEALER)
	||  IS_SET(victim->act,ACT_IS_SPELLWIZ)
	||  IS_SET(victim->act,ACT_IS_PRIEST))
	    return TRUE;

	if (!IS_NPC(ch))
	{
	    /* no pets */
	    if (IS_SET(victim->act,ACT_PET))
	   	return TRUE;

	    /* no charmed creatures unless owner */
	    if (IS_AFFECTED(victim,AFF_CHARM) && (area || ch != victim->master))
		return TRUE;

	    /* legal kill? -- cannot hit mob fighting non-group member */
	    if (victim->fighting != NULL && !is_same_group(ch,victim->fighting))
		return TRUE;
	}
	else
	{
	    /* area effect spells do not hit other mobs */
	    if (area && !is_same_group(victim,ch->fighting))
		return TRUE;
	}
    }
    /* killing players */
    else
    {
	if (area && IS_IMMORTAL(victim) && victim->level > LEVEL_IMMORTAL)
	    return TRUE;

	/* NPC doing the killing */
	if (IS_NPC(ch))
	{
	    /* charmed mobs and pets cannot attack players while owned */
	    if (((IS_AFFECTED(ch,AFF_CHARM)) & (ch->master != NULL))
	    &&  (ch->master->fighting != victim))
		return TRUE;
	
	    /* safe room? */
	    if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
		return TRUE;

	    /* legal kill? -- mobs only hit players grouped with opponent*/
	    if (ch->fighting != NULL && !is_same_group(ch->fighting,victim))
		return TRUE;
	}

	/* player doing the killing */
	else
	{
            if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
                return TRUE;

	    if (IS_SET(victim->act,PLR_TWIT) )
		return FALSE;

	    if (((victim->level > 19)
	    || ((victim->class >= MAX_CLASS/2)
	    && (victim->level > 14)))
	    && (is_voodood(ch, victim) ) )
		return FALSE;

	    if (ch->in_arena)
		return FALSE;

	    if (!is_clan(ch))
		return TRUE;

	    if (!is_pkill(ch))
		return TRUE;

            if (ch->on_quest) 
                return TRUE; 

            if (victim->on_quest)
                return TRUE; 

	    if (!is_clan(victim))
		return TRUE;

	    if (!is_pkill(victim))
		return TRUE;

	    if (is_same_clan(ch,victim))
		return TRUE;

            if (IS_SET(victim->in_room->room_flags,ROOM_LIMIT_PK))
            {
		if (ch->level > victim->level + 5)
		{
		    send_to_char("PK is limited to within 5 levels in this room.\n\r",ch);
		    return TRUE;
		}
		if (ch->level < victim->level - 5)
		{
		    send_to_char("PK is limited to within 5 levels in this room.\n\r",ch);
		    return TRUE;
		}
            }


	    if ( ( ( ch->class < MAX_CLASS/2 )
	    && ( victim->class < MAX_CLASS/2 ) )
	    || ( ( ch->class >= MAX_CLASS/2 )
	    && ( victim->class >= MAX_CLASS/2 ) ) )
	    {
		if (ch->level > victim->level + 10)
		{
		    return TRUE;
		}
		if (ch->level < victim->level - 10)
		{
		    return TRUE;
		}
	    } else {
		return TRUE;
	    }
	}
    }
    return FALSE;
}

/*
 * Check for parry.
 */
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    chance = get_skill(victim,gsn_parry) / 2;

    if ( get_eq_char( victim, WEAR_WIELD ) == NULL )
    {
	if (IS_NPC(victim))
	    chance /= 2;
	else
	    return FALSE;
    }

    if (victim->stunned)
        return FALSE;

    if (!can_see(ch,victim))
	chance /= 2;

    if ( number_percent( ) >= chance + victim->level - ch->level )
	return FALSE;

    act2( "{iYou parry $n's attack.{x",  ch, NULL, victim, TO_VICT, ACTION_BATTLE_NODAM);
    act2( "{h$N parries your attack.{x", ch, NULL, victim, TO_CHAR, ACTION_BATTLE_NODAM);
    check_improve(victim,gsn_parry,TRUE,6);
    return TRUE;
}

/*
 * Check for shield block.
 */
bool check_shield_block( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
        return FALSE;


    chance = get_skill(victim,gsn_shield_block) / 5 + 3;


    if ( get_eq_char( victim, WEAR_SHIELD ) == NULL )
        return FALSE;

    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;

    if (victim->stunned)
        return FALSE;

    act( "{iYou block $n's attack with your shield.{x",  ch, NULL, victim,  TO_VICT    );
    act( "{h$N blocks your attack with a shield.{x", ch, NULL, victim, TO_CHAR    );
    check_improve(victim,gsn_shield_block,TRUE,6);
    return TRUE;
}


/*
 * Check for dodge.
 */
bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    chance = get_skill(victim,gsn_dodge) / 2;

    if (!can_see(victim,ch))
	chance /= 2;

    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;

    if (victim->stunned)
        return FALSE;

    act_new_2( "{iYou dodge $n's attack.{x", ch, NULL, victim, TO_VICT, POS_RESTING, 0, ACTION_BATTLE_NODAM );
    act_new_2( "{h$N dodges your attack.{x", ch, NULL, victim, TO_CHAR, POS_RESTING, 0, ACTION_BATTLE_NODAM );
    check_improve(victim,gsn_dodge,TRUE,6);
    return TRUE;
}



/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    start_hit_gain(victim);
    start_mana_move_gain(victim);

    if ( victim->hit > 0 )
    {
    	if ( victim->position <= POS_STUNNED )
	    victim->position = POS_STANDING;
	return;
    }

    if ( IS_NPC(victim) && victim->hit < 1 )
    {
	victim->position = POS_DEAD;
	return;
    }

    if ( victim->hit <= -11 )
    {
	victim->position = POS_DEAD;
	return;
    }

         if ( victim->hit <= -6 ) victim->position = POS_MORTAL;
    else if ( victim->hit <= -3 ) victim->position = POS_INCAP;
    else                          victim->position = POS_STUNNED;

    return;
}



/*
 * Start fights.
 */
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->fighting != NULL )
    {
	bug( "Set_fighting: already fighting", 0 );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_SLEEP) )
	affect_strip( ch, gsn_sleep );

    interrupt_char(ch, EVENT_INTERRUPT_ON_FIGHT);

    ch->fighting = victim;
    ch->position = POS_FIGHTING;
    ch->stunned = 0;

    ch->fight_damage = 0;
    ch->fight_rounds = 0;
    victim->fight_damage = 0;
    victim->fight_rounds = 0;
    if(IS_NPC(victim)) victim->prekill_hit = victim->hit;
    if(IS_NPC(ch)) ch->prekill_hit = ch->hit;

    return;
}



/*
 * Stop fights.
 */
void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
    CHAR_DATA *fch;
    char buf[MAX_STRING_LENGTH];

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
	if ( fch == ch || ( fBoth && fch->fighting == ch ) )
	{

	    fch->fighting	= NULL;
	    fch->position	= IS_NPC(fch) ? fch->default_pos : POS_STANDING;
	    fch->stunned	= 0;
	    update_pos( fch );


            sprintf(buf, "%s dealt %ld total in %d rounds",
                (IS_NPC(ch) ? ch->short_descr : ch->name),
                fch->fight_damage, fch->fight_rounds);

            wiznet(buf,fch,NULL,WIZ_NUMBERS,0,0);

            ch->fight_damage = 0;
            ch->fight_rounds = 0;
            fch->fight_damage = 0;
            fch->fight_rounds = 0;


	    if (IS_SET(fch->comm,COMM_STORE))
		if(fch->tells)
		{
		    sprintf( buf, "You have {R%d{x tells waiting.\n\r", fch->tells );
		    send_to_char( buf, fch );
		    send_to_char("Type 'replay' to see tells.\n\r",fch);
		}
	}
    }

    return;
}



/*
 * Make a corpse out of a character.
 */
void make_corpse( CHAR_DATA *ch, CHAR_DATA *killer )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char *name;

    if ( IS_NPC(ch) )
    {
	if (IS_SET(ch->act, ACT_NO_BODY) )
	{
	    if (IS_SET(ch->act, ACT_NB_DROP) )
	    {
		for ( obj = ch->carrying; obj != NULL; obj = obj_next )
		{
		    obj_next = obj->next_content;
		    obj_from_char( obj );
		    if (obj->item_type == ITEM_POTION)
			obj->timer = number_range(500,1000);
		    if (obj->item_type == ITEM_SCROLL)
			obj->timer = number_range(1000,2500);
		    if (IS_SET(obj->extra_flags,ITEM_ROT_DEATH))
		    {
			obj->timer = number_range(5,10);
			REMOVE_BIT(obj->extra_flags,ITEM_ROT_DEATH);
		    }
		    REMOVE_BIT(obj->extra_flags,ITEM_VIS_DEATH);

		    if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
			extract_obj( obj );
		    act("$p falls to the floor.",ch,obj,NULL,TO_ROOM);
		    obj_to_room(obj,ch->in_room);
		}
	    }
	    return;
	}
	name		= ch->short_descr;
	corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0);
	corpse->timer	= number_range( 3, 6 );
	if ( ch->gold > 0 || ch->platinum > 0 )
	{
	    obj_to_obj( create_money( ch->platinum, ch->gold, ch->silver ), corpse );
	    ch->platinum = 0;
	    ch->gold = 0;
	    ch->silver = 0;
	}
	corpse->cost = 0;
    }
    else
    {
	name		= ch->name;
	corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
	corpse->timer	= number_range( 25, 40 );
	REMOVE_BIT(ch->act,PLR_CANLOOT);
	if (!is_clan(ch))
	{
	    corpse->owner = str_dup(ch->name);
	    corpse->killer = NULL;
	}
	else
	{
	    corpse->owner = str_dup(ch->name);
	    corpse->killer = str_dup(killer->name);
	    if (ch->platinum > 1 || ch->gold > 1 || ch->silver > 1)
	    {
		obj_to_obj(create_money(ch->platinum/2, ch->gold / 2, ch->silver/2), corpse);
		ch->platinum -= ch->platinum/2;
		ch->gold -= ch->gold/2;
		ch->silver -= ch->silver/2;
	    }
	}
		
	corpse->cost = 0;
    }

    corpse->level = ch->level;

    sprintf( buf, corpse->short_descr, name );
    free_string( corpse->short_descr );
    corpse->short_descr = str_dup( buf );

    sprintf( buf, corpse->description, name );
    free_string( corpse->description );
    corpse->description = str_dup( buf );

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	bool floating = FALSE;

	obj_next = obj->next_content;
	if (obj->wear_loc == WEAR_FLOAT)
	    floating = TRUE;
	obj_from_char( obj );
	if (obj->item_type == ITEM_POTION)
	    obj->timer = number_range(500,1000);
	if (obj->item_type == ITEM_SCROLL)
	    obj->timer = number_range(1000,2500);
	if (IS_SET(obj->extra_flags,ITEM_ROT_DEATH) && !floating)
	{
	    obj->timer = number_range(5,10);
	    REMOVE_BIT(obj->extra_flags,ITEM_ROT_DEATH);
	}
	REMOVE_BIT(obj->extra_flags,ITEM_VIS_DEATH);


	if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
	    extract_obj( obj );
	else if (floating)
	{
	    if (IS_OBJ_STAT(obj,ITEM_ROT_DEATH)) /* get rid of it! */
	    { 
		if (obj->contains != NULL)
		{
		    OBJ_DATA *in, *in_next;

		    act("$p evaporates,scattering its contents.",
			ch,obj,NULL,TO_ROOM);
		    for (in = obj->contains; in != NULL; in = in_next)
		    {
			in_next = in->next_content;
			obj_from_obj(in);
			obj_to_room(in,ch->in_room);
		    }
		 }
		 else
		    act("$p evaporates.",
			ch,obj,NULL,TO_ROOM);
		 extract_obj(obj);
	    }
	    else
	    {
		act("$p falls to the floor.",ch,obj,NULL,TO_ROOM);
		obj_to_room(obj,ch->in_room);
	    }
	}
	else
	    obj_to_obj( obj, corpse );
    }

    obj_to_room( corpse,
        ch->in_arena ? arena.teams[ch->arena_team].hq : ch->in_room );
    return;
}



/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *was_in_room;
    char *msg;
    int door;
    int vnum;

    vnum = 0;
    msg = "You hear $n's death cry.";
    if (IS_NPC(ch))
    {
	if (!IS_SET(ch->act, ACT_NO_BODY) )
	{
	    switch ( number_bits(4))
	    {
	    case  0:
		msg  = "$n hits the ground ... DEAD.";
		vnum = OBJ_VNUM_BLOOD;
		break;
	    case  1:
		msg  = "$n splatters blood on your armor.";
		vnum = OBJ_VNUM_BLOOD;
		break;
	    case  2:
		if (IS_SET(ch->parts,PART_GUTS))
		{
		    msg = "$n spills $s guts all over the floor.";
		    vnum = OBJ_VNUM_GUTS;
		}
		break;
	    case  3: 
		if (IS_SET(ch->parts,PART_HEAD))
		{
		    msg  = "$n's severed head plops on the ground.";
		    vnum = OBJ_VNUM_SEVERED_HEAD;
		}
		break;
	    case  4:
		if (IS_SET(ch->parts,PART_HEART))
		{
		    msg  = "$n's heart is torn from $s chest.";
		    vnum = OBJ_VNUM_TORN_HEART;
		}
		break;
	    case  5:
		if (IS_SET(ch->parts,PART_ARMS))
		{
		    msg  = "$n's arm is sliced from $s dead body.";
		    vnum = OBJ_VNUM_SLICED_ARM;
		}
		break;
	    case  6:
		if (IS_SET(ch->parts,PART_LEGS))
		{
		    msg  = "$n's leg is sliced from $s dead body.";
		    vnum = OBJ_VNUM_SLICED_LEG;				
		}
		break;
	    case 7:
		if (IS_SET(ch->parts,PART_BRAINS))
		{
		    msg = "$n's head is shattered, and $s brains splash all over you.";
		    vnum = OBJ_VNUM_BRAINS;
		}
		break;
	    case  8:
		msg  = "$n hits the ground ... DEAD.";
		vnum = OBJ_VNUM_BLOOD;
		break;
	    case  9:
		msg  = "$n hits the ground ... DEAD.";
		vnum = OBJ_VNUM_BLOOD;
	    }
	}
    } else if (ch->level > 19)
    {
	switch ( number_bits(4))
	{
	    case  0:
		msg  = "$n hits the ground ... DEAD.";
		vnum = OBJ_VNUM_BLOOD;
		break;
	    case  1:
		msg  = "$n splatters blood on your armor.";
		vnum = OBJ_VNUM_BLOOD;
		break;
	    case  2:
		if (IS_SET(ch->parts,PART_GUTS))
		{
		    msg = "$n spills $s guts all over the floor.";
		    vnum = OBJ_VNUM_GUTS;
		}
		break;
	    case  3:
		if (IS_SET(ch->parts,PART_HEAD))
		{
		    msg  = "$n's severed head plops on the ground.";
		    vnum = OBJ_VNUM_SEVERED_HEAD;
		}
		break;
	    case  4:
		if (IS_SET(ch->parts,PART_HEART))
		{
		    msg  = "$n's heart is torn from $s chest.";
		    vnum = OBJ_VNUM_TORN_HEART;
		}
		break;
	    case  5:
		if (IS_SET(ch->parts,PART_ARMS))
		{
		    msg  = "$n's arm is sliced from $s dead body.";
		    vnum = OBJ_VNUM_SLICED_ARM;
		}
		break;
	    case  6:
		if (IS_SET(ch->parts,PART_LEGS))
		{
		    msg  = "$n's leg is sliced from $s dead body.";
		    vnum = OBJ_VNUM_SLICED_LEG;
		}
		break;
	    case 7:
		if (IS_SET(ch->parts,PART_BRAINS))
		{
		    msg = "$n's head is shattered, and $s brains splash all over you.";
		    vnum = OBJ_VNUM_BRAINS;
		}
		break;
	    case  8:
		msg  = "$n hits the ground ... DEAD.";
		vnum = OBJ_VNUM_BLOOD;
		break;
	    case  9:
		msg  = "$n hits the ground ... DEAD.";
		vnum = OBJ_VNUM_BLOOD;
		break;
	    case  10:
		if (IS_SET(ch->parts,PART_HEAD))
		{
		    msg  = "$n's severed head plops on the ground.";
		    vnum = OBJ_VNUM_SEVERED_HEAD;
		}
		break;
	    case  11:
		if (IS_SET(ch->parts,PART_HEART))
		{
		    msg  = "$n's heart is torn from $s chest.";
		    vnum = OBJ_VNUM_TORN_HEART;
		}
		break;
	    case  12:
		if (IS_SET(ch->parts,PART_ARMS))
		{
		    msg  = "$n's arm is sliced from $s dead body.";
		    vnum = OBJ_VNUM_SLICED_ARM;
		}
		break;
	    case  13:
		if (IS_SET(ch->parts,PART_LEGS))
		{
		    msg  = "$n's leg is sliced from $s dead body.";
		    vnum = OBJ_VNUM_SLICED_LEG;
		}
		break;
	    case 14:
		if (IS_SET(ch->parts,PART_BRAINS))
		{
		    msg = "$n's head is shattered, and $s brains splash all over you.";
		    vnum = OBJ_VNUM_BRAINS;
		}
	}
    }

    act( msg, ch, NULL, NULL, TO_ROOM );

    if (( vnum == 0 ) && !IS_SET(ch->act, ACT_NO_BODY) )
    {
	switch ( number_bits(4))
	{
	case  0:
		vnum = 0;
		break;
	case  1: 
		vnum = OBJ_VNUM_BLOOD;
		break;
	case  2: 							
		vnum = 0;
		break;
	case  3: 
		vnum = OBJ_VNUM_BLOOD;				
		break;
	case  4: 
		vnum = 0;				
		break;
	case  5: 
		vnum = OBJ_VNUM_BLOOD;				
		break;
	case  6: 
		vnum = 0;				
		break;
	case 7:
		vnum = OBJ_VNUM_BLOOD;
	}
    }

    if ( vnum != 0 )
    {
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	char *name;

	name		= IS_NPC(ch) ? ch->short_descr : ch->name;
	obj		= create_object( get_obj_index( vnum ), 0 );
	obj->timer	= number_range( 4, 7 );
	if (!IS_NPC(ch))
	{
	    obj->timer = number_range( 12, 18 );
	}
	if (vnum == OBJ_VNUM_BLOOD)
	{
	    obj->timer = number_range( 1, 4 );
	}

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );

        sprintf( buf, obj->name, name );
        free_string( obj->name );
        obj->name = str_dup( buf );

	if (obj->item_type == ITEM_FOOD)
	{
	    if (IS_SET(ch->form,FORM_POISON))
		obj->value[3] = 1;
	    else if (!IS_SET(ch->form,FORM_EDIBLE))
		obj->item_type = ITEM_TRASH;
	}

	if (IS_NPC(ch))
	{
	    obj->value[4] = 0;
	} else {
	    obj->value[4] = 1;
	}

	obj_to_room( obj, ch->in_room );
    }

    if ( IS_NPC(ch) )
	msg = "You hear something's death cry.";
    else
	msg = "You hear someone's death cry.";

    was_in_room = ch->in_room;
    for ( door = 0; door <= 5; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = was_in_room->exit[door] ) != NULL
	&&   pexit->u1.to_room != NULL
	&&   pexit->u1.to_room != was_in_room )
	{
	    ch->in_room = pexit->u1.to_room;
	    act( msg, ch, NULL, NULL, TO_ROOM );
	}
    }
    ch->in_room = was_in_room;

    return;
}



void raw_kill( CHAR_DATA *victim, CHAR_DATA *killer )
{
    int i;
    int qps;

/*    OBJ_DATA *wascarrying = NULL, *obj, *obj_next, *obj_prev;*/
/*    bool found = FALSE;*/
    char buf[MAX_STRING_LENGTH];

    death_cry( victim );
    stop_fighting( victim, TRUE );
/*
    if(!IS_NPC(victim))
    {
        for ( obj = victim->carrying; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;
            if ( obj->item_type == ITEM_IKEEP )
            {
                found = TRUE;
                break;
            }
        }

        if(found)
        {

            act( "Your $p envelops your inventory in a bright light and vanishes.{x", victim, obj, NULL, TO_CHAR );
            act( "$n's $p envelops $s inventory in a bright light and vanishes.{x", victim, obj, NULL, TO_ROOM );


            if ( victim->carrying )
            {

                for ( obj = victim->carrying; obj != NULL; obj = obj_next )
                {
                    int oldwear;
                    obj_next = obj->next_content;
                    oldwear = obj->wear_loc;
                    unequip_char(victim,obj);
                    obj->wear_loc = oldwear;
                }        

                wascarrying = victim->carrying;
                victim->carrying = NULL;

            }

        }
        else
	 {
            make_corpse(victim,killer);

	}

    }
    else*/

/*
    obj_prev = NULL;
    for ( obj = victim->carrying; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;

        if(IS_SET(obj->extra_flags, ITEM_BOUND))
        {

#if 0
            if(obj->wear_loc)
            {
                int oldwear;
                oldwear = obj->wear_loc;
                unequip_char(victim,obj);
                obj->wear_loc = oldwear;
            }
#endif

            obj->next_content = wascarrying;
            wascarrying = obj;

            if(obj_prev)
                obj_prev->next = obj_next;
            else
                victim->carrying = obj_next;
        }
        else
            obj_prev = obj;
    }
*/
	if( ! (IS_NPC(killer) && !IS_NPC(victim)) )
	{
	    if(! (IS_NPC(killer) || IS_NPC(victim) ) )
	    {
		qps = UMIN( 200, victim->pcdata->questpoints );
		victim->pcdata->questpoints -= qps;
		killer->pcdata->questpoints += qps;
		sprintf(buf,"You gain {G%d{x questpoints.\n\r",qps);
		send_to_char(buf,killer);
                sprintf(buf,"You lost {G%d{x questpoints.\n\r",qps);
                send_to_char(buf,victim);
	    }

	    make_corpse(victim,killer);

  /* GM, transfer the corpse to the clan morgue... */
	   if ( !IS_NPC(victim) && (is_clan(victim) &&
	        clan_table[victim->clan].room[1] != ROOM_VNUM_ALTAR &&
	        clan_table[victim->clan].room[1] > 0) )
	   {
	     OBJ_DATA *corpse;
	     ROOM_INDEX_DATA *morgue;

	     corpse = get_obj_list(victim, "corpse", victim->in_room->contents );
	     morgue = get_room_index(clan_table[victim->clan].room[1]);
    
	     /* Bug fix provided by Mat Reda.  Thanks Matt! */
	     if (corpse != NULL && morgue != NULL)
	      {
	        obj_from_room(corpse);
	        obj_to_room(corpse, morgue);
	        if (victim->alignment > 0)
	         act("The Creator protects this one...",victim,NULL,NULL,TO_ROOM);
	        else
	         act("The Lord of the Grave claims his own!",victim,NULL,NULL,TO_ROOM);
	      }
	   }
	}


    if ( IS_NPC(victim) )
    {
	victim->pIndexData->killed++;
	kill_table[URANGE(0, victim->level, MAX_LEVEL-1)].killed++;
	extract_char( victim, TRUE );
	return;
    }

    extract_char( victim, FALSE );

    while ( victim->affected )
	affect_remove( victim, victim->affected );
    victim->affected_by	= race_table[victim->race].aff;
    victim->shielded_by	= race_table[victim->race].shd;
        if( ! (IS_NPC(killer) && !IS_NPC(victim)) )
	{
	    for (i = 0; i < 4; i++)
	    	victim->armor[i]= 100;
	}
    victim->position	= POS_RESTING;
    victim->hit		= UMAX( 1, victim->hit  );
    victim->mana	= UMAX( 1, victim->mana );
    victim->move	= UMAX( 1, victim->move );

/*
    if (wascarrying)
    {
        for ( obj = wascarrying; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;

            obj_to_char(obj, victim);

            if(obj->wear_loc != WEAR_NONE)
                equip_char(victim,obj,obj->wear_loc); 
        }
    }
*/

/*  save_char_obj( victim ); we're stable enough to not need this :) */

    if ( victim->in_arena )
        arena_death(victim, killer);
    else
    {
        if(is_pkill(victim))
        {
            AFFECT_DATA af;

	    af.where      = TO_SHIELDS;
	    af.type       = skill_lookup("peace");
	    af.level      = 200;
	    af.duration   = 3;
	    af.location   = APPLY_NONE;
	    af.modifier   = 0;
	    af.bitvector  = SHD_PEACE;

            affect_join(victim, &af);
        }

    }

    if(victim != killer && !IS_NPC(victim) && !IS_NPC(killer)
          && !IS_IMMORTAL(victim) && !IS_IMMORTAL(killer))
    update_pk_count(killer,victim);


    return;
}



void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    int xp;
    int members;
    int group_levels;

    /*
     * Monsters don't get kill xp's or alignment changes.
     * P-killing doesn't help either.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if ( victim == ch )
	return;
    
    members = 0;
    group_levels = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
        {
	    members++;
	    group_levels += IS_NPC(gch) ?
                ((IS_SET(gch->act, ACT_PET) && gch->master == ch) 
                   ? gch->level / 8 : gch->level / 2)
                : gch->level;
	}
    }

    if ( members == 0 )
    {
	bug( "Group_gain: members.", members );
	members = 1;
	group_levels = ch->level ;
    }

    lch = (ch->leader != NULL) ? ch->leader : ch;

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	if ( !is_same_group( gch, ch ) || IS_NPC(gch))
	    continue;

/*	Taken out, add it back if you want it
	if ( gch->level - lch->level >= 5 )
	{
	    send_to_char( "You are too high for this group.\n\r", gch );
	    continue;
	}

	if ( gch->level - lch->level <= -5 )
	{
	    send_to_char( "You are too low for this group.\n\r", gch );
	    continue;
	}
*/

	xp = xp_compute( gch, victim, group_levels ) * 1;
	sprintf( buf, "{BYou receive {W%d{B experience points.{x\n\r", xp );
	send_to_char( buf, gch );
	gain_exp( gch, xp );

	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->wear_loc == WEAR_NONE )
		continue;

	    if ( ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch)    )
	    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch)    )
	    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch) ) )
	    {
		act( "{cYou are {Wzapped{c by $p.{x", ch, obj, NULL, TO_CHAR );
		act( "$n is {Wzapped{x by $p.",   ch, obj, NULL, TO_ROOM );
		obj_from_char( obj );
		obj_to_room( obj, ch->in_room );
	    }
	}

        if (!IS_NPC(gch) && IS_SET(gch->plt,PLT_QUESTOR )
        && IS_NPC(victim))
        {
            if (gch->pcdata->questmob == victim->pIndexData->vnum)
            {

                if (gch->pcdata->questmob > 0 && gch->pcdata->questgiver->short_descr)
                {
                    sprintf(buf, "{RYour {Gquest{R is ALMOST complete!{x\n\r"
                                 "Get back to {G%s{x before you run out of time!\n\r",
                    gch->pcdata->questgiver->short_descr);
                send_to_char(buf, gch);
                gch->pcdata->questmob *= -1;
                }
            }
        }

    }

    return;
}



/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim, int total_levels )
{
    int xp,base_exp;
    int align,level_range,capped_range;
    int change;
    float exp_align_multi;
    int time_per_level;

    if( gch->in_arena ) return 0;

    level_range = victim->level - gch->level;
    if (!IS_NPC(gch))
    {
	if (gch->class >= MAX_CLASS/2)
	    level_range -= 5;
    }
 
    if (!IS_NPC(gch) && !IS_NPC(victim))
    {
        if(gch->alignment > victim->alignment
        && gch->alignment - victim->alignment > 250)
        {
        }
        else if(gch->alignment < victim->alignment
        && victim->alignment - gch->alignment > 250)
        {
        }
        else
        {
            if(IS_GOOD(victim))
                gch->alignment -= 50;
        }

	if ( gch->pet != NULL )
	    gch->pet->alignment = gch->alignment;

	xp = 1;
	return xp;
    }

	capped_range = UMAX(UMIN( level_range, 29 ),-9);
	base_exp = 12 * capped_range + 108 + exp( log(3) * capped_range / 6 );

    /* do alignment computations */
   
    align = victim->alignment - gch->alignment;

    if(victim->alignment > 800 && gch->alignment > 800)
        align += 550;

    exp_align_multi = UMAX(base_exp / 500, 0.05);

    if(victim->alignment > 800 && gch->level > victim->level + 20)
        exp_align_multi += 0.5;

    if (IS_SET(victim->act,ACT_NOALIGN))
    {
	/* no change */
    }

    else if (align > 500) /* monster is more good than slayer */
    {
	change = (align - 500) * exp_align_multi * gch->level/total_levels;
	change = UMAX(1,change);
        gch->alignment = UMAX(-1000,gch->alignment - change);
	if ( gch->pet != NULL )
	    gch->pet->alignment = gch->alignment;
    }

    else if (align < -500) /* monster is more evil than slayer */
    {
	change =  ( -1 * align - 500) * exp_align_multi * gch->level/total_levels;
	change = UMAX(1,change);
	gch->alignment = UMIN(1000,gch->alignment + change);
        if ( gch->pet != NULL )
            gch->pet->alignment = gch->alignment;
    }

    else /* improve this someday */
    {
	change =  align * exp_align_multi * gch->level/total_levels;  
	gch->alignment -= change;
        if ( gch->pet != NULL )
            gch->pet->alignment = gch->alignment;
    }
    
    /* calculate exp multiplier */
    if (IS_SET(victim->act,ACT_NOALIGN))
	xp = base_exp;

    else if (gch->alignment > 500)  /* for goodie two shoes */
    {
	if (victim->alignment < -750)
	    xp = (base_exp *4)/3;
   
 	else if (victim->alignment < -500)
	    xp = (base_exp * 5)/4;

        else if (victim->alignment < -250)
	    xp = (base_exp * 3)/4; 

        else if (victim->alignment > 750)
	    xp = base_exp / 4;

   	else if (victim->alignment > 500)
	    xp = base_exp / 2;

	else
	    xp = base_exp;
    }

    else if (gch->alignment < -500) /* for baddies */
    {
	if (victim->alignment > 750)
	    xp = (base_exp * 5)/4;
	
  	else if (victim->alignment > 500)
	    xp = (base_exp * 11)/10; 

   	else if (victim->alignment < -750)
	    xp = base_exp/2;

	else if (victim->alignment < -500)
	    xp = (base_exp * 3)/4;

	else if (victim->alignment < -250)
	    xp = (base_exp * 9)/10;

	else
	    xp = base_exp;
    }

    else if (gch->alignment > 200)  /* a little good */
    {

	if (victim->alignment < -500)
	    xp = (base_exp * 6)/5;

 	else if (victim->alignment > 750)
	    xp = base_exp/2;

	else if (victim->alignment > 0)
	    xp = (base_exp * 3)/4; 
	
	else
	    xp = base_exp;
    }

    else if (gch->alignment < -200) /* a little bad */
    {
	if (victim->alignment > 500)
	    xp = (base_exp * 6)/5;
 
	else if (victim->alignment < -750)
	    xp = base_exp/2;

	else if (victim->alignment < 0)
	    xp = (base_exp * 3)/4;

	else
	    xp = base_exp;
    }

    else /* neutral */
    {

	if (victim->alignment > 500 || victim->alignment < -500)
	    xp = (base_exp * 4)/3;

	else if (victim->alignment < 200 && victim->alignment > -200)
	    xp = base_exp/2;

 	else
	    xp = base_exp;
    }

    /* more exp at the low levels */
    if (gch->level < 11)
    	xp = 14 * xp / (gch->level + 4);

    /* less at high */
    if (gch->level > 80 )
	xp =  (28 * xp) / gch->level;
    else if (gch->level > 50 )
	xp =  (30 * xp) / ((level_range > 0)?gch->level:gch->level-35);

    /* reduce for playing time */

    {
	/* compute quarter-hours per level */
	time_per_level = 4 *
			 (gch->played + (int) (current_time - gch->logon))/3600
			 / gch->level;

	time_per_level = URANGE(2,time_per_level,12);
	if (gch->level < 25)  /* make it a curve */
	    time_per_level = UMAX(time_per_level,(25 - gch->level));
/*
 *	xp = xp * time_per_level / 12;
 */
    }
   
    /* randomize the rewards */
    xp = number_range (xp * 3/4, xp * 5/4);

    /* adjust for grouping */
    xp = xp * gch->level/( UMAX(1,total_levels -1) );

/*
    if(IS_NPC(victim) && victim->prekill_hit < victim->max_hit/2.5
       && level_range > 30)
        xp = xp * ((1.5*victim->prekill_hit)/victim->max_hit);
*/

    if(xp > exp_per_level(gch,gch->pcdata->points) * 4)
        xp = exp_per_level(gch,gch->pcdata->points) * 3.75;

    return xp;
}


void dam_message( CHAR_DATA *ch, CHAR_DATA *victim,int dam,int dt,bool immune )
{
    char buf1[256], buf2[256], buf3[256];
    const char *vs;
    const char *vp;
    const char *attack;
    char punct;
    int spam = 0;

    if (ch == NULL || victim == NULL)
	return;

	 if ( dam ==   0 ) { vs = "{dmiss";	vp = "misses";		}
    else if ( dam <=   4 ) { vs = "scratch";	vp = "scratches";	}
    else if ( dam <=   8 ) { vs = "graze";	vp = "grazes";		}
    else if ( dam <=  12 ) { vs = "hit";	vp = "hits";		}
    else if ( dam <=  16 ) { vs = "injure";	vp = "injures";		}
    else if ( dam <=  20 ) { vs = "wound";	vp = "wounds";		}
    else if ( dam <=  24 ) { vs = "maul";       vp = "mauls";		}
    else if ( dam <=  28 ) { vs = "decimate";	vp = "decimates";	}
    else if ( dam <=  32 ) { vs = "devastate";	vp = "devastates";	}
    else if ( dam <=  36 ) { vs = "maim";	vp = "maims";		}
    else if ( dam <=  40 ) { vs = "MUTILATE";	vp = "MUTILATES";	}
    else if ( dam <=  44 ) { vs = "DISEMBOWEL";	vp = "DISEMBOWELS";	}
    else if ( dam <=  48 ) { vs = "DISMEMBER";	vp = "DISMEMBERS";	}
    else if ( dam <=  52 ) { vs = "MASSACRE";	vp = "MASSACRES";	}
    else if ( dam <=  56 ) { vs = "MANGLE";	vp = "MANGLES";		}
    else if ( dam <=  60 ) { vs = "*** DEMOLISH ***";
			     vp = "*** DEMOLISHES ***";			}
    else if ( dam <=  75 ) { vs = "*** DEVASTATE ***";
			     vp = "*** DEVASTATES ***";			}
    else if ( dam <= 100)  { vs = "=== OBLITERATE ===";
			     vp = "=== OBLITERATES ===";		}
    else if ( dam <= 125)  { vs = "{+{M>>> {rANNIHILATE{m <<<";
			     vp = "{+{M>>> {rANNIHILATES {M<<<{=";	}
    else if ( dam <= 150)  { vs = "{+{G<<< {RERADICATE {G>>>{=";
			     vp = "{+{G<#< {RERADICATES {G>#>{="; }
    else                   { vs = "do %% {+{RUNSPEAKABLE {=%% things to";
			     vp = "does %% {+{RUNSPEAKABLE {=%% things to";}

    punct   = (dam <= 24) ? '.' : '!';

    if ( dt == TYPE_HIT )
    {
        if (ch  == victim)
        {
	    sprintf( buf1, "{k$n %s $melf%c {D({w%d{D){x",vp,punct,dam);
	    sprintf( buf2, "{hYou %s yourself%c {D({w%d{D){x",vs,punct,dam);
        }
        else
        {
	    sprintf( buf1, "{k$n %s $N%c {D({w%d{D){x",  vp, punct,dam );
	    sprintf( buf2, "{hYou %s $N%c {D({w%d{D){x", vs, punct,dam );
	    sprintf( buf3, "{i$n %s you%c {D({w%d{D){x", vp, punct,dam );
        }
    }
    else
    {
	if ( dt >= 0 && dt < MAX_SKILL )
	    attack	= skill_table[dt].noun_damage;
	else if ( dt >= TYPE_HIT
	&& dt < TYPE_HIT + MAX_DAMAGE_MESSAGE) 
	    attack	= attack_table[dt - TYPE_HIT].noun;
	else
	{
	    bug( "Dam_message: bad dt %d.", dt );
	    dt  = TYPE_HIT;
	    attack  = attack_table[0].name;
	}

	if (immune)
	{
	    if (ch == victim)
	    {
		sprintf(buf1,"{k$n is unaffected by $s own %s.{x",attack);
		sprintf(buf2,"{hLuckily, you are immune to that.{x");
	    } 
	    else
	    {
	    	sprintf(buf1,"{k$N is unaffected by $n's %s!{x",attack);
	    	sprintf(buf2,"{h$N is unaffected by your %s!{x",attack);
	    	sprintf(buf3,"{i$n's %s is powerless against you.{x",attack);
	    }
	}
	else
	{
	    if (ch == victim)
	    {
		sprintf( buf1, "{k$n's %s %s $m%c {D({w%d{D){x",attack,vp,punct,dam);
		sprintf( buf2, "{hYour %s %s you%c {D({W%d{D){x",attack,vp,punct,dam);
	    }
	    else
	    {
	    	sprintf( buf1, "{k$n's %s %s $N%c {D({w%d{D){x", attack, vp, punct, dam );
	    	sprintf( buf2, "{hYour %s %s $N%c {D({W%d{D){x", attack, vp, punct, dam );
	    	sprintf( buf3, "{i$n's %s %s you%c {D({w%d{D){x", attack, vp, punct, dam );
	    }
	}
    }

    if(dam == 0) spam = ACTION_BATTLE_NODAM;

    if (ch == victim)
    {
	act2(buf1,ch,NULL,NULL,TO_ROOM, spam);
	act2(buf2,ch,NULL,NULL,TO_CHAR, spam);
    }
    else
    {
    	act_new_2( buf1, ch, NULL, victim, TO_NOTVICT, POS_RESTING, 0, spam );
    	act_new_2( buf2, ch, NULL, victim, TO_CHAR, POS_RESTING, 0, spam );
    	act_new_2( buf3, ch, NULL, victim, TO_VICT, POS_RESTING, 0, spam );
    }

    return;
}



/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
	return;

    if ( IS_OBJ_STAT(obj,ITEM_NOREMOVE) || IS_OBJ_STAT(obj,ITEM_BOUND))
    {
	act("{j$S weapon won't budge!{x",ch,NULL,victim,TO_CHAR);
	act2("{j$n tries to disarm you, but your weapon won't budge!{x",
	    ch,NULL,victim,TO_VICT, ACTION_BATTLE_NODAM);
	act2("{k$n tries to disarm $N, but fails.{x",ch,NULL,victim,TO_NOTVICT, ACTION_BATTLE_NODAM);
	return;
    }

    act( "{j$n DISARMS you and sends your weapon flying!{x", 
	 ch, NULL, victim, TO_VICT    );
    act( "{jYou disarm $N!{x",  ch, NULL, victim, TO_CHAR    );
    act( "{k$n disarms $N!{x",  ch, NULL, victim, TO_NOTVICT );

    obj_from_char( obj );
    if ( IS_OBJ_STAT(obj,ITEM_NODROP) || IS_OBJ_STAT(obj,ITEM_INVENTORY) )
	obj_to_char( obj, victim );
    else
    {
	obj_to_room( obj, victim->in_room );
	if (IS_NPC(victim) && victim->wait == 0 && can_see_obj(victim,obj))
	    get_obj(victim,obj,NULL);
    }

    return;
}

void do_berserk( CHAR_DATA *ch, char *argument)
{
    int chance, hp_percent;

    if ((chance = get_skill(ch,gsn_berserk)) == 0
    ||  (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_BERSERK))
    ||  (!IS_NPC(ch)
    &&   ch->level < skill_table[gsn_berserk].skill_level[ch->class]))
    {
	send_to_char("{hYou turn {rred{h in the face, but nothing happens.{x\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch,AFF_BERSERK) || is_affected(ch,gsn_berserk)
    ||  is_affected(ch,skill_lookup("frenzy")))
    {
	send_to_char("{hYou get a little madder.{x\n\r",ch);
	return;
    }

    if (IS_AFFECTED(ch,AFF_CALM))
    {
	send_to_char("{hYou're feeling to mellow to berserk.{x\n\r",ch);
	return;
    }

    if (ch->mana < 50)
    {
	send_to_char("{hYou can't get up enough energy.{x\n\r",ch);
	return;
    }

    /* modifiers */

    /* fighting */
    if (ch->position == POS_FIGHTING)
	chance += 10;

    /* damage -- below 50% of hp helps, above hurts */
    hp_percent = 100 * ch->hit/ch->max_hit;
    chance += 25 - hp_percent/2;

    if (number_percent() < chance)
    {
	AFFECT_DATA af;

	WAIT_STATE(ch,PULSE_VIOLENCE);
	ch->mana -= 50;
	ch->move /= 2;

	/* heal a little damage */
	ch->hit += ch->level * 2;
	ch->hit = UMIN(ch->hit,ch->max_hit);

	send_to_char("{hYour pulse races as you are consumed by {rrage!{x\n\r",ch);
	act("{k$n gets a {cw{gi{rl{yd{k look in $s eyes.{x",ch,NULL,NULL,TO_ROOM);
	check_improve(ch,gsn_berserk,TRUE,2);

	af.where	= TO_AFFECTS;
	af.type		= gsn_berserk;
	af.level	= ch->level;
	af.duration	= number_fuzzy(ch->level / 8);
	af.modifier	= UMAX(1,ch->level/5);
	af.bitvector 	= AFF_BERSERK;

	af.location	= APPLY_HITROLL;
	affect_to_char(ch,&af);

	af.location	= APPLY_DAMROLL;
	affect_to_char(ch,&af);

	af.modifier	= UMAX(10,10 * (ch->level/5));
	af.location	= APPLY_AC;
	affect_to_char(ch,&af);
    }

    else
    {
	WAIT_STATE(ch,3 * PULSE_VIOLENCE);
	ch->mana -= 25;
	ch->move /= 2;

	send_to_char("{hYour pulse speeds up, but nothing happens.{x\n\r",ch);
	check_improve(ch,gsn_berserk,FALSE,2);
    }
}

void do_voodoo( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *doll;
 
    if (IS_NPC(ch))
        return;
 
    doll = get_eq_char(ch,WEAR_HOLD);
    if  (doll == NULL || (doll->pIndexData->vnum != OBJ_VNUM_VOODOO))
    {
        send_to_char("You are not holding a voodoo doll.\n\r",ch);
        return;
    }

    one_argument(argument,arg);

    if (arg[0] == '\0')
    {
	send_to_char("Syntax: voodoo <action>\n\r",ch);
	send_to_char("Actions: pin trip throw\n\r",ch);
	return;
    }

    if (!str_cmp(arg,"pin"))
    {
	do_vdpi(ch,doll->name);
	return;
    }

    if (!str_cmp(arg,"trip"))
    {
	do_vdtr(ch,doll->name);
	return;
    }

    if (!str_cmp(arg,"throw"))
    {
	do_vdth(ch,doll->name);
	return;
    }

    do_voodoo(ch,"");
}

void do_vdpi( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    AFFECT_DATA af;
    bool found = FALSE;

    argument = one_argument( argument, arg1 );

    for (d = descriptor_list; d != NULL; d = d->next)
    {
	CHAR_DATA *wch;

	if (d->connected != CON_PLAYING || !can_see(ch,d->character))
	    continue;

	wch = ( d->original != NULL ) ? d->original : d->character;

	if (!can_see(ch,wch))
	    continue;

	if (!str_cmp(arg1,wch->name) && !found)
	{
	    if (IS_NPC(wch))
		continue;

	    if (IS_IMMORTAL(wch) && (wch->level > ch->level))
	    {
		send_to_char( "That's not a good idea.\n\r", ch);
		return;
	    }

	    if ((wch->level < 20) && !IS_IMMORTAL(ch))
	    {
		send_to_char( "They are a little too young for that.\n\r",ch);
		return;
	    }

	    if (IS_SHIELDED(wch,SHD_PROTECT_VOODOO))
	    {
		send_to_char( "They are still reeling from a previous voodoo.\n\r",ch);
		return;
	    }

	    if (IS_SET(wch->in_room->room_flags,ROOM_SAFE) &&
		!IS_IMMORTAL(ch))
            {
		send_to_char( "They are protected by the Gods.\n\r", ch);
		return;
            }

	    found = TRUE;

	    send_to_char( "You stick a pin into your voodoo doll.\n\r",ch);
	    act( "$n sticks a pin into a voodoo doll.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "{RYou double over with a sudden pain in your gut!{x\n\r",wch);
	    act( "$n suddenly doubles over with a look of extreme pain!",wch, NULL,NULL,TO_ROOM);
	    af.where      = TO_SHIELDS;
	    af.type       = skill_lookup("protection voodoo");
	    af.level      = wch->level;
	    af.duration   = 1;
	    af.location   = APPLY_NONE;
	    af.modifier   = 0;
	    af.bitvector  = SHD_PROTECT_VOODOO;
	    affect_to_char(wch,&af);
	    return;
	}
    }
    send_to_char("Your victim doesn't seem to be in the realm.\n\r",ch);
    return;
}

void do_vdtr( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    AFFECT_DATA af;
    bool found = FALSE; 
 
    argument = one_argument( argument, arg1 );
 
    for (d = descriptor_list; d != NULL; d = d->next)
    { 
	CHAR_DATA *wch; 
 
	if (d->connected != CON_PLAYING || !can_see(ch,d->character)) 
	    continue; 
 
	wch = ( d->original != NULL ) ? d->original : d->character; 
 
	if (!can_see(ch,wch)) 
	    continue; 
 
	if (!str_cmp(arg1,wch->name) && !found) 
	{ 
	    if (IS_NPC(wch)) 
		continue; 
 
	    if (IS_IMMORTAL(wch) && (wch->level > ch->level)) 
	    {         
		send_to_char( "That's not a good idea.\n\r", ch); 
		return; 
	    } 
 
	    if ((wch->level < 20) && !IS_IMMORTAL(ch))
	    {
		send_to_char( "They are a little too young for that.\n\r",ch);
		return;
	    }

	    if (IS_SHIELDED(wch,SHD_PROTECT_VOODOO))
	    {
                send_to_char( "They are still reeling from a previous voodoo.\n\r",ch);
		return;
	    }

	    if (IS_SET(wch->in_room->room_flags,ROOM_SAFE) &&
		!IS_IMMORTAL(ch))
            {
		send_to_char( "They are protected by the Gods.\n\r", ch);
		return;
            }

	    found = TRUE;
 
	    send_to_char( "You slam your voodoo doll against the ground.\n\r",ch); 
	    act( "$n slams a voodoo doll against the ground.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "{RYour feet slide out from under you!{x\n\r",wch);
	    send_to_char( "{RYou hit the ground face first!{x\n\r",wch);
	    act( "$n trips over $s own feet, and does a nose dive into the ground!",wch, NULL,NULL,TO_ROOM);
            af.where      = TO_SHIELDS;
            af.type       = skill_lookup("protection voodoo");
            af.level      = wch->level;
            af.duration   = 1;
            af.location   = APPLY_NONE;
            af.modifier   = 0;
            af.bitvector  = SHD_PROTECT_VOODOO;
            affect_to_char(wch,&af);
	    return;
	}
    }
    send_to_char("Your victim doesn't seem to be in the realm.\n\r",ch);
    return;
}

void do_vdth( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    AFFECT_DATA af;
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    bool found = FALSE; 
    int attempt;
 
    argument = one_argument( argument, arg1 );
 
    for (d = descriptor_list; d != NULL; d = d->next)
    { 
	CHAR_DATA *wch; 
 
	if (d->connected != CON_PLAYING || !can_see(ch,d->character)) 
	    continue; 
 
	wch = ( d->original != NULL ) ? d->original : d->character; 
 
	if (!can_see(ch,wch)) 
	    continue; 
 
	if (!str_cmp(arg1,wch->name) && !found) 
	{ 
	    if (IS_NPC(wch)) 
		continue; 
 
	    if (IS_IMMORTAL(wch) && (wch->level > ch->level)) 
	    {         
		send_to_char( "That's not a good idea.\n\r", ch); 
		return; 
	    } 
 
	    if ((wch->level < 20) && !IS_IMMORTAL(ch))
	    {
		send_to_char("They are a little too young for that.\n\r",ch);
		return;
	    }

	    if (IS_SHIELDED(wch,SHD_PROTECT_VOODOO))
	    {
                send_to_char( "They are still reeling from a previous voodoo.\n\r",ch);
		return;
	    }

	    if (IS_SET(wch->in_room->room_flags,ROOM_SAFE) &&
		!IS_IMMORTAL(ch))
            {
		send_to_char( "They are protected by the Gods.\n\r", ch);
		return;
            }

	    found = TRUE;
 
	    send_to_char( "You toss your voodoo doll into the air.\n\r",ch); 
	    act( "$n tosses a voodoo doll into the air.", ch, NULL, NULL, TO_ROOM );
            af.where      = TO_SHIELDS;
            af.type       = skill_lookup("protection voodoo");
            af.level      = wch->level;
            af.duration   = 1;
            af.location   = APPLY_NONE;
            af.modifier   = 0;
            af.bitvector  = SHD_PROTECT_VOODOO;
            affect_to_char(wch,&af);
	    if ((wch->fighting != NULL) || (number_percent() < 25))
	    {
		send_to_char( "{RA sudden gust of wind throws you through the air!{x\n\r",wch);
		send_to_char( "{RYou slam face first into the nearest wall!{x\n\r",wch);
		act( "A sudden gust of wind picks up $n and throws $m into a wall!",wch, NULL,NULL,TO_ROOM);
		return;
	    }
	    wch->position = POS_STANDING;
	    was_in = wch->in_room;
	    for ( attempt = 0; attempt < 6; attempt++ )
	    {
		EXIT_DATA *pexit;
		int door;

		door = number_door( );
		if ( ( pexit = was_in->exit[door] ) == 0
		||   pexit->u1.to_room == NULL
		||   IS_SET(pexit->exit_info, EX_CLOSED)
		|| ( IS_NPC(wch)
		&&   IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB) ) )
		    continue;

		move_char( wch, door, FALSE, TRUE );
		if ( ( now_in = wch->in_room ) == was_in )
		    continue;

		wch->in_room = was_in;
		sprintf(buf, "A sudden gust of wind picks up $n and throws $m to the %s.", dir_name[door]);
		act( buf, wch, NULL, NULL, TO_ROOM );
		send_to_char( "{RA sudden gust of wind throws you through the air!{x\n\r",wch);
		wch->in_room = now_in;
		act( "$n sails into the room and slams face first into a wall!",wch,NULL,NULL,TO_ROOM);
		do_look( wch, "auto" );
		send_to_char( "{RYou slam face first into the nearest wall!{x\n\r",wch);
		return;
	    }
	    send_to_char( "{RA sudden gust of wind throws you through the air!{x\n\r",wch);
            send_to_char( "{RYou slam face first into the nearest wall!{x\n\r",wch);
            act( "A sudden gust of wind picks up $n and throws $m into a wall!",wch, NULL,NULL,TO_ROOM);
            return;
	}
    }
    send_to_char("Your victim doesn't seem to be in the realm.\n\r",ch);
    return;
}

void do_bash( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument(argument,arg);
 
    if ( (chance = get_skill(ch,gsn_bash)) == 0
    ||	 (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_BASH))
    ||	 (!IS_NPC(ch)
    &&	  ch->level < skill_table[gsn_bash].skill_level[ch->class]))
    {	
	send_to_char("Bashing? What's that?\n\r",ch);
	return;
    }
 
    if (arg[0] == '\0')
    {
	victim = ch->fighting;
	if (victim == NULL)
	{
	    send_to_char("But you aren't fighting anyone!\n\r",ch);
	    return;
	}
    }

    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }

    if (victim->position < POS_FIGHTING)
    {
	act("You'll have to let $M get back up first.",ch,NULL,victim,TO_CHAR);
	return;
    } 

    if (victim == ch)
    {
	send_to_char("You try to bash your brains out, but fail.\n\r",ch);
	return;
    }

    if (is_safe(ch,victim))
	return;

    if ( IS_NPC(victim) && 
	victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
	act("But $N is your friend!",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

    if ( !can_see( ch, victim ) )
    {
	send_to_char("You get a running start, and slam right into a wall.\n\r",ch);
	return;
    }

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }

    /* modifiers */

    /* size  and weight */
    chance += ch->carry_weight / 200;
    chance -= victim->carry_weight / 200;

    if (ch->size < victim->size)
	chance += (ch->size - victim->size) * 15;
    else
	chance += (ch->size - victim->size) * 10; 


    /* stats */
    chance += get_curr_stat(ch,STAT_STR);
    chance -= (get_curr_stat(victim,STAT_DEX) * 4)/3;
    chance -= GET_AC(victim,AC_BASH) /25;
    /* speed */
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
        chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
        chance -= 30;

    /* level */
    chance += (ch->level - victim->level);

    if (!IS_NPC(victim) 
	&& chance < get_skill(victim,gsn_dodge) )
    {	/*
        act("{i$n tries to bash you, but you dodge it.{x",ch,NULL,victim,TO_VICT);
        act("{h$N dodges your bash, you fall flat on your face.{x",ch,NULL,victim,TO_CHAR);
        WAIT_STATE(ch,skill_table[gsn_bash].beats);
        return;*/
	chance -= 2 * (get_skill(victim,gsn_dodge) - chance);
    }

    /* now the attack */
    if (number_percent() < chance )
    {
    
	act("{i$n sends you sprawling with a powerful bash!{x",
		ch,NULL,victim,TO_VICT);
	act("{hYou slam into $N, and send $M flying!{x",ch,NULL,victim,TO_CHAR);
	act("{k$n sends $N sprawling with a powerful bash.{x",
		ch,NULL,victim,TO_NOTVICT);
	check_improve(ch,gsn_bash,TRUE,1);

	DAZE_STATE(victim, 4 * PULSE_VIOLENCE);
	WAIT_STATE(ch,skill_table[gsn_bash].beats);
	victim->position = POS_RESTING;
	damage(ch,victim,number_range(5,5 + 2 * ch->size + 
               chance/20),gsn_bash,  DAM_BASH,FALSE);
	chance = (get_skill(ch,gsn_stun)/5);
	if (number_percent() < chance )
	{
	    chance = (get_skill(ch,gsn_stun)/5);
	    if (number_percent() < chance )
	    {
		victim->stunned = number_range(2,3);
	    } else {
		victim->stunned = 1;
	    }
	    act("{iYou are stunned, and have trouble getting back up!{x",
		ch,NULL,victim,TO_VICT);
	    act("{h$N is stunned by your bash!{x",ch,NULL,victim,TO_CHAR);
	    act("{k$N is having trouble getting back up.{x",
		ch,NULL,victim,TO_NOTVICT);
	    check_improve(ch,gsn_stun,TRUE,1);
	}
    }
    else
    {
	damage(ch,victim,0,gsn_bash,DAM_BASH,FALSE);
	act("{hYou fall flat on your face!{x",
	    ch,NULL,victim,TO_CHAR);
	act("{k$n falls flat on $s face.{x",
	    ch,NULL,victim,TO_NOTVICT);
	act("{iYou evade $n's bash, causing $m to fall flat on $s face.{x",
	    ch,NULL,victim,TO_VICT);
	check_improve(ch,gsn_bash,FALSE,1);
	ch->position = POS_RESTING;
	WAIT_STATE(ch,skill_table[gsn_bash].beats * 3/2); 
    }
}

void do_dirt( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument(argument,arg);

    if ( (chance = get_skill(ch,gsn_dirt)) == 0
    ||   (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_KICK_DIRT))
    ||   (!IS_NPC(ch)
    &&    ch->level < skill_table[gsn_dirt].skill_level[ch->class]))
    {
	send_to_char("{hYou get your feet dirty.{x\n\r",ch);
	return;
    }

    if (arg[0] == '\0')
    {
	victim = ch->fighting;
	if (victim == NULL)
	{
	    send_to_char("But you aren't in combat!\n\r",ch);
	    return;
	}
    }

    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(victim,AFF_BLIND))
    {
	act("{h$E's already been blinded.{x",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (victim == ch)
    {
	send_to_char("Very funny.\n\r",ch);
	return;
    }

    if (is_safe(ch,victim))
	return;

    if (IS_NPC(victim) &&
	 victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
	act("But $N is such a good friend!",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }

    /* modifiers */

    /* dexterity */
    chance += get_curr_stat(ch,STAT_DEX);
    chance -= 2 * get_curr_stat(victim,STAT_DEX);

    /* speed  */
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
	chance -= 25;

    /* level */
    chance += (ch->level - victim->level) * 2;

    /* sloppy hack to prevent false zeroes */
    if (chance % 5 == 0)
	chance += 1;

    /* terrain */

    switch(ch->in_room->sector_type)
    {
	case(SECT_INSIDE):		chance -= 20;	break;
	case(SECT_CITY):		chance -= 10;	break;
	case(SECT_FIELD):		chance +=  5;	break;
	case(SECT_FOREST):				break;
	case(SECT_HILLS):				break;
	case(SECT_MOUNTAIN):		chance -= 10;	break;
	case(SECT_WATER_SWIM):		chance  =  0;	break;
	case(SECT_WATER_NOSWIM):	chance  =  0;	break;
	case(SECT_AIR):			chance  =  0;  	break;
	case(SECT_VOID):		chance  =  0;  	break;
	case(SECT_INFERNO):		chance +=  5;  	break;
	case(SECT_DESERT):		chance += 20;   break;
    }

    if (chance == 0)
    {
	send_to_char("{hThere isn't any dirt to kick.{x\n\r",ch);
	return;
    }

    if(victim->in_arena)
        chance -= 30;

    /* now the attack */
    if (number_percent() < chance)
    {
	AFFECT_DATA af;
	act("{k$n is blinded by the dirt in $s eyes!{x",victim,NULL,NULL,TO_ROOM);
	act("{i$n kicks dirt in your eyes!{x",ch,NULL,victim,TO_VICT);

        damage(ch,victim,number_range(2,5),gsn_dirt,DAM_NONE,FALSE);
	send_to_char("{DYou can't see a thing!{x\n\r",victim);
	check_improve(ch,gsn_dirt,TRUE,2);
	WAIT_STATE(ch,skill_table[gsn_dirt].beats);

	af.where	= TO_AFFECTS;
	af.type 	= gsn_dirt;
	af.level 	= ch->level;
	af.duration	= 0;
	af.location	= APPLY_HITROLL;
	af.modifier	= -4;
	af.bitvector 	= AFF_BLIND;

	affect_to_char(victim,&af);
    }
    else
    {
	damage(ch,victim,0,gsn_dirt,DAM_NONE,TRUE);
	check_improve(ch,gsn_dirt,FALSE,2);
	WAIT_STATE(ch,skill_table[gsn_dirt].beats);
    }
}

void do_gouge( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument(argument,arg);

    if ( (chance = get_skill(ch,gsn_gouge)) == 0
    ||   (!IS_NPC(ch)
    &&    ch->level < skill_table[gsn_gouge].skill_level[ch->class]))
    {
	send_to_char("Gouge?  What's that?{x\n\r",ch);
	return;
    }

    if (arg[0] == '\0')
    {
	victim = ch->fighting;
	if (victim == NULL)
	{
	    send_to_char("But you aren't in combat!\n\r",ch);
	    return;
	}
    }

    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }

    if (IS_AFFECTED(victim,AFF_BLIND))
    {
	act("{h$E's already been blinded.{x",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (victim == ch)
    {
	send_to_char("Very funny.\n\r",ch);
	return;
    }

    if (is_safe(ch,victim))
	return;

    if (IS_NPC(victim) &&
	 victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
	act("But $N is such a good friend!",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }

    /* modifiers */

    /* dexterity */
    chance += get_curr_stat(ch,STAT_DEX);
    chance -= 2 * get_curr_stat(victim,STAT_DEX);

    /* speed  */
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
	chance -= 25;

    /* level */
    chance += (ch->level - victim->level) * 2;

    if(victim->in_arena)
        chance -= 30;

    /* sloppy hack to prevent false zeroes */
    if (chance % 5 == 0)
	chance += 1;

    /* now the attack */
    if (number_percent() < chance)
    {
	AFFECT_DATA af;
	act("{k$n is blinded by a poke in the eyes!{x",victim,NULL,NULL,TO_ROOM);
	act("{i$n gouges at your eyes!{x",ch,NULL,victim,TO_VICT);
        damage(ch,victim,number_range(2,5),gsn_gouge,DAM_NONE,FALSE);
	send_to_char("{DYou see nothing but stars!{x\n\r",victim);
	check_improve(ch,gsn_gouge,TRUE,2);
	WAIT_STATE(ch,skill_table[gsn_gouge].beats);

	af.where	= TO_AFFECTS;
	af.type 	= gsn_gouge;
	af.level 	= ch->level;
	af.duration	= 0;
	af.location	= APPLY_HITROLL;
	af.modifier	= -4;
	af.bitvector 	= AFF_BLIND;

	affect_to_char(victim,&af);
    }
    else
    {
	damage(ch,victim,0,gsn_gouge,DAM_NONE,TRUE);
	check_improve(ch,gsn_gouge,FALSE,2);
	WAIT_STATE(ch,skill_table[gsn_gouge].beats);
    }
}

void do_trip( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument(argument,arg);

    if ( (chance = get_skill(ch,gsn_trip)) == 0
    ||   (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_TRIP))
    ||   (!IS_NPC(ch) 
	  && ch->level < skill_table[gsn_trip].skill_level[ch->class]))
    {
	send_to_char("Tripping?  What's that?\n\r",ch);
	return;
    }


    if (arg[0] == '\0')
    {
	victim = ch->fighting;
	if (victim == NULL)
	{
	    send_to_char("But you aren't fighting anyone!\n\r",ch);
	    return;
 	}
    }

    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }

    if (is_safe(ch,victim))
	return;

    if (IS_NPC(victim) &&
	 victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
	send_to_char("Kill stealing is not permitted.\n\r",ch);
	return;
    }
    
    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

    if (IS_AFFECTED(victim,AFF_FLYING))
    {
	act("{h$S feet aren't on the ground.{x",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (victim->position < POS_FIGHTING)
    {
	act("{h$N is already down.{c",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (victim == ch)
    {
	send_to_char("{hYou fall flat on your face!{x\n\r",ch);
	WAIT_STATE(ch,2 * skill_table[gsn_trip].beats);
	act("{k$n trips over $s own feet!{x",ch,NULL,NULL,TO_ROOM);
	return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
	act("$N is your beloved master.",ch,NULL,victim,TO_CHAR);
	return;
    }

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }

    /* modifiers */

    /* size */
    if (ch->size < victim->size)
        chance += (ch->size - victim->size) * 10;  /* bigger = harder to trip */

    /* dex */
    chance += get_curr_stat(ch,STAT_DEX);
    chance -= get_curr_stat(victim,STAT_DEX) * 3 / 2;

    /* speed */
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
	chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
	chance -= 20;

    /* level */
    chance += (ch->level - victim->level) * 2;


    /* now the attack */
    if (number_percent() < chance)
    {
	act("{i$n trips you and you go down!{x",ch,NULL,victim,TO_VICT);
	act("{hYou trip $N and $N goes down!{x",ch,NULL,victim,TO_CHAR);
	act("{k$n trips $N, sending $M to the ground.{x",ch,NULL,victim,TO_NOTVICT);
	check_improve(ch,gsn_trip,TRUE,1);

	DAZE_STATE(victim,2 * PULSE_VIOLENCE);
        WAIT_STATE(ch,skill_table[gsn_trip].beats);
	victim->position = POS_RESTING;
	damage(ch,victim,number_range(2, 2 +  2 * victim->size),gsn_trip,
	    DAM_BASH,TRUE);
    }
    else
    {
	damage(ch,victim,0,gsn_trip,DAM_BASH,TRUE);
	WAIT_STATE(ch,skill_table[gsn_trip].beats*2/3);
	check_improve(ch,gsn_trip,FALSE,1);
    } 
}



void do_kill( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Kill whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( victim == ch )
    {
	send_to_char( "{hYou hit yourself.  {z{COuch!{x\n\r", ch );
	multi_hit( ch, ch, TYPE_UNDEFINED );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if ( !IS_NPC(victim) )
    {
        if ( !IS_SET(victim->act, PLR_TWIT) || victim->in_arena )
        {
            send_to_char( "You must MURDER a player.\n\r", ch );
            return;
        }
    }
    if ( victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}

void do_mock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Mock hit whom?\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
        send_to_char( "You can't pretend to do ... anything.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( is_safe_mock( ch, victim ) )
	return;

    if ( victim->fighting != NULL)
    {
        send_to_char("{gThis player is busy at the moment.{x\n\r",ch);
        return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "{gYou've already got your hands full!{x\n\r", ch );
	return;
    }

    one_hit_mock( ch, victim, TYPE_UNDEFINED, FALSE );

    return;
}

void do_murde( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to {RMURDER{x, spell it out.\n\r", ch );
    return;
}



void do_murder( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Murder whom?\n\r", ch );
	return;
    }

    if (IS_NPC(ch))
	return;

    if (IS_AFFECTED(ch,AFF_CHARM))
	return;

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
        WAIT_STATE( ch, 4);
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if (IS_NPC(victim) &&
	 victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );

    if (!IS_NPC(ch))
    	sprintf( buf, "Help! I am being attacked by %s!", ch->name );
    
    if(!ch->in_arena)
        do_shout( victim, buf );

    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}



void do_backstab( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int chance;

    one_argument( argument, arg );

    if (arg[0] == '\0')
    {
        send_to_char("Backstab whom?\n\r",ch);
        return;
    }

    if (ch->fighting != NULL)
    {
	send_to_char("{hYou're facing the wrong end.{x\n\r",ch);
	return;
    }
 
    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        WAIT_STATE( ch, 3 );
        return;
    }

    if ( victim == ch )
    {
	send_to_char( "How can you sneak up on yourself?\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if (IS_NPC(victim) &&
	 victim->fighting != NULL && 
	!is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL)
    {
	send_to_char( "{hYou need to wield a primary weapon to backstab.{x\n\r", ch );
	return;
    }

    if ( victim->hit < victim->max_hit / 2
       || (!can_see(victim,ch) && victim->hit < victim->max_hit / 3.2) )
    {
	act( "$N is hurt and suspicious ... you can't sneak up.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ( ch->fighting == NULL )
    && ( !IS_NPC( ch ) )
    && ( !IS_NPC( victim ) ) )
    {
	ch->attacker = TRUE;
	victim->attacker = FALSE;
    }

    WAIT_STATE( ch, skill_table[gsn_backstab].beats );

    chance = get_skill(ch,gsn_backstab);

    if( can_see(victim,ch) )
        chance /= 1.1;

    if ( number_percent( ) < chance
    || ( get_skill(ch,gsn_backstab) >= 50 && !IS_AWAKE(victim) ) )
    {
	check_improve(ch,gsn_backstab,TRUE,1);
	multi_hit( ch, victim, gsn_backstab );
    }
    else
    {
	check_improve(ch,gsn_backstab,FALSE,1);
	damage( ch, victim, 0, gsn_backstab,DAM_NONE,TRUE);
    }

    return;
}

void do_circle( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
  
    if ( get_skill(ch,gsn_circle) == 0
    ||	 (!IS_NPC(ch)
    &&	  ch->level < skill_table[gsn_circle].skill_level[ch->class]))
    {	
	send_to_char("Circle? What's that?\n\r",ch);
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }
 
    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL)
    {
        send_to_char( "You need to wield a primary weapon to circle.\n\r", ch );
        return;
    }

    if ( victim->hit < victim->max_hit / 6)
    {
	act( "$N is hurt and suspicious ... you can't sneak around.",
	    ch, NULL, victim, TO_CHAR );
	return;
    }
  
    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

    if ( !can_see( ch, victim ) )
    {
	send_to_char("You stumble blindly into a wall.\n\r",ch);
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_circle].beats );
    if ( number_percent( ) < get_skill(ch,gsn_circle)
    || ( get_skill(ch,gsn_circle) >= 2 && !IS_AWAKE(victim) ))
    {
        check_improve(ch,gsn_circle,TRUE,1);
	act( "{i$n circles around behind you.{x", ch, NULL, victim, TO_VICT);
	act( "{hYou circle around $N.{x", ch, NULL, victim, TO_CHAR);
	act( "{k$n circles around behind $N.{x", ch, NULL, victim, TO_NOTVICT);
        multi_hit( ch, victim, gsn_circle );
    }
    else
    {
        check_improve(ch,gsn_circle,FALSE,1);
	act( "{i$n tries to circle around you.{x", ch, NULL, victim, TO_VICT);
	act( "{h$N circles with you.{x", ch, NULL, victim, TO_CHAR);
	act( "{k$n tries to circle around $N.{x", ch, NULL, victim, TO_NOTVICT);
        damage( ch, victim, 0, gsn_circle,DAM_NONE,TRUE);
    }
 
    return;
}

void do_feed( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam;
  
    if ( get_skill(ch,gsn_feed) == 0
    ||	 (!IS_NPC(ch)
    &&	  ch->level < skill_table[gsn_feed].skill_level[ch->class]))
    {	
	send_to_char("Feed? What's that?\n\r",ch);
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }
 
    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }
  
    WAIT_STATE( ch, skill_table[gsn_feed].beats );
    if ( number_percent( ) < get_skill(ch,gsn_feed)/8 * 7
    || ( get_skill(ch,gsn_feed) >= 2 && !IS_AWAKE(victim) ) )
    {
        check_improve(ch,gsn_feed,TRUE,1);
	act( "{i$n bites you.{x", ch, NULL, victim, TO_VICT);
	act( "{hYou bite $N.{x", ch, NULL, victim, TO_CHAR);
	act( "{k$n bites $N.{x", ch, NULL, victim, TO_NOTVICT);

	dam=number_range( 2 * (ch->level + 50) * (ch->level - victim->level + LEVEL_HERO) / LEVEL_HERO,
		2 * (ch->level + 150) * (ch->level - victim->level + LEVEL_HERO) / LEVEL_HERO );

/*        dam = interpolate(1, 101, 1, 150,
            number_fuzzy(ch->level + victim->level)); */
        damage( ch, victim, dam, gsn_feed,DAM_NEGATIVE,TRUE);
    }
    else
    {
        check_improve(ch,gsn_feed,FALSE,1);
	act( "{i$n tries to bite you, but hits only air.{x", ch, NULL, victim, TO_VICT);
	act( "{hYou chomp a mouthfull of air.{x", ch, NULL, victim, TO_CHAR);
	act( "{k$n tries to bite $N.{x", ch, NULL, victim, TO_NOTVICT);
        damage( ch, victim, 0, gsn_feed,DAM_NEGATIVE,TRUE);
    }
 
    return;
}


void do_flee( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    CHAR_DATA *victim;
    int attempt;

    if ( ( victim = ch->fighting ) == NULL )
    {
        if ( ch->position == POS_FIGHTING )
            ch->position = POS_STANDING;
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    was_in = ch->in_room;
    for ( attempt = 0; attempt < 12; attempt++ )
    {
	EXIT_DATA *pexit;
	int door;
        int number;

        number = 60 - (can_see(victim,ch) ? 0 : 40)
                + 3 * (get_curr_stat(victim,STAT_DEX) - 
                       get_curr_stat(ch,STAT_DEX)); /* lower is better */

	door = number_door( );
	if ( ( pexit = was_in->exit[door] ) == 0
	||   pexit->u1.to_room == NULL
	|| ( IS_SET(pexit->exit_info, EX_CLOSED)
           && !IS_AFFECTED(ch, AFF_PASS_DOOR) )
	||   number_range(0,UMAX(ch->daze, 1)) != 0
        || number_percent() < number
	|| !can_see_room(ch, pexit->u1.to_room)
	|| ( IS_NPC(ch)
	&&   IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB) ) )
	    continue;

	move_char( ch, door, FALSE, FALSE );
	if ( ( now_in = ch->in_room ) == was_in )
	    continue;

	ch->in_room = was_in;
	act( "$n has {Yfled{x!", ch, NULL, NULL, TO_ROOM );
        if ( !IS_NPC(ch) )
        {
            send_to_char( "{BYou {Yflee{B from combat!{x\n\r", ch );
            if( ( (ch->class == 2) || (ch->class == (MAX_CLASS/2)+1) )
            && (number_percent() < 3*(ch->level/2) ) && !ch->in_arena)
            {
                if ( IS_NPC(victim) || ch->attacker == FALSE)
                {
                    send_to_char( "You {Ysnuck away{x safely.\n\r", ch);
                } else
                {    
                    send_to_char( "You feel something singe your butt on the way out.\n\r", ch);
                    act( "$n is nearly {Yzapped{x in the butt by a lightning bolt from above!", ch, NULL, NULL, TO_ROOM );
                    /* ch->hit -= (ch->hit/8); */
                }
            } else
            {    
                if ( !IS_NPC(victim) && ch->attacker == TRUE && !ch->in_arena )
                {
                    send_to_char( "The {RWrath of Thoth {YZAPS{x your butt on the way out!\n\r", ch);
                    act( "$n is {Yzapped{x in the butt by a lightning bolt from above!", ch, NULL, NULL, TO_ROOM );
                    /*ch->hit -= (ch->hit/4);*/
                }
                send_to_char( "You lost 20 exp.\n\r", ch);
                gain_exp( ch, -20 );
            }
        }    
	ch->in_room = now_in;
	stop_fighting( ch, TRUE );
	return;
    }

    send_to_char( "{z{CPANIC!{x{B You couldn't escape!{x\n\r", ch );
    return;
}



void do_rescue( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *fch;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Rescue whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "What about {Yfleeing{x instead?\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && IS_NPC(victim) )
    {
	send_to_char( "Doesn't need your help!\n\r", ch );
	return;
    }

    if ( ch->fighting == victim )
    {
	send_to_char( "Too late.\n\r", ch );
	return;
    }

    if ( ( fch = victim->fighting ) == NULL )
    {
	send_to_char( "That person is not fighting right now.\n\r", ch );
	return;
    }

    if ( IS_NPC(fch) && !is_same_group(ch,victim))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    WAIT_STATE( ch, skill_table[gsn_rescue].beats );
    if ( number_percent( ) > get_skill(ch,gsn_rescue))
    {
	send_to_char( "You fail the rescue.\n\r", ch );
	check_improve(ch,gsn_rescue,FALSE,1);
	return;
    }

    act( "{yYou rescue $N!{x",  ch, NULL, victim, TO_CHAR    );
    act( "{y$n rescues you!{x", ch, NULL, victim, TO_VICT    );
    act( "{y$n rescues $N!{x",  ch, NULL, victim, TO_NOTVICT );
    check_improve(ch,gsn_rescue,TRUE,1);

    stop_fighting( fch, FALSE );
    stop_fighting( victim, FALSE );

    set_fighting( ch, fch );
    set_fighting( fch, ch );
    return;
}



void do_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam;

    if ( !IS_NPC(ch)
    &&   ch->level < skill_table[gsn_kick].skill_level[ch->class] )
    {
	send_to_char(
	    "You better leave the martial arts to fighters.\n\r", ch );
	return;
    }

    if (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_KICK))
	return;

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

    dam = number_range( 1, ch->level );
    WAIT_STATE( ch, skill_table[gsn_kick].beats );
    if ( get_skill(ch,gsn_kick) > number_percent())
    {
	damage(ch,victim,number_range( dam, (ch->level*7.5) ), gsn_kick,DAM_BASH,TRUE);
	check_improve(ch,gsn_kick,TRUE,1);
    }
    else
    {
	damage( ch, victim, 0, gsn_kick,DAM_BASH,TRUE);
	check_improve(ch,gsn_kick,FALSE,1);
    }
    return;
}


void do_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int chance,hth,ch_weapon,vict_weapon,ch_vict_weapon;

    hth = 0;

    if ((chance = get_skill(ch,gsn_disarm)) == 0)
    {
	send_to_char( "You don't know how to disarm opponents.\n\r", ch );
	return;
    }

    if ( get_eq_char( ch, WEAR_WIELD ) == NULL 
    &&   ((hth = get_skill(ch,gsn_hand_to_hand)) < 80
    ||    (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_DISARM))))
    {
	send_to_char( "You must wield a weapon to disarm.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
    {
	send_to_char( "{hYour opponent is not wielding a weapon.{x\n\r", ch );
	return;
    }

    /* find weapon skills */
    ch_weapon = get_weapon_skill(ch,get_weapon_sn(ch));
    vict_weapon = get_weapon_skill(victim,get_weapon_sn(victim));
    ch_vict_weapon = get_weapon_skill(ch,get_weapon_sn(victim));

    /* modifiers */

    /* skill */
    if ( get_eq_char(ch,WEAR_WIELD) == NULL)
	chance = chance * hth/150;
    else
	chance = chance * ch_weapon/100;


    chance += (ch_vict_weapon/2 - vict_weapon) / 2; 

    /* dex vs. strength */
    chance += get_curr_stat(ch,STAT_DEX);
    chance -= 2 * get_curr_stat(victim,STAT_STR);
    chance -= get_curr_stat(victim,STAT_DEX)/3;

    /* level */
    chance += (ch->level - victim->level) * 2;

    if(can_see(ch,victim))
        chance /= 10;

    chance /= 3;
 
    /* and now the attack */
    if (number_percent() < chance)
    {
	if (((chance = get_skill(victim,gsn_grip)) == 0)
	||   (!IS_NPC(victim)
	&&    victim->level < skill_table[gsn_grip].skill_level[victim->class]))
	{
	    WAIT_STATE( ch, skill_table[gsn_disarm].beats );
	    disarm( ch, victim );
	    check_improve(ch,gsn_disarm,TRUE,1);
	    return;
	}
	if (number_percent() > (chance/5)*4)
	{
	    WAIT_STATE( ch, skill_table[gsn_disarm].beats );
	    disarm( ch, victim );
	    check_improve(ch,gsn_disarm,TRUE,1);
	    check_improve(victim,gsn_grip,FALSE,1);
	    return;
	}
	check_improve(victim,gsn_grip,TRUE,1);
    }
    WAIT_STATE(ch,skill_table[gsn_disarm].beats);
    act("{hYou fail to disarm $N.{x",ch,NULL,victim,TO_CHAR);
    act2("{i$n tries to disarm you, but fails.{x",ch,NULL,victim,TO_VICT, ACTION_BATTLE_NODAM);
    act2("{k$n tries to disarm $N, but fails.{x",ch,NULL,victim,TO_NOTVICT, ACTION_BATTLE_NODAM);
    check_improve(ch,gsn_disarm,FALSE,1);
    return;
}

void do_surrender( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *mob;
    if ( (mob = ch->fighting) == NULL )
    {
	send_to_char( "But you're not fighting!\n\r", ch );
	return;
    }

    act( "You surrender to $N!", ch, NULL, mob, TO_CHAR );
    act( "$n surrenders to you!", ch, NULL, mob, TO_VICT );
    act( "$n tries to surrender to $N!", ch, NULL, mob, TO_NOTVICT );
    stop_fighting( ch, TRUE );

    if( !IS_NPC(ch) && !IS_NPC(mob) )
    {
        WAIT_STATE(ch, 5 * PPS);
        do_sit(ch,"");
        return;
    }

    if ( !IS_NPC( ch ) && IS_NPC( mob ) 
    &&   ( !HAS_TRIGGER( mob, TRIG_SURR ) 
        || !mp_percent_trigger( mob, ch, NULL, NULL, TRIG_SURR ) ) )
    {
	act( "$N seems to ignore your cowardly act!", ch, NULL, mob, TO_CHAR );
	multi_hit( mob, ch, TYPE_UNDEFINED );
    }
}

void do_sla( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to {RSLAY{x, spell it out.\n\r", ch );
    return;
}



void do_slay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slay whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) && victim->level >= get_trust(ch) )
    {
	send_to_char( "{hYou failed.{c\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) || get_trust(ch) >= CREATOR )
    { 
	act( "{hYou slay $M in cold blood!{x",  ch, NULL, victim, TO_CHAR    );
	act( "{i$n slays you in cold blood!{x", ch, NULL, victim, TO_VICT    );
	act( "{k$n slays $N in cold blood!{x",  ch, NULL, victim, TO_NOTVICT );
	raw_kill( victim, ch );
    }
    else
    {
	act( "{i$N wields a sword called '{z{RGodSlayer{i'!{x",  ch, NULL, victim, TO_CHAR    );
	act( "{hYou wield a sword called '{z{RGodSlayer{h'!{x", ch, NULL, victim, TO_VICT    );
	act( "{k$N wields a sword called '{z{RGodSlayer{k'!{x",  ch, NULL, victim, TO_NOTVICT );
        act("{i$N's slice takes off your left arm!{x",ch,NULL,victim,TO_CHAR);
        act("{hYour slice takes off $n's left arm!{x",ch,NULL,victim,TO_VICT);
        act("{k$N's slice takes off $n's left arm!{x",ch,NULL,victim,TO_NOTVICT);
        act("{i$N's slice takes off your right arm!{x",ch,NULL,victim,TO_CHAR);
        act("{hYour slice takes off $n's right arm!{x",ch,NULL,victim,TO_VICT);
        act("{k$N's slice takes off $n's right arm!{x",ch,NULL,victim,TO_NOTVICT);
        act("{i$N's slice cuts off both of your legs!{x",ch,NULL,victim,TO_CHAR);
        act("{hYour slice cuts off both of $n's legs!{x",ch,NULL,victim,TO_VICT);
        act("{k$N's slice cuts off both of $n's legs!{x",ch,NULL,victim,TO_NOTVICT);
        act("{i$N's slice beheads you!{x",ch,NULL,victim,TO_CHAR);
        act("{hYour slice beheads $n!{x",ch,NULL,victim,TO_VICT);
        act("{k$N's slice beheads $n!{x",ch,NULL,victim,TO_NOTVICT);
        act("{iYou are DEAD!!!{x",ch,NULL,victim,TO_CHAR);
	act("{h$n is DEAD!!!{x",ch,NULL,victim,TO_VICT);
	act("{k$n is DEAD!!!{x", ch,NULL,victim,TO_NOTVICT);
	act("A sword called '{z{RGodSlayer{x' vanishes.",ch,NULL,victim,TO_VICT);
	act("A sword called '{z{RGodSlayer{x' vanishes.",ch,NULL,victim,TO_NOTVICT);
	raw_kill( ch, victim );
    }
    return;
}

void do_strike( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int count = 0;

    int swings = (number_range(1,6) < 6)
                   ? number_range(2,4) : number_range(5,6);

    if ( !IS_NPC(ch)
    &&   ch->level < skill_table[gsn_furor].skill_level[ch->class] )
    {
	send_to_char(
	    "You better leave the martial arts to fighters.\n\r", ch );
	return;
    }

    if (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_KICK))
	return;

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return;
    }

    WAIT_STATE( ch, skill_table[gsn_furor].beats );

    act( "{i$n furiously strikes at you.{x", ch, NULL, victim, TO_VICT);
    act( "{hYou furiously strike at $N.{x", ch, NULL, victim, TO_CHAR);
    act( "{k$n furiously strikes at $N.{x", ch, NULL, victim, TO_NOTVICT);


    for (count = 0; count < swings; count++ )
    {
        int number = number_percent();

        if ( get_skill(ch,gsn_furor) >
               number +
               (!can_see(ch,victim) ? 70 :
               (count <= 1 ? 10 : 30 + count * 5 )))
        {
            if(number_percent() < 15)
                check_improve(ch,gsn_furor,TRUE,1);
            one_hit(ch, ch->fighting, gsn_furor, FALSE);
        }
        else
        {
            damage( ch, victim, 0, gsn_furor,DAM_BASH,TRUE);
            if(number_percent() < 15)
                check_improve(ch,gsn_furor, FALSE, 1);
        }
    }
    return;
}

void update_pk_count(CHAR_DATA *ch, CHAR_DATA *victim)
{
    PK_COUNT_DATA *pkc;
    if(IS_NPC(ch) || IS_NPC(victim)) return;

    pkc = get_create_pk_count(&ch->pcdata->pk_counter, victim->name);
    pkc->wins++;
    pkc->last_fight = current_time;

    pkc = get_create_pk_count(&ch->pcdata->pk_counter, "!TOTAL");
    pkc->wins++;
    pkc->type = PK_TOTAL;
    pkc->last_fight = current_time;

    pkc = get_create_pk_count(&victim->pcdata->pk_counter, ch->name);
    pkc->losses++;
    pkc->last_fight = current_time;

    pkc = get_create_pk_count(&victim->pcdata->pk_counter, "!TOTAL");
    pkc->losses++;
    pkc->type = PK_TOTAL;
    pkc->last_fight = current_time;


}

PK_COUNT_DATA *get_create_pk_count(PK_COUNT_DATA **list, char *name)
{
    PK_COUNT_DATA *pks, *pkc = NULL;

    for(pks = *list; pks; pks = pks->next)
    {
        if(!str_cmp(pks->name,name))
            pkc = pks;
    }
    if(!pkc)
    {
        pkc = new_pk_count();
        pkc->name = str_dup(name);
        pkc->next = *list;
        (*list) = pkc;
    }
    return pkc;
}

bool check_phase( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
	return FALSE;

    chance = get_skill(victim,gsn_phase) / 2;

    if (!can_see(victim,ch))
	chance /= 2;

    if (victim->stunned)
        return chance /= 3;

    if( chance == 0 && (!IS_NPC(ch) ||
        !IS_SET(ch->act,ACT_MAGE)) ) return FALSE;

    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;

    act_new_2( "{iYour body {mphases{i to avoid $n's attack.{x", ch, NULL, victim, TO_VICT, POS_RESTING, 0, ACTION_BATTLE_NODAM );
    act_new_2( "{h$N's body {mphases{h to avoid your attack.{x", ch, NULL, victim, TO_CHAR, POS_RESTING, 0, ACTION_BATTLE_NODAM );

    check_improve(victim,gsn_phase,TRUE,6);
    return TRUE;
}
