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
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"

/* command procedures needed */
DECLARE_DO_FUN(do_yell		);
DECLARE_DO_FUN(do_open		);
DECLARE_DO_FUN(do_close		);
DECLARE_DO_FUN(do_say	);
DECLARE_DO_FUN(do_backstab);
DECLARE_DO_FUN(do_flee);
DECLARE_DO_FUN(do_murder);
DECLARE_DO_FUN(do_shout);


void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool secondary );

/*
 * The following special functions are available for mobiles.
 */
DECLARE_SPEC_FUN(	spec_boaz		);
DECLARE_SPEC_FUN(	spec_breath_any		);
DECLARE_SPEC_FUN(	spec_breath_acid	);
DECLARE_SPEC_FUN(	spec_breath_fire	);
DECLARE_SPEC_FUN(	spec_breath_frost	);
DECLARE_SPEC_FUN(	spec_breath_gas		);
DECLARE_SPEC_FUN(	spec_breath_lightning	);
DECLARE_SPEC_FUN(	spec_cast_adept		);
DECLARE_SPEC_FUN(	spec_cast_cleric	);
DECLARE_SPEC_FUN(	spec_cast_judge		);
DECLARE_SPEC_FUN(	spec_cast_mage		);
DECLARE_SPEC_FUN(	spec_cast_undead	);
DECLARE_SPEC_FUN(	spec_executioner	);
DECLARE_SPEC_FUN(	spec_fido		);
DECLARE_SPEC_FUN(	spec_guard		);
DECLARE_SPEC_FUN(	spec_janitor		);
DECLARE_SPEC_FUN(	spec_mayor		);
DECLARE_SPEC_FUN(	spec_poison		);
DECLARE_SPEC_FUN(	spec_thief		);
DECLARE_SPEC_FUN(	spec_nasty		);
DECLARE_SPEC_FUN(	spec_troll_member	);
DECLARE_SPEC_FUN(	spec_ogre_member	);
DECLARE_SPEC_FUN(	spec_patrolman		);
DECLARE_SPEC_FUN(	spec_dog_pee		);
DECLARE_SPEC_FUN(	spec_cast_clan_adept	);
DECLARE_SPEC_FUN(	spec_questmaster	);
DECLARE_SPEC_FUN(	spec_quest_vip		);
DECLARE_SPEC_FUN(       spec_furor              );

/* the function table */
const   struct  spec_type    spec_table[] =
{
    {	"spec_boaz",			spec_boaz		},
    {	"spec_breath_any",		spec_breath_any		},
    {	"spec_breath_acid",		spec_breath_acid	},
    {	"spec_breath_fire",		spec_breath_fire	},
    {	"spec_breath_frost",		spec_breath_frost	},
    {	"spec_breath_gas",		spec_breath_gas		},
    {	"spec_breath_lightning",	spec_breath_lightning	},	
    {	"spec_cast_adept",		spec_cast_adept		},
    {	"spec_cast_cleric",		spec_cast_cleric	},
    {	"spec_cast_judge",		spec_cast_judge		},
    {	"spec_cast_mage",		spec_cast_mage		},
    {	"spec_cast_undead",		spec_cast_undead	},
    {	"spec_executioner",		spec_executioner	},
    {	"spec_fido",			spec_fido		},
    {	"spec_guard",			spec_guard		},
    {	"spec_janitor",			spec_janitor		},
    {	"spec_mayor",			spec_mayor		},
    {	"spec_poison",			spec_poison		},
    {	"spec_thief",			spec_thief		},
    {	"spec_nasty",			spec_nasty		},
    {	"spec_troll_member",		spec_troll_member	},
    {	"spec_ogre_member",		spec_ogre_member	},
    {	"spec_patrolman",		spec_patrolman		},
    {	"spec_dog_pee",			spec_dog_pee		},
    {	"spec_cast_clan_adept",		spec_cast_clan_adept	},
    {	"spec_questmaster",		spec_questmaster	},
    {	"spec_quest_vip",		spec_quest_vip		},
    {   "spec_furor",			spec_furor		},
    {	NULL,				NULL			}
};

/*
 * Given a name, return the appropriate spec fun.
 */
SPEC_FUN *spec_lookup( const char *name )
{
   int i;
 
   for ( i = 0; spec_table[i].name != NULL; i++)
   {
        if (LOWER(name[0]) == LOWER(spec_table[i].name[0])
        &&  !str_prefix( name,spec_table[i].name))
            return spec_table[i].function;
   }
 
    return 0;
}

char *spec_name( SPEC_FUN *function)
{
    int i;

    for (i = 0; spec_table[i].function != NULL; i++)
    {
	if (function == spec_table[i].function)
	    return spec_table[i].name;
    }

    return NULL;
}

bool spec_troll_member( CHAR_DATA *ch)
{
    CHAR_DATA *vch, *victim = NULL;
    int count = 0;
    char *message;

    if (!IS_AWAKE(ch) || IS_AFFECTED(ch,AFF_CALM) || ch->in_room == NULL 
    ||  IS_AFFECTED(ch,AFF_CHARM) || ch->fighting != NULL)
	return FALSE;

    /* find an ogre to beat up */
    for (vch = ch->in_room->people;  vch != NULL;  vch = vch->next_in_room)
    {
	if (!IS_NPC(vch) || ch == vch)
	    continue;

	if (vch->pIndexData->vnum == MOB_VNUM_PATROLMAN)
	    return FALSE;

	if (vch->pIndexData->group == GROUP_VNUM_OGRES
	&&  ch->level > vch->level - 2 && !is_safe(ch,vch))
	{
	    if (number_range(0,count) == 0)
		victim = vch;

	    count++;
	}
    }

    if (victim == NULL)
	return FALSE;

    /* say something, then raise hell */
    switch (number_range(0,6))
    {
	default:  message = NULL; 	break;
	case 0:	message = "$n yells '{aI've been looking for you, punk!{x'";
		break;
	case 1: message = "With a scream of rage, $n attacks $N.";
		break;
	case 2: message = 
		"$n says '{aWhat's slimy Ogre trash like you doing around here?{x'";
		break;
	case 3: message = "$n cracks his knuckles and says '{SDo ya feel lucky?{x'";
		break;
	case 4: message = "$n says '{aThere's no cops to save you this time!{x'";
		break;	
	case 5: message = "$n says '{aTime to join your brother, spud.{x'";
		break;
	case 6: message = "$n says '{aLet's rock.{x'";
		break;
    }

    if (message != NULL)
    	act(message,ch,NULL,victim,TO_ALL);
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return TRUE;
}

bool spec_ogre_member( CHAR_DATA *ch)
{
    CHAR_DATA *vch, *victim = NULL;
    int count = 0;
    char *message;
 
    if (!IS_AWAKE(ch) || IS_AFFECTED(ch,AFF_CALM) || ch->in_room == NULL
    ||  IS_AFFECTED(ch,AFF_CHARM) || ch->fighting != NULL)
        return FALSE;

    /* find an troll to beat up */
    for (vch = ch->in_room->people;  vch != NULL;  vch = vch->next_in_room)
    {
        if (!IS_NPC(vch) || ch == vch)
            continue;
 
        if (vch->pIndexData->vnum == MOB_VNUM_PATROLMAN)
            return FALSE;
 
        if (vch->pIndexData->group == GROUP_VNUM_TROLLS
        &&  ch->level > vch->level - 2 && !is_safe(ch,vch))
        {
            if (number_range(0,count) == 0)
                victim = vch;
 
            count++;
        }
    }
 
    if (victim == NULL)
        return FALSE;
 
    /* say something, then raise hell */
    switch (number_range(0,6))
    {
	default: message = NULL;	break;
        case 0: message = "$n yells '{aI've been looking for you, punk!{x'";
                break;
        case 1: message = "With a scream of rage, $n attacks $N.'";
                break;
        case 2: message =
                "$n says '{aWhat's Troll filth like you doing around here?{x'";
                break;
        case 3: message = "$n cracks his knuckles and says '{SDo ya feel lucky?{x'";
                break;
        case 4: message = "$n says '{aThere's no cops to save you this time!{x'";
                break;
        case 5: message = "$n says '{aTime to join your brother, spud.{x'";
                break;
        case 6: message = "$n says '{aLet's rock.{x'";
                break;
    }
 
    if (message != NULL)
    	act(message,ch,NULL,victim,TO_ALL);
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return TRUE;
}

bool spec_boaz( CHAR_DATA *ch)
{
    CHAR_DATA *vch, *victim = NULL;
    int count = 0;
    char *message;
    bool found = FALSE;

    if (!IS_AWAKE(ch) || IS_AFFECTED(ch,AFF_CALM) || ch->in_room == NULL
    ||  IS_AFFECTED(ch,AFF_CHARM) || ch->fighting != NULL)
        return FALSE;

    /* find clayton */
    for (vch = ch->in_room->people;  vch != NULL;  vch = vch->next_in_room)
    {
        if (!IS_NPC(vch))
            found = TRUE;

        if (!IS_NPC(vch) || ch == vch)
            continue;

        if (vch->pIndexData->vnum == MOB_VNUM_CLAYTON)
        {
            if (number_range(0,count) == 0)
                victim = vch;

            count++;
        }
    }    

    if (!found)
        return FALSE;

    if (victim == NULL)
        return FALSE;

    switch (number_range(0,9))
    {
	default: message = NULL;	break;
	case 0: message = NULL;	break;
	case 1: message = NULL;	break;
        case 2: message = "blank";
                break;
	case 3: message = NULL;	break;
	case 4: message = NULL;	break;
        case 5: message = "blank";
                break;
	case 6: message = NULL;	break;
	case 7: message = NULL;	break;
	case 8: message = NULL;	break;
	case 9: message = NULL;	break;
    }

    if (message == NULL)
	return FALSE;

    /* say something, then raise hell */
    switch (number_range(0,9))
    {
	default: message = NULL;	break;
        case 0: message = "$n yells '{aLeave me alone!{x'";
                break;
        case 1: message = "With a scream of rage, $n attacks $N.";
                break;
        case 2: message = "$n shouts '{aStop that! Stop that!{x' and attacks.";
                break;
	case 3: message = NULL;	break;
        case 4: message = "$n snorts a line of cocaine.";
                break;
        case 5: message = "$n yells '{aGet out!{x'";
                break;
        case 6: message = "$n shouts '{aLeave mommy alone, you little shit!{x'";
                break;
        case 7: message = "$n lights up another cigarette.";
                break;
	case 8: message = NULL;	break;
	case 9: message = NULL;	break;
    }
 
    if (message != NULL)
    {
    	act(message,ch,NULL,victim,TO_ALL);
	switch (number_range(0,8))
	{
	    default: message = NULL;	break;
	    case 0: message = "$n yanks $N up by the arm and throws him through the door.";
                break;
	    case 1: message = "$n slaps $N across the face.";
                break;
	    case 2: message = NULL;	break;
	    case 3: message = "$n throws $N onto the couch.";
                break;
	    case 4: message = NULL;	break;
	    case 5: message = "$n spanks $N.";
                break;
	    case 6: message = "$n sits down and ignores $N.";
                break;
	    case 7: message = NULL;	break;
	    case 8: message = NULL;	break;
	}
	if (message != NULL)
	{
	    act(message,ch,NULL,victim,TO_ALL);
	}
    }
    switch (number_range(0,6))
    {
	default: message = NULL;	break;
	case 0: message = "$N screams at $n.";
                break;
	case 1: message = "$N screams in pain.";
                break;
	case 2: message = "$N lies down and cries.";
                break;
	case 3: message = "$N steals his mommy's cigarettes.";
                break;
	case 4: message = "$N screams in agony!";
                break;
	case 5: message = "$N starts wailing at the top of his lungs.";
                break;
	case 6: message = "$N runs to you to get away from his mommy.";
                break;
    }
 
    if (message != NULL)
    {
    	act(message,ch,NULL,victim,TO_ALL);
    }

    return TRUE;
}

bool spec_patrolman(CHAR_DATA *ch)
{
    CHAR_DATA *vch,*victim = NULL;
    OBJ_DATA *obj;
    char *message;
    int count = 0;

    if (!IS_AWAKE(ch) || IS_AFFECTED(ch,AFF_CALM) || ch->in_room == NULL
    ||  IS_AFFECTED(ch,AFF_CHARM) || ch->fighting != NULL)
        return FALSE;

    /* look for a fight in the room */
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (vch == ch)
	    continue;

	if (vch->fighting != NULL)  /* break it up! */
	{
	    if (number_range(0,count) == 0)
	        victim = (vch->level > vch->fighting->level) 
		    ? vch : vch->fighting;
	    count++;
	}
    }

    if (victim == NULL || (IS_NPC(victim) && victim->spec_fun == ch->spec_fun))
	return FALSE;

    if (((obj = get_eq_char(ch,WEAR_NECK_1)) != NULL 
    &&   obj->pIndexData->vnum == OBJ_VNUM_WHISTLE)
    ||  ((obj = get_eq_char(ch,WEAR_NECK_2)) != NULL
    &&   obj->pIndexData->vnum == OBJ_VNUM_WHISTLE))
    {
	act("You blow down hard on $p.",ch,obj,NULL,TO_CHAR);
	act("$n blows on $p, ***WHEEEEEEEEEEEET***",ch,obj,NULL,TO_ROOM);

    	for ( vch = char_list; vch != NULL; vch = vch->next )
    	{
            if ( vch->in_room == NULL )
            	continue;

            if (vch->in_room != ch->in_room 
	    &&  vch->in_room->area == ch->in_room->area)
            	send_to_char( "You hear a shrill whistling sound.\n\r", vch );
    	}
    }

    switch (number_range(0,6))
    {
	default:	message = NULL;		break;
	case 0:	message = "$n yells '{aAll roit! All roit! break it up!{x'";
		break;
	case 1: message = 
		"$n says '{aSociety's to blame, but what's a bloke to do?{x'";
		break;
	case 2: message = 
		"$n mumbles '{abloody kids will be the death of us all.{x'";
		break;
	case 3: message = "$n shouts '{aStop that! Stop that!{T' and attacks.";
		break;
	case 4: message = "$n pulls out his billy and goes to work.";
		break;
	case 5: message = 
		"$n sighs in resignation and proceeds to break up the fight.";
		break;
	case 6: message = "$n says '{aSettle down, you hooligans!{x'";
		break;
    }

    if (message != NULL)
	act(message,ch,NULL,NULL,TO_ALL);

    multi_hit(ch,victim,TYPE_UNDEFINED);

    return TRUE;
}
	
bool spec_dog_pee( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;

    if ( !IS_AWAKE(ch) )
        return FALSE;
 
    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0 
         && !IS_NPC(victim))
            break;
    }
 
    if ( victim == NULL )
        return FALSE;

    switch ( number_bits( 3 ) )
    {
    case 0:
        return FALSE;
 
    case 1:
        return FALSE;
 
    case 2:
        act("$n lifts $s hind leg, and pees on $N's feet.",ch,NULL,victim,TO_NOTVICT);
        act("$n lifts $s hind leg, and pees on your feet.",ch,NULL,victim,TO_VICT);
        return TRUE;
    }
 
    return FALSE;
}

bool spec_nasty( CHAR_DATA *ch )
{
    CHAR_DATA *victim, *v_next;
    long gold;
 
    if (!IS_AWAKE(ch)) {
       return FALSE;
    }
 
    if (ch->position != POS_FIGHTING) {
       for ( victim = ch->in_room->people; victim != NULL; victim = v_next)
       {
          v_next = victim->next_in_room;
          if (!IS_NPC(victim)
             && (victim->level > ch->level)
             && (victim->level < ch->level + 10))
          {
	     do_backstab(ch,victim->name);
             if (ch->position != POS_FIGHTING)
                 do_murder(ch,victim->name);
             /* should steal some coins right away? :) */
             return TRUE;
          }
       }
       return FALSE;    /*  No one to attack */
    }
 
    /* okay, we must be fighting.... steal some coins and flee */
    if ( (victim = ch->fighting) == NULL)
        return FALSE;   /* let's be paranoid.... */
 
    switch ( number_bits(2) )
    {
        case 0:  act( "$n rips apart your coin purse, spilling your gold!",
                     ch, NULL, victim, TO_VICT);
                 act( "You slash apart $N's coin purse and gather his gold.",
                     ch, NULL, victim, TO_CHAR);
                 act( "$N's coin purse is ripped apart!",
                     ch, NULL, victim, TO_NOTVICT);
                 gold = victim->gold / 10;  /* steal 10% of his gold */
                 victim->gold -= gold;
                 ch->gold     += gold;
                 return TRUE;
 
        case 1:  do_flee( ch, "");
                 return TRUE;
 
        default: return FALSE;
    }
}

/*
 * Core procedure for dragons.
 */
bool dragon( CHAR_DATA *ch, char *spell_name )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    int sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting == ch && number_bits( 3 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    if (ch->stunned)
	return FALSE;

    if ( ( sn = skill_lookup( spell_name ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim, TARGET_CHAR);
    return TRUE;
}



/*
 * Special procedures for mobiles.
 */
bool spec_breath_any( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING )
	return FALSE;

    switch ( number_bits( 3 ) )
    {
    case 0: return spec_breath_fire		( ch );
    case 1:
    case 2: return spec_breath_lightning	( ch );
    case 3: return spec_breath_gas		( ch );
    case 4: return spec_breath_acid		( ch );
    case 5:
    case 6:
    case 7: return spec_breath_frost		( ch );
    }

    return FALSE;
}



bool spec_breath_acid( CHAR_DATA *ch )
{
    return dragon( ch, "acid breath" );
}



bool spec_breath_fire( CHAR_DATA *ch )
{
    return dragon( ch, "fire breath" );
}



bool spec_breath_frost( CHAR_DATA *ch )
{
    return dragon( ch, "frost breath" );
}



bool spec_breath_gas( CHAR_DATA *ch )
{
    int sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    if (ch->stunned)
	return FALSE;

    if ( ( sn = skill_lookup( "gas breath" ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, NULL,TARGET_CHAR);
    return TRUE;
}



bool spec_breath_lightning( CHAR_DATA *ch )
{
    return dragon( ch, "lightning breath" );
}



bool spec_cast_adept( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;

    if ( !IS_AWAKE(ch) )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim != ch && can_see( ch, victim ) && number_bits( 3 ) == 0
	     && !IS_NPC(victim) && victim->level < 31)
	    break;
    }

    if ( victim == NULL )
    {
        for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
        {
            v_next = victim->next_in_room;
            if ( victim != ch && can_see( ch, victim ) && number_bits( 2 ) == 0
                 && IS_NPC(victim) && !IS_AFFECTED(ch,AFF_CHARM) )
                break;
        }
    }

    if (victim == NULL)
	return FALSE;

    switch ( (!IS_NPC(victim)) ? number_bits( 5 ) : number_range(0,6) )
    {
    case 0:
    case 1:
	act( "$n utters the word 'abrazak'.", ch, NULL, NULL, TO_ROOM );
	spell_armor( skill_lookup( "armor" ), ch->level,ch,victim,TARGET_CHAR);
	return TRUE;

    case 2:
    case 3:
	act( "$n utters the word 'fido'.", ch, NULL, NULL, TO_ROOM );
	spell_bless( skill_lookup( "bless" ), ch->level,ch,victim,TARGET_CHAR);
	return TRUE;

    case 4:
	act("$n utters the words 'judicandus noselacri'.",ch,NULL,NULL,TO_ROOM);
	spell_cure_blindness( skill_lookup( "cure blindness" ),
	    ch->level, ch, victim,TARGET_CHAR);
	return TRUE;

    case 5:
	act("$n utters the words 'judicandus dies'.", ch,NULL, NULL, TO_ROOM );
	spell_cure_light( skill_lookup( "cure light" ),
	    ch->level, ch, victim,TARGET_CHAR);
	return TRUE;

    case 6:
	act( "$n utters the words 'judicandus sausabru'.",ch,NULL,NULL,TO_ROOM);
	spell_cure_poison( skill_lookup( "cure poison" ),
	    ch->level, ch, victim,TARGET_CHAR);
	return TRUE;

    case 7:
	act("$n utters the words 'judicandus eugzagz'.",ch,NULL,NULL,TO_ROOM);
	spell_cure_disease(skill_lookup("cure disease"),
	    ch->level,ch,victim,TARGET_CHAR);
	return TRUE;

    case 8:
	act("$n utters the word 'candusima'.", ch, NULL, NULL, TO_ROOM );
	spell_refresh( skill_lookup("refresh"),ch->level,ch,victim,TARGET_CHAR);
	return TRUE;

    case 9:
	act("$n utters the words 'judicandus eqtuyp'.",ch,NULL,NULL,TO_ROOM);
        spell_sanctuary(skill_lookup("sanctuary"),
            ch->level,ch,victim,TARGET_CHAR);
        return TRUE;

    case 10:
	act("$n utters the words 'qwerty'.",ch,NULL,NULL,TO_ROOM);
	spell_shield(skill_lookup("shield"),
            ch->level,ch,victim,TARGET_CHAR); 
        return TRUE;

    case 11:
	act("$n utters the words 'saspell'.",ch,NULL,NULL,TO_ROOM);
	spell_heal(skill_lookup("heal"),
            ch->level,ch,victim,TARGET_CHAR);  

        return TRUE;

    case 12:
        act("$n utters the words 'chillse'.",ch,NULL,NULL,TO_ROOM);
        spell_iceshield(skill_lookup("iceshield"),
            ch->level,ch,victim,TARGET_CHAR);

        return TRUE;

    case 13:
        act("$n utters the words 'firese'.",ch,NULL,NULL,TO_ROOM);
        spell_fireshield(skill_lookup("fireshield"),
            ch->level,ch,victim,TARGET_CHAR);
        return TRUE;

    case 14:
        act("$n utters the words 'enerse'.",ch,NULL,NULL,TO_ROOM);
        spell_shockshield(skill_lookup("shockshield"),
            ch->level,ch,victim,TARGET_CHAR);
        return TRUE;

    case 15:
        act("$n utters the words 'quickse'.",ch,NULL,NULL,TO_ROOM);
        spell_haste(skill_lookup("haste"),
            ch->level,ch,victim,TARGET_CHAR);
        return TRUE;

    case 17:
        act("$n utters the words 'strongse'.",ch,NULL,NULL,TO_ROOM);
        spell_giant_strength(skill_lookup("giant strength"),
            ch->level,ch,victim,TARGET_CHAR);

        return TRUE;

    }

    return FALSE;
}

bool spec_cast_clan_adept( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;

    if ( !IS_AWAKE(ch) )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0 
	     && !IS_NPC(victim) && (victim->level < 61)
	    && (victim->clan == ch->in_room->clan))
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    switch ( number_bits( 4 ) )
    {
    case 0:
	act( "$n utters the word 'abrazak'.", ch, NULL, NULL, TO_ROOM );
	spell_armor( skill_lookup( "armor" ), ch->level/2,ch,victim,TARGET_CHAR);
	return TRUE;

    case 1:
	act( "$n utters the word 'fido'.", ch, NULL, NULL, TO_ROOM );
	spell_bless( skill_lookup( "bless" ), ch->level/2,ch,victim,TARGET_CHAR);
	return TRUE;

    case 2:
	act("$n utters the words 'judicandus noselacri'.",ch,NULL,NULL,TO_ROOM);
	spell_cure_blindness( skill_lookup( "cure blindness" ),
	    ch->level/2, ch, victim,TARGET_CHAR);
	return TRUE;

    case 3:
	act("$n utters the words 'judicandus dies'.", ch,NULL, NULL, TO_ROOM );
	spell_cure_light( skill_lookup( "cure light" ),
	    ch->level/2, ch, victim,TARGET_CHAR);
	return TRUE;

    case 4:
	act( "$n utters the words 'judicandus sausabru'.",ch,NULL,NULL,TO_ROOM);
	spell_cure_poison( skill_lookup( "cure poison" ),
	    ch->level/2, ch, victim,TARGET_CHAR);
	return TRUE;

    case 5:
	act("$n utters the word 'candusima'.", ch, NULL, NULL, TO_ROOM );
	spell_refresh( skill_lookup("refresh"),ch->level/2,ch,victim,TARGET_CHAR);
	return TRUE;

    case 6:
	act("$n utters the words 'judicandus eugzagz'.",ch,NULL,NULL,TO_ROOM);
	spell_cure_disease(skill_lookup("cure disease"),
	    ch->level/2,ch,victim,TARGET_CHAR);
	return TRUE;

    case 7:
	act("$n utters the words 'judicandus eqtuyp'.",ch,NULL,NULL,TO_ROOM);
        spell_sanctuary(skill_lookup("sanctuary"),
            ch->level/2,ch,victim,TARGET_CHAR);
        return TRUE;

    case 8:
	act("$n utters the words 'qwerty'.",ch,NULL,NULL,TO_ROOM);
	spell_shield(skill_lookup("shield"),
            ch->level/2,ch,victim,TARGET_CHAR); 
        return TRUE;

    case 9:
	act("$n utters the words 'saspell'.",ch,NULL,NULL,TO_ROOM);
	spell_heal(skill_lookup("heal"),
            ch->level/2,ch,victim,TARGET_CHAR);  
    }

    return FALSE;
}



bool spec_cast_cleric( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    for ( ;; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "blindness";      break;
	case  1: min_level =  3; spell = "cause serious";  break;
	case  2: min_level =  7; spell = "earthquake";     break;
	case  3: min_level =  9; spell = "cause critical"; break;
	case  4: min_level = 10; spell = "dispel evil";    break;
	case  5: min_level = 22; spell = "curse";          break;
	case  6: min_level = 20; spell = "change sex";     break;
	case  7: min_level = 13; spell = "flamestrike";    break;
	case  8: min_level = 45; spell = "accelerate poison";  break;
	case  9: min_level = 25; spell = "poison";	   break;
	case 10: min_level = 15; spell = "harm";           break;
	case 11: min_level = 25; spell = "plague";	   break;
	case 12: min_level = 120; spell = "divine wrath";  break;
	default: min_level = 16; spell = "dispel magic";   break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if (ch->stunned)
	return FALSE;

    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim,TARGET_CHAR);
    return TRUE;
}

bool spec_cast_judge( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;
 
    if ( ch->position != POS_FIGHTING )
        return FALSE;
 
    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim->fighting == ch && number_bits( 2 ) == 0 )
            break;
    }
 
    if ( victim == NULL )
        return FALSE;

    if (ch->stunned)
	return FALSE;
 
    spell = "high explosive";
    if ( ( sn = skill_lookup( spell ) ) < 0 )
        return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim,TARGET_CHAR);
    return TRUE;
}



bool spec_cast_mage( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    for ( ;; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "blindness";      break;
	case  1: min_level =  3; spell = "chill touch";    break;
	case  2: min_level =  7; spell = "weaken";         break;
	case  3: min_level =  8; spell = "teleport";       break;
	case  4: min_level = 11; spell = "colour spray";   break;
	case  5: min_level = 12; spell = "change sex";     break;
	case  6: min_level = 13; spell = "energy drain";   break;

	case  7: min_level = 45; spell = "accelerate poison";  break;
	case  8: min_level = 25; spell = "poison";	   break;

	case  9: min_level = 15; spell = "fireball";       break;
	case 10: min_level = 20; spell = "plague";	   break;
	case 11: min_level = 40; spell = "harm";	   break;
	case 12: min_level = 70; spell = "flamestrike";	   break;
	case 13: min_level = 90; spell = "magic missile";  break;
	case 14: min_level = 130; spell = "shockwave";     break;
	default: min_level = 20; spell = "acid blast";     break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if (ch->stunned)
	return FALSE;

    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim,TARGET_CHAR);
    return TRUE;
}



bool spec_cast_undead( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if ( ch->position != POS_FIGHTING )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;
	if ( victim->fighting == ch && number_bits( 2 ) == 0 )
	    break;
    }

    if ( victim == NULL )
	return FALSE;

    for ( ;; )
    {
	int min_level;

	switch ( number_bits( 4 ) )
	{
	case  0: min_level =  0; spell = "curse";          break;
	case  1: min_level =  3; spell = "weaken";         break;
	case  2: min_level =  6; spell = "chill touch";    break;
	case  3: min_level =  9; spell = "blindness";      break;
	case  4: min_level = 12; spell = "poison";         break;
	case  5: min_level = 15; spell = "energy drain";   break;
	case  6: min_level = 18; spell = "harm";           break;
	case  7: min_level = 21; spell = "teleport";       break;
	case  8: min_level = 20; spell = "plague";	   break;
	default: min_level = 18; spell = "harm";           break;
	}

	if ( ch->level >= min_level )
	    break;
    }

    if (ch->stunned)
	return FALSE;

    if ( ( sn = skill_lookup( spell ) ) < 0 )
	return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim,TARGET_CHAR);
    return TRUE;
}


bool spec_executioner( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *crime;

    if ( !IS_AWAKE(ch) || ch->fighting != NULL )
	return FALSE;

    crime = "";
    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;

	if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_TWIT) 
	&&   can_see(ch,victim))
	    { crime = "TWIT"; break; }
    }

    if ( victim == NULL )
	return FALSE;

    sprintf( buf, "{a%s is a %s!  PROTECT THE INNOCENT!  MORE BLOOOOD!!!{x",
	victim->name, crime );
    REMOVE_BIT(ch->comm,COMM_NOSHOUT);
    do_yell( ch, buf );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return TRUE;
}

			

bool spec_fido( CHAR_DATA *ch )
{
    OBJ_DATA *corpse;
    OBJ_DATA *c_next;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    if ( !IS_AWAKE(ch) )
	return FALSE;

    for ( corpse = ch->in_room->contents; corpse != NULL; corpse = c_next )
    {
	c_next = corpse->next_content;
	if ( corpse->item_type != ITEM_CORPSE_NPC )
	    continue;

	act( "$n savagely devours a corpse.", ch, NULL, NULL, TO_ROOM );
	for ( obj = corpse->contains; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    obj_from_obj( obj );
	    obj_to_room( obj, ch->in_room );
	}
	extract_obj( corpse );
	return TRUE;
    }

    return FALSE;
}



bool spec_guard( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    CHAR_DATA *ech;
    char *crime;
    int max_evil;

    if ( !IS_AWAKE(ch) || ch->fighting != NULL )
	return FALSE;

    max_evil = 300;
    ech      = NULL;
    crime    = "";

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;

	if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_TWIT) 
	&&   can_see(ch,victim))
	    { crime = "TWIT"; break; }

	if ( victim->fighting != NULL
	&&   victim->fighting != ch
	&&   victim->alignment < max_evil )
	{
	    max_evil = victim->alignment;
	    ech      = victim;
	}
    }

    if ( victim != NULL )
    {
	sprintf( buf, "{a%s is a %s!  PROTECT THE INNOCENT!!  BANZAI!!{x",
	    victim->name, crime );
 	REMOVE_BIT(ch->comm,COMM_NOSHOUT);
	do_yell( ch, buf );
	multi_hit( ch, victim, TYPE_UNDEFINED );
	return TRUE;
    }

    if ( ech != NULL )
    {
/*
	act( "$n screams '{aPROTECT THE INNOCENT!!  BANZAI!!{x",
	    ch, NULL, NULL, TO_ROOM );
*/
        do_shout(ch, "{aPROTECT THE INNOCENT!!  BANZAI!!{x");
	multi_hit( ch, ech, TYPE_UNDEFINED );
	return TRUE;
    }

    return FALSE;
}



bool spec_janitor( CHAR_DATA *ch )
{
    OBJ_DATA *trash;
    OBJ_DATA *trash_next;

    if ( !IS_AWAKE(ch) )
	return FALSE;

    if( IS_SET(ch->in_room->room_flags, ROOM_SAFE) )
	return FALSE;

    for ( trash = ch->in_room->contents; trash != NULL; trash = trash_next )
    {
	trash_next = trash->next_content;
	if ( !IS_SET( trash->wear_flags, ITEM_TAKE ) || !can_loot(ch,trash))
	    continue;
	if ( trash->item_type == ITEM_DRINK_CON
	||   trash->item_type == ITEM_TRASH
	||   trash->cost < 10 )
	{
	    act( "$n picks up some trash.", ch, NULL, NULL, TO_ROOM );
	    obj_from_room( trash );
	    obj_to_char( trash, ch );
	    return TRUE;
	}
    }

    return FALSE;
}



bool spec_mayor( CHAR_DATA *ch )
{
    static const char open_path[] =
	"W3a3003b330f00c1f11d0d11f1Oe33f3333Oe2f2c22211221f2111a1S.";

    static const char close_path[] =
	"W3a3003b33000c111d0d111CE3333f33CE22c222112f212111a1S.";

    static const char *path;
    static int pos;
    static bool move;

    if ( !move )
    {
	if ( time_info.hour ==  6 )
	{
	    path = open_path;
	    move = TRUE;
	    pos  = 0;
	}

	if ( time_info.hour == 20 )
	{
	    path = close_path;
	    move = TRUE;
	    pos  = 0;
	}
    }

    if ( ch->fighting != NULL )
	return spec_cast_mage( ch );
    if ( !move || ch->position < POS_SLEEPING )
	return FALSE;

    switch ( path[pos] )
    {
    case '0':
    case '1':
    case '2':
    case '3':
	move_char( ch, path[pos] - '0', FALSE, FALSE );
	break;

    case 'W':
	ch->position = POS_STANDING;
	act( "$n awakens and groans loudly.", ch, NULL, NULL, TO_ROOM );
	break;

    case 'S':
	ch->position = POS_SLEEPING;
	act( "$n lies down and falls asleep.", ch, NULL, NULL, TO_ROOM );
	break;

    case 'a':
	act( "{x$n says '{aHello Honey!{x'", ch, NULL, NULL, TO_ROOM );
	break;

    case 'b':
	act( "{x$n says '{aWhat a view!  I must do something about that dump!{x'",
	    ch, NULL, NULL, TO_ROOM );
	break;

    case 'c':
	act( "{x$n says '{aVandals!  Youngsters have no respect for anything!{x'",
	    ch, NULL, NULL, TO_ROOM );
	break;

    case 'd':
	act( "{x$n says '{aGood day, citizens!{x'", ch, NULL, NULL, TO_ROOM );
	break;

    case 'f':
	do_say(ch,"Did you know Midgaard bank is open again?");
	do_say(ch,"It's right above the Grunting Boar Inn.");
	break;

    case 'e':
	act( "{x$n says '{aI hereby declare the city of Midgaard open!{x'",
	    ch, NULL, NULL, TO_ROOM );
	break;

    case 'E':
	act( "{x$n says '{aI hereby declare the city of Midgaard closed!{x'",
	    ch, NULL, NULL, TO_ROOM );
	break;

    case 'O':
/*	do_unlock( ch, "gate" ); */
	do_open( ch, "gate" );
	break;

    case 'C':
	do_close( ch, "gate" );
/*	do_lock( ch, "gate" ); */
	break;

    case '.' :
	move = FALSE;
	break;
    }

    pos++;
    return FALSE;
}



bool spec_poison( CHAR_DATA *ch )
{
    CHAR_DATA *victim;

    if ( ch->position != POS_FIGHTING
    || ( victim = ch->fighting ) == NULL
    ||   number_percent( ) > 2 * ch->level )
	return FALSE;

    if (ch->stunned)
	return FALSE;

    act( "You bite $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n bites $N!",  ch, NULL, victim, TO_NOTVICT );
    act( "$n bites you!", ch, NULL, victim, TO_VICT    );
    spell_poison( gsn_poison, ch->level, ch, victim,TARGET_CHAR);
    return TRUE;
}



bool spec_thief( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    long gold,silver;

    if ( ch->position != POS_STANDING )
	return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
	v_next = victim->next_in_room;

	if ( IS_NPC(victim)
	||   victim->level >= LEVEL_IMMORTAL
	||   number_bits( 5 ) != 0 
	||   !can_see(ch,victim))
	    continue;

	if ( IS_AWAKE(victim) && number_range( 0, ch->level ) == 0 )
	{
	    act( "You discover $n's hands in your {zwallet!{x",
		ch, NULL, victim, TO_VICT );

	    act( "$N discovers $n's hands in $S {zwallet!{x",
		ch, NULL, victim, TO_NOTVICT );
	    return TRUE;
	}
	else
	{
	    gold = victim->gold * UMIN(number_range(1,20),ch->level / 2) / 100;
	    gold = UMIN(gold, ch->level * ch->level * 10 );
	    ch->gold     += gold;
	    victim->gold -= gold;
	    silver = victim->silver * UMIN(number_range(1,20),ch->level/2)/100;
	    silver = UMIN(silver,ch->level*ch->level * 25);
	    ch->silver	+= silver;
	    victim->silver -= silver;
	    return TRUE;
	}
    }

    return FALSE;
}


bool spec_questmaster( CHAR_DATA *ch )
{
    return FALSE;
}

bool spec_quest_vip( CHAR_DATA *ch )
{
    return FALSE;
}

bool spec_hunter( CHAR_DATA *ch )
{
    return FALSE;
}

bool spec_furor( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    int count = 0;

    int swings = (number_range(1,6) < 6)
                   ? number_range(2,4) : number_range(5,6);

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return FALSE;
    }

    if (ch->stunned)
    {
        send_to_char("You're still a little woozy.\n\r",ch);
        return FALSE;
    }

	int number = number_percent();

	if( number > 33 )
		return FALSE;

    WAIT_STATE( ch, skill_table[gsn_furor].beats );

    act( "{i$n furiously strikes at you.{x", ch, NULL, victim, TO_VICT);
    act( "{hYou furiously strike at $N.{x", ch, NULL, victim, TO_CHAR);
    act( "{k$n furiously strikes at $N.{x", ch, NULL, victim, TO_NOTVICT);


    for (count = 0; count < swings; count++ )
    {
        number = number_percent();

        if ( 100 >
               number +
               (!can_see(ch,victim) ? 70 :
               (count <= 1 ? 10 : 30 + count * 5 )))
        {
            one_hit(ch, ch->fighting, gsn_furor, FALSE);
        }
        else
        {
            damage( ch, victim, 0, gsn_furor,DAM_BASH,TRUE);
        }
    }
    return TRUE;
}

