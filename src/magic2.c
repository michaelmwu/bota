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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "tables.h"

extern char *target_name;

void spell_farsight(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_FARSIGHT) )
    {
        if (victim == ch)
          send_to_char("Your eyes are already as good as they get.\n\r",ch);
        else
          act("$N can see just fine.",ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_FARSIGHT;
    affect_to_char( victim, &af );
    send_to_char( "Your eyes jump into focus.\n\r", victim );
    if ( ch != victim )
        send_to_char( "Ok.\n\r", ch );
    return;
}

void spell_protection_voodoo(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, SHD_PROTECT_VOODOO) )
    {
	return;
    }
    af.where	= TO_SHIELDS;
    af.type	= sn;
    af.level	= level;
    af.duration	= level;
    af.location	= APPLY_NONE;
    af.modifier	= 0;
    af.bitvector = SHD_PROTECT_VOODOO;
    affect_to_char( victim, &af );
    return;
}

void spell_peace(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, SHD_PEACE) )
    {
	return;
    }
    af.where	= TO_SHIELDS;
    af.type	= sn;
    af.level	= 200;
    af.duration	= 10;
    af.location	= APPLY_NONE;
    af.modifier	= 0;
    af.bitvector = SHD_PEACE;
    affect_to_char( victim, &af );
    return;
}

void spell_portal( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim;
    OBJ_DATA *portal, *stone;

        if ( ( victim = get_char_world( ch, target_name ) ) == NULL
    ||   victim == ch
    ||   victim->in_room == NULL
    ||   ch->in_arena
    ||   victim->in_arena
    ||   !can_see_room(ch,victim->in_room)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SAFE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   victim->level >= level + 3
    ||   (!IS_NPC(victim) && victim->level >= LEVEL_HERO)  /* NOT trust */
    ||   (IS_NPC(victim) && IS_SET(victim->imm_flags,IMM_SUMMON))
    ||   (IS_NPC(victim) && saves_spell( level, victim,DAM_NONE) )
    ||	(is_clan(victim) && (!is_same_clan(ch,victim)
    ||  is_independent(victim))))
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    stone = get_eq_char(ch,WEAR_HOLD);
    if (!IS_IMMORTAL(ch)
    &&  (stone == NULL || stone->item_type != ITEM_WARP_STONE))
    {
	send_to_char("You lack the proper component for this spell.\n\r",ch);
	return;
    }

    if (stone != NULL && stone->item_type == ITEM_WARP_STONE)
    {
     	act("You draw upon the power of $p.",ch,stone,NULL,TO_CHAR);
     	act("It flares brightly and vanishes!",ch,stone,NULL,TO_CHAR);
     	extract_obj(stone);
    }

    portal = create_object(get_obj_index(OBJ_VNUM_PORTAL),0);
    portal->timer = 2 + level / 25;
    portal->value[3] = victim->in_room->vnum;

    obj_to_room(portal,ch->in_room);

    act("$p rises up from the ground.",ch,portal,NULL,TO_ROOM);
    act("$p rises up before you.",ch,portal,NULL,TO_CHAR);
}

void spell_nexus( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *victim;
    OBJ_DATA *portal, *stone;
    ROOM_INDEX_DATA *to_room, *from_room;

    from_room = ch->in_room;

        if ( ( victim = get_char_world( ch, target_name ) ) == NULL
    ||   victim == ch
    ||   (to_room = victim->in_room) == NULL
    ||   !can_see_room(ch,to_room) || !can_see_room(ch,from_room)
    ||   IS_SET(to_room->room_flags, ROOM_SAFE)
    ||	 IS_SET(from_room->room_flags,ROOM_SAFE)
    ||   IS_SET(to_room->room_flags, ROOM_PRIVATE)
    ||   IS_SET(to_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(to_room->room_flags, ROOM_NO_RECALL)
    ||   IS_SET(from_room->room_flags,ROOM_NO_RECALL)
    ||   victim->level >= level + 3
    ||   (!IS_NPC(victim) && victim->level >= LEVEL_HERO)  /* NOT trust */
    ||   (IS_NPC(victim) && IS_SET(victim->imm_flags,IMM_SUMMON))
    ||   (IS_NPC(victim) && saves_spell( level, victim,DAM_NONE) )
    ||	 (is_clan(victim) && (!is_same_clan(ch,victim)
    ||   is_independent(victim))))
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    stone = get_eq_char(ch,WEAR_HOLD);
    if (!IS_IMMORTAL(ch)
    &&  (stone == NULL || stone->item_type != ITEM_WARP_STONE))
    {
        send_to_char("You lack the proper component for this spell.\n\r",ch);
        return;
    }

    if (stone != NULL && stone->item_type == ITEM_WARP_STONE)
    {
        stone->condition = UMAX(stone->condition - 50, 0);
        act("You draw upon the power of $p.",ch,stone,NULL,TO_CHAR);

        if(stone->condition <= 0)
        {
          act("It flares brightly and vanishes!",ch,stone,NULL,TO_CHAR);
          extract_obj(stone);
        }
    }

    /* portal one */
    portal = create_object(get_obj_index(OBJ_VNUM_PORTAL),0);
    portal->timer = 1 + level / 10;
    portal->value[3] = to_room->vnum;

    obj_to_room(portal,from_room);

    act("$p rises up from the ground.",ch,portal,NULL,TO_ROOM);
    act("$p rises up before you.",ch,portal,NULL,TO_CHAR);

    /* no second portal if rooms are the same */
    if (to_room == from_room)
	return;

    /* portal two */
    portal = create_object(get_obj_index(OBJ_VNUM_PORTAL),0);
    portal->timer = 1 + level/10;
    portal->value[3] = from_room->vnum;

    obj_to_room(portal,to_room);

    if (to_room->people != NULL)
    {
	act("$p rises up from the ground.",to_room->people,portal,NULL,TO_ROOM);
	act("$p rises up from the ground.",to_room->people,portal,NULL,TO_CHAR);
    }
}

void spell_empower( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    OBJ_DATA *object;
    char buf[MAX_STRING_LENGTH];
    char *name;
    int new_sn;
    int mana;
    int newmana;
    int newtarget;

    if ( ( new_sn = find_spell( ch,target_name ) ) < 0
    || ( !IS_NPC(ch) && (ch->level < skill_table[new_sn].skill_level[ch->class]
    ||                   ch->pcdata->learned[new_sn] == 0)))
    {
        send_to_char( "What spell do you wish to bind?\n\r", ch );
        return;
    }

    name = skill_table[new_sn].name;
    if (!strcmp(name, "empower") )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
    newtarget = skill_table[new_sn].target;
    if (ch->level + 2 == skill_table[sn].skill_level[ch->class])
	mana = 50;
    else
    	mana = UMAX(
	    skill_table[sn].min_mana,
	    100 / ( 2 + ch->level - skill_table[sn].skill_level[ch->class] ) );

    if (ch->level + 2 == skill_table[new_sn].skill_level[ch->class])
	newmana = 50;
    else
    	newmana = UMAX(
	    skill_table[new_sn].min_mana,
	    100 / ( 2 + ch->level - skill_table[new_sn].skill_level[ch->class] ) );

    if ( (ch->mana - mana - newmana) < 0)
    {
        send_to_char( "You do not have enough mana.\n\r", ch );
        return;
    }
    ch->mana -= newmana;

    if ((newtarget == TAR_CHAR_DEFENSIVE) || (newtarget == TAR_CHAR_SELF))
    {
	object = create_object(get_obj_index(OBJ_VNUM_POTION), 0);
    }
    else
    {
	object = create_object(get_obj_index(OBJ_VNUM_SCROLL), 0);
    }
    object->value[0] = ch->level;
    object->value[1] = new_sn;
    object->level = ch->level-5;

    sprintf( buf, "%s%s", object->short_descr, name);
    free_string(object->short_descr);
    object->short_descr = str_dup(buf);

    sprintf( buf, "%s %s", object->name, skill_table[new_sn].name);
    free_string(object->name);
    object->name = str_dup(buf);


    if ((newtarget == TAR_CHAR_DEFENSIVE) || (newtarget == TAR_CHAR_SELF))
    {
	sprintf( buf, "$n has created a potion of %s!",name);
	act(buf,ch,object,NULL,TO_ROOM);
	sprintf( buf, "You create a potion of %s!\n\r",name);
	send_to_char(buf,ch);
    }
    else
    {
	sprintf( buf, "$n has created a scroll of %s!",name);
	act(buf,ch,object,NULL,TO_ROOM);
	sprintf( buf, "You create a scroll of %s!\n\r",name);
	send_to_char(buf,ch);
    }
    obj_to_char(object,ch);
    return;
}

void spell_resurrect( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_STRING_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    OBJ_DATA *obj;
    OBJ_DATA *cobj;
    OBJ_DATA *obj_next;
    CHAR_DATA *pet;
    int	length;

    if ( ( obj = get_obj_here( ch, "corpse" ) ) == NULL )
    {
	send_to_char( "There's no corpse here.\n\r", ch );
	return;
    }
    if ( ch->pet != NULL )
    {
	send_to_char("You failed.\n\r",ch);
	return;
    }
    pMobIndex = get_mob_index( MOB_VNUM_CORPSE );
    pet = create_mobile( pMobIndex );
    if (!IS_SET(pet->act, ACT_PET) )
        SET_BIT(pet->act, ACT_PET);
    if (!IS_SET(pet->affected_by, AFF_CHARM) )
        SET_BIT(pet->affected_by, AFF_CHARM);
    pet->comm = COMM_NOTELL|COMM_NOSHOUT|COMM_NOCHANNELS;
    sprintf( buf, "%s{GThe mark of %s is on it's forehead.{x.\n\r",
	pet->description, ch->name );
    free_string( pet->description );
    pet->description = str_dup( buf );
    free_string( pet->short_descr );
    pet->short_descr = str_dup( str_replace(obj->short_descr, "corpse", "zombie") );
    sprintf( buf, "%s", str_replace(obj->description, "corpse", "zombie") );
    length = strlen(buf)-12;
    strncpy( arg, buf, length);
    arg[length] = '\0';
    sprintf( buf, "%s standing here.\n\r", arg);
    free_string( pet->long_descr );
    pet->long_descr = str_dup( buf );
    char_to_room( pet, ch->in_room );
    add_follower( pet, ch );
    pet->leader = ch;
    ch->pet = pet;
    pet->alignment = ch->alignment;
    pet->level = UMAX(1, UMIN(109, ((ch->level/2)+(obj->level/2))));
    pet->max_hit = pet->level * 15;
    pet->hit = pet->max_hit;
    pet->armor[0] = pet->level/6;
    pet->armor[1] = pet->level/6;
    pet->armor[2] = pet->level/6;
    pet->armor[3] = pet->level/8;
    for ( cobj = obj->contains; cobj != NULL; cobj = obj_next )
    {
	obj_next = cobj->next_content;
	obj_from_obj( cobj );
	obj_to_room( cobj, ch->in_room );
    }
    extract_obj( obj );
    sprintf( buf, "%s stands up and starts following you.\n\r", pet->short_descr);
    send_to_char( buf, ch);
    sprintf( buf, "%s stands up and starts following $n.", pet->short_descr);
    act( buf, ch, NULL, NULL, TO_ROOM);
    return;
}

void spell_conjure( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    char buf[MAX_STRING_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    OBJ_DATA *stone;
    CHAR_DATA *pet;

    if (IS_NPC(ch))
	return;

    stone = get_eq_char(ch,WEAR_HOLD);
    if (!IS_IMMORTAL(ch)
    &&  (stone == NULL || stone->item_type != ITEM_DEMON_STONE))
    {
        send_to_char("You lack the proper component for this spell.\n\r",ch);
        return;
    }

    if ( ch->pet != NULL )
    {
	send_to_char("You failed.\n\r",ch);
	return;
    }

    if (stone != NULL && stone->item_type == ITEM_DEMON_STONE)
    {
	if (stone->value[0] < 1)
	{
	    act("You draw upon the power of $p.",ch,stone,NULL,TO_CHAR);
	    act("$n draws upon the power of $p.",ch,stone,NULL,TO_ROOM);
	    act("It flares brightly and explodes into dust.",ch,stone,NULL,TO_CHAR);
	    act("It flares brightly and explodes into dust.",ch,stone,NULL,TO_ROOM);
	    extract_obj( stone );
	    return;
	}
    }

    pMobIndex = get_mob_index( MOB_VNUM_DEMON );
    pet = create_mobile( pMobIndex );
    if (!IS_SET(pet->act, ACT_PET) )
        SET_BIT(pet->act, ACT_PET);
    if (!IS_SET(pet->affected_by, AFF_CHARM) )
        SET_BIT(pet->affected_by, AFF_CHARM);
    pet->comm = COMM_NOTELL|COMM_NOSHOUT|COMM_NOCHANNELS;
    sprintf( buf, "%s{GThe mark of %s is on it's forehead.{x.\n\r",
        pet->description, ch->name );
    free_string( pet->description );
    pet->description = str_dup( buf );
    char_to_room( pet, ch->in_room );
    if (stone != NULL && stone->item_type == ITEM_DEMON_STONE)
    {
        stone->value[0] = UMAX(0, stone->value[0]-1);
        act("You draw upon the power of $p.",ch,stone,NULL,TO_CHAR);
        act("$n draws upon the power of $p.",ch,stone,NULL,TO_ROOM);
        act("It flares brightly and $N appears.",ch,stone,pet,TO_CHAR);
        act("It flares brightly and $N appears.",ch,stone,pet,TO_ROOM);
    } else
    {
        act("$N suddenly appears in the room.",ch,NULL,pet,TO_CHAR);
        act("$N suddenly appears in the room.",ch,NULL,pet,TO_ROOM);
    }
    add_follower( pet, ch );
    pet->leader = ch;
    ch->pet = pet;
    pet->alignment = ch->alignment;
    pet->level = ch->level;
    pet->max_hit = pet->level * 40;
    pet->hit = pet->max_hit;
    pet->armor[0] = pet->level/2;
    pet->armor[1] = pet->level/2;
    pet->armor[2] = pet->level/2;
    pet->armor[3] = pet->level/3;

    return;
}



void spell_animate( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_STRING_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    CHAR_DATA *pet;
    int	length;

    if  ((obj->pIndexData->vnum > 17)
	|| (obj->pIndexData->vnum < 12))
    {
	send_to_char( "That's not a body part!\n\r", ch );
	return;
    }

    pMobIndex = get_mob_index( MOB_VNUM_ANIMATE );
    pet = create_mobile( pMobIndex );
    SET_BIT(pet->affected_by, AFF_CHARM);
    pet->comm = COMM_NOTELL|COMM_NOSHOUT|COMM_NOCHANNELS;
    sprintf( buf, "%s{GIt's branded with the mark of %s.{x.\n\r",
	obj->description, ch->name );
    free_string( pet->description );
    pet->description = str_dup( buf );
    free_string( pet->short_descr );
    pet->short_descr = str_dup( obj->short_descr );
    free_string( pet->name );
    pet->name = str_dup( obj->name );
    sprintf( buf, "%s", obj->description);
    length = strlen(buf)-12;
    strncpy( arg, buf, length);
    arg[length] = '\0';
    sprintf( buf, "%s floating here.\n\r", arg);
    free_string( pet->long_descr );
    pet->long_descr = str_dup( buf );
    char_to_room( pet, ch->in_room );
    add_follower( pet, ch );
    pet->leader = ch;
    obj_from_char( obj );
    sprintf( buf, "%s floats up and starts following you.\n\r", pet->short_descr);
    send_to_char( buf, ch);
    sprintf( buf, "%s floats up and starts following $n.", pet->short_descr);
    act( buf, ch, NULL, NULL, TO_ROOM);
    return;
}

void spell_iceshield( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_SHIELDED(victim, SHD_ICE))
    {
	if(victim == ch)
	    send_to_char("You are already surrounded by an {Cicy{x shield.\n\r", ch);
	else
	    act("$N is already surrounded by an {Cicy{x shield.",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (IS_NPC(victim) && !IS_IMMORTAL(ch) )
    {
        send_to_char("You failed.\n\r", ch);
        return;
    }

/*
    if ( (skill_table[sn].skill_level[victim->class] > LEVEL_HERO)
	&& (victim->level < LEVEL_IMMORTAL) )
    {
	send_to_char("You are surrounded by an {Cicy{x shield.\n\r", victim);
	act("$n is surrounded by an {Cicy{x shield.",victim, NULL,NULL,TO_ROOM);
	send_to_char("Your {Cicy{x shield quickly melts away.\n\r", victim);
	act("$n's {Cicy{x shield quickly melts away.",victim, NULL,NULL,TO_ROOM);
	return;
    }
*/

    af.where     = TO_SHIELDS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/3 + 5;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = SHD_ICE;

   affect_to_char(victim, &af);
   send_to_char("You are surrounded by an {Cicy{x shield.\n\r", victim);
   act("$n is surrounded by an {Cicy{x shield.",victim, NULL,NULL,TO_ROOM);
   return;
}

void spell_fireshield(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{

    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_SHIELDED(victim, SHD_FIRE))
    {
	if (victim == ch)
	    send_to_char("You are already surrounded by a {Rfirey{x shield.\r\n", ch);
	else
	    act("$N is already surrounded by a {Rfiery{x shield.",ch,NULL,victim,TO_CHAR);
	return;
    }

    if (IS_NPC(victim) && !IS_IMMORTAL(ch) )
    {
        send_to_char("You failed.\n\r", ch);
        return;
    }

/*
    if ( (skill_table[sn].skill_level[victim->class] > LEVEL_HERO)
	&& (victim->level < LEVEL_IMMORTAL) )
    {
	send_to_char("You are surrounded by a {Rfiery{x shield.\n\r", victim);
	act("$n is surrounded by a {Rfiery{x shield.",victim, NULL,NULL,TO_ROOM);
	send_to_char("Your {Rfirey{x shield gutters out.\n\r", victim);
	act("$n's {Rfirey{x shield gutters out.",victim, NULL,NULL,TO_ROOM);
	return;
    }
*/

    af.where     = TO_SHIELDS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/3 + 5;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = SHD_FIRE;

    affect_to_char(victim, &af);
    send_to_char("You are surrounded by a {Rfiery{x shield.\n\r", victim);
    act("$n is surrounded by a {Rfiery{x shield.",victim, NULL,NULL,TO_ROOM);
    return;

}

void spell_shockshield(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{

    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_SHIELDED(victim, SHD_SHOCK))
    {
	if (victim == ch)
	    send_to_char("You are already surrounded in a {Bcrackling{x shield.\n\r", ch);
	else
	    act("$N is already surrounded by a {Bcrackling{x shield.",ch, NULL, victim, TO_CHAR);
	return;
    }

    if (IS_NPC(victim) && !IS_IMMORTAL(ch) )
    {
        send_to_char("You failed.\n\r", ch);
        return;
    }

/*
    if ( (skill_table[sn].skill_level[victim->class] > LEVEL_HERO)
	&& (victim->level < LEVEL_IMMORTAL) )
    {
	send_to_char("You are surrounded by a {Bcrackling{x shield.\n\r", victim);
	act("$n is surrounded by a {Bcrackling{x shield.",victim, NULL,NULL,TO_ROOM);
	send_to_char("Your {Bcrackling{x shield sizzles and fades.\n\r", victim);
	act("$n's {Bcrackling{x shield sizzles and fades.",victim, NULL,NULL,TO_ROOM);
	return;
    }
*/

    af.where     = TO_SHIELDS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/3 + 5;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = SHD_SHOCK;

    affect_to_char(victim, &af);
    send_to_char("You are surrounded by a {Bcrackling{x field.\n\r",victim);
    act("$n is surrounded by a {Bcrackling{x shield.",victim, NULL,NULL, TO_ROOM);
    return;
}

void spell_quest_pill( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if (IS_NPC(victim))
	return;

    victim->qps++;
    send_to_char( "{YYou've gained a {RQuest Point{Y!{x\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}

void spell_voodoo( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    char name[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *bpart;
    OBJ_DATA *doll;

    bpart = get_eq_char(ch,WEAR_HOLD);
    if  ((bpart == NULL)
    ||   (bpart->pIndexData->vnum < 12)
    ||   (bpart->pIndexData->vnum > 17))
    {
	send_to_char("You are not holding a body part.\n\r",ch);
	return;
    }
    if (bpart->value[4] == 0)
    {
	send_to_char("This body part is from a mobile.\n\r",ch);
	return;
    }
    one_argument(bpart->name, name);
    doll = create_object(get_obj_index(OBJ_VNUM_VOODOO), 0);
    sprintf( buf, doll->short_descr, name );
    free_string( doll->short_descr );
    doll->short_descr = str_dup( buf );
    sprintf( buf, doll->description, name );
    free_string( doll->description );
    doll->description = str_dup( buf );
    sprintf( buf, doll->name, name );
    free_string( doll->name );
    doll->name = str_dup( buf );
    doll->timer = number_range(10,30);
    act( "$p morphs into a voodoo doll",ch,bpart,NULL,TO_CHAR);
    obj_from_char( bpart );
    obj_to_char(doll,ch);
    equip_char(ch,doll,WEAR_HOLD);
    act( "$n has created $p!", ch, doll, NULL, TO_ROOM );
    return;
}

void spell_create_cloud( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *cloud;

    cloud = create_object( get_obj_index( OBJ_VNUM_MAGIC_CLOUD ), 0 );
    cloud->level = UMAX(ch->level - 5,1);
    obj_to_room( cloud, ch->in_room );
    act( "$p poofs into the room.", ch, cloud, NULL, TO_ROOM );
    act( "$p poofs into the room.", ch, cloud, NULL, TO_CHAR );
    return;
}

void spell_vacillating_aura(int sn, int level, CHAR_DATA *ch, void *vo,int target)
{

    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_SHIELDED(victim, SHD_VACILLATING))
    {
	if (victim == ch)
	    send_to_char("You are already surrounded in a {Bfluctuating{x shield.\n\r", ch);
	else
	    act("$N is already surrounded by a {Bfluctuating{x shield.",ch, NULL, victim, TO_CHAR);
	return;
    }

    if (IS_NPC(victim) && !IS_IMMORTAL(ch) )
    {
	send_to_char("You failed.\n\r", ch);
	return;
    }


    af.where     = TO_SHIELDS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/10;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = SHD_VACILLATING;

    affect_to_char(victim, &af);
    send_to_char("You are surrounded by a {Bfluctuating{x field.\n\r",victim);
    act("$n is surrounded by a {Bfluctuating{x shield.",victim, NULL,NULL, TO_ROOM);
    return;
}



void spell_call_beast( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    char buf[MAX_STRING_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *pet;

    if (IS_NPC(ch))
	return;

    if ( ch->pet != NULL )
    {
	send_to_char("You failed.\n\r",ch);
	return;
    }

    switch(ch->in_room->sector_type)
    {
        case SECT_FOREST:
            if(level > 80)
                pMobIndex = get_mob_index( MOB_VNUM_DEMON );
            else
                pMobIndex = get_mob_index( MOB_VNUM_DEMON );
            break;

        default:
	    send_to_char("You failed.\n\r",ch);
            return;
    }

    if (!pMobIndex)
    {
        send_to_char("Your calls go unheeded.\n\r",ch);
        return;
    }


    pet = create_mobile( pMobIndex );
    if (!IS_SET(pet->act, ACT_PET) )
        SET_BIT(pet->act, ACT_PET);
    if (!IS_SET(pet->affected_by, AFF_CHARM) )
        SET_BIT(pet->affected_by, AFF_CHARM);
    pet->comm = COMM_NOTELL|COMM_NOSHOUT|COMM_NOCHANNELS;
    sprintf( buf, "%s{GA leaf with the name %s hangs from its neck.{x.\n\r",
        pet->description, ch->name );
    free_string( pet->description );
    pet->description = str_dup( buf );
    char_to_room( pet, ch->in_room );
    act("You call out into the wild and $N comes running up to you.",ch,NULL,pet,TO_CHAR);
    act("$N emerges from the wild and begins to follow $n.",ch,NULL,pet,TO_ROOM);
    add_follower( pet, ch );
    pet->leader = ch;
    ch->pet = pet;
    pet->alignment = ch->alignment;
    pet->level = ch->level;
    pet->max_hit = pet->level * 30;
    pet->hit = pet->max_hit;
    pet->armor[0] = pet->level * 2/3;
    pet->armor[1] = pet->level * 2/3;
    pet->armor[2] = pet->level * 2/3;
    pet->armor[3] = pet->level/3;

    return;
}

void spell_manashield(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{

    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_SHIELDED(victim, SHD_MANA))
    {
        if (victim == ch)
            send_to_char("You are already surrounded by a manashield.\r\n", ch);
        else
            act("$N is already surrounded by a manashield.",ch,NULL,victim,TO_CHAR);
        return;
    }

    if (victim != ch && !IS_IMMORTAL(ch))
    {
	send_to_char("Only immortals may cast mana shield on another",ch);
	return;
    }

    if( 100 * ch->hit / ch->max_hit < 40 )
    {
        if (victim == ch)
            send_to_char("You do not have enough mana to maintain a manashield.\r\n", ch);
        else
            act("$N does not have enough mana to maintain a mana shield.",ch,NULL,victim,TO_CHAR);
       return;
    }

    if (IS_NPC(victim) && !IS_IMMORTAL(ch) )
    {
        send_to_char("You failed.\n\r", ch);
        return;
    }

/*
    if ( (skill_table[sn].skill_level[victim->class] > LEVEL_HERO)
        && (victim->level < LEVEL_IMMORTAL) )
    {
        send_to_char("A protective manashield surrounds you.\n\r", victim);
        act("$n emanates a protective shield of mana.",victim, NULL,NULL,TO_ROOM);
        send_to_char("Your manashield fades away.\n\r", victim);
        act("$n's manashield fades away.",victim, NULL,NULL,TO_ROOM);
        return;
    }
*/

    af.where     = TO_SHIELDS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/3 + 5;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = SHD_MANA;

    affect_to_char(victim, &af);
    send_to_char("A protective manashield surrounds you.\n\r", victim);
    act("$n emanates a protective shield of mana.",victim, NULL,NULL,TO_ROOM);
    return;

}
