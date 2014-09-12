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
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "olc.h"
#include "magic.h"

#include <sys/resource.h>

/* command procedures needed */
DECLARE_DO_FUN(do_at		);
DECLARE_DO_FUN(do_rstat		);
DECLARE_DO_FUN(do_mstat		);
DECLARE_DO_FUN(do_ostat		);
DECLARE_DO_FUN(do_rset		);
DECLARE_DO_FUN(do_mset		);
DECLARE_DO_FUN(do_oset		);
DECLARE_DO_FUN(do_sset		);
DECLARE_DO_FUN(do_clanset	);
DECLARE_DO_FUN(do_langset	);
DECLARE_DO_FUN(do_mfind		);
DECLARE_DO_FUN(do_ofind		);
DECLARE_DO_FUN(do_rfind		);
DECLARE_DO_FUN(do_slookup	);
DECLARE_DO_FUN(do_mload		);
DECLARE_DO_FUN(do_oload		);
DECLARE_DO_FUN(do_vload		);
DECLARE_DO_FUN(do_pload		);
DECLARE_DO_FUN(do_force		);
DECLARE_DO_FUN(do_quit		);
DECLARE_DO_FUN(do_save		);
DECLARE_DO_FUN(do_transfer	);
DECLARE_DO_FUN(do_look		);
DECLARE_DO_FUN(do_force		);
DECLARE_DO_FUN(do_stand		);
DECLARE_DO_FUN(do_disconnect	);
DECLARE_DO_FUN(do_restore	);
DECLARE_DO_FUN(do_allpeace	);
DECLARE_DO_FUN(do_spellup_auto	);
DECLARE_DO_FUN(do_security	);
DECLARE_DO_FUN(do_addlag	);
DECLARE_DO_FUN(do_avatar	);
DECLARE_DO_FUN(do_resets	);
DECLARE_DO_FUN(do_olevel	);
DECLARE_DO_FUN(do_mlevel	);
DECLARE_DO_FUN(do_randomize	);

extern bool    remove_obj      args( (CHAR_DATA *ch, int iWear, bool fReplace ) );

void wiznet_new args((char *string, CHAR_DATA *ch, OBJ_DATA *obj,
	    long flag, long flag_skip, int min_level, int max_level));

void save_clanlist args((void));

bool check_parse_name(char *name);

extern char * smash_colour(char *buf);

void revoke_level args(( CHAR_DATA *ch, CHAR_DATA *victim, int level ));

/*
 * Local functions.
 */

void do_spellup(CHAR_DATA *ch,char *argument)
{
    CHAR_DATA *vch;
    char arg[MAX_INPUT_LENGTH];

    if(!IS_IMMORTAL(ch))
    {
        do_spellup_auto(ch,argument);
        return;
    }

    argument = one_argument(argument,arg);

    if (IS_NPC(ch))
        return;

    if (arg[0] == '\0')
    {
        send_to_char("Spellup whom?\n\r",ch);    
        send_to_char("Also: Spellup room, spellup auto\n\r",ch);    
        return;
    }

    if(!strcmp("auto",arg))
    {
        do_spellup_auto(ch,argument);
        return;
    }

    act("You snap your fingers.",ch,NULL,NULL,TO_CHAR);
    act("$n snaps $s fingers.",ch,NULL,NULL,TO_ROOM);

    if (!str_cmp("room",arg))
    {
        for (vch = ch->in_room->people;vch;vch = vch->next_in_room)
        {
            if (IS_NPC(vch) || vch == ch)
                continue;

            spell_shockshield(skill_lookup("shockshield"),ch->level,ch,vch,TARGET_CHAR);
            spell_fireshield(skill_lookup("fireshield"),ch->level,ch,vch,TARGET_CHAR);
            spell_iceshield(skill_lookup("iceshield"),ch->level,ch,vch,TARGET_CHAR);
            spell_shield(skill_lookup("shield"),ch->level,ch,vch,TARGET_CHAR);
            spell_armor(skill_lookup("armor"),ch->level,ch,vch,TARGET_CHAR);
            spell_sanctuary(skill_lookup("sanctuary"),ch->level,ch,vch,TARGET_CHAR);
            spell_fly(skill_lookup("fly"),ch->level,ch,vch,TARGET_CHAR);
            spell_frenzy(skill_lookup("frenzy"),ch->level,ch,vch,TARGET_CHAR);
            spell_giant_strength(skill_lookup("giant strength"),ch->level,ch,vch,TARGET_CHAR);
            spell_bless(skill_lookup("bless"),ch->level,ch,vch,TARGET_CHAR);
            spell_invis(skill_lookup("invis"),ch->level,ch,vch,TARGET_CHAR);
            spell_haste(skill_lookup("haste"),ch->level,ch,vch,TARGET_CHAR);
	    spell_detect_hidden(skill_lookup("detect hidden"),ch->level,ch,vch,TARGET_CHAR);
            spell_detect_invis(skill_lookup("detect invis"),ch->level,ch,vch,TARGET_CHAR);
	        spell_vacillating_aura(skill_lookup("vacillating aura"),ch->level,ch,vch,TARGET_CHAR);

            affect_strip(vch,gsn_plague);
            affect_strip(vch,gsn_poison);
            affect_strip(vch,gsn_accelerate_poison);
            affect_strip(vch,gsn_blindness);
            affect_strip(vch,gsn_sleep);
            affect_strip(vch,gsn_curse);
            affect_strip(vch,gsn_fire_breath);
            affect_strip(vch,gsn_weaken);

        }

        return;
    }

    if ((vch = get_char_room(ch,arg)) == NULL
         && (vch=get_char_world(ch,arg)) == NULL )
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }

    spell_shockshield(skill_lookup("shockshield"),ch->level,ch,vch,TARGET_CHAR);
    spell_fireshield(skill_lookup("fireshield"),ch->level,ch,vch,TARGET_CHAR);
    spell_iceshield(skill_lookup("iceshield"),ch->level,ch,vch,TARGET_CHAR);
    spell_shield(skill_lookup("shield"),ch->level,ch,vch,TARGET_CHAR);
    spell_armor(skill_lookup("armor"),ch->level,ch,vch,TARGET_CHAR);
    spell_sanctuary(skill_lookup("sanctuary"),ch->level,ch,vch,TARGET_CHAR);
    spell_fly(skill_lookup("fly"),ch->level,ch,vch,TARGET_CHAR);
    spell_frenzy(skill_lookup("frenzy"),ch->level,ch,vch,TARGET_CHAR);
    spell_giant_strength(skill_lookup("giant strength"),ch->level,ch,vch,TARGET_CHAR);
    spell_bless(skill_lookup("bless"),ch->level,ch,vch,TARGET_CHAR);
    spell_invis(skill_lookup("invis"),ch->level,ch,vch,TARGET_CHAR);
    spell_haste(skill_lookup("haste"),ch->level,ch,vch,TARGET_CHAR);
	spell_detect_hidden(skill_lookup("detect hidden"),ch->level,ch,vch,TARGET_CHAR);
	spell_detect_invis(skill_lookup("detect invis"),ch->level,ch,vch,TARGET_CHAR);
	spell_vacillating_aura(skill_lookup("vacillating aura"),ch->level,ch,vch,TARGET_CHAR);

    affect_strip(vch,gsn_plague);
    affect_strip(vch,gsn_poison);
    affect_strip(vch,gsn_accelerate_poison);
    affect_strip(vch,gsn_blindness);
    affect_strip(vch,gsn_sleep);
    affect_strip(vch,gsn_curse);
    affect_strip(vch,gsn_fire_breath);
    affect_strip(vch,gsn_weaken);
                          
    return;
}  
                                                                             


void do_wiznet( CHAR_DATA *ch, char *argument )
{
    int flag;
    char buf[MAX_STRING_LENGTH];

    if ( argument[0] == '\0' )
    {
      	if (IS_SET(ch->wiznet,WIZ_ON))
      	{
            send_to_char("Signing off of Wiznet.\n\r",ch);
            REMOVE_BIT(ch->wiznet,WIZ_ON);
      	}
      	else
      	{
            send_to_char("Welcome to Wiznet!\n\r",ch);
            SET_BIT(ch->wiznet,WIZ_ON);
      	}
      	return;
    }

    if (!str_prefix(argument,"on"))
    {
	send_to_char("Welcome to Wiznet!\n\r",ch);
	SET_BIT(ch->wiznet,WIZ_ON);
	return;
    }

    if (!str_prefix(argument,"off"))
    {
	send_to_char("Signing off of Wiznet.\n\r",ch);
	REMOVE_BIT(ch->wiznet,WIZ_ON);
	return;
    }

    /* show wiznet status */
    if (!str_prefix(argument,"status")) 
    {
	buf[0] = '\0';

	if (!IS_SET(ch->wiznet,WIZ_ON))
	    strcat(buf,"off ");

	for (flag = 0; wiznet_table[flag].name != NULL; flag++)
	    if (IS_SET(ch->wiznet,wiznet_table[flag].flag))
	    {
		strcat(buf,wiznet_table[flag].name);
		strcat(buf," ");
	    }

	strcat(buf,"\n\r");

	send_to_char("Wiznet status:\n\r",ch);
	send_to_char(buf,ch);
	return;
    }

    if (!str_prefix(argument,"show"))
    /* list of all wiznet options */
    {
	buf[0] = '\0';

	for (flag = 0; wiznet_table[flag].name != NULL; flag++)
	{
	    if (wiznet_table[flag].level <= get_trust(ch))
	    {
	    	strcat(buf,wiznet_table[flag].name);
	    	strcat(buf," ");
	    }
	}

	strcat(buf,"\n\r");

	send_to_char("Wiznet options available to you are:\n\r",ch);
	send_to_char(buf,ch);
	return;
    }
   
    flag = wiznet_lookup(argument);

    if (flag == -1 || get_trust(ch) < wiznet_table[flag].level)
    {
	send_to_char("No such option.\n\r",ch);
	return;
    }
   
    if (IS_SET(ch->wiznet,wiznet_table[flag].flag))
    {
	sprintf(buf,"You will no longer see %s on wiznet.\n\r",
	        wiznet_table[flag].name);
	send_to_char(buf,ch);
	REMOVE_BIT(ch->wiznet,wiznet_table[flag].flag);
    	return;
    }
    else
    {
    	sprintf(buf,"You will now see %s on wiznet.\n\r",
		wiznet_table[flag].name);
	send_to_char(buf,ch);
    	SET_BIT(ch->wiznet,wiznet_table[flag].flag);
	return;
    }

}

void wiznet(char *string, CHAR_DATA *ch, OBJ_DATA *obj,
	    long flag, long flag_skip, int min_level)
{
    wiznet_new(string, ch, obj, flag, flag_skip, min_level, MAX_LEVEL);
    return;
}

void wiznet_new(char *string, CHAR_DATA *ch, OBJ_DATA *obj,
	    long flag, long flag_skip, int min_level, int max_level) 
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    sprintf(buf, "{Y({RWizNet{Y){R: {V%s{x", string);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if (d->connected == CON_PLAYING
	&&  (IS_HERO(d->character)
	||  (d->character->class >= MAX_CLASS/2))
	&&  IS_SET(d->character->wiznet,WIZ_ON) 
	&&  (!flag || IS_SET(d->character->wiznet,flag))
	&&  (!flag_skip || !IS_SET(d->character->wiznet,flag_skip))
	&&  get_trust(d->character) >= min_level
	&&  get_trust(d->character) <= max_level
	&&  d->character != ch)
        {
	    if (IS_SET(d->character->wiznet,WIZ_PREFIX))
	  	send_to_char("{Y-->{x ",d->character);
            act_new(buf,d->character,obj,ch,TO_CHAR,POS_DEAD);
        }
    }
 
    return;
}

void do_iquest( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;

    argument = one_argument( argument, arg1 );

    if ( ( arg1[0] == '\0' ) || ( !IS_IMMORTAL(ch) ) )
    {
	if (!global_quest)
	{
	    send_to_char("There is no quest in progress.\n\r",ch);
	    return;
	}
	if (ch->on_quest)
	{
	    send_to_char("You'll have to wait till the quest is over.\n\r",ch);
	    return;
	}
	ch->on_quest = TRUE;
	send_to_char("Your quest flag is now on.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1, "on"))
    {
	if (global_quest)
	{
	    send_to_char("The global quest flag is already on.\n\r",ch);
	    return;
	}
	global_quest = TRUE;
	send_to_char("The global quest flag is now on.\n\r",ch);
	return;
    }
    if (!str_cmp(arg1, "off"))
    {
	if (!global_quest)
	{
	    send_to_char("The global quest flag is not on.\n\r",ch);
	    return;
	}
	global_quest = FALSE;
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected == CON_PLAYING )
	    {
		d->character->on_quest = FALSE;
	    }
	}
	send_to_char("The global quest flag is now off.\n\r",ch);
	return;
    }
    do_iquest(ch, "");
    return;
}

/* equips a character */
void do_outfit ( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int i,sn,vnum;

    if (ch->level > 9 || IS_NPC(ch))
    {
	send_to_char("Find it yourself!\n\r",ch);
	return;
    }

    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) == NULL )
    {
	if (ch->carry_number + 1 > can_carry_n(ch))
	{
	    send_to_char("You can't carry any more items.\n\r",ch);
	    return;
	}
        obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_BANNER), 0 );
	obj->cost = 0;
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_LIGHT );
	act("$G gives you a light.",ch,NULL,NULL,TO_CHAR);
    }
 
    if ( ( obj = get_eq_char( ch, WEAR_BODY ) ) == NULL )
    {
        if (ch->carry_number + 1 > can_carry_n(ch))
        {
            send_to_char("You can't carry any more items.\n\r",ch);
            return;
        }
	obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_VEST), 0 );
	obj->cost = 0;
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_BODY );
        act("$G gives you a vest.",ch,NULL,NULL,TO_CHAR);
    }

    /* do the weapon thing */
    if ((obj = get_eq_char(ch,WEAR_WIELD)) == NULL)
    {
    	sn = 0; 
    	vnum = OBJ_VNUM_SCHOOL_SWORD; /* just in case! */

        if (ch->carry_number + 1 > can_carry_n(ch))
        {
            send_to_char("You can't carry any more items.\n\r",ch);
            return;
        }

    	for (i = 0; weapon_table[i].name != NULL; i++)
    	{
	    if (ch->pcdata->learned[sn] < 
		ch->pcdata->learned[*weapon_table[i].gsn])
	    {
	    	sn = *weapon_table[i].gsn;
	    	vnum = weapon_table[i].vnum;
	    }
    	}

    	obj = create_object(get_obj_index(vnum),0);
     	obj_to_char(obj,ch);
    	equip_char(ch,obj,WEAR_WIELD);
        act("$G gives you a weapon.",ch,NULL,NULL,TO_CHAR);
    }

    if (((obj = get_eq_char(ch,WEAR_WIELD)) == NULL 
    ||   !IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS)) 
    &&  (obj = get_eq_char( ch, WEAR_SHIELD ) ) == NULL )
    {
        if (ch->carry_number + 1 > can_carry_n(ch))
        {
            send_to_char("You can't carry any more items.\n\r",ch);
            return;
        }
        obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SHIELD), 0 );
	obj->cost = 0;
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_SHIELD );
        act("$G gives you a shield.",ch,NULL,NULL,TO_CHAR);
    }
}

     
/* RT nochannels command, for those spammers */
void do_nochannels( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
 
    one_argument( argument, arg );
 
    if ( arg[0] == '\0' )
    {
        send_to_char( "Nochannel whom?", ch );
        return;
    }
 
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
 
    if ( victim->level >= ch->level )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }
 
    if ( IS_SET(victim->comm, COMM_NOCHANNELS) )
    {
        REMOVE_BIT(victim->comm, COMM_NOCHANNELS);
        send_to_char( "The gods have restored your channel priviliges.\n\r", 
		      victim );
        send_to_char( "NOCHANNELS removed.\n\r", ch );
	sprintf(buf,"$N restores channels to %s",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,get_trust(ch));
    }
    else
    {
        SET_BIT(victim->comm, COMM_NOCHANNELS);
        send_to_char( "The gods have revoked your channel priviliges.\n\r", 
		       victim );
        send_to_char( "NOCHANNELS set.\n\r", ch );
	sprintf(buf,"$N revokes %s's channels.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,get_trust(ch));
    }
 
    return;
}


void do_smote(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    char *letter,*name;
    char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
    int matches = 0;
 
    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
        send_to_char( "You can't show your emotions.\n\r", ch );
        return;
    }
 
    if ( argument[0] == '\0' )
    {
        send_to_char( "Emote what?\n\r", ch );
        return;
    }
    
    if (strstr(argument,ch->name) == NULL)
    {
	send_to_char("You must include your name in an smote.\n\r",ch);
	return;
    }
   
    send_to_char(argument,ch);
    send_to_char("\n\r",ch);
 
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
        if (vch->desc == NULL || vch == ch)
            continue;
 
        if ((letter = strstr(argument,vch->name)) == NULL)
        {
	    send_to_char(argument,vch);
	    send_to_char("\n\r",vch);
            continue;
        }
 
        strcpy(temp,argument);
        temp[strlen(argument) - strlen(letter)] = '\0';
        last[0] = '\0';
        name = vch->name;
 
        for (; *letter != '\0'; letter++)
        {
            if (*letter == '\'' && matches == strlen(vch->name))
            {
                strcat(temp,"r");
                continue;
            }
 
            if (*letter == 's' && matches == strlen(vch->name))
            {
                matches = 0;
                continue;
            }
 
            if (matches == strlen(vch->name))
            {
                matches = 0;
            }
 
            if (*letter == *name)
            {
                matches++;
                name++;
                if (matches == strlen(vch->name))
                {
                    strcat(temp,"you");
                    last[0] = '\0';
                    name = vch->name;
                    continue;
                }
                strncat(last,letter,1);
                continue;
            }
 
            matches = 0;
            strcat(temp,last);
            strncat(temp,letter,1);
            last[0] = '\0';
            name = vch->name;
        }
 
	send_to_char(temp,vch);
	send_to_char("\n\r",vch);
    }
 
    return;
}

void do_bamfin( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( !IS_NPC(ch) )
    {
	smash_tilde( argument );

	if (argument[0] == '\0')
	{
	    sprintf(buf,"Your poofin is %s\n\r",ch->pcdata->bamfin);
	    send_to_char(buf,ch);
	    return;
	}

	if ( strstr(argument,ch->name) == NULL)
	{
	    send_to_char("You must include your name.\n\r",ch);
	    return;
	}
	     
	free_string( ch->pcdata->bamfin );
	ch->pcdata->bamfin = str_dup( argument );

        sprintf(buf,"Your poofin is now %s\n\r",ch->pcdata->bamfin);
        send_to_char(buf,ch);
    }
    return;
}



void do_bamfout( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
 
    if ( !IS_NPC(ch) )
    {
        smash_tilde( argument );
 
        if (argument[0] == '\0')
        {
            sprintf(buf,"Your poofout is %s\n\r",ch->pcdata->bamfout);
            send_to_char(buf,ch);
            return;
        }
 
        if ( strstr(argument,ch->name) == NULL)
        {
            send_to_char("You must include your name.\n\r",ch);
            return;
        }
 
        free_string( ch->pcdata->bamfout );
        ch->pcdata->bamfout = str_dup( argument );
 
        sprintf(buf,"Your poofout is now %s\n\r",ch->pcdata->bamfout);
        send_to_char(buf,ch);
    }
    return;
}



void do_deny( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Deny whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    SET_BIT(victim->act, PLR_DENY);
    send_to_char( "You are denied access!\n\r", victim );
    sprintf(buf,"$N denies access to %s",victim->name);
    wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,get_trust(ch));
    send_to_char( "OK.\n\r", ch );
    save_char_obj(victim);
    stop_fighting(victim,TRUE);
    do_quit( victim, "" );

    return;
}

void do_wipe( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Wipe whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    SET_BIT(victim->comm, COMM_WIPED);
    sprintf(buf,"$N wipes access to %s",victim->name);
    wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,get_trust(ch));
    send_to_char( "OK.\n\r", ch );
    save_char_obj(victim);
    stop_fighting(victim,TRUE);
    do_disconnect( ch, victim->name);

    return;
}


void do_disconnect( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Disconnect whom?\n\r", ch );
	return;
    }

    if (is_number(arg))
    {
	int desc;

	if (ch->level < MAX_LEVEL)
	{
	    return;
	}

	desc = atoi(arg);
    	for ( d = descriptor_list; d != NULL; d = d->next )
    	{
            if ( d->descriptor == desc )
            {
            	close_socket( d );
            	send_to_char( "Ok.\n\r", ch );
            	return;
            }
	}
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL
    || (victim->level > ch->level && victim->level == MAX_LEVEL))
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->desc == NULL )
    {
	act( "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if (IS_SET(victim->act, PLR_KEY)
    && (ch->level < MAX_LEVEL) )
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d == ch->desc )
	    {
		close_socket( d );
		return;
	    }
	}
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d == victim->desc )
	{
	    close_socket( d );
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
    }

    bug( "Do_disconnect: desc not found.", 0 );
    send_to_char( "Descriptor not found!\n\r", ch );
    return;
}



void do_twit( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax: twit <character>.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( (victim->level >= ch->level) && (victim != ch) )
    {
	send_to_char( "Your command backfires!\n\r", ch );
	send_to_char( "You are now considered a TWIT.\n\r", ch );
	SET_BIT( ch->act, PLR_TWIT );
	return;
    }

    if ( IS_SET(victim->act, PLR_TWIT) )
    {
	send_to_char( "Someone beat you to it.\n\r", ch );
    } else
    {
	SET_BIT( victim->act, PLR_TWIT );
	send_to_char( "Twit flag set.\n\r", ch );
	send_to_char( "You are now considered a TWIT.\n\r", victim );
    }
    return;
}


void do_pardon( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

    if ( argument[0] == '\0' )
    {
        send_to_char( "Syntax: pardon <character>.\n\r", ch );
        return;
    }  

    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }  

    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }
       
    if ( IS_SET(victim->act, PLR_TWIT) )
    {
        REMOVE_BIT( victim->act, PLR_TWIT );
        send_to_char( "Twit flag removed.\n\r", ch );
        send_to_char( "You are no longer a TWIT.\n\r", victim );
    }
    return;
}



void do_echo( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    
    if ( argument[0] == '\0' )
    {
	send_to_char( "Global echo what?\n\r", ch );
	return;
    }
    
    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING )
	{
	    if (d->character->level >= ch->level)
		send_to_char( "global> ",d->character);
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r",   d->character );
	}
    }

    return;
}
/*
void do_writeall( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    
    if ( argument[0] == '\0' )
    {
	send_to_char( "Write all what?\n\r", ch );
	return;
    }
    
    for ( d = descriptor_list; d; d = d->next )
    {
	    if (d->character && d->character->level >= ch->level)
                write_to_desc(d->desc, "global#>\n",0);
            write_to_desc(d->desc, argument,0);
            write_to_desc(d->desc, "\n\r",0);

    }

    return;
}
*/

void do_wecho( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( argument[0] == '\0' )
    {
	send_to_char( "Warn echo what?\n\r", ch );
	return;
    }

    sprintf(buf, "{z{B***{x {R%s{x {z{B***{x", argument);
    do_echo(ch, buf);
    do_restore(ch, "all");
    do_allpeace(ch, "");
    return;
}

void do_recho( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    
    if ( argument[0] == '\0' )
    {
	send_to_char( "Local echo what?\n\r", ch );

	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	&&   d->character->in_room == ch->in_room )
	{
            if (d->character->level >= ch->level && d->character != ch )
                send_to_char( "local> ",d->character);
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r",   d->character );
	}
    }

    return;
}

void do_zecho(CHAR_DATA *ch, char *argument)
{
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0')
    {
	send_to_char("Zone echo what?\n\r",ch);
	return;
    }

    for (d = descriptor_list; d; d = d->next)
    {
	if (d->connected == CON_PLAYING
	&&  d->character->in_room != NULL && ch->in_room != NULL
	&&  d->character->in_room->area == ch->in_room->area)
	{
	    if (d->character->level >= ch->level)
		send_to_char("zone> ",d->character);
	    send_to_char(argument,d->character);
	    send_to_char("\n\r",d->character);
	}
    }
}

void do_pecho( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument(argument, arg);
 
    if ( argument[0] == '\0' || arg[0] == '\0' )
    {
	send_to_char("Personal echo what?\n\r", ch); 
	return;
    }
   
    if  ( (victim = get_char_world(ch, arg) ) == NULL )
    {
	send_to_char("Target not found.\n\r",ch);
	return;
    }

    if (victim->level >= ch->level)
        send_to_char( "personal> ",victim);

    send_to_char(argument,victim);
    send_to_char("\n\r",victim);
    send_to_char( "personal> ",ch);
    send_to_char(argument,ch);
    send_to_char("\n\r",ch);
}

void do_beep( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
 
    if ( argument[0] == '\0' )
    {
	send_to_char("Beep whom?\n\r", ch); 
	return;
    }
   
    if  ( (victim = get_char_world(ch, argument) ) == NULL
          || get_trust(victim) > get_trust(ch) )
    {
	send_to_char("Target not found.\n\r",ch);
	return;
    }

    send_to_char("\a",victim);
}


ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int vnum;

    if ( is_number(arg) )
	return get_room_index( atoi( arg ) );

    if ( ( victim = get_char_world( ch, arg ) ) != NULL )
	return victim->in_room;

    if ( ( obj = get_obj_world( ch, arg ) ) != NULL )
	return obj->in_room;

    if ( (vnum = quicklist_lookup(arg)) )
        return get_room_index( vnum );

    return NULL;
}

void do_corner( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Corner whom?\n\r", ch);
	return;
    }

    sprintf(buf, "%s %d", arg, ROOM_VNUM_CORNER);
    do_transfer( ch, buf );

    return;
}

void do_transfer( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Transfer whom (and where)?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg1, "all" ) && (ch->level >= CREATOR))
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&   d->character != ch
	    &&   d->character->in_room != NULL
	    &&   ch->level >= d->character->ghost_level
	    &&   can_see( ch, d->character ) )
	    {
		char buf[MAX_STRING_LENGTH];
		sprintf( buf, "%s %s", d->character->name, arg2 );
		do_transfer( ch, buf );
	    }
	}
	return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[0] == '\0' )
    {
	location = ch->in_room;
    }
    else
    {
	if ( ( location = find_location( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "No such location.\n\r", ch );
	    return;
	}

	if ( !is_room_owner(ch,location) && room_is_private( ch,location ) 
	&&  ch->level < MAX_LEVEL)
	{
	    send_to_char( "That room is private right now.\n\r", ch );
	    return;
	}
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (( victim->level > ch->level
    && !IS_SET(ch->act, PLR_KEY) 
    && (victim->level != MAX_LEVEL))
    || ((IS_SET(victim->act, PLR_KEY)) 
    && (ch->level != MAX_LEVEL)))
    {
        send_to_char( "You failed!\n\r", ch);
	return;
    }

    if ( victim->in_room == NULL )
    {
	send_to_char( "They are in limbo.\n\r", ch );
	return;
    }

    if ( victim->fighting != NULL )
	stop_fighting( victim, TRUE );
    act_new_2( "$n disappears in a mushroom cloud.", victim, NULL, NULL, TO_ROOM, POS_RESTING, 0, ACTION_MUST_SEE );
    char_from_room( victim );
    char_to_room( victim, location );
    act_new_2( "$n arrives from a puff of smoke.", victim, NULL, NULL, TO_ROOM, POS_RESTING, 0, ACTION_MUST_SEE );
    if ( ch != victim )
	act( "$n has transferred you.", ch, NULL, victim, TO_VICT );
    do_look( victim, "auto" );
    send_to_char( "Ok.\n\r", ch );
}

void do_allpeace( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	&&   d->character != ch
	&&   d->character->in_room != NULL
	&&   ch->level >= d->character->ghost_level
	&&   can_see( ch, d->character ) )
	{
	    char buf[MAX_STRING_LENGTH];
	    sprintf( buf, "%s peace", d->character->name );
	    do_at( ch, buf );
	}
    }
}

void do_wedpost( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax: wedpost <char>\n\r",ch);
        return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't playing.\n\r", ch );
        return;
    }
    
    if (victim->wedpost)
    {
        send_to_char("They are no longer allowed to post wedding announcements.\n\r",ch);
        victim->wedpost = FALSE;
    }
    else
    {
        send_to_char("They are now allowed to post wedding announcements.\n\r",ch);
        victim->wedpost = TRUE;
    }
}

void do_recover( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Recover whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (IS_NPC(victim) && !IS_SET(ch->act,ACT_PET) )
    {
	send_to_char( "You can't recover NPC's.\n\r", ch );
	return;
    }

    if (( get_trust( victim ) > get_trust( ch )
    && !IS_SET(ch->act, PLR_KEY) 
    && (victim->level != MAX_LEVEL))
    || ((IS_SET(victim->act, PLR_KEY)) 
    && (ch->level != MAX_LEVEL)))
    {
        send_to_char( "You failed!\n\r", ch);
	return;
    }

    if ( victim->in_room == NULL )
    {
	send_to_char( "They are in limbo.\n\r", ch );
	return;
    }

    if ( victim->fighting != NULL )
    {
	send_to_char( "They are fighting.\n\r", ch );
	return;
    }

    if ( victim->alignment < 0 )
    {
	if ( ( location = get_room_index( ROOM_VNUM_TEMPLEB ) ) == NULL )
	{
	    send_to_char( "The recall point seems to be missing.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( location = get_room_index( ROOM_VNUM_TEMPLE ) ) == NULL )
	{
	    send_to_char( "The recall point seems to be missing.\n\r", ch );
	    return;
	}
    }

    if (is_clan(victim)
    && (clan_table[victim->clan].room[0] != ROOM_VNUM_ALTAR)
    && !IS_SET(victim->act, PLR_TWIT))
	location = get_room_index( clan_table[victim->clan].room[0] );

    if (IS_NPC(victim) && IS_SET(ch->act,ACT_PET)
    && is_clan(victim->master)
    && (clan_table[victim->master->clan].room[0] != ROOM_VNUM_ALTAR)
    && !IS_SET(victim->master->act, PLR_TWIT)) 
        location = get_room_index( clan_table[victim->master->clan].room[0] );

    if ( victim->in_room == location )
    {
	act( "$N does not need recovering.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( !IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
    &&   !IS_AFFECTED(victim, AFF_CURSE))
    {
	act( "$N does not need recovering.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( victim->fighting != NULL )
	stop_fighting( victim, TRUE );
    act( "$n disappears in a flash.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, location );
    act( "$n arrives from a flash of light.", victim, NULL, NULL, TO_ROOM );
    if ( ch != victim )
	act( "$n has recovered you.", ch, NULL, victim, TO_VICT );
    do_look( victim, "auto" );
    act( "$N has been recovered.", ch, NULL, victim, TO_CHAR);
    if (victim->pet != NULL)
	do_recover(victim->pet,"");
}

void do_at( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    OBJ_DATA *on;
    CHAR_DATA *wch;
    
    if (IS_NPC(ch))
    {
	send_to_char( "NPC's cannot use this command.\n\r", ch);
	return;
    }
    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "At where what?\n\r", ch );
	return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if (!is_room_owner(ch,location) && room_is_private( ch,location ) 
    &&  ch->level < MAX_LEVEL)
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    original = ch->in_room;
    on = ch->on;
    char_from_room( ch );
    char_to_room( ch, location );
    interpret( ch, argument );

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
	if ( wch == ch )
	{
	    char_from_room( ch );
	    char_to_room( ch, original );
	    ch->on = on;
	    break;
	}
    }

    return;
}



void do_goto( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *location;
    CHAR_DATA *rch;
    char arg[MAX_INPUT_LENGTH];
    int count = 0;

    if ( ( argument[0] == '\0' )
	&& ( IS_NPC(ch) ) )
    {
	send_to_char( "Goto where?\n\r", ch );
	return;
    }
    if ( ( argument[0] == '\0' )
	&& ( !ch->pcdata->recall ) ) 
    {
	send_to_char( "Goto where?\n\r", ch );
	return;
    }
    if ( ( argument[0] == '\0' )
      && (ch->pcdata->recall) )
    {
	sprintf(arg, "%d", ch->pcdata->recall);
    }
    else
    {
	sprintf(arg, "%s", argument);
    }
    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    count = 0;
    for ( rch = location->people; rch != NULL; rch = rch->next_in_room )
        count++;

    if (!is_room_owner(ch,location) && room_is_private(ch,location) 
    &&  (count > 1 || ch->level < MAX_LEVEL))
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    if ( ch->fighting != NULL )
	stop_fighting( ch, TRUE );

    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
    {
	if ((rch->level >= ch->invis_level)
	&& (rch->level >= ch->ghost_level))
	{
	    if (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
            {
                if(get_trust(rch) > get_trust(ch))
		    act("*poof* $t",ch,ch->pcdata->bamfout,rch,TO_VICT);
                else
		    act("$t",ch,ch->pcdata->bamfout,rch,TO_VICT);
            }
	    else
		act("$n leaves in a swirling mist.",ch,NULL,rch,TO_VICT);
	}
    }

    char_from_room( ch );
    char_to_room( ch, location );


    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
    {
        if ((rch->level >= ch->invis_level)
	&& (rch->level >= ch->ghost_level))
        {
            if (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
            {
                if(get_trust(rch) > get_trust(ch))
                    act("*poof* $t",ch,ch->pcdata->bamfin,rch,TO_VICT);
                else
                    act("$t",ch,ch->pcdata->bamfin,rch,TO_VICT);
            }
            else
                act("$n appears in a swirling mist.",ch,NULL,rch,TO_VICT);
        }
    }
    if ( ( argument[0] == '\0' )
      && (ch->pet != NULL) )
    {
	char_from_room( ch->pet );
	char_to_room( ch->pet, location );
    }
    do_look( ch, "auto" );
    return;
}

void do_violate( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *location;
    CHAR_DATA *rch;
 
    if ( argument[0] == '\0' )
    {
        send_to_char( "Goto where?\n\r", ch );
        return;
    }
 
    if ( ( location = find_location( ch, argument ) ) == NULL )
    {
        send_to_char( "No such location.\n\r", ch );
        return;
    }

    if (!room_is_private( ch,location ))
    {
        send_to_char( "That room isn't private, use goto.\n\r", ch );
        return;
    }
 
    if ( ch->fighting != NULL )
        stop_fighting( ch, TRUE );
 
    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
    {
        if ((rch->level >= ch->invis_level)
	&& (rch->level >= ch->ghost_level))
        {
            if (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
                act("$t",ch,ch->pcdata->bamfout,rch,TO_VICT);
            else
                act("$n leaves in a swirling mist.",ch,NULL,rch,TO_VICT);
        }
    }
 
    char_from_room( ch );
    char_to_room( ch, location );
 
 
    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
    {
        if ((rch->level >= ch->invis_level)
	&& (rch->level >= ch->ghost_level))
        {
            if (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
                act("$t",ch,ch->pcdata->bamfin,rch,TO_VICT);
            else
                act("$n appears in a swirling mist.",ch,NULL,rch,TO_VICT);
        }
    }
 
    do_look( ch, "auto" );
    return;
}

/* RT to replace the 3 stat commands */

void do_stat ( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char *string;
   OBJ_DATA *obj;
   ROOM_INDEX_DATA *location;
   CHAR_DATA *victim;

   string = one_argument(argument, arg);
   if ( arg[0] == '\0')
   {
	send_to_char("Syntax:\n\r",ch);
	send_to_char("  stat <name>\n\r",ch);
	send_to_char("  stat obj <name>\n\r",ch);
	send_to_char("  stat mob <name>\n\r",ch);
 	send_to_char("  stat room <number>\n\r",ch);
	return;
   }

   if (!str_cmp(arg,"room"))
   {
	do_rstat(ch,string);
	return;
   }
  
   if (!str_cmp(arg,"obj"))
   {
	do_ostat(ch,string);
	return;
   }

   if(!str_cmp(arg,"char")  || !str_cmp(arg,"mob"))
   {
	do_mstat(ch,string);
	return;
   }
   
   /* do it the old way */

   obj = get_obj_world(ch,argument);
   if (obj != NULL)
   {
     do_ostat(ch,argument);
     return;
   }

  victim = get_char_world(ch,argument);
  if (victim != NULL)
  {
    do_mstat(ch,argument);
    return;
  }

  location = find_location(ch,argument);
  if (location != NULL)
  {
    do_rstat(ch,argument);
    return;
  }

  send_to_char("Nothing by that name found anywhere.\n\r",ch);
}

void do_rstat( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    OBJ_DATA *obj;
    CHAR_DATA *rch;
    int door;

    one_argument( argument, arg );
    location = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );
    if ( location == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if (!is_room_owner(ch,location) && ch->in_room != location 
    &&  room_is_private( ch,location ) && !IS_TRUSTED(ch,IMPLEMENTOR))
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }

    sprintf( buf, "Name: '%s'\n\rArea: '%s'\n\r",
	location->name,
	location->area->name );
    send_to_char( buf, ch );

    sprintf( buf,
	"Vnum: %d  Sector: %d  Light: %d  Healing: %d  Mana: %d\n\r",
	location->vnum,
	location->sector_type,
	location->light,
	location->heal_rate,
	location->mana_rate );
    send_to_char( buf, ch );

    sprintf( buf,
	"Room flags: %s.\n\rDescription:\n\r%s",
	room_bit_name(location->room_flags),
	location->description );
    send_to_char( buf, ch );

    if ( location->extra_descr != NULL )
    {
	EXTRA_DESCR_DATA *ed;

	send_to_char( "Extra description keywords: '", ch );
	for ( ed = location->extra_descr; ed; ed = ed->next )
	{
	    send_to_char( ed->keyword, ch );
	    if ( ed->next != NULL )
		send_to_char( " ", ch );
	}
	send_to_char( "'.\n\r", ch );
    }

    send_to_char( "Characters:", ch );
    for ( rch = location->people; rch; rch = rch->next_in_room )
    {
	if (( get_trust(ch) >= rch->ghost_level)
	&& (can_see(ch,rch)))
        {
	    send_to_char( " ", ch );
	    one_argument( rch->name, buf );
	    send_to_char( buf, ch );
	}
    }

    send_to_char( ".\n\rObjects:   ", ch );
    for ( obj = location->contents; obj; obj = obj->next_content )
    {
	send_to_char( " ", ch );
	one_argument( obj->name, buf );
	send_to_char( buf, ch );
    }
    send_to_char( ".\n\r", ch );

    if(location->pyobject != NULL)
        send_to_char("\n\rRoom has an associated Python object.\n\r",ch);


    for ( door = 0; door <= 5; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = location->exit[door] ) != NULL )
	{
	    sprintf( buf,
		"Door: %d.  To: %d.  Key: %d.  Exit flags: %ld.\n\rKeyword: '%s'.  Description: %s",

		door,
		(pexit->u1.to_room == NULL ? -1 : pexit->u1.to_room->vnum),
	    	pexit->key,
	    	pexit->exit_info,
	    	pexit->keyword,
	    	pexit->description[0] != '\0'
		    ? pexit->description : "(none).\n\r" );
	    send_to_char( buf, ch );
	}
    }

    return;
}



void do_ostat( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Stat what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_world( ch, argument ) ) == NULL )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
	return;
    }

    sprintf( buf, "Name(s): %s\n\r",
	obj->name );
    send_to_char( buf, ch );

    sprintf( buf, "Vnum: %d  Format: %s  Type: %s  Resets: %d\n\r",
	obj->pIndexData->vnum, obj->pIndexData->new_format ? "new" : "old",
	item_type_name(obj), obj->pIndexData->reset_num );
    send_to_char( buf, ch );

    sprintf( buf, "Short description: %s\n\rLong description: %s\n\r",
	obj->short_descr, obj->description );
    send_to_char( buf, ch );

    sprintf( buf, "Wear bits: %s\n\rExtra bits: %s\n\r",
	wear_bit_name(obj->wear_flags), extra_bit_name( obj->extra_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "Number: %d/%d  Weight: %d/%d/%d (10th pounds)\n\r",
	1,           get_obj_number( obj ),
	obj->weight, get_obj_weight( obj ),get_true_weight(obj) );
    send_to_char( buf, ch );

    sprintf( buf, "Level: %d  Cost: %d  Condition: %d  Timer: %d\n\r",
	obj->level, obj->cost, obj->condition, obj->timer );
    send_to_char( buf, ch );

    sprintf( buf,
	"In room: %d  In object: %s  Carried by: %s  Wear_loc: %d\n\r",
	obj->in_room    == NULL    ?        0 : obj->in_room->vnum,
	obj->in_obj     == NULL    ? "(none)" : obj->in_obj->short_descr,
	obj->carried_by == NULL    ? "(none)" : 
	    can_see(ch,obj->carried_by) ? obj->carried_by->name
				 	: "someone",
	obj->wear_loc );
    send_to_char( buf, ch );
    
    sprintf( buf, "Values: %d %d %d %d %d\n\r",
	obj->value[0], obj->value[1], obj->value[2], obj->value[3],
	obj->value[4] );
    send_to_char( buf, ch );
    
    /* now give out vital statistics as per identify */
    
    switch ( obj->item_type )
    {
    	case ITEM_SCROLL: 
    	case ITEM_POTION:
    	case ITEM_PILL:
	    sprintf( buf, "Level %d spells of:", obj->value[0] );
	    send_to_char( buf, ch );

	    if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
	    {
	    	send_to_char( " '", ch );
	    	send_to_char( skill_table[obj->value[1]].name, ch );
	    	send_to_char( "'", ch );
	    }

	    if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
	    {
	    	send_to_char( " '", ch );
	    	send_to_char( skill_table[obj->value[2]].name, ch );
	    	send_to_char( "'", ch );
	    }

	    if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
	    {
	    	send_to_char( " '", ch );
	    	send_to_char( skill_table[obj->value[3]].name, ch );
	    	send_to_char( "'", ch );
	    }

	    if (obj->value[4] >= 0 && obj->value[4] < MAX_SKILL)
	    {
		send_to_char(" '",ch);
		send_to_char(skill_table[obj->value[4]].name,ch);
		send_to_char("'",ch);
	    }

	    send_to_char( ".\n\r", ch );
	break;

    	case ITEM_WAND: 
    	case ITEM_STAFF: 
	    sprintf( buf, "Has %d(%d) charges of level %d",
	    	obj->value[1], obj->value[2], obj->value[0] );
	    send_to_char( buf, ch );
      
	    if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
	    {
	    	send_to_char( " '", ch );
	    	send_to_char( skill_table[obj->value[3]].name, ch );
	    	send_to_char( "'", ch );
	    }

	    send_to_char( ".\n\r", ch );
	break;

	case ITEM_DRINK_CON:
	    sprintf(buf,"It holds %s-colored %s.\n\r",
		liq_table[obj->value[2]].liq_color,
		liq_table[obj->value[2]].liq_name);
	    send_to_char(buf,ch);
	    break;
		
      
    	case ITEM_WEAPON:
 	    send_to_char("Weapon type is ",ch);
	    switch (obj->value[0])
	    {
	    	case(WEAPON_EXOTIC): 
		    send_to_char("exotic\n\r",ch);
		    break;
	    	case(WEAPON_SWORD): 
		    send_to_char("sword\n\r",ch);
		    break;	
	    	case(WEAPON_DAGGER): 
		    send_to_char("dagger\n\r",ch);
		    break;
	    	case(WEAPON_SPEAR):
		    send_to_char("spear/staff\n\r",ch);
		    break;
	    	case(WEAPON_MACE): 
		    send_to_char("mace/club\n\r",ch);	
		    break;
	   	case(WEAPON_AXE): 
		    send_to_char("axe\n\r",ch);	
		    break;
	    	case(WEAPON_FLAIL): 
		    send_to_char("flail\n\r",ch);
		    break;
	    	case(WEAPON_WHIP): 
		    send_to_char("whip\n\r",ch);
		    break;
	    	case(WEAPON_POLEARM): 
		    send_to_char("polearm\n\r",ch);
		    break;
	    	default: 
		    send_to_char("unknown\n\r",ch);
		    break;
 	    }
	    if (obj->clan)
	    {
		sprintf( buf, "Damage is variable.\n\r");
	    } else
	    {
		if (obj->pIndexData->new_format)
		    sprintf(buf,"Damage is %dd%d (average %d)\n\r",
		    obj->value[1],obj->value[2],
		    (1 + obj->value[2]) * obj->value[1] / 2);
		else
		    sprintf( buf, "Damage is %d to %d (average %d)\n\r",
	    	    obj->value[1], obj->value[2],
	    	    ( obj->value[1] + obj->value[2] ) / 2 );
	    }
	    send_to_char( buf, ch );

	    sprintf(buf,"Damage noun is %s.\n\r",
		attack_table[obj->value[3]].noun);
	    send_to_char(buf,ch);
	    
	    if (obj->value[4])  /* weapon flags */
	    {
	        sprintf(buf,"Weapons flags: %s\n\r",
		    weapon_bit_name(obj->value[4]));
	        send_to_char(buf,ch);
            }
	break;

    	case ITEM_ARMOR:
	    if (obj->clan)
	    {
		sprintf( buf, "Armor class is variable.\n\r");
	    } else
	    {
		sprintf( buf, 
		"Armor class is %d pierce, %d bash, %d slash, and %d vs. magic\n\r",
	        obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
	    }
	    send_to_char( buf, ch );
	break;

        case ITEM_CONTAINER:
        case ITEM_PIT:
            sprintf(buf,"Capacity: %d#  Maximum weight: %d#  flags: %s\n\r",
                obj->value[0], obj->value[3], cont_bit_name(obj->value[1]));
            send_to_char(buf,ch);
            if (obj->value[4] != 100)
            {
                sprintf(buf,"Weight multiplier: %d%%\n\r",
		    obj->value[4]);
                send_to_char(buf,ch);
            }
        break;
    }

    if (is_clan_obj(obj))
    {
	sprintf( buf, "This object is owned by the [{%s%s{x] clan.\n\r",
	    is_clan_pkill(obj->clan) ? "B" : "M",
	    clan_table[obj->clan].who_name
	    );
	send_to_char( buf, ch );
    }

    if (is_class_obj(obj))
    {
	sprintf( buf, "This object may only be used by a %s.\n\r",
	    class_table[obj->class].name
	    );
	send_to_char( buf, ch );
    }

    if ( obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL )
    {
	EXTRA_DESCR_DATA *ed;

	send_to_char( "Extra description keywords: '", ch );

	for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
	{
	    send_to_char( ed->keyword, ch );
	    if ( ed->next != NULL )
	    	send_to_char( " ", ch );
	}

	for ( ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next )
	{
	    send_to_char( ed->keyword, ch );
	    if ( ed->next != NULL )
		send_to_char( " ", ch );
	}

	send_to_char( "'\n\r", ch );
    }

    if(obj->pyobject != NULL)
        send_to_char("\n\rObject has an associated Python object.\n\r",ch);

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	sprintf( buf, "Affects %s by %d, level %d",
	    affect_loc_name( paf->location ), paf->modifier,paf->level );
	send_to_char(buf,ch);
	if ( paf->duration > -1)
	    sprintf(buf,", %d hours.\n\r",paf->duration);
	else
	    sprintf(buf,".\n\r");
	send_to_char( buf, ch );
	if (paf->bitvector)
	{
	    switch(paf->where)
	    {
		case TO_AFFECTS:
		    sprintf(buf,"Adds %s affect.\n",
			affect_bit_name(paf->bitvector));
		    break;
                case TO_WEAPON:
                    sprintf(buf,"Adds %s weapon flags.\n",
                        weapon_bit_name(paf->bitvector));
		    break;
		case TO_OBJECT:
		    sprintf(buf,"Adds %s object flag.\n",
			extra_bit_name(paf->bitvector));
		    break;
		case TO_IMMUNE:
		    sprintf(buf,"Adds immunity to %s.\n",
			imm_bit_name(paf->bitvector));
		    break;
		case TO_RESIST:
		    sprintf(buf,"Adds resistance to %s.\n\r",
			imm_bit_name(paf->bitvector));
		    break;
		case TO_VULN:
		    sprintf(buf,"Adds vulnerability to %s.\n\r",
			imm_bit_name(paf->bitvector));
		    break;
		case TO_SHIELDS:
		    sprintf(buf,"Adds %s shield.\n",
			shield_bit_name(paf->bitvector));
		    break;
		default:
		    sprintf(buf,"Unknown bit %d: %d\n\r",
			paf->where,paf->bitvector);
		    break;
	    }
	    send_to_char(buf,ch);
	}
    }

    if (!obj->enchanted)
    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    {
	sprintf( buf, "Affects %s by %d, level %d.\n\r",
	    affect_loc_name( paf->location ), paf->modifier,paf->level );
	send_to_char( buf, ch );
        if (paf->bitvector)
        {
            switch(paf->where)
            {
                case TO_AFFECTS:
                    sprintf(buf,"Adds %s affect.\n",
                        affect_bit_name(paf->bitvector));
                    break;
                case TO_OBJECT:
                    sprintf(buf,"Adds %s object flag.\n",
                        extra_bit_name(paf->bitvector));
                    break;
                case TO_IMMUNE:
                    sprintf(buf,"Adds immunity to %s.\n",
                        imm_bit_name(paf->bitvector));
                    break;
                case TO_RESIST:
                    sprintf(buf,"Adds resistance to %s.\n\r",
                        imm_bit_name(paf->bitvector));
                    break;
                case TO_VULN:
                    sprintf(buf,"Adds vulnerability to %s.\n\r",
                        imm_bit_name(paf->bitvector));
                    break;
                case TO_SHIELDS:
                    sprintf(buf,"Adds %s shield.\n",
                        shield_bit_name(paf->bitvector));
                    break;
                default:
                    sprintf(buf,"Unknown bit %d: %d\n\r",
                        paf->where,paf->bitvector);
                    break;
            }
            send_to_char(buf,ch);
        }
    }

    return;
}



void do_mstat( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    BUFFER *output;
    AFFECT_DATA *paf;
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Stat whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, argument ) ) == NULL 
    || (victim->level > ch->level && victim->level == MAX_LEVEL))
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    output = new_buf();

    if (!IS_NPC(victim))
    {
	sprintf( buf, "Name: %s\n\rSocket: %s\n\r",
	    victim->name, victim->pcdata->socket);
    } else
    {
	sprintf( buf, "Name: %s\n\rSocket: <mobile>\n\r",
	    victim->name);
    }
    add_buf(output,buf);

    sprintf( buf, 
	"Vnum: %d  Format: %s  Race: %s  Group: %d  Sex: %s  Room: %d\n\r",
	IS_NPC(victim) ? victim->pIndexData->vnum : 0,
	IS_NPC(victim) ? victim->pIndexData->new_format ? "new" : "old" : "pc",
	race_table[victim->race].name,
	IS_NPC(victim) ? victim->group : 0, sex_table[victim->sex].name,
	victim->in_room == NULL    ?        0 : victim->in_room->vnum
	);
    add_buf(output,buf);

    if (IS_NPC(victim))
    {
	sprintf(buf,"Count: %d  Killed: %d\n\r",
	    victim->pIndexData->count,victim->pIndexData->killed);
	add_buf(output,buf);
    }

    if (is_clan(victim))
    {
       sprintf(buf, "Clan: %-25s  Rank: %s\n\r",
               player_clan(victim), player_rank(victim));
       send_to_char(buf, ch);
    }

    sprintf( buf, 
   	"Str: %d(%d)  Int: %d(%d)  Wis: %d(%d)  Dex: %d(%d)  Con: %d(%d)\n\r",
	victim->perm_stat[STAT_STR],
	get_curr_stat(victim,STAT_STR),
	victim->perm_stat[STAT_INT],
	get_curr_stat(victim,STAT_INT),
	victim->perm_stat[STAT_WIS],
	get_curr_stat(victim,STAT_WIS),
	victim->perm_stat[STAT_DEX],
	get_curr_stat(victim,STAT_DEX),
	victim->perm_stat[STAT_CON],
	get_curr_stat(victim,STAT_CON) );
    add_buf(output,buf);

    sprintf( buf, "Hp: %d/%d  Mana: %d/%d  Move: %d/%d  Trains  %d  Pracs: %d\n\r",
	victim->hit,         victim->max_hit,
	victim->mana,        victim->max_mana,
	victim->move,        victim->max_move,
        IS_NPC(ch) ? 0 : victim->train,
	IS_NPC(ch) ? 0 : victim->practice );
    add_buf(output,buf);
	
    sprintf( buf,
	"Lv: %d  Class: %s  Align: %d  Exp: %ld\n\r",
	victim->level,       
	IS_NPC(victim) ? "mobile" : class_table[victim->class].name,            
	victim->alignment,
	victim->exp );
    add_buf(output,buf);

    sprintf( buf,
	"Platinum: %ld  Gold: %ld  Silver: %ld   Bank: %ld\n\r",
	victim->platinum, victim->gold, victim->silver,
        (IS_NPC(victim) || !victim->pcdata->bank_account)  ? 0 :
            victim->pcdata->bank_account->balance);
    add_buf(output,buf);

    sprintf(buf,"Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",
	    GET_AC(victim,AC_PIERCE), GET_AC(victim,AC_BASH),
	    GET_AC(victim,AC_SLASH),  GET_AC(victim,AC_EXOTIC));
    add_buf(output,buf);

    sprintf( buf, 
	"Hit: %d  Dam: %d  Saves: %d  Size: %s  Position: %s  Wimpy: %d\n\r",
	GET_HITROLL(victim), GET_DAMROLL(victim), victim->saving_throw,
	size_table[victim->size].name, position_table[victim->position].name,
	victim->wimpy );
    add_buf(output,buf);

    if (IS_NPC(victim) && victim->pIndexData->new_format)
    {
	sprintf(buf, "Damage: %dd%d  Message:  %s\n\r",
	    victim->damage[DICE_NUMBER],victim->damage[DICE_TYPE],
	    attack_table[victim->dam_type].noun);
	add_buf(output,buf);
    }
    sprintf( buf, "Fighting: %s\n\r",
	victim->fighting ? victim->fighting->name : "(none)" );
    add_buf(output,buf);

    if ( !IS_NPC(victim) )
    {
	sprintf( buf,
	    "Thirst: %d  Hunger: %d  Full: %d  Drunk: %d  Quest: %d\n\r",
	    victim->pcdata->condition[COND_THIRST],
	    victim->pcdata->condition[COND_HUNGER],
	    victim->pcdata->condition[COND_FULL],
	    victim->pcdata->condition[COND_DRUNK],
	    victim->qps );
	add_buf(output,buf);
    }

    sprintf( buf, "Carry number: %d  Carry weight: %ld\n\r",
	victim->carry_number, get_carry_weight(victim) / 10 );
    add_buf(output,buf);

    if (!IS_NPC(victim))
    {
    	sprintf( buf, 
	    "Age: %d  Played: %d  Last Level: %d  Timer: %d Aqps: %d\n\r",
	    get_age(victim), 
	    (int) (victim->played + current_time - victim->logon) / 3600, 
	    victim->pcdata->last_level, 
	    victim->timer,
            victim->pcdata->questpoints );
	add_buf(output,buf);
    }

    sprintf(buf, "Act: %s\n\r",act_bit_name(victim->act));
    add_buf(output,buf);
    
    if (victim->comm)
    {
    	sprintf(buf,"Comm: %s\n\r",comm_bit_name(victim->comm));
	add_buf(output,buf);
    }

    if (IS_NPC(victim) && victim->off_flags)
    {
    	sprintf(buf, "Offense: %s\n\r",off_bit_name(victim->off_flags));
	add_buf(output,buf);
    }

    if (victim->imm_flags)
    {
	sprintf(buf, "Immune: %s\n\r",imm_bit_name(victim->imm_flags));
	add_buf(output,buf);
    }
 
    if (victim->res_flags)
    {
	sprintf(buf, "Resist: %s\n\r", imm_bit_name(victim->res_flags));
	add_buf(output,buf);
    }

    if (victim->vuln_flags)
    {
	sprintf(buf, "Vulnerable: %s\n\r", imm_bit_name(victim->vuln_flags));
	add_buf(output,buf);
    }

/*
    sprintf(buf, "Form: %s\n\rParts: %s\n\r", 
	form_bit_name(victim->form), part_bit_name(victim->parts));
    add_buf(output,buf);*/

    if (victim->affected_by)
    {
	sprintf(buf, "Affected by %s\n\r", 
	    affect_bit_name(victim->affected_by));
	add_buf(output,buf);
    }

    if (victim->shielded_by)
    {
	sprintf(buf, "Shielded by %s\n\r", 
	    shield_bit_name(victim->shielded_by));
	add_buf(output,buf);
    }

    sprintf( buf, "Master: %s  Leader: %s  Pet: %s\n\r",
	victim->master      ? victim->master->name   : "(none)",
	victim->leader      ? victim->leader->name   : "(none)",
	victim->pet 	    ? victim->pet->name	     : "(none)");
    add_buf(output,buf);

    if (!IS_NPC(victim))
    {
	sprintf( buf, "Security: %d.\n\r", victim->pcdata->security );
	add_buf( output, buf);
    }

    if (!IS_NPC(victim))
    {
        sprintf( buf, "Trust: %d.\n\r", get_trust(victim) );
        add_buf( output, buf);
    }

    sprintf( buf, "Short description: %s\n\rLong  description: %s",
	victim->short_descr,
	victim->long_descr[0] != '\0' ? victim->long_descr : "(none)\n\r" );
    add_buf(output,buf);

    if ( IS_NPC(victim) && victim->spec_fun != 0 )
    {
	sprintf(buf,"Mobile has special procedure %s.\n\r",
		spec_name(victim->spec_fun));
	add_buf(output,buf);
    }

    if(victim->pyobject != NULL)
        add_buf(output,"\n\rCharacter has an associated Python object.\n\r");

    for ( paf = victim->affected; paf != NULL; paf = paf->next )
    {
	sprintf( buf,
	    "Spell: '%s' modifies %s by %d for %d hours with bits %s, level %d.\n\r",
	    skill_table[(int) paf->type].name,
	    affect_loc_name( paf->location ),
	    paf->modifier,
	    paf->duration,
	    paf->where == TO_SHIELDS ?
	    shield_bit_name( paf->bitvector ) :
	    affect_bit_name( paf->bitvector ),
	    paf->level
	    );
	add_buf(output,buf);
    }

    page_to_char( buf_string(output), ch );
    free_buf(output);
    return;
}

/* ofind and mfind replaced with vnum, vnum skill also added */

void do_vnum(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    AREA_DATA *pArea1;
    AREA_DATA *pArea2;
    int iArea;
    int iAreaHalf;
    char *string;

    string = one_argument(argument,arg);
 
    if (arg[0] == '\0')
    {
	send_to_char("Syntax:\n\r",ch);
	send_to_char("  vnum obj <name>\n\r",ch);
	send_to_char("  vnum mob <name>\n\r",ch);
	send_to_char("  vnum room <name>\n\r",ch);
	send_to_char("  vnum skill <skill or spell>\n\r",ch);
	send_to_char("  vnum areas\n\r",ch);
	return;
    }

    if (!str_cmp(arg,"obj"))
    {
	do_ofind(ch,string);
 	return;
    }

    if (!str_cmp(arg,"mob") || !str_cmp(arg,"char"))
    { 
	do_mfind(ch,string);
	return;
    }

    if (!str_cmp(arg,"room"))
    { 
	do_rfind(ch,string);
	return;
    }

    if (!str_cmp(arg,"skill") || !str_cmp(arg,"spell"))
    {
	do_slookup(ch,string);
	return;
    }

    if (!str_cmp(arg,"areas") || !str_cmp(arg,"area"))
    {
	output = new_buf();
	iAreaHalf = (top_area + 1) / 2;
	pArea1    = area_first;
	pArea2    = area_first;
	for ( iArea = 0; iArea < iAreaHalf; iArea++ )
	    pArea2 = pArea2->next;

	for ( iArea = 0; iArea < iAreaHalf; iArea++ )
	{
	    sprintf( buf, "%-26s {R%5d %5d{x  %-26s {R%5d %5d{x\n\r",
		pArea1->name, pArea1->min_vnum, pArea1->max_vnum,
		(pArea2 != NULL) ? pArea2->name : "",
		(pArea2 != NULL) ? pArea2->min_vnum : 0,
		(pArea2 != NULL) ? pArea2->max_vnum : 0);
	    add_buf(output,buf);
	    pArea1 = pArea1->next;
	    if ( pArea2 != NULL )
		pArea2 = pArea2->next;
	}
	page_to_char( buf_string(output), ch );
	free_buf(output);
 	return;
    }

    /* do both */
    do_mfind(ch,argument);
    do_ofind(ch,argument);
}


void do_mfind( CHAR_DATA *ch, char *argument )
{
    extern int top_mob_index;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    BUFFER *output;
    MOB_INDEX_DATA *pMobIndex;
    int vnum;
    int nMatch;
    bool fAll;
    bool found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Find whom?\n\r", ch );
	return;
    }

    fAll	= FALSE; /* !str_cmp( arg, "all" ); */
    found	= FALSE;
    nMatch	= 0;
    output	= new_buf();

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_mob_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_mob_index; vnum++ )
    {
	if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
	{
	    nMatch++;
	    if ( fAll || is_name( argument, pMobIndex->player_name ) )
	    {
		found = TRUE;
		sprintf( buf, "[%5d] %s\n\r",
		    pMobIndex->vnum, pMobIndex->short_descr );
		add_buf(output,buf);
	    }
	}
    }

    if ( !found )
    {
	send_to_char( "No mobiles by that name.\n\r", ch );
    }
    else
    {
	page_to_char( buf_string(output), ch );
    }
    free_buf(output);
    return;
}



void do_ofind( CHAR_DATA *ch, char *argument )
{
    extern int top_obj_index;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    BUFFER *output;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    int nMatch;
    bool fAll;
    bool found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Find what?\n\r", ch );
	return;
    }

    fAll	= FALSE; /* !str_cmp( arg, "all" ); */
    found	= FALSE;
    nMatch	= 0;
    output	= new_buf();

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_obj_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
	if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
	{
	    nMatch++;
	    if ( fAll || is_name( argument, pObjIndex->name ) )
	    {
		found = TRUE;
		sprintf( buf, "[%5d] %s\n\r",
		    pObjIndex->vnum, pObjIndex->short_descr );
		add_buf(output,buf);
	    }
	}
    }

    if ( !found )
    {
	send_to_char( "No objects by that name.\n\r", ch );
    }
    else
    {
	page_to_char( buf_string(output), ch );
    }
    free_buf(output);
    return;
}

void do_rfind( CHAR_DATA *ch, char *argument )
{
    extern int top_room;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    BUFFER *output;
    ROOM_INDEX_DATA *pRoomIndex;
    int vnum;
    int nMatch;
    bool found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Find what?\n\r", ch );
	return;
    }

    found	= FALSE;
    nMatch	= 0;
    output	= new_buf();

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_obj_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_room; vnum++ )
    {
	if ( ( pRoomIndex = get_room_index( vnum ) ) != NULL )
	{
	    nMatch++;
	    if ( is_name( argument, pRoomIndex->name ) )
	    {
		found = TRUE;
		sprintf( buf, "[%5d] %s\n\r",
		    pRoomIndex->vnum, pRoomIndex->name );
		add_buf(output,buf);
	    }
	}
    }

    if ( !found )
    {
	send_to_char( "No rooms by that name.\n\r", ch );
    }
    else
    {
	page_to_char( buf_string(output), ch );
    }
    free_buf(output);
    return;
}


void do_owhere(CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];
    BUFFER *buffer;
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    bool found;
    int number = 0, max_found;

    found = FALSE;
    number = 0;
    max_found = 200;

    buffer = new_buf();

    if (argument[0] == '\0')
    {
	send_to_char("Find what?\n\r",ch);
	return;
    }
 
    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
        if ( !can_see_obj( ch, obj ) || !is_name( argument, obj->name )
        ||   ch->level < obj->level
	||   (obj->carried_by != NULL && !can_see(ch,obj->carried_by)))
            continue;
 
        found = TRUE;
        number++;
 
        for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
            ;
 
        if ( in_obj->carried_by != NULL && can_see(ch,in_obj->carried_by)
	&&   in_obj->carried_by->in_room != NULL)
            sprintf( buf, "%3d) %s is carried by %s [Room %d]\n\r",
                number, obj->short_descr,PERS(in_obj->carried_by, ch),
		in_obj->carried_by->in_room->vnum );
        else if (in_obj->in_room != NULL && can_see_room(ch,in_obj->in_room))
            sprintf( buf, "%3d) %s is in %s [Room %d]\n\r",
                number, obj->short_descr,in_obj->in_room->name, 
	   	in_obj->in_room->vnum);
	else
            sprintf( buf, "%3d) %s is somewhere\n\r",number, obj->short_descr);
 
        buf[0] = UPPER(buf[0]);
        add_buf(buffer,buf);
 
        if (number >= max_found)
            break;
    }
 
    if ( !found )
        send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
    else
        page_to_char(buf_string(buffer),ch);

    free_buf(buffer);
}


void do_mwhere( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *buffer;
    CHAR_DATA *victim;
    bool found;
    int count = 0;

    if ( argument[0] == '\0' )
    {
	DESCRIPTOR_DATA *d;

	/* show characters logged */

	buffer = new_buf();
	for (d = descriptor_list; d != NULL; d = d->next)
	{
	    if (d->character != NULL && d->connected == CON_PLAYING
	    &&  d->character->in_room != NULL && can_see(ch,d->character)
	    &&  can_see_room(ch,d->character->in_room))
	    {
		victim = d->character;
		if ((victim->level <= CREATOR && ch->level <= CREATOR)
		    || ch->level > CREATOR)
		{
		    count++;
		    if (d->original != NULL)
			sprintf(buf,"%3d) %s (in the body of %s) is in %s [%d]\n\r",
			count, d->original->name,victim->short_descr,
			victim->in_room->name,victim->in_room->vnum);
		    else
			sprintf(buf,"%3d) %s is in %s [%d]\n\r",
			count, victim->name,victim->in_room->name,
			victim->in_room->vnum);
		    add_buf(buffer,buf);
		}
	    }
	}

        page_to_char(buf_string(buffer),ch);
	free_buf(buffer);
	return;
    }

    found = FALSE;
    buffer = new_buf();
    for ( victim = char_list; victim != NULL; victim = victim->next )
    {
	if ( victim->in_room != NULL
	&&   is_name( argument, victim->name ) )
	{
	    if ((victim->level <= CREATOR && ch->level <= CREATOR)
		|| ch->level > CREATOR)
	    {
		found = TRUE;
		count++;
		sprintf( buf, "%3d) [%5d] %-28s [%5d] %s\n\r", count,
		IS_NPC(victim) ? victim->pIndexData->vnum : 0,
		IS_NPC(victim) ? victim->short_descr : victim->name,
		victim->in_room->vnum,
		victim->in_room->name );
		add_buf(buffer,buf);
	    }
	}
    }

    if ( !found )
	act( "You didn't find any $T.", ch, NULL, argument, TO_CHAR );
    else
    	page_to_char(buf_string(buffer),ch);

    free_buf(buffer);

    return;
}



void do_reboo( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to REBOOT, spell it out.\n\r", ch );
    return;
}



void do_reboot( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    extern bool merc_down;
    DESCRIPTOR_DATA *d,*d_next;

    if (ch->invis_level < LEVEL_HERO)
    {
    	sprintf( buf, "Reboot by %s.", ch->name );
    	do_echo( ch, buf );
    }
    do_force ( ch, "all save");
    do_save (ch, "");
    merc_down = TRUE;
    for ( d = descriptor_list; d != NULL; d = d_next )
    {
	d_next = d->next;
    	close_socket(d);
    }

    unlink(SHUTDOWN_FILE);
    
    return;
}



void do_shutdow( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SHUTDOWN, spell it out.\n\r", ch );
    return;
}



void do_shutdown( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    extern bool merc_down;
    DESCRIPTOR_DATA *d,*d_next;

    if (ch->invis_level < LEVEL_HERO)
    sprintf( buf, "Shutdown by %s.", ch->name );
    append_file( ch, SHUTDOWN_FILE, buf );
    strcat( buf, "\n\r" );
    if (ch->invis_level < LEVEL_HERO)
    	do_echo( ch, buf );
    do_force ( ch, "all save");
    do_save (ch, "");
    merc_down = TRUE;
    for ( d = descriptor_list; d != NULL; d = d_next)
    {
	d_next = d->next;
	close_socket(d);
    }
    return;
}

void do_protect( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;

    if (argument[0] == '\0')
    {
	send_to_char("Protect whom from snooping?\n\r",ch);
	return;
    }

    if ((victim = get_char_world(ch,argument)) == NULL)
    {
	send_to_char("You can't find them.\n\r",ch);
	return;
    }

    if (IS_SET(victim->comm,COMM_SNOOP_PROOF))
    {
	act_new("$N is no longer snoop-proof.",ch,NULL,victim,TO_CHAR,POS_DEAD);
	send_to_char("Your snoop-proofing was just removed.\n\r",victim);
	REMOVE_BIT(victim->comm,COMM_SNOOP_PROOF);
    }
    else
    {
	act_new("$N is now snoop-proof.",ch,NULL,victim,TO_CHAR,POS_DEAD);
	send_to_char("You are now immune to snooping.\n\r",victim);
	SET_BIT(victim->comm,COMM_SNOOP_PROOF);
    }
}
  


void do_snoop( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Snoop whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->desc == NULL )
    {
	send_to_char( "No descriptor to snoop.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Cancelling all snoops.\n\r", ch );
	if (!IS_TRUSTED(ch,IMPLEMENTOR))
	{
	    wiznet("$N stops being such a snoop.",
		ch,NULL,WIZ_SNOOPS,WIZ_SECURE,get_trust(ch));
	}
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->snoop_by == ch->desc )
		d->snoop_by = NULL;
	}
	return;
    }

    if ( victim->desc->snoop_by != NULL )
    {
	send_to_char( "Busy already.\n\r", ch );
	return;
    }

    if (!is_room_owner(ch,victim->in_room) && ch->in_room != victim->in_room 
    &&  room_is_private(ch,victim->in_room) && !IS_TRUSTED(ch,IMPLEMENTOR))
    {
        send_to_char("That character is in a private room.\n\r",ch);
        return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) 
    || ( IS_SET(victim->comm,COMM_SNOOP_PROOF) && !IS_TRUSTED(ch,IMPLEMENTOR)))
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( ch->desc != NULL )
    {
	for ( d = ch->desc->snoop_by; d != NULL; d = d->snoop_by )
	{
	    if ( d->character == victim || d->original == victim )
	    {
		send_to_char( "No snoop loops.\n\r", ch );
		return;
	    }
	}
    }

/*
    if(arg2[0] != '\0' && is_number(arg2))
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->descriptor == atoi(arg2) )
            {
                if(d->hostip == ch->desc->hostip)
                {
		    victim->desc->snoop_by = d;
                    d->connected = CON_INFO;
                    send_to_char("Okay.\n\r", ch);
                    return;
                }
            }
	}

        send_to_char("Failed. (Must be same IP.)\n\r",ch);
        return;
    }
    else*/
        victim->desc->snoop_by = ch->desc;

    if (!IS_TRUSTED(ch,IMPLEMENTOR))
    {
	sprintf(buf,"$N starts snooping on %s",
	    (IS_NPC(ch) ? victim->short_descr : victim->name));
	wiznet(buf,ch,NULL,WIZ_SNOOPS,WIZ_SECURE,get_trust(ch) + 1);
    }
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_switch( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    
    if ( arg[0] == '\0' )
    {
	send_to_char( "Switch into whom?\n\r", ch );
	return;
    }

    if ( ch->desc == NULL )
	return;
    
    if ( ch->desc->original != NULL )
    {
	send_to_char( "You are already switched.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if (!IS_NPC(victim))
    {
	send_to_char("You can only switch into mobiles.\n\r",ch);
	return;
    }

    if (victim->level > ch->level)
    {
	send_to_char("That character is too powerful for you to handle.\n\r",ch);
	return;
    }

    if (!is_room_owner(ch,victim->in_room) && ch->in_room != victim->in_room 
    &&  room_is_private(ch,victim->in_room) && !IS_TRUSTED(ch,IMPLEMENTOR))
    {
	send_to_char("That character is in a private room.\n\r",ch);
	return;
    }

    if ( victim->desc != NULL )
    {
	send_to_char( "Character in use.\n\r", ch );
	return;
    }

    sprintf(buf,"$N switches into %s",victim->short_descr);
    wiznet(buf,ch,NULL,WIZ_SWITCHES,WIZ_SECURE,get_trust(ch) + 1);

    ch->desc->character = victim;
    ch->desc->original  = ch;
    victim->desc        = ch->desc;
    ch->desc            = NULL;
    /* change communications to match */
    if (ch->prompt != NULL)
        victim->prompt = str_dup(ch->prompt);
    victim->comm = ch->comm;
    victim->lines = ch->lines;
    send_to_char( "Ok.\n\r", victim );
    return;
}



void do_return( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( ch->desc == NULL )
	return;

    if ( ch->desc->original == NULL )
    {
	send_to_char( "You aren't switched.\n\r", ch );
	return;
    }

    send_to_char( 
"You return to your original body. Type replay to see any missed tells.\n\r", 
	ch );
    if (ch->prompt != NULL)
    {
	free_string(ch->prompt);
	ch->prompt = NULL;
    }

    sprintf(buf,"$N returns from %s.",ch->short_descr);
    wiznet(buf,ch->desc->original,0,WIZ_SWITCHES,WIZ_SECURE,get_trust(ch) + 1);
    ch->desc->character       = ch->desc->original;
    ch->desc->original        = NULL;
    ch->desc->character->desc = ch->desc; 
    ch->desc                  = NULL;
    return;
}

/* trust levels for load and clone */
bool obj_check (CHAR_DATA *ch, OBJ_DATA *obj)
{
    if (IS_TRUSTED(ch,GOD)
	|| (IS_TRUSTED(ch,IMMORTAL) && obj->level <= 105)
	|| (IS_TRUSTED(ch,DEMI)	    && obj->level <= 100)
	|| (IS_TRUSTED(ch,KNIGHT)    && obj->level <= 20)
	|| (IS_TRUSTED(ch,SQUIRE)   && obj->level ==  5))
	return TRUE;
    else
	return FALSE;
}

/* for clone, to insure that cloning goes many levels deep */
void recursive_clone(CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *clone)
{
    OBJ_DATA *c_obj, *t_obj;


    for (c_obj = obj->contains; c_obj != NULL; c_obj = c_obj->next_content)
    {
	if (obj_check(ch,c_obj))
	{
	    t_obj = create_object(c_obj->pIndexData,0);
	    clone_object(c_obj,t_obj);
	    obj_to_obj(t_obj,clone);
	    recursive_clone(ch,c_obj,t_obj);
	}
    }
}

/* command that is similar to load */
void do_clone(CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char *rest;
    CHAR_DATA *mob;
    OBJ_DATA  *obj;

    rest = one_argument(argument,arg);

    if (arg[0] == '\0')
    {
	send_to_char("Clone what?\n\r",ch);
	return;
    }

    if (!str_prefix(arg,"object"))
    {
	mob = NULL;
	obj = get_obj_here(ch,rest);
	if (obj == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}
    }
    else if (!str_prefix(arg,"mobile") || !str_prefix(arg,"character"))
    {
	obj = NULL;
	mob = get_char_room(ch,rest);
	if (mob == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}
    }
    else /* find both */
    {
	mob = get_char_room(ch,argument);
	obj = get_obj_here(ch,argument);
	if (mob == NULL && obj == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}
    }

    /* clone an object */
    if (obj != NULL)
    {
	OBJ_DATA *clone;

	if (!obj_check(ch,obj))
	{
	    send_to_char(
		"Your powers are not great enough for such a task.\n\r",ch);
	    return;
	}
	if (obj->item_type == ITEM_EXIT)
	{
	    send_to_char("You cannot clone an exit object.\n\r",ch);
	    return;
	}
	clone = create_object(obj->pIndexData,0); 
	clone_object(obj,clone);
	if (obj->carried_by != NULL)
	    obj_to_char(clone,ch);
	else
	    obj_to_room(clone,ch->in_room);
 	recursive_clone(ch,obj,clone);

	act("$n has created $p.",ch,clone,NULL,TO_ROOM);
	act("You clone $p.",ch,clone,NULL,TO_CHAR);
	wiznet("$N clones $p.",ch,clone,WIZ_LOAD,WIZ_SECURE,get_trust(ch));
	return;
    }
    else if (mob != NULL)
    {
	CHAR_DATA *clone;
	OBJ_DATA *new_obj;
	char buf[MAX_STRING_LENGTH];

	if (!IS_NPC(mob))
	{
	    send_to_char("You can only clone mobiles.\n\r",ch);
	    return;
	}

	if ((mob->level > 100 && !IS_TRUSTED(ch,GOD))
	||  (mob->level > 90 && !IS_TRUSTED(ch,IMMORTAL))
	||  (mob->level > 85 && !IS_TRUSTED(ch,DEMI))
	||  (mob->level >  0 && !IS_TRUSTED(ch,KNIGHT))
	||  !IS_TRUSTED(ch,SQUIRE))
	{
	    send_to_char(
		"Your powers are not great enough for such a task.\n\r",ch);
	    return;
	}

	clone = create_mobile(mob->pIndexData);
	clone_mobile(mob,clone); 
	
	for (obj = mob->carrying; obj != NULL; obj = obj->next_content)
	{
	    if (obj_check(ch,obj))
	    {
		new_obj = create_object(obj->pIndexData,0);
		clone_object(obj,new_obj);
		recursive_clone(ch,obj,new_obj);
		obj_to_char(new_obj,clone);
		new_obj->wear_loc = obj->wear_loc;
	    }
	}
	char_to_room(clone,ch->in_room);
        act("$n has created $N.",ch,NULL,clone,TO_ROOM);
        act("You clone $N.",ch,NULL,clone,TO_CHAR);
	sprintf(buf,"$N clones %s.",clone->short_descr);
	wiznet(buf,ch,NULL,WIZ_LOAD,WIZ_SECURE,get_trust(ch));
        return;
    }
}

/* RT to replace the two load commands */

void do_load(CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];

    argument = one_argument(argument,arg);

    if (arg[0] == '\0')
    {
	send_to_char("Syntax:\n\r",ch);
	send_to_char("  load mob <vnum>\n\r",ch);
	send_to_char("  load obj <vnum> <level>\n\r",ch);
	if (ch->level >= CREATOR)
	    send_to_char("  load voodoo <player>\n\r",ch);
	if (ch->level == MAX_LEVEL)
	    send_to_char("  load player <player>\n\r",ch);
	return;
    }

    if (!str_cmp(arg,"mob") || !str_cmp(arg,"char"))
    {
	do_mload(ch,argument);
	return;
    }

    if (!str_cmp(arg,"obj"))
    {
	do_oload(ch,argument);
	return;
    }

    if (!str_cmp(arg,"voodoo") && (ch->level >= CREATOR))
    {
	do_vload(ch,argument);
	return;
    }

    if (!str_cmp(arg,"player") && (ch->level == MAX_LEVEL))
    {
	do_pload(ch,argument);
	return;
    }

    /* echo syntax */
    do_load(ch,"");
}


void do_mload( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    
    one_argument( argument, arg );

    if ( arg[0] == '\0' || !is_number(arg) )
    {
	send_to_char( "Syntax: load mob <vnum>.\n\r", ch );
	return;
    }

    if ( ( pMobIndex = get_mob_index( atoi( arg ) ) ) == NULL )
    {
	send_to_char( "No mob has that vnum.\n\r", ch );
	return;
    }

    victim = create_mobile( pMobIndex );
    char_to_room( victim, ch->in_room );
    act( "$n has created $N!", ch, NULL, victim, TO_ROOM );
    sprintf(buf,"$N loads %s.",victim->short_descr);
    wiznet(buf,ch,NULL,WIZ_LOAD,WIZ_SECURE,get_trust(ch));
    send_to_char( "Ok.\n\r", ch );
    return;
}

void do_oload( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH] ,arg2[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    int level;
    
    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || !is_number(arg1))
    {
	send_to_char( "Syntax: load obj <vnum> <level>.\n\r", ch );
	return;
    }
    
    level = get_trust(ch); /* default */
  
    if ( arg2[0] != '\0')  /* load with a level */
    {
	if (!is_number(arg2))
        {
	  send_to_char( "Syntax: oload <vnum> <level>.\n\r", ch );
	  return;
	}
        level = atoi(arg2);
        if (level < 0 || level > get_trust(ch))
	{
	  send_to_char( "Level must be be between 0 and your level.\n\r",ch);
  	  return;
	}
    }

    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
	send_to_char( "No object has that vnum.\n\r", ch );
	return;
    }
    if (pObjIndex->item_type == ITEM_EXIT)
    {
	send_to_char("You cannot load an exit object.\n\r",ch);
	return;
    }

    obj = create_object( pObjIndex, level );
    if ( CAN_WEAR(obj, ITEM_TAKE) )
	obj_to_char( obj, ch );
    else
	obj_to_room( obj, ch->in_room );
    act( "$n has created $p!", ch, obj, NULL, TO_ROOM );
    wiznet("$N loads $p.",ch,obj,WIZ_LOAD,WIZ_SECURE,get_trust(ch));
    send_to_char( "Ok.\n\r", ch );
    return;
}

void do_vload( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    DESCRIPTOR_DATA *d;
    bool found = FALSE;
    char *name;
   
    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0')
    {
        send_to_char( "Syntax: load voodoo <player>\n\r", ch );
        return;
    }

    log_f("%s: load voodoo %s", ch->name, argument);
    
    for (d = descriptor_list; d != NULL; d = d->next)
    {
	CHAR_DATA *wch;

	if (d->connected != CON_PLAYING || !can_see(ch,d->character))
	    continue;

	wch = ( d->original != NULL ) ? d->original : d->character;

	if (!can_see(ch,wch))
	    continue;

	if (!str_prefix(arg1,wch->name) && !found)
	{
	    if (IS_NPC(wch))
		continue;

	    if (wch->level > ch->level)
		continue;

	    found = TRUE;
 
	    if ( ( pObjIndex = get_obj_index( OBJ_VNUM_VOODOO ) ) == NULL )
	    {
		send_to_char( "Cannot find the voodoo doll vnum.\n\r", ch );
		return;
	    }
	    obj = create_object( pObjIndex, 0 );
	    name = wch->name;
	    sprintf( buf, obj->short_descr, name );
	    free_string( obj->short_descr );
	    obj->short_descr = str_dup( buf );
	    sprintf( buf, obj->description, name );
	    free_string( obj->description );
	    obj->description = str_dup( buf );
	    sprintf( buf, obj->name, name );
	    free_string( obj->name );
	    obj->name = str_dup( buf );
	    if ( CAN_WEAR(obj, ITEM_TAKE) )
		obj_to_char( obj, ch );
	    else
		obj_to_room( obj, ch->in_room );
	    act( "$n has created $p!", ch, obj, NULL, TO_ROOM );
	    wiznet("$N loads $p.",ch,obj,WIZ_LOAD,WIZ_SECURE,get_trust(ch));
	    send_to_char( "Ok.\n\r", ch );
	    return;
	}
    }
    send_to_char("No one of that name is playing.\n\r",ch);
    return;
}

void do_purge( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[100];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    DESCRIPTOR_DATA *d;
    bool force = FALSE;

    one_argument( argument, arg );

    if ( arg[0] == '\0' || !str_cmp(arg,"force") )
    {

	/* 'purge' */
	CHAR_DATA *vnext;
	OBJ_DATA  *obj_next;

        if ( !str_cmp(arg,"force") )
            force = TRUE;

	for ( victim = ch->in_room->people; victim != NULL; victim = vnext )
	{
	    vnext = victim->next_in_room;
	    if ( IS_NPC(victim) && !(
                     IS_SET(victim->act,ACT_NOPURGE) && !force ) 
	    &&   victim != ch /* safety precaution */ )
		extract_char( victim, TRUE );
	}

	for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if (!IS_OBJ_STAT(obj,ITEM_NOPURGE) || force)
	      extract_obj( obj );
	}

	act( "$n purges the room!", ch, NULL, NULL, TO_ROOM);
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) )
    {

	if (ch == victim)
	{
	  send_to_char("Ho ho ho.\n\r",ch);
	  return;
	}

	if (get_trust(ch) <= get_trust(victim))
	{
	  send_to_char("Maybe that wasn't a good idea...\n\r",ch);
	  sprintf(buf,"%s tried to purge you!\n\r",ch->name);
	  send_to_char(buf,victim);
	  return;
	}

	if (get_trust(ch) <= DEITY)
	{
	  send_to_char("Not against PC's!\n\r",ch);
	  return;
	}

	act("$n disintegrates $N.",ch,0,victim,TO_NOTVICT);

    	if (victim->level > 1)
	    save_char_obj( victim );
    	d = victim->desc;
    	extract_char( victim, TRUE );
    	if ( d != NULL )
          close_socket( d );

	return;
    }

    act( "$n purges $N.", ch, NULL, victim, TO_NOTVICT );
    extract_char( victim, TRUE );
    return;
}



void do_advance( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int level;
    int iLevel;
    int oldlevel, count;
    int oldtrust;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char( "Syntax: advance <char> <level>.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( ( level = atoi( arg2 ) ) < 1 || level > 110 )
    {
	send_to_char( "Level must be 1 to 110.\n\r", ch );
	return;
    }

    if ( level > get_trust( ch ) )
    {
	send_to_char( "Limited to your trust level.\n\r", ch );
	return;
    }


    /*
     * Lower level:
     *   Reset to level 1.
     *   Then raise again.
     *   Currently, an imp can lower another imp.
     *   -- Swiftest
     */
    if ( level <= victim->level )
    {
        int temp_prac;

	send_to_char( "Lowering a player's level!\n\r", ch );
	send_to_char( "{R******** {GOOOOHHHHHHHHHH  NNNNOOOO {R*******{x\n\r", victim );
	sprintf(buf, "{R**** {WYou've been demoted to level %d {R****{x\n\r", level );
	send_to_char(buf, victim);
	if ((victim->level > HERO) || (level > HERO))
	{
	    update_wizlist(victim, level);
	}
	temp_prac = victim->practice;
        oldlevel	 = victim->level;
	victim->level    = 1;
	victim->exp      = exp_per_level(victim,victim->pcdata->points);
	victim->max_hit  = 100;
	victim->max_mana = 100;
	victim->max_move = 100;
	victim->practice = 0;
	victim->hit      = victim->max_hit;
	victim->mana     = victim->max_mana;
	victim->move     = victim->max_move;
	advance_level( victim, TRUE );
	victim->practice = temp_prac;

        if(level < oldlevel)
        {
            for(count = get_trust(ch); count <= MAX_LEVEL; count++)
            {
                revoke_level(ch,victim,count);
            }
        }

    }
    else
    {
	send_to_char( "Raising a player's level!\n\r", ch );
	send_to_char( "{B******* {GOOOOHHHHHHHHHH  YYYYEEEESSS {B******{x\n\r", victim );
	sprintf(buf, "{B**** {WYou've been advanced to level %d {B****{x\n\r", level );
	send_to_char(buf, victim);
	if ((victim->level > HERO) || (level > HERO))
	{
	    update_wizlist(victim, level);
	}
    }

    oldtrust = get_trust( victim );

    for ( iLevel = victim->level ; iLevel < level; iLevel++ )
    {
	victim->level += 1;
	advance_level( victim, TRUE );
    }
    victim->exp   = exp_per_level(victim,victim->pcdata->points) 
		  * UMAX( 1, victim->level );
    victim->trust = level;

    if(level >= oldlevel && oldtrust > level)
    {
	victim->trust = oldtrust;
    }

    save_char_obj(victim);
    return;
}


void do_knight( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int level;
    int iLevel;
 
    argument = one_argument( argument, arg1 );

    if (!IS_SET(ch->act, PLR_KEY))
    {
        send_to_char( "This function is not currently implemented.\n\r", ch );
        return;
    }

    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax: knight <char>.\n\r", ch );
        return;
    }
 
    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
        send_to_char( "That player is not here.\n\r", ch);
        return;
    }
 
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    level = 103;
 
    if ( level <= victim->level )
    {
	return;
    }
    else
    {
	act( "You touch $Ns shoulder with a sword called {GKnight's Faith{x.", ch,NULL,victim,TO_CHAR);
	act("$n touches your shoulder with a sword called {GKnight's Faith{x.",ch,NULL,victim,TO_VICT);
	act("$n touches $Ns shoulder with a sword called {GKnight's Faith{x.",ch,NULL,victim,TO_NOTVICT);
	act("$N glows with an unearthly light as $S mortality slips away.",ch,NULL,victim,TO_NOTVICT);
    }
    update_wizlist(victim, level);
    for ( iLevel = victim->level ; iLevel < level; iLevel++ )
    {
        send_to_char( "You raise a level!!  ", victim );
        victim->level += 1;
        advance_level( victim, TRUE );
    }
    victim->exp   = exp_per_level(victim,victim->pcdata->points) 
                  * UMAX( 1, victim->level );
    victim->trust = level;
    save_char_obj(victim);
    return;
}

void do_squire( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int level;
    int iLevel;
 
    argument = one_argument( argument, arg1 );

    if (!IS_SET(ch->act, PLR_KEY))
    {
        send_to_char( "This function is not currently implemented.\n\r", ch );
        return;
    }

    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax: squire <char>.\n\r", ch );
        return;
    }
 
    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
        send_to_char( "That player is not here.\n\r", ch);
        return;
    }
 
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    level = 102;
 
    if ( level <= victim->level )
    {
	return;
    }
    else
    {
	act( "You touch $Ns shoulder with a sword called {BSquire's Faith{x.", ch,NULL,victim,TO_CHAR);
	act("$n touches your shoulder with a sword called {BSquire's Faith{x.",ch,NULL,victim,TO_VICT);
	act("$n touches $Ns shoulder with a sword called {BSquire's Faith{x.",ch,NULL,victim,TO_NOTVICT);
	act("$N glows with an unearthly light as $S mortality slips away.",ch,NULL,victim,TO_NOTVICT);
    }
    update_wizlist(victim, level);
    for ( iLevel = victim->level ; iLevel < level; iLevel++ )
    {
        send_to_char( "You raise a level!!  ", victim );
        victim->level += 1;
        advance_level( victim, TRUE );
    }
    victim->exp   = exp_per_level(victim,victim->pcdata->points) 
                  * UMAX( 1, victim->level );
    victim->trust = level;
    save_char_obj(victim);
    return;
}



void do_trust( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int level;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char( "Syntax: trust <char> <level>.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( ( level = atoi( arg2 ) ) < 0 || level > 110 )
    {
	send_to_char( "Level must be 0 (reset) or 1 to 110.\n\r", ch );
	return;
    }

    if ( level > get_trust( ch ) )
    {
	send_to_char( "Limited to your trust.\n\r", ch );
	return;
    }

    victim->trust = level;
    return;
}



void do_restore( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *vch;
    DESCRIPTOR_DATA *d;

    one_argument( argument, arg );
    if (arg[0] == '\0' || !str_cmp(arg,"room"))
    {
    /* cure room */
    	
        for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
        {
	    if ( IS_SET(vch->act, PLR_NORESTORE) )
	    {
		act("$n attempts to restore you, but fails.",ch,NULL,vch,TO_VICT);
	    } else {
		affect_strip(vch,gsn_plague);
		affect_strip(vch,gsn_poison);
		affect_strip(vch,gsn_blindness);
		affect_strip(vch,gsn_sleep);
		affect_strip(vch,gsn_curse);
		affect_strip(vch,gsn_fire_breath);

                if(!IS_NPC(vch)) vch->pcdata->moves_this_tick = 0;

		vch->hit 	= vch->max_hit;
		vch->mana	= vch->max_mana;
		vch->move	= vch->max_move;
		update_pos( vch);
		act_new_2("$n has restored you.",ch,NULL,vch,TO_VICT,
                    POS_DEAD, 0, ACTION_GLOBAL);
	    }
        }

        sprintf(buf,"$N restored room %d.",ch->in_room->vnum);
        wiznet(buf,ch,NULL,WIZ_RESTORE,WIZ_SECURE,get_trust(ch));
        
        send_to_char("Room restored.\n\r",ch);
        return;

    }
    
    if ( get_trust(ch) >=  MAX_LEVEL - 2 && !str_cmp(arg,"all"))
    {
    /* cure all */
    	
        for (d = descriptor_list; d != NULL; d = d->next)
        {
	    victim = d->character;

	    if (victim == NULL || IS_NPC(victim))
		continue;
                
            if ( IS_SET(victim->act, PLR_NORESTORE) )  
            {
                act("$n attempts to restore you, but fails.",ch,NULL,victim,TO_VICT);          
            } else {
		affect_strip(victim,gsn_plague);
		affect_strip(victim,gsn_poison);
		affect_strip(victim,gsn_blindness);
		affect_strip(victim,gsn_sleep);
		affect_strip(victim,gsn_curse);
		affect_strip(victim,gsn_fire_breath);
                if(!IS_NPC(victim)) victim->pcdata->moves_this_tick = 0;
            
		victim->hit 	= victim->max_hit;
		victim->mana	= victim->max_mana;
		victim->move	= victim->max_move;
		update_pos( victim);
		if (victim->in_room != NULL)
		    act("$n has restored you.",ch,NULL,victim,TO_VICT);
	    }
        }
	send_to_char("All active players restored.\n\r",ch);
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->act, PLR_NORESTORE) )
    {
	act("$n attempts to restore you, but fails.",ch,NULL,victim,TO_VICT);
	send_to_char( "You failed.\n\r", ch );
	return;
    }
    affect_strip(victim,gsn_plague);
    affect_strip(victim,gsn_poison);
    affect_strip(victim,gsn_blindness);
    affect_strip(victim,gsn_sleep);
    affect_strip(victim,gsn_curse);
    if(!IS_NPC(victim)) victim->pcdata->moves_this_tick = 0;

    victim->hit  = victim->max_hit;
    victim->mana = victim->max_mana;
    victim->move = victim->max_move;
    update_pos( victim );
    act( "$n has restored you.", ch, NULL, victim, TO_VICT );
    sprintf(buf,"$N restored %s",
	IS_NPC(victim) ? victim->short_descr : victim->name);
    wiznet(buf,ch,NULL,WIZ_RESTORE,WIZ_SECURE,get_trust(ch));
    send_to_char( "Ok.\n\r", ch );
    return;
}

void do_immkiss( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if (arg[0] == '\0')
    {
	send_to_char( "Who do you want to kiss?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
    if ( ch->in_room != victim->in_room )
    {
	send_to_char( "Your lips aren't that long!\n\r", ch);
	return;
    }
    affect_strip(victim,gsn_plague);
    affect_strip(victim,gsn_poison);
    affect_strip(victim,gsn_blindness);
    affect_strip(victim,gsn_sleep);
    affect_strip(victim,gsn_curse);
    victim->hit  = victim->max_hit;
    victim->mana = victim->max_mana;
    victim->move = victim->max_move;
    update_pos( victim );
    act( "$n kisses you, and you feel a sudden rush of adrenaline.", ch, NULL, victim, TO_VICT );
    send_to_char( "You feel MUCH better now!\n\r", victim);
    send_to_char( "They feel MUCH better now!\n\r", ch);
    sprintf(buf,"$N immkissed %s",
        IS_NPC(victim) ? victim->short_descr : victim->name);
    wiznet(buf,ch,NULL,WIZ_RESTORE,WIZ_SECURE,get_trust(ch));
    return;
}

 	
void do_freeze( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Freeze whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->act, PLR_FREEZE) )
    {
	REMOVE_BIT(victim->act, PLR_FREEZE);
	send_to_char( "You can play again.\n\r", victim );
	send_to_char( "FREEZE removed.\n\r", ch );
	sprintf(buf,"$N thaws %s.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
    else
    {
	SET_BIT(victim->act, PLR_FREEZE);
	send_to_char( "You can't do ANYthing!\n\r", victim );
	send_to_char( "FREEZE set.\n\r", ch );
	sprintf(buf,"$N puts %s in the deep freeze.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }

    save_char_obj( victim );

    return;
}

void do_norestore( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
 
    one_argument( argument, arg );
 
    if ( arg[0] == '\0' )
    {
        send_to_char( "Norestore whom?\n\r", ch );
        return;
    }
       
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
       
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }
       
    if ( get_trust( victim ) >= get_trust( ch ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }
       
    if ( IS_SET(victim->act, PLR_NORESTORE) )
    {
        REMOVE_BIT(victim->act, PLR_NORESTORE);
        send_to_char( "NORESTORE removed.\n\r", ch );
        sprintf(buf,"$N allows %s restores.",victim->name);
        wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
    else
    {
        SET_BIT(victim->act, PLR_NORESTORE);
        send_to_char( "NORESTORE set.\n\r", ch );
        sprintf(buf,"$N denys %s restores.",victim->name);
        wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
       
    save_char_obj( victim );
 
    return;
}


void do_notitle( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
 
    one_argument( argument, arg );
 
    if ( arg[0] == '\0' )
    {
        send_to_char( "Notitle whom?\n\r", ch );
        return;
    }
       
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
       
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }
       
    if ( get_trust( victim ) >= get_trust( ch ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }
       
    if ( IS_SET(victim->act, PLR_NOTITLE) )
    {
        REMOVE_BIT(victim->act, PLR_NOTITLE);
        send_to_char( "NOTITLE removed.\n\r", ch );
        sprintf(buf,"$N allows %s title.",victim->name);
        wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
    else
    {
        SET_BIT(victim->act, PLR_NOTITLE);
        send_to_char( "NOTITLE set.\n\r", ch );
        sprintf(buf,"$N denys %s title.",victim->name);
        wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
       
    save_char_obj( victim );
 
    return;
}

void do_log( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Log whom?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	if ( fLogAll )
	{
	    fLogAll = FALSE;
	    send_to_char( "Log ALL off.\n\r", ch );
	}
	else
	{
	    fLogAll = TRUE;
	    send_to_char( "Log ALL on.\n\r", ch );
	}
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    /*
     * No level check, gods can log anyone.
     */
    if ( IS_SET(victim->act, PLR_LOG) )
    {
	REMOVE_BIT(victim->act, PLR_LOG);
	send_to_char( "LOG removed.\n\r", ch );
        ch->log_level = 0;
    }
    else
    {
	SET_BIT(victim->act, PLR_LOG);
	send_to_char( "LOG set.\n\r", ch );
        ch->log_level = 5;
    }

    return;
}



void do_noemote( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Noemote whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }


    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->comm, COMM_NOEMOTE) )
    {
	REMOVE_BIT(victim->comm, COMM_NOEMOTE);
	send_to_char( "You can emote again.\n\r", victim );
	send_to_char( "NOEMOTE removed.\n\r", ch );
	sprintf(buf,"$N restores emotes to %s.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
    else
    {
	SET_BIT(victim->comm, COMM_NOEMOTE);
	send_to_char( "You can't emote!\n\r", victim );
	send_to_char( "NOEMOTE set.\n\r", ch );
	sprintf(buf,"$N revokes %s's emotes.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }

    return;
}



void do_noshout( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Noshout whom?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->comm, COMM_NOSHOUT) )
    {
	REMOVE_BIT(victim->comm, COMM_NOSHOUT);
	send_to_char( "You can shout again.\n\r", victim );
	send_to_char( "NOSHOUT removed.\n\r", ch );
	sprintf(buf,"$N restores shouts to %s.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
    else
    {
	SET_BIT(victim->comm, COMM_NOSHOUT);
	send_to_char( "You can't shout!\n\r", victim );
	send_to_char( "NOSHOUT set.\n\r", ch );
	sprintf(buf,"$N revokes %s's shouts.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }

    return;
}



void do_notell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Notell whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if ( IS_SET(victim->comm, COMM_NOTELL) )
    {
	REMOVE_BIT(victim->comm, COMM_NOTELL);
	send_to_char( "You can tell again.\n\r", victim );
	send_to_char( "NOTELL removed.\n\r", ch );
	sprintf(buf,"$N restores tells to %s.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
    else
    {
	SET_BIT(victim->comm, COMM_NOTELL);
	send_to_char( "You can't tell!\n\r", victim );
	send_to_char( "NOTELL set.\n\r", ch );
	sprintf(buf,"$N revokes %s's tells.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }

    return;
}



void do_peace( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch->fighting != NULL ) {
	    stop_fighting( rch, TRUE );
	    if (!IS_NPC(rch) ) {
		send_to_char( "Ok.\n\r", ch );
	    }
	}
	if (IS_NPC(rch) && IS_SET(rch->act,ACT_AGGRESSIVE))
	    REMOVE_BIT(rch->act,ACT_AGGRESSIVE);
    }
    return;
}

void do_wizlock( CHAR_DATA *ch, char *argument )
{
    extern bool wizlock;
    wizlock = !wizlock;

    if ( wizlock )
    {
	wiznet("$N has wizlocked the game.",ch,NULL,0,0,0);
	send_to_char( "Game wizlocked.\n\r", ch );
    }
    else
    {
	wiznet("$N removes wizlock.",ch,NULL,0,0,0);
	send_to_char( "Game un-wizlocked.\n\r", ch );
    }

    return;
}

/* RT anti-newbie code */

void do_newlock( CHAR_DATA *ch, char *argument )
{
    extern bool newlock;
    newlock = !newlock;
 
    if ( newlock )
    {
	wiznet("$N locks out new characters.",ch,NULL,0,0,get_trust(ch));
        send_to_char( "New characters have been locked out.\n\r", ch );
    }
    else
    {
	wiznet("$N allows new characters back in.",ch,NULL,0,0,get_trust(ch));
        send_to_char( "Newlock removed.\n\r", ch );
    }
 
    return;
}


void do_slookup( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int sn;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Lookup which skill or spell?\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name == NULL )
		break;
	    sprintf( buf, "Sn: %3d  Slot: %3d  Skill/spell: '%s'\n\r",
		sn, skill_table[sn].slot, skill_table[sn].name );
	    send_to_char( buf, ch );
	}
    }
    else
    {
	if ( ( sn = skill_lookup( arg ) ) < 0 )
	{
	    send_to_char( "No such skill or spell.\n\r", ch );
	    return;
	}

	sprintf( buf, "Sn: %3d  Slot: %3d  Skill/spell: '%s'\n\r",
	    sn, skill_table[sn].slot, skill_table[sn].name );
	send_to_char( buf, ch );
    }

    return;
}

/* RT set replaces sset, mset, oset, and rset */

void do_set( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument(argument,arg);

    if (arg[0] == '\0')
    {
	send_to_char("Syntax:\n\r",ch);
	send_to_char("  set mob   <name> <field> <value>\n\r",ch);
	send_to_char("  set obj   <name> <field> <value>\n\r",ch);
	send_to_char("  set room  <room> <field> <value>\n\r",ch);
        send_to_char("  set skill <name> <spell or skill> <value>\n\r",ch);
        send_to_char("  set language <name> <language> <percent>\n\r",ch);
	send_to_char("  set char  <name> <field> <value>\n\r",ch);
	return;
    }

    if (!str_prefix(arg,"mobile") || !str_prefix(arg,"character"))
    {
	do_mset(ch,argument);
	return;
    }

    if (!str_prefix(arg,"skill") || !str_prefix(arg,"spell"))
    {
	do_sset(ch,argument);
	return;
    }

    if (!str_prefix(arg,"languages"))
    {
	do_langset(ch,argument);
	return;
    }

    if (!str_prefix(arg,"object"))
    {
	do_oset(ch,argument);
	return;
    }

    if (!str_prefix(arg,"room"))
    {
	do_rset(ch,argument);
	return;
    }
    /* echo syntax */
    do_set(ch,"");
}


void do_sset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int value;
    int sn;
    bool fAll;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax:\n\r",ch);
	send_to_char( "  set skill <name> <spell or skill> <value>\n\r", ch);
	send_to_char( "  set skill <name> all <value>\n\r",ch);  
	send_to_char("   (use the name of the skill, not the number)\n\r",ch);
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    fAll = !str_cmp( arg2, "all" );
    sn   = 0;
    if ( !fAll && ( sn = skill_lookup( arg2 ) ) < 0 )
    {
	send_to_char( "No such skill or spell.\n\r", ch );
	return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\n\r", ch );
	return;
    }

    value = atoi( arg3 );
    if ( value < 0 || value > 100 )
    {
	send_to_char( "Value range is 0 to 100.\n\r", ch );
	return;
    }

    if ( fAll )
    {
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name != NULL )
		victim->pcdata->learned[sn]	= value;
	}
    }
    else
    {
	victim->pcdata->learned[sn] = value;
    }

    return;
}



void do_mset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char buf[100];
    CHAR_DATA *victim;
    int value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char("Syntax:\n\r",ch);
	send_to_char("  set char <name> <field> <value>\n\r",ch); 
	send_to_char( "  Field being one of:\n\r",			ch );
	send_to_char( "    str int wis dex con sex class level\n\r",	ch );
	send_to_char( "    race group platinum gold silver hp\n\r",	ch );
	send_to_char( "    mana move prac align train thirst\n\r",	ch );
	send_to_char( "    hunger drunk full quest security\n\r",	ch );
	send_to_char( "    aquest questnext path act\n\r",		ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL 
    || (victim->level > ch->level && victim->level == MAX_LEVEL))
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    /* clear zones for mobs */
    victim->zone = NULL;

    /*
     * Snarf the value (which need not be numeric).
     */
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "str" ) )
    {
	if ( value < 3 || value > get_max_train(victim,STAT_STR) )
	{
	    sprintf(buf,
		"Strength range is 3 to %d\n\r.",
		get_max_train(victim,STAT_STR));
	    send_to_char(buf,ch);
	    return;
	}

	victim->perm_stat[STAT_STR] = value;
	return;
    }

    if ( !str_cmp( arg2, "act" ) )
    {
        if ( ( value = flag_value( act_flags, arg3 ) ) != NO_FLAG )
        {
	    if(IS_NPC(victim))
	    {
		victim->act ^= value;
		SET_BIT( victim->act, ACT_IS_NPC );
	    }
	    else
	    {
		victim->act ^= value;
                REMOVE_BIT( victim->act, ACT_IS_NPC );
	    }

            send_to_char( "Act flag toggled.\n\r", ch);
            return;
        }
    }

    if ( !str_cmp( arg2, "security" ) )	/* OLC */
    {
	if ( IS_NPC(ch) )
	{
		send_to_char( "Yes, of course.\n\r", ch );
		return;
	}

        if ( IS_NPC( victim ) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }

	if ( value > 10 || value < 0 )
	{
            sprintf( buf, "Valid security is 0-10\n\r" );
		send_to_char( buf, ch );
	    return;
	}
	victim->pcdata->security = value;
	return;
    }

    if ( !str_cmp( arg2, "int" ) )
    {
        if ( value < 3 || value > get_max_train(victim,STAT_INT) )
        {
            sprintf(buf,
		"Intelligence range is 3 to %d.\n\r",
		get_max_train(victim,STAT_INT));
            send_to_char(buf,ch);
            return;
        }
 
        victim->perm_stat[STAT_INT] = value;
        return;
    }

    if ( !str_cmp( arg2, "wis" ) )
    {
	if ( value < 3 || value > get_max_train(victim,STAT_WIS) )
	{
	    sprintf(buf,
		"Wisdom range is 3 to %d.\n\r",get_max_train(victim,STAT_WIS));
	    send_to_char( buf, ch );
	    return;
	}

	victim->perm_stat[STAT_WIS] = value;
	return;
    }

    if ( !str_cmp( arg2, "dex" ) )
    {
	if ( value < 3 || value > get_max_train(victim,STAT_DEX) )
	{
	    sprintf(buf,
		"Dexterity ranges is 3 to %d.\n\r",
		get_max_train(victim,STAT_DEX));
	    send_to_char( buf, ch );
	    return;
	}

	victim->perm_stat[STAT_DEX] = value;
	return;
    }

    if ( !str_cmp( arg2, "con" ) )
    {
	if ( value < 3 || value > get_max_train(victim,STAT_CON) )
	{
	    sprintf(buf,
		"Constitution range is 3 to %d.\n\r",
		get_max_train(victim,STAT_CON));
	    send_to_char( buf, ch );
	    return;
	}

	victim->perm_stat[STAT_CON] = value;
	return;
    }

    if ( !str_prefix( arg2, "sex" ) )
    {
	if ( value < 0 || value > 2 )
	{
	    send_to_char( "Sex range is 0 to 2.\n\r", ch );
	    return;
	}
	victim->sex = value;
	if (!IS_NPC(victim))
	    victim->pcdata->true_sex = value;
	return;
    }

    if ( !str_prefix( arg2, "class" ) )
    {
	int class;

	if (IS_NPC(victim))
	{
	    send_to_char("Mobiles have no class.\n\r",ch);
	    return;
	}

	class = class_lookup(arg3);
	if ( class == -1 )
	{
	    char buf[MAX_STRING_LENGTH];

        	strcpy( buf, "Possible classes are: " );
        	for ( class = 0; class < MAX_CLASS; class++ )
        	{
            	    if ( class > 0 )
                    	strcat( buf, " " );
            	    strcat( buf, class_table[class].name );
        	}
            strcat( buf, ".\n\r" );

	    send_to_char(buf,ch);
	    return;
	}

	victim->class = class;
	return;
    }

    if ( !str_prefix( arg2, "level" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}

	if ( value < 0 || value > get_trust(ch) )
	{
	    sprintf(buf, "Level range is 0 to %d.\n\r", get_trust(ch) );
	    send_to_char(buf,ch);
	    return;
	}
	victim->level = value;
	return;
    }

    if ( !str_prefix( arg2, "platinum" ) )
    {
	victim->platinum = value;
	return;
    }

    if ( !str_prefix( arg2, "gold" ) )
    {
	victim->gold = value;
	return;
    }

    if ( !str_prefix(arg2, "silver" ) )
    {
	victim->silver = value;
	return;
    }

    if ( !str_prefix( arg2, "hp" ) )
    {
	if ( value < -10 || value > 99999 )
	{
	    send_to_char( "Hp range is -10 to 99999 hit points.\n\r", ch );
	    return;
	}
	victim->max_hit = value;
        if (!IS_NPC(victim))
            victim->pcdata->perm_hit = value;
	return;
    }

    if ( !str_prefix( arg2, "mana" ) )
    {
	if ( value < 0 || value > 99999 )
	{
	    send_to_char( "Mana range is 0 to 99999 mana points.\n\r", ch );
	    return;
	}
	victim->max_mana = value;
        if (!IS_NPC(victim))
            victim->pcdata->perm_mana = value;
	return;
    }

    if ( !str_prefix( arg2, "move" ) )
    {
	if ( value < 0 || value > 99999 )
	{
	    send_to_char( "Move range is 0 to 99999 move points.\n\r", ch );
	    return;
	}
	victim->max_move = value;
        if (!IS_NPC(victim))
            victim->pcdata->perm_move = value;
	return;
    }

    if ( !str_prefix( arg2, "practice" ) )
    {
	if ( value < 0 || value > 750 )
	{
	    send_to_char( "Practice range is 0 to 750 sessions.\n\r", ch );
	    return;
	}
	victim->practice = value;
	return;
    }

    if ( !str_prefix( arg2, "train" ))
    {
	if (value < 0 || value > 200 )
	{
	    send_to_char("Training session range is 0 to 200 sessions.\n\r",ch);
	    return;
	}
	victim->train = value;
	return;
    }

    if ( !str_prefix( arg2, "align" ) )
    {
	if ( value < -1000 || value > 1000 )
	{
	    send_to_char( "Alignment range is -1000 to 1000.\n\r", ch );
	    return;
	}
	victim->alignment = value;
	if ( victim->pet != NULL )
	    victim->pet->alignment = victim->alignment;
	return;
    }

    if ( !str_prefix( arg2, "thirst" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < -1 || value > 100 )
	{
	    send_to_char( "Thirst range is -1 to 100.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_THIRST] = value;
	return;
    }

    if ( !str_prefix( arg2, "drunk" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < -1 || value > 100 )
	{
	    send_to_char( "Drunk range is -1 to 100.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_DRUNK] = value;
	return;
    }

    if ( !str_prefix( arg2, "full" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}

	if ( value < -1 || value > 100 )
	{
	    send_to_char( "Full range is -1 to 100.\n\r", ch );
	    return;
	}

	victim->pcdata->condition[COND_FULL] = value;
	return;
    }

    if ( !str_prefix( arg2, "hunger" ) )
    {
        if ( IS_NPC(victim) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }
 
        if ( value < -1 || value > 100 )
        {
            send_to_char( "Full range is -1 to 100.\n\r", ch );
            return;
        }
 
        victim->pcdata->condition[COND_HUNGER] = value;
        return;
    }

    if ( !str_prefix( arg2, "quest" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "NPC's don't need quest points.\n\r", ch );
	    return;
	}

	victim->qps = value;
	return;
    }

    if ( !str_prefix( arg2, "aquest" ) )
    {

        if ( IS_NPC(victim) )
        {
            send_to_char( "NPC's don't need quest points.\n\r", ch );
            return;
        }

        if ( value < 0 || value > 90000 )
        {
            send_to_char( "aquest range is 0 to 90000.\n\r", ch );
            return;
        }

        victim->pcdata->questpoints = value;
        return;
    }

    if ( !str_prefix( arg2, "questnext" ) )
    {

        if ( IS_NPC(victim) )
        {
            send_to_char( "NPC's don't go on quests.\n\r", ch );
            return;
        }

        if ( value < 0 || value > 100 )
        {
            send_to_char( "questnext range is 0 to 100.\n\r", ch );
            return;
        }

        victim->pcdata->nextquest = value;
        return;
    }

    if (!str_prefix( arg2, "race" ) )
    {
	int race;

	race = race_lookup(arg3);

	if ( race == 0)
	{
	    send_to_char("That is not a valid race.\n\r",ch);
	    return;
	}

	if (!IS_NPC(victim) && !race_table[race].pc_race)
	{
	    send_to_char("That is not a valid player race.\n\r",ch);
	    return;
	}

	victim->race = race;
	return;
    }
   
    if (!str_prefix(arg2,"group"))
    {
	if (!IS_NPC(victim))
	{
	    send_to_char("Only on NPCs.\n\r",ch);
	    return;
	}
	victim->group = value;
	return;
    }

    if (!str_prefix(arg2,"path"))
    {
	if (!IS_NPC(victim))
	{
	    send_to_char("Only on NPCs.\n\r",ch);
	    return;
	}

        free_string(victim->path);
	victim->path = str_dup(arg3);

        send_to_char("Path set.\n\r", ch);

	return;
    }


    /*
     * Generate usage message.
     */
    do_mset( ch, "" );
    return;
}

void do_string( CHAR_DATA *ch, char *argument )
{
    char type [MAX_INPUT_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_STRING_LENGTH];
    char buf [MAX_STRING_LENGTH];
    char buf2 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int cnt, plc;

    smash_tilde( argument );
    argument = one_argument( argument, type );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( type[0] == '\0' || arg1[0] == '\0' || arg2[0] == '\0')
    {
	send_to_char("Syntax:\n\r",ch);
	send_to_char("  string char <name> <field> <string>\n\r",ch);
	send_to_char("    fields: name short long title who spec\n\r",ch);
	send_to_char("  string obj  <name> <field> <string>\n\r",ch);
	send_to_char("    fields: name short long extended\n\r",ch);
	return;
    }
    
    if (!str_prefix(type,"character") || !str_prefix(type,"mobile"))
    {
    	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
    	}

	/* clear zone for mobs */
	victim->zone = NULL;

	/* string something */

	if ((victim->level >= ch->level) && (ch != victim))
	{
	    send_to_char("That will not be done.\n\r", ch);
	    return;
	}
    	
    	if ( !str_prefix( arg2, "who" ) )
    	{
	    if ( IS_NPC(victim) )
	    {
	    	send_to_char( "Not on NPC's.\n\r", ch );
	    	return;
	    }
	    if ((ch->level < CREATOR)
	    && (victim->level < HERO)
	    && (victim->class < MAX_CLASS/2))
	    {
		send_to_char( "Not on 1st tier mortals.\n\r", ch);
		return;
	    }
	    buf[0] = '\0';
	    buf2[0] = '\0';
    	    victim->pcdata->who_descr = str_dup( "" );
	    if (arg3[0] == '\0')
	    {
		return;
	    }
	    cnt = 0;
	    for(plc = 0; plc < strlen(arg3); plc++)
	    {
		if (arg3[plc] != '{')
		{
		    if (buf[0] == '\0')
		    {
			sprintf(buf2, "%c", arg3[plc]);
		    } else
		    {
			sprintf(buf2, "%s%c", buf, arg3[plc]);
		    }
		    sprintf(buf, "%s", buf2);
		    cnt++;
		} else if (arg3[plc+1] == '{')
		{
		    if (buf[0] == '\0')
		    {
			sprintf(buf2, "{{");
		    } else
		    {
			sprintf(buf2, "%s{{", buf);
		    }
		    sprintf(buf, "%s", buf2);
		    cnt++;
		    plc++;
		} else
		{
		    if (buf[0] == '\0')
		    {
			sprintf(buf2, "{%c", arg3[plc+1]);
		    } else
		    {
			sprintf(buf2, "%s{%c", buf, arg3[plc+1]);
		    }
		    sprintf(buf, "%s", buf2);
		    plc++;
		}
		if (cnt >= 14)
		{
		    plc = strlen(arg3);
		}
	    }
	    sprintf(buf2, "%s{0", buf);
	    sprintf(buf, "%s", buf2);
	    while (cnt < 14)
	    {
		sprintf(buf2, "%s ", buf);
		sprintf(buf, "%s", buf2);
		cnt++;
	    }
    	    victim->pcdata->who_descr = str_dup(buf);
	    buf[0] = '\0';
	    buf2[0] = '\0';
    	    return;
    	}

	if (arg3[0] == '\0')
	{
	    do_string(ch,"");
	    return;
	}

     	if ( !str_prefix( arg2, "name" ) )
    	{
	    if ( !IS_NPC(victim) )
	    {
	    	send_to_char( "Not on PC's.\n\r", ch );
	    	return;
	    }
	    free_string( victim->name );
	    victim->name = str_dup( arg3 );
	    return;
    	}

    	if ( !str_prefix( arg2, "short" ) )
    	{
	    free_string( victim->short_descr );
	    victim->short_descr = str_dup( arg3 );
	    return;
    	}

    	if ( !str_prefix( arg2, "long" ) )
    	{
	    free_string( victim->long_descr );
	    strcat(arg3,"\n\r");
	    victim->long_descr = str_dup( arg3 );
	    return;
    	}

    	if ( !str_prefix( arg2, "title" ) )
    	{
	    if ( IS_NPC(victim) )
	    {
	    	send_to_char( "Not on NPC's.\n\r", ch );
	    	return;
	    }

	    set_title( victim, arg3 );
	    return;
    	}

    	if ( !str_prefix( arg2, "spec" ) )
    	{
	    if ( !IS_NPC(victim) )
	    {
	    	send_to_char( "Not on PC's.\n\r", ch );
	    	return;
	    }

	    if ( ( victim->spec_fun = spec_lookup( arg3 ) ) == 0 )
	    {
	    	send_to_char( "No such spec fun.\n\r", ch );
	    	return;
	    }

	    return;
    	}
    }
    
    if (arg3[0] == '\0')
    {
	do_string(ch,"");
	return;
    }
    if (!str_prefix(type,"object"))
    {
    	/* string an obj */
    	
   	if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
    	{
	    send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
	    return;
    	}
    	if (obj->item_type == ITEM_EXIT)
	{
	    send_to_char("You cannot modify exit objects.\n\r",ch);
	    return;
	}
        if ( !str_prefix( arg2, "name" ) )
    	{
	    free_string( obj->name );
	    obj->name = str_dup( arg3 );
	    return;
    	}

    	if ( !str_prefix( arg2, "short" ) )
    	{
	    free_string( obj->short_descr );
	    obj->short_descr = str_dup( arg3 );
	    return;
    	}

    	if ( !str_prefix( arg2, "long" ) )
    	{
	    free_string( obj->description );
	    obj->description = str_dup( arg3 );
	    return;
    	}

    	if ( !str_prefix( arg2, "ed" ) || !str_prefix( arg2, "extended"))
    	{
	    EXTRA_DESCR_DATA *ed;

	    argument = one_argument( argument, arg3 );
	    if ( argument == NULL )
	    {
	    	send_to_char( "Syntax: oset <object> ed <keyword> <string>\n\r",
		    ch );
	    	return;
	    }

 	    strcat(argument,"\n\r");

	    ed = new_extra_descr();

	    ed->keyword		= str_dup( arg3     );
	    ed->description	= str_dup( argument );
	    ed->next		= obj->extra_descr;
	    obj->extra_descr	= ed;
	    return;
    	}
    }
    
    	
    /* echo bad use message */
    do_string(ch,"");
}



void do_oset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int value;
    int class;
    sh_int clan;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char("Syntax:\n\r",ch);
	send_to_char("  set obj <object> <field> <value>\n\r",ch);
	send_to_char("  Field being one of:\n\r",				ch );
	send_to_char("    value0 value1 value2 value3 value4 (v1-v4)\n\r",	ch );
	send_to_char("    level weight cost timer clan guild\n\r",		ch );
	return;
    }

    if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
	return;
    }
    if (obj->item_type == ITEM_EXIT)
    {
	send_to_char( "You cannot modify exit objects.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "clan" ) )
    {
	if (!str_prefix(arg3,"none"))
	{
	    obj->clan = 0;
	    return;
	}
	if ((clan = clan_lookup(arg3)) == 0)
	{
	    send_to_char("No such clan exists.\n\r",ch);
	    return;
	}
	obj->clan = clan;
	return;
    }
    if ( !str_prefix( arg2, "guild" ) )
    {
	if (!str_prefix(arg3,"none"))
	{
	    obj->class = 0;
	    return;
	}
	if ((class = class_lookup(arg3)) == 0)
	{
	    send_to_char("No such guild exists.\n\r",ch);
	    return;
	}
	obj->class = class;
	return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = atoi( arg3 );

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "value0" ) || !str_cmp( arg2, "v0" ) )
    {
	if (obj->item_type == ITEM_WEAPON)
	{
	    obj->value[0] = UMIN(MAX_WEAPON,value);
	    obj->value[0] = UMAX(0,obj->value[0]);
	    return;
	}
	if ((obj->item_type == ITEM_WAND)
	||  (obj->item_type == ITEM_STAFF)
	||  (obj->item_type == ITEM_POTION)
	||  (obj->item_type == ITEM_SCROLL)
	||  (obj->item_type == ITEM_PILL))
	{
	    obj->value[0] = UMIN(MAX_LEVEL,value);
	    obj->value[0] = UMAX(0,obj->value[0]);
	    return;
	}
	obj->value[0] = value;
	return;
    }

    if ( !str_cmp( arg2, "value1" ) || !str_cmp( arg2, "v1" ) )
    {
	obj->value[1] = value;
	return;
    }

    if ( !str_cmp( arg2, "value2" ) || !str_cmp( arg2, "v2" ) )
    {
	if ((obj->item_type == ITEM_FOUNTAIN)
	||  (obj->item_type == ITEM_DRINK_CON))
	{
	    obj->value[2] = UMIN(MAX_LIQUID,value);
	    obj->value[2] = UMAX(0,obj->value[2]);
	    return;
	}
	obj->value[2] = value;
	return;
    }

    if ( !str_cmp( arg2, "value3" ) || !str_cmp( arg2, "v3" ) )
    {
	if (obj->item_type == ITEM_WEAPON)
	{
	    obj->value[3] = UMIN(MAX_DAMAGE_MESSAGE,value);
	    obj->value[3] = UMAX(0,obj->value[3]);
	    return;
	}
	obj->value[3] = value;
	return;
    }

    if ( !str_cmp( arg2, "value4" ) || !str_cmp( arg2, "v4" ) )
    {
	obj->value[4] = value;
	return;
    }

    if ( !str_prefix( arg2, "extra" ) )
    {
	send_to_char( "Use the flag command instead.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "wear" ) )
    {
	send_to_char( "Use the flag command instead.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "level" ) )
    {
	if ( (get_trust( ch ) < CREATOR && (obj->pIndexData->level - 5) > value)
	&& !IS_SET(ch->act, PLR_KEY) )
	{
	    send_to_char("You may not lower an item more than 5 levels!\n\r",ch);
	    return;
	}
        if ( (get_trust( ch ) == CREATOR && (obj->pIndexData->level - 10) > value)
	&& !IS_SET(ch->act, PLR_KEY) )
        { 
            send_to_char("You may not lower an item more than 10 levels!\n\r",ch);
            return; 
        }
//	obj->level = UMIN(MAX_LEVEL,value);
//	obj->level = UMAX(0,obj->level);
	obj->level = UMAX(0,value);
	return;
    }
	
    if ( !str_prefix( arg2, "weight" ) )
    {
	obj->weight = value;
	return;
    }

    if ( !str_prefix( arg2, "cost" ) )
    {
	obj->cost = value;
	return;
    }

    if ( !str_prefix( arg2, "timer" ) )
    {
	obj->timer = value;
	return;
    }
	
    /*
     * Generate usage message.
     */
    do_oset( ch, "" );
    return;
}



void do_rset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    int value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax:\n\r",ch);
	send_to_char( "  set room <location> <field> <value>\n\r",ch);
	send_to_char( "  Field being one of:\n\r",			ch );
	send_to_char( "    sector\n\r",				ch );
	return;
    }

    if ( ( location = find_location( ch, arg1 ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }

    if (!is_room_owner(ch,location) && ch->in_room != location 
    &&  room_is_private(ch,location) && !IS_TRUSTED(ch,IMPLEMENTOR))
    {
        send_to_char("That room is private right now.\n\r",ch);
        return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\n\r", ch );
	return;
    }
    value = atoi( arg3 );

    /*
     * Set something.
     */
    if ( !str_prefix( arg2, "flags" ) )
    {
	send_to_char( "Use the flag command instead.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg2, "sector" ) )
    {
	location->sector_type	= value;
	return;
    }

    /*
     * Generate usage message.
     */
    do_rset( ch, "" );
    return;
}

void do_sockets( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *vch;
    DESCRIPTOR_DATA *d;
    char            buf  [ MAX_STRING_LENGTH ];
    char            buf2 [ MAX_STRING_LENGTH ];
    int             count;
    char *          st;
    char            s[100];
    char            idle[10];
    char            hostname[ MAX_STRING_LENGTH ];
    int		    asip = 0;
    count       = 0;
    buf[0]      = '\0';
    buf2[0]     = '\0';

    if( !strcmp(argument,"ip") )
        asip = 1;
    else if( !strcmp(argument,"ipnum") )
        asip = 2;
    else
        asip = 0;

    strcat( buf2, 
"\n\r{B[{xNum Connected-State   Login@ Idl{B]{x Player-Name  Host\n\r" );
    strcat( buf2,
"--------------------------------------------------------------------------\n\r");
    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->character && can_see( ch, d->character ) )
        {
           switch( d->connected )
           {
              case CON_PLAYING:              st = "{G    PLAYING    ";    break;
              case CON_GET_NAME:             st = "{C   Get Name    ";    break;
              case CON_GET_OLD_PASSWORD:     st = "{CGet Old Passwd ";    break;
              case CON_CONFIRM_NEW_NAME:     st = "{C Confirm Name  ";    break;
              case CON_GET_NEW_PASSWORD:     st = "{CGet New Passwd ";    break;
              case CON_CONFIRM_NEW_PASSWORD: st = "{CConfirm Passwd ";    break;
              case CON_GET_NEW_RACE:         st = "{C  Get New Race ";    break;
              case CON_GET_NEW_SEX:          st = "{C  Get New Sex  ";    break;
              case CON_GET_NEW_CLASS:        st = "{C Get New Class ";    break;
              case CON_GET_ALIGNMENT:        st = "{C Get New Align ";    break;
              case CON_DEFAULT_CHOICE:       st = "{C Choosing Cust ";    break;
              case CON_GEN_GROUPS:           st = "{C Customization ";    break;
              case CON_PICK_WEAPON:          st = "{C Picking Weapon";    break;
              case CON_READ_IMOTD:           st = "{C Reading IMOTD ";    break;
              case CON_READ_MOTD:            st = "{C  Reading MOTD ";    break;
              case CON_BREAK_CONNECT:        st = "{Y   LINKDEAD    ";    break;
	      case CON_COPYOVER_RECOVER:     st = "{Y   Copyover    ";    break;
	      case CON_INFO:		     st = "{Y  Data Output  ";    break;
	      case CON_NOTE_TO:		     st = "{BWriting Note(T)";    break;
	      case CON_NOTE_SUBJECT:	     st = "{BWriting Note(S)";    break;
	      case CON_NOTE_EXPIRE:	     st = "{BWriting Note(E)";    break;
	      case CON_NOTE_TEXT:	     st = "{BWriting Note(B)";    break;
	      case CON_NOTE_FINISH:	     st = "{BWriting Note(F)";    break;
              default:                       st = "{Y   !UNKNOWN!   ";    break;
           }

           if ( d->editor > 0 )
           {
                st = " ONLINE EDITOR ";
           }

           switch (d->editor)
           {
              case ED_AREA  : st = "{R OLC Area Edit "; break;
              case ED_ROOM  : st = "{R OLC Room Edit "; break;
              case ED_MOBILE: st = "{R OLC Mob  Edit "; break;
              case ED_OBJECT: st = "{R OLC Obj  Edit "; break;
              case ED_MPCODE: st = "{R OLC MPrg Edit "; break;
            /*case ED_HELP  : st = "{R OLC Help Edit "; break;*/
              case ED_NONE  :                         break;
           }

           count++;

           /* Format "login" value... */
           vch = d->original ? d->original : d->character;
           strftime( s, 100, "%I:%M%p", localtime( &vch->logon ) );

           if ( vch->timer > 0 )
              sprintf( idle, "%-2d", vch->timer );
           else
              sprintf( idle, "  " );

           if(asip == 0)
              sprintf(hostname,"%s",d->host);
           else if(asip == 1)
           {
              sprintf(hostname,"%d.%d.%d.%d",
                ( d->hostip >> 24 ) & 0xFF, ( d->hostip >> 16 ) & 0xFF,
                ( d->hostip >>  8 ) & 0xFF, ( d->hostip       ) & 0xFF );
           }
           else if(asip == 2)
              sprintf(hostname,"%d",d->hostip);
           else
              strcpy(hostname,"<unknown>");

           sprintf( buf,
           "{B[{R%3d{x %s{x {M%9s{x %2s{B]{x %-12s %-30.30s\n\r",
              d->descriptor,
              st,
              s,
              idle,
              ( d->original ) ? d->original->name
                              : ( d->character )  ? d->character->name
                                                  : "(None!)",
              hostname );

           strcat( buf2, buf );

        }
    }

    sprintf( buf, "\n\r%d user%s\n\r", count, count == 1 ? "" : "s" );
    strcat( buf2, buf );
    send_to_char( buf2, ch );

    send_to_char( "You may also: sockets ip, sockets ipnum\n\r", ch );

    return;
}


/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
void do_force( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Force whom to do what?\n\r", ch );
	return;
    }

    one_argument(argument,arg2);
  
    if (!str_cmp(arg2,"delete") || !str_prefix(arg2,"mob") )
    {
	send_to_char("That will NOT be done.\n\r",ch);
	return;
    }

    if (!str_cmp(arg2,"reroll"))
    {
	send_to_char("That will NOT be done.\n\r",ch);
	return;
    }

    if (!str_cmp(arg2,"grant") || !str_cmp(arg2,"revoke"))
    {
	send_to_char("That will NOT be done.\n\r",ch);
	return;
    }

    sprintf( buf, "$n forces you to '%s'.", argument );

    if ( !str_cmp( arg, "all" ) )
    {
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;

	if (get_trust(ch) < MAX_LEVEL - 3)
	{
	    send_to_char("Not at your level!\n\r",ch);
	    return;
	}

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next;

	    if ( !IS_NPC(vch) && get_trust( vch ) < get_trust( ch ) )
	    {
		act( buf, ch, NULL, vch, TO_VICT );
		interpret( vch, argument );
	    }
	}
    }
    else if (!str_cmp(arg,"players"))
    {
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;
 
        if (get_trust(ch) < MAX_LEVEL - 2)
        {
            send_to_char("Not at your level!\n\r",ch);
            return;
        }
 
        for ( vch = char_list; vch != NULL; vch = vch_next )
        {
            vch_next = vch->next;
 
            if ( !IS_NPC(vch) && get_trust( vch ) < get_trust( ch ) 
	    &&	 vch->level < LEVEL_HERO)
            {
                act( buf, ch, NULL, vch, TO_VICT );
                interpret( vch, argument );
            }
        }
    }
    else if (!str_cmp(arg,"gods"))
    {
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;
 
        if (get_trust(ch) < MAX_LEVEL - 2)
        {
            send_to_char("Not at your level!\n\r",ch);
            return;
        }
 
        for ( vch = char_list; vch != NULL; vch = vch_next )
        {
            vch_next = vch->next;
 
            if ( !IS_NPC(vch) && get_trust( vch ) < get_trust( ch )
            &&   vch->level >= LEVEL_HERO)
            {
                act( buf, ch, NULL, vch, TO_VICT );
                interpret( vch, argument );
            }
        }
    }
    else
    {
	CHAR_DATA *victim;

	if ( ( victim = get_char_world( ch, arg ) ) == NULL 
	|| (victim->level >= ch->level && victim->level == MAX_LEVEL))
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\n\r", ch );
	    return;
	}

    	if (!is_room_owner(ch,victim->in_room) 
	&&  ch->in_room != victim->in_room 
        &&  room_is_private(ch,victim->in_room) && !IS_TRUSTED(ch,IMPLEMENTOR))
    	{
            send_to_char("That character is in a private room.\n\r",ch);
            return;
        }

	if (( get_trust( victim ) >= get_trust( ch )
	&& !IS_SET(ch->act, PLR_KEY)
	&& (victim->level != MAX_LEVEL))
	|| (!IS_NPC(victim)
	&& (IS_SET(victim->act, PLR_KEY))
	&& (ch->level != MAX_LEVEL)))
	{
	    send_to_char( "Do it yourself!\n\r", ch );
	    return;
	}

	if ( !IS_NPC(victim) && get_trust(ch) < MAX_LEVEL -3)
	{
	    send_to_char("Not at your level!\n\r",ch);
	    return;
	}

	act( buf, ch, NULL, victim, TO_VICT );
	interpret( victim, argument );
    }

    send_to_char( "Ok.\n\r", ch );
    return;
}



/*
 * New routines by Dionysos.
 */
void do_invis( CHAR_DATA *ch, char *argument )
{
    int level;
    char arg[MAX_STRING_LENGTH];

    /* RT code for taking a level argument */
    one_argument( argument, arg );

    if ( arg[0] == '\0' ) 
    /* take the default path */

      if ( ch->invis_level)
      {
          ch->invis_level = 0;
          if (!IS_TRUSTED(ch,IMPLEMENTOR))
          {
              act_new_2( "$n slowly fades into existance.",
                  ch, NULL, NULL, TO_ROOM, POS_RESTING, 0, ACTION_MUST_SEE );

//	   act( "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
          }
          else 
          {
             act( "$n appears in a blinding {z{Wflash{x!", ch, NULL, NULL, TO_ROOM );
	  }
	  send_to_char( "You slowly fade back into existence.\n\r", ch );
      }
      else
      {
          if (!IS_TRUSTED(ch,IMPLEMENTOR)) 
          {

              act_new_2( "$n slowly fades into thin air.",
                  ch, NULL, NULL, TO_ROOM, POS_RESTING, 0, ACTION_MUST_SEE );

          //    act( "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
	  }
          else 
          { 
             act( "A {Wblinding white light{x envelops $n, then {z{Dvanishes{x.", ch, NULL, NULL, TO_ROOM );
          }
	  send_to_char( "You slowly vanish into thin air.\n\r", ch );
          ch->invis_level = get_trust(ch);
      }
    else
    /* do the level thing */
    {
      level = atoi(arg);
      if (level < 2 || level > get_trust(ch))
      {
	send_to_char("Invis level must be between 2 and your level.\n\r",ch);
        return;
      }
      else
      {
	  if (!IS_TRUSTED(ch,IMPLEMENTOR))
	  {
             act( "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
	  }
	  else
	  {
             act( "A {Wblinding white light{x envelops $n, then {z{Dvanishes{x.", ch, NULL, NULL, TO_ROOM );
	  }
	  send_to_char( "You slowly vanish into thin air.\n\r", ch );
          ch->reply = NULL;
          ch->invis_level = level;
      }
    }

    return;
}


void do_incognito( CHAR_DATA *ch, char *argument )
{
    int level;
    char arg[MAX_STRING_LENGTH];
 
    /* RT code for taking a level argument */
    one_argument( argument, arg );
 
    if ( arg[0] == '\0' )
    /* take the default path */
 
      if ( ch->incog_level)
      {
          ch->incog_level = 0;
          act( "$n is no longer cloaked.", ch, NULL, NULL, TO_ROOM );
          send_to_char( "You are no longer cloaked.\n\r", ch );
      }
      else
      {
          ch->incog_level = get_trust(ch);
          ch->ghost_level = 0;
          act( "$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM );
          send_to_char( "You cloak your presence.\n\r", ch );
      }
    else
    /* do the level thing */
    {
      level = atoi(arg);
      if (level < 2 || level > get_trust(ch))
      {
        send_to_char("Incog level must be between 2 and your level.\n\r",ch);
        return;
      }
      else
      {
          ch->reply = NULL;
          ch->incog_level = level;
          ch->ghost_level = 0;
          act( "$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM );
          send_to_char( "You cloak your presence.\n\r", ch );
      }
    }
 
    return;
}

void do_ghost( CHAR_DATA *ch, char *argument )
{
    int level;
    char arg[MAX_STRING_LENGTH];
 
    /* RT code for taking a level argument */
    one_argument( argument, arg );
 
    if ( arg[0] == '\0' )
    /* take the default path */
 
      if ( ch->ghost_level)
      {
          ch->ghost_level = 0;
          act( "$n steps out from the mist.", ch, NULL, NULL, TO_ROOM );
          send_to_char( "You step out from the mist.\n\r", ch );
      }
      else
      {
          ch->ghost_level = get_trust(ch);
          ch->incog_level = 0;
          act( "$n vanishes into a mist.", ch, NULL, NULL, TO_ROOM );
          send_to_char( "You vanish into a mist.\n\r", ch );
      }
    else
    /* do the level thing */
    {
      level = atoi(arg);
      if (level < 2 || level > get_trust(ch))
      {
        send_to_char("Ghost level must be between 2 and your level.\n\r",ch);
        return;
      }
      else
      {
          ch->reply = NULL;
          ch->ghost_level = level;
          ch->incog_level = 0;
          act( "$n vanishes into a mist.", ch, NULL, NULL, TO_ROOM );
          send_to_char( "You vanish into a mist.\n\r", ch );
      }
    }
 
    return;
}



void do_holylight( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if ( IS_SET(ch->act, PLR_HOLYLIGHT) )
    {
	REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Holy light mode off.\n\r", ch );
    }
    else
    {
	SET_BIT(ch->act, PLR_HOLYLIGHT);
	send_to_char( "Holy light mode on.\n\r", ch );
    }

    return;
}

/* prefix command: it will put the string typed on each line typed */

void do_prefi (CHAR_DATA *ch, char *argument)
{
    send_to_char("You cannot abbreviate the prefix command.\r\n",ch);
    return;
}

void do_prefix (CHAR_DATA *ch, char *argument)
{
    char buf[MAX_INPUT_LENGTH];

    if (argument[0] == '\0')
    {
	if (ch->prefix[0] == '\0')
	{
	    send_to_char("You have no prefix to clear.\r\n",ch);
	    return;
	}

	send_to_char("Prefix removed.\r\n",ch);
	free_string(ch->prefix);
	ch->prefix = str_dup("");
	return;
    }

    if (ch->prefix[0] != '\0')
    {
	sprintf(buf,"Prefix changed to %s.\r\n",argument);
	free_string(ch->prefix);
    }
    else
    {
	sprintf(buf,"Prefix set to %s.\r\n",argument);
    }

    ch->prefix = str_dup(argument);
}

void do_mquest (CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;

    if ( argument[0] == '\0' )
    {
        send_to_char( "Make a quest item of what?\n\r", ch );
        return;
    }
    if ( ( obj = get_obj_carry( ch, argument ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if (IS_OBJ_STAT(obj,ITEM_QUEST))
    {
	REMOVE_BIT(obj->extra_flags,ITEM_QUEST);
	act("$p is no longer a quest item.",ch,obj,NULL,TO_CHAR);
    }
    else
    {
	SET_BIT(obj->extra_flags,ITEM_QUEST);
	act("$p is now a quest item.",ch,obj,NULL,TO_CHAR);
    }

    return;
}

void do_mpoint (CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;

    if ( argument[0] == '\0' )
    {
        send_to_char( "Make a questpoint item of what?\n\r", ch );
        return;
    }
    if ( ( obj = get_obj_carry( ch, argument ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if (IS_OBJ_STAT(obj,ITEM_QUESTPOINT))
    {
	REMOVE_BIT(obj->extra_flags,ITEM_QUESTPOINT);
	act("$p is no longer a questpoint item.",ch,obj,NULL,TO_CHAR);
    }
    else
    {
	SET_BIT(obj->extra_flags,ITEM_QUESTPOINT);
	act("$p is now a questpoint item.",ch,obj,NULL,TO_CHAR);
    }

    return;
}

void do_gset (CHAR_DATA *ch, char *argument)
{
    if ( IS_NPC(ch) )
	return;

    if ( ( argument[0] == '\0' ) || !is_number( argument ) )
    {
        send_to_char( "Goto point cleared.\n\r", ch );
	ch->pcdata->recall = 0;
        return;
    }

    ch->pcdata->recall = atoi(argument);

    send_to_char( "Ok.\n\r", ch );

    return;
}

void do_wizslap( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *pRoomIndex;
    AFFECT_DATA af;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "WizSlap whom?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( victim->level >= ch->level )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
    pRoomIndex = get_random_room(victim);

    act( "$n slaps you, sending you reeling through time and space!", ch, NULL, victim, TO_VICT);
    act( "$n slaps $N, sending $M reeling through time and space!", ch, NULL, victim, TO_NOTVICT );
    act( "You send $N reeling through time and space!", ch, NULL, victim, TO_CHAR );
    char_from_room( victim );
    char_to_room( victim, pRoomIndex );
    act( "$n crashes to the ground!", victim, NULL, NULL, TO_ROOM );
    af.where     = TO_AFFECTS;
    af.type      = skill_lookup("weaken");
    af.level     = 105;
    af.duration  = 5;
    af.location  = APPLY_STR;
    af.modifier  = -1 * (105 / 5);
    af.bitvector = AFF_WEAKEN;
    affect_to_char( victim, &af );
    send_to_char( "You feel your strength slip away.\n\r", victim );
    do_look( victim, "auto" );
    return;
}

void do_pack ( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *pack;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Send a survival pack to whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( (victim->level >= 10) && (ch->level < DEMI) )
    {
	send_to_char("They don't need one at their level.\n\r", ch);
	return;
    }

    if (!can_pack(victim) )
    {
	send_to_char("They already have a survival pack.\n\r",ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char("They don't need a survival pack.\n\r",ch);
	return;
    }

    pack = create_pack();

    obj_to_char( pack, victim );

    send_to_char("Ok.\n\r", ch);
    act( "$p suddenly appears in your inventory.", ch, pack, victim, TO_VICT);
    return;
}


bool can_pack(CHAR_DATA *ch)
{
    OBJ_DATA *object;
    bool found;
 
    if ( ch->desc == NULL )
        return TRUE;
 
    if ( ch->level > HERO )
	return TRUE;

    /*
     * search the list of objects.
     */
    found = TRUE;
    for ( object = ch->carrying; object != NULL; object = object->next_content )
    {
    	if (object->pIndexData->vnum == OBJ_VNUM_SURVIVAL_PACK)
	    found = FALSE;
    }
    if (found)
	return TRUE;
 
    return FALSE;
}

void do_dupe(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH],arg2[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int pos;
    bool found = FALSE;

    if (IS_NPC(ch))
	return;

    smash_tilde( argument );

    argument = one_argument(argument,arg);
    one_argument(argument,arg2);
    
    if (arg[0] == '\0')
    {
	send_to_char("Dupe whom?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (IS_NPC(victim))
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if (arg2[0] == '\0')
    {
	if (victim->pcdata->dupes[0] == NULL)
	{
	    send_to_char("They have no dupes set.\n\r",ch);
	    return;
	}
	send_to_char("They currently have the following dupes:\n\r",ch);

	for (pos = 0; pos < MAX_DUPES; pos++)
	{
	    if (victim->pcdata->dupes[pos] == NULL)
		break;

	    sprintf(buf,"    %s\n\r",victim->pcdata->dupes[pos]);
	    send_to_char(buf,ch);
	}
	return;
    }

    for (pos = 0; pos < MAX_DUPES; pos++)
    {
	if (victim->pcdata->dupes[pos] == NULL)
	    break;

	if (!str_cmp(arg2,victim->pcdata->dupes[pos]))
	{
	    found = TRUE;
	}
    }

    if (found)
    {
	found = FALSE;
	for (pos = 0; pos < MAX_DUPES; pos++)
	{
	    if (victim->pcdata->dupes[pos] == NULL)
		break;

	    if (found)
	    {
		victim->pcdata->dupes[pos-1]		= victim->pcdata->dupes[pos];
		victim->pcdata->dupes[pos]		= NULL;
		continue;
	    }

	    if(!strcmp(arg2,victim->pcdata->dupes[pos]))
	    {
		send_to_char("Dupe removed.\n\r",ch);
		free_string(victim->pcdata->dupes[pos]);
		victim->pcdata->dupes[pos] = NULL;
		found = TRUE;
	    }
	}
	return;
    }

    for (pos = 0; pos < MAX_DUPES; pos++)
    {
	if (victim->pcdata->dupes[pos] == NULL)
	    break;
     }

     if (pos >= MAX_DUPES)
     {
	send_to_char("Sorry, they've reached the limit for dupes.\n\r",ch);
	return;
     }
  
     /* make a new dupe */
     victim->pcdata->dupes[pos]		= str_dup(arg2);
     sprintf(buf,"%s now has the dupe %s set.\n\r",victim->name,arg2);
     send_to_char(buf,ch);
}

void do_pload( CHAR_DATA *ch, char *argument )
{
  DESCRIPTOR_DATA d;
  bool isChar = FALSE;
  char name[MAX_INPUT_LENGTH];

  if (argument[0] == '\0')
  {
    send_to_char("Load who?\n\r", ch);
    return;
  }

  argument[0] = UPPER(argument[0]);
  argument = one_argument(argument, name);

  /* Dont want to load a second copy of a player who's already online! */
  if ( get_char_world( ch, name ) != NULL )
  {
    send_to_char( "That person is already connected!\n\r", ch );
    return;
  }

  isChar = load_char_obj(&d, name); /* char pfile exists? */

  if (!isChar)
  {
    send_to_char("Load Who? Are you sure? I cant seem to find them.\n\r", ch);
    return;
  }

  d.character->desc     = NULL;
  d.character->next     = char_list;
  char_list             = d.character;
  d.character->plr_next = plr_list;
  plr_list             = d.character;
  d.connected           = CON_PLAYING;
  
  reset_char(d.character);

  d.character->pcdata->socket = str_dup("");

  if ( get_trust(d.character) >= get_trust(ch) && get_trust(ch) < MAX_LEVEL )
      do_quit(d.character,"");

  /* bring player to imm */
  if ( d.character->in_room != NULL )
  {
    char_to_room( d.character, ch->in_room); /* put in room imm is in */
  }

  if (d.character->pet != NULL)
  {
    char_to_room(d.character->pet,d.character->in_room);
     act("$n has entered the game.",d.character->pet,NULL,NULL,TO_ROOM);
   }

}

void do_punload( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char who[MAX_INPUT_LENGTH];

  argument = one_argument(argument, who);

  if ( ( victim = get_char_world( ch, who ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  /** Person is legitametly logged on... was not ploaded.
   *  Or, might be linkdead.
   */
  if (victim->desc != NULL)
  {
    send_to_char("I dont think that would be a good idea...\n\r", ch);
    return;
  }

  if (victim->was_in_room != NULL) /* return player and pet to orig room */
  {
    char_to_room(victim, victim->was_in_room);
    if (victim->pet != NULL)
      char_to_room(victim->pet, victim->was_in_room);
  }

  save_char_obj(victim);
  do_quit(victim,"");

  act("$n has released $N back to the Pattern.",
       ch, NULL, victim, TO_ROOM);
}

void do_vap ( CHAR_DATA *ch, char *argument )
{
    send_to_char("If you want to VAPE, spell it out.\n\r",ch);
    return;
}

void do_vape ( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char strsave[MAX_INPUT_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;

    if ( IS_NPC(ch) || get_trust(ch) != MAX_LEVEL )
    {
        send_to_char("Yeah right.\n\r",ch);
        return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char("Syntax: vape <unlucky victim>\n\r",ch);
        return;
    }

    argument = one_argument(argument, arg);

    if( (victim = get_char_world(ch,arg)) == NULL || IS_NPC(victim) )
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }

    if ( get_trust(victim) >= get_trust(ch) )
    {
        send_to_char("Nope, can't do that.\n\r",ch);
        return;
    }

    act( "You have been {RVAPORIZED{x!", victim, NULL, NULL, TO_CHAR );


    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->connected == CON_PLAYING && d->character != victim )
        {
            if (d->character->level >= ch->level)
                send_to_char( "global> ",d->character);
            act( "{G$T has been {RVAPORIZED{x!", d->character, NULL, victim->name, TO_CHAR );
        }
    }

    if (ch->clan && !is_independent(ch))
    {
        ch->clan = 0;
        ch->rank = 0;
    }

    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( victim->name ) );
    log_f("%s was VAPED by %s", victim->name, ch->name);
    stop_fighting(victim,TRUE);
    if (victim->level > HERO)
    {
        update_wizlist(victim, 1);
    }

    do_quit(victim,"");
    unlink(strsave);

    return;    

}



void do_langset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int value;
    int sn;
    bool fAll;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax:\n\r",ch);
	send_to_char( "  set language <name> <language> <value>\n\r", ch);
	send_to_char( "  set language <name> all <value>\n\r",ch);  
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    fAll = !str_cmp( arg2, "all" );
    sn   = 0;
    if ( !fAll && ( sn = language_lookup( arg2 ) ) < 0 )
    {
	send_to_char( "No such language.\n\r", ch );
	return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\n\r", ch );
	return;
    }

    value = atoi( arg3 );
    if ( value < 0 || value > 100 )
    {
	send_to_char( "Value range is 0 to 100.\n\r", ch );
	return;
    }

    if ( fAll )
    {
	for ( sn = 0; sn < MAX_LANGUAGE; sn++ )
	{
	    if ( language_table[sn].name != NULL )
		victim->language_known[sn]	= value;
	}
    }
    else
    {
	victim->language_known[sn] = value;
    }

    return;
}


void do_award(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int amount;

    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );

    if ( arg[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Award auto-quest points to whom, how many?\n\r", ch);
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if(IS_NPC(victim)) return;

    if ( !is_number(arg2) )
        return;

    amount = atoi (arg2);
    victim->pcdata->questpoints += amount;

    send_to_char("Ok.\n\r",ch);
}

void do_rusage(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    struct rusage r;

    if(!getrusage(RUSAGE_SELF,&r))
    { /* successful query */
    
        sprintf(buf, "System Time Used:   %ld\n\r"
                     "Shared memory size: %ld\n\r"
                     "",
                     (long)(r.ru_stime.tv_sec),
                     (long)(r.ru_ixrss)
               );
        send_to_char(buf, ch);

    }

    return;
}

void do_coinflip(CHAR_DATA *ch, char *argument)
{
    bool result = number_bits(1);

    act_new_2( "You flip a coin. The coin lands $t up.",
        ch, result ? "heads" : "tails",
        NULL, TO_CHAR, POS_RESTING, 0, ACTION_MUST_SEE );

    act_new_2( "$n flips a coin. The coin lands $t up.",
        ch, result ? "heads" : "tails",
        NULL, TO_ROOM, POS_RESTING, 0, ACTION_MUST_SEE );

}

void do_repop(CHAR_DATA *ch, char *argument)
{
    char arg1[MAX_STRING_LENGTH];

    if(argument[0] == '\0')
    {
        return;
    }
    
    argument = one_argument(arg1,argument);

    if(!str_cmp(arg1, "room"))
    {
        if(!IS_BUILDER(ch,ch->in_room->area))
        {
            send_to_char("You can't do that in this area.\n\r",ch);
            return;
        }

        reset_room(ch->in_room, TRUE);
    }
}

/*
 * Coded by: Thale (Andrew Maslin)
 * Syntax: Rename <victim> <new_name>
 * Limitations: This header must be kept with this function.  In addition,
 * this file is subject to the ROM license.  The code in this file is
 * copywritten by Andrew Maslin, 1998.  If you have a "credits" help in your
 * mud, please add the name Thale to that as credit for this function.
 */
void do_renam ( CHAR_DATA *ch, char *argument )
{
    send_to_char("If you want to RENAME, spell it out.\n\r",ch);
    return;
}

void do_rename(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  FILE *fp;
  char strsave[MAX_INPUT_LENGTH];
  char *name;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  char playerfile[MAX_INPUT_LENGTH];


  if (!IS_IMMORTAL(ch))
  {
    send_to_char("You don't have the power to do that.\n\r",ch);
    return;
  }

  argument = one_argument(argument,arg1);
  argument = one_argument(argument,arg2);

  if (arg1[0] == '\0')
  {
    send_to_char("Rename who?\n\r",ch);
    return;
  }
  if (arg2[0] == '\0')
  {
    send_to_char("What should their new name be?\n\r",ch);
    return;
  }

  arg2[0] = UPPER(arg2[0]);

  if ((victim = get_char_world(ch,arg1)) == NULL)
  {
    send_to_char("They aren't connected.\n\r",ch);
    return;
  }

  if (IS_NPC(victim))
  {
    send_to_char("Use string for NPC's.\n\r",ch);
    return;
  }

  if (!check_parse_name(arg2))
  {
    sprintf(buf,"The name {c%s{x is {Rnot allowed{x.\n\r",arg2);
    send_to_char(buf,ch);
    return;
  }

  sprintf(playerfile, "%s%s", PLAYER_DIR, capitalize(arg2));
  if ((fp = fopen(playerfile, "r")) != NULL)
  {
    sprintf(buf,"There is already someone named %s.\n\r",capitalize(arg2));
    send_to_char(buf,ch);
    fclose(fp);
    return;
  }

  if ((victim->level >= ch->level) && (victim->level >= get_trust(ch))
    &&((ch->level != IMPLEMENTOR) || (get_trust(ch) != IMPLEMENTOR))
    &&(ch != victim))
  {
    send_to_char("I don't think that's a good idea.\n\r",ch);
    return;
  }

  if(IS_IMMORTAL(victim)) return;

  if (victim->position == POS_FIGHTING)
  {
    send_to_char("They are fighting right now.\n\r",ch);
    return;
  }

  name = str_dup(victim->name);
  sprintf( strsave, "%s%s", PLAYER_DIR, capitalize(victim->name));
  arg2[0] = UPPER(arg2[0]);
  free_string(victim->name);
  victim->name = str_dup(arg2);
  save_char_obj(victim);
  unlink(strsave);

#if defined(unix)
  if (IS_IMMORTAL(victim))
  {
    sprintf(strsave,"%s%s", GOD_DIR, capitalize(name));
    unlink(strsave);
  }
#endif
  if (victim != ch)
  {
    sprintf(buf,"{YNOTICE: {xYou have been renamed to {c%s{x.\n\r",arg2);
    send_to_char(buf,victim);
  }
  send_to_char("Done.\n\r",ch);

return;
}

void do_nopk( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
 
    one_argument( argument, arg );
 
    if ( arg[0] == '\0' )
    {
        send_to_char( "NoPK whom?\n\r", ch );
        return;
    }
       
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
       
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }
       
    if ( get_trust( victim ) >= get_trust( ch ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }
       
    if ( IS_SET(victim->plt, PLT_NO_PK) )
    {
        REMOVE_BIT(victim->plt, PLT_NO_PK);
        send_to_char( "NOPK removed.\n\r", ch );
    }
    else
    {
        SET_BIT(victim->plt, PLT_NO_PK);
        send_to_char( "NOPK set.\n\r", ch );
    }
       
    save_char_obj( victim );
 
    return;
}

void do_lastcomm(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int delta;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if(d->connected != CON_PLAYING) continue;
        if(IS_NPC(d->character)) continue;
        if(d == ch->desc) continue;
        if(get_trust(d->character) >= get_trust(ch)) continue;
        if(d->character->pcdata->last_command_timestamp < 1) continue;

        delta = current_time - d->character->pcdata->last_command_timestamp;
      
        sprintf(buf,"%15s    %3d seconds ago\n\r", d->character->name,
            delta);
        send_to_char(buf, ch);
    }
}

/* **************************************************************
November 17th, 1997
Security manager version 1.1
  1997 Copyright <* FREEWARE *>
  If you use this code:
     1.Keep all the credits in the code.
     2.Use the help entry for bank
     3.Send a bug report,any comments or ideas
       Ian McCormick (aka Gothar)
       mcco0055@algonquinc.on.ca
  What this code does:
   Here is my version of the security changer for OLC users.
   -> I removed all my little extras.

A few things you can add:
1. Place the security in the player score board.
2. Make an automatic security rating change when an Immortal is created.
3. Better error checking :p

Gothar. 
************************************************************** */

void do_security( CHAR_DATA *ch, char *argument )
{
   /*
    *  Gothar Security Manager
    *   November 16, 1997.	
    */	
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char buf[100];
   CHAR_DATA *victim;
   int value;
   
   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   if( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char("Syntax:\n\r",ch);
      send_to_char("Security <char name> <value>\n\r",ch);
      send_to_char("Security value 0 >> Player\n\r",ch);      
      send_to_char("Security value 1 >> Builder\n\r",ch);
      send_to_char("Security value 5 >> Immortal\n\r",ch);
      send_to_char("Security value 9 >> Implementor\n\r",ch);
      return;
   }
   if( (victim = get_char_world(ch, arg1)) == NULL )
   {
      send_to_char("They aren't here.\n\r",ch);
      return;
   }
   /* clear zones for mobs */
   victim->zone = NULL;
   value = is_number( arg2 ) ? atoi( arg2 ) : 0;
   /*
    *  Set security level
    */   
    if( IS_NPC(victim) )
    {
       send_to_char("NPC's lack security now!",ch);
       return;
    }

   /*  No security above your means
    *  can be altered.
    */ 
    if(victim->level > ch->level)       
    {
       /* 
        *  level check  => I have stuff
	*   to keep lower level imms in check
        */
       send_to_char("At your level try again!\n\r",ch);
    }   
    if( value > ch->pcdata->security && ch->level < IMPLEMENTOR )  
    {
       /* 
        *  security level check and to
	*   see if not an IMPLEMENTOR
        */
       sprintf(buf, "Your level %d security isn't high enough\n\r"
                    ,ch->pcdata->security);
       send_to_char( buf, ch );
       return;             
    }
    else
    {
       /* 
	*   change the security and inform
        *   the player of new status
        */
       victim->pcdata->security = value;
       send_to_char("Your security rating has been changed.\n\r", victim);	
       sprintf(buf,"%s you changed %s's security rating.\n\r",ch->name,victim->name);
       send_to_char( buf, ch );
    }
    return;
    
}

void do_addlag(CHAR_DATA *ch, char *argument)
{

	CHAR_DATA *victim;
	char arg1[MAX_STRING_LENGTH];
	int x;

	argument = one_argument(argument, arg1);

	if (arg1[0] == '\0')
	{
		send_to_char("addlag to who?", ch);
		return;
	}

	if ((victim = get_char_world(ch, arg1)) == NULL)
	{
		send_to_char("They're not here.", ch);
		return;
	}

	if ((x = atoi(argument)) <= 0)
	{
		send_to_char("That makes a LOT of sense.", ch);
		return;
	}

	if (x > 100)
	{
		send_to_char("There's a limit to cruel and unusual punishment", ch);
		return;
	}

	send_to_char("Somebody REALLY didn't like you\n\r", victim);
	WAIT_STATE(victim, x);
	send_to_char("Adding lag now...\n\r", ch);
	return;
}

void do_avatar( CHAR_DATA *ch, char *argument ) /* Procedure Avatar */
{  /* Declaration */
    char buf[MAX_STRING_LENGTH];	/* buf */
    char arg1[MAX_INPUT_LENGTH];	/* arg1 */
//    OBJ_DATA *obj_next;			/* obj data which is a pointer */
//    OBJ_DATA *obj;			/* obj */
    int level;				/* level */
    int iLevel;				/* ilevel */
    int temp_trust;

    argument = one_argument( argument, arg1 );
/* Check statements */
    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
        send_to_char( "Syntax: avatar <level>.\n\r", ch );
        return;
    }

    if ( IS_NPC(ch) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( ( level = atoi( arg1 ) ) < 1 || level > MAX_LEVEL )
    {
        sprintf(buf, "Level must be 1 to %d.\n\r", MAX_LEVEL );
        send_to_char( buf, ch );
        return;
    }

    if ( level > get_trust( ch ) )
    {
        send_to_char( "Limited to your trust level.\n\r", ch );
        sprintf(buf, "Your Trust is %d.\n\r",get_trust(ch));
        send_to_char(buf,ch);
        return;
    }
/* Your trust stays so you will have all immortal command */
        /* SO their trust stays there */
     if(ch->trust == 0)
     {
        ch->trust = ch->level;
     }
     temp_trust = ch->trust;
/* Level gains*/
     if ( level <= ch->level )
     {
        int temp_prac;

        send_to_char( "Lowering a player's level!\n\r", ch );
        send_to_char( "{R******** {GOOOOHHHHHHHHHH  NNNNOOOO {R*******{x\n\r",ch );
        temp_prac = ch->practice;
        ch->level    = 1;
        ch->exp      = exp_per_level(ch,ch->pcdata->points);
        ch->max_hit  = 20;
        ch->max_mana = 100;
        ch->max_move = 100;
        ch->practice = 0;
        ch->hit      = ch->max_hit;
        ch->mana     = ch->max_mana;
        ch->move     = ch->max_move;
        advance_level( ch, TRUE );
        ch->practice = temp_prac;
    }
    else
    {
        send_to_char( "Raising a player's level!\n\r", ch );
        send_to_char( "{B******* {GOOOOHHHHHHHHHH  YYYYEEEESSS {B******{x\n\r", ch);
    }

    for ( iLevel = ch->level ; iLevel < level; iLevel++ )
    {
        ch->level += 1;
        advance_level( ch,TRUE);
    }
    sprintf(buf,"You are now level %d.\n\r",ch->level);
    send_to_char(buf,ch);
    ch->exp = exp_per_level(ch,ch->pcdata->points)
                  * UMAX( 1, ch->level );
    ch->trust = temp_trust;
/* Forces the person to remove all the eq....  so level restriction still apply */
/*   if(ch->level < 102)
   {
        for (obj = ch->carrying; obj; obj = obj_next)
            obj_next = obj->next_content;


        if (obj->wear_loc != WEAR_NONE && can_see_obj (ch, obj))
        {
            remove_obj (ch, obj->wear_loc, TRUE);
        }
    }
*/
    save_char_obj(ch); /* save character */
    return;
}

void display_roomcheck_syntax(CHAR_DATA *ch)
{
  send_to_char("SYNTAX:\n",ch);
  send_to_char("roomcheck <command>\n\r",ch);
  send_to_char("    white   - Check Room names for white.\n\r",ch);
  send_to_char("    link    - Check Room's for links.\n\r",ch);
  send_to_char("    private - Check Room for Private w/o SAFE.\n\r",ch);
  send_to_char("    descr   - Check Room for BLANK descriptions.\n\r",ch);
  send_to_char("    delete  - Check DELETED status on rooms.\n\r",ch);
}

void do_roomcheck( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_INPUT_LENGTH];
  BUFFER *buffer;
  char arg1[MIL];
  int first = TRUE;
  OBJ_INDEX_DATA *pObjIndex;
  AREA_DATA *area;
  ROOM_INDEX_DATA *room, *room2;
  int number = 0, max_found = 100, vnum=0, tvnum=0, exit = 0;

  argument = one_argument( argument, arg1 );

  if (!str_cmp(arg1,"\0")) {
    display_roomcheck_syntax(ch);
    return;
  }

  buffer = new_buf();
  for ( area = area_first; area; area = area->next )
    {
      for ( vnum = area->min_vnum; vnum <= area->max_vnum; vnum++ )
	{
	  if ( !( room = get_room_index( vnum ) ) )
	    continue;
	  if (!strcmp(arg1,"white")) {
	    if ( strstr( room->name,"{W" ) )
	      continue;

	    ++number; /*count it if we found a match */
 
	    sprintf( buf, "%3d) [%5d] %s (%s)\n\r",
		     number, vnum, room->name, area->name );
	    add_buf( buffer, buf );

	    if ( number >= max_found )
	      break;
	  } else if (!strcmp(arg1,"private")) {
	    if ( !IS_SET(room->room_flags, ROOM_PRIVATE) )
	      continue;
	    if ( IS_SET(room->room_flags, ROOM_SAFE) )
	      continue;

	    ++number; /*count it if we found a match */
 
	    sprintf( buf, "%3d) [%5d] %s (%s)\n\r",
		     number, vnum, room->name, area->name );
	    add_buf( buffer, buf );

	    if ( number >= max_found )
	      break;
	  } else if (!strcmp(arg1,"delete")) {
	    if (!is_name("delete",room->name))
	      continue;
	    
	    ++number; /*count it if we found a match */
 
	    sprintf( buf, "%3d) [%5d] %s (%s)\n\r",
		     number, vnum, room->name, area->name );
	    add_buf( buffer, buf );

	    if ( number >= max_found )
	      break;
	  } else if (!strcmp(arg1,"descr")) {
	    if (room->description[0] != '\0') 
	      continue;

	    ++number; /*count it if we found a match */
	    sprintf( buf, "%3d) [%5d] %s (%s)\n\r",
		     number, vnum, room->name, area->name );
	    add_buf( buffer, buf );

	    if ( number >= max_found )
	      break;
	  } else if (!strcmp(arg1, "link")) {

	    if (!first) {
	      /* Clear the Linked Flag */
	      for(tvnum=0; tvnum<=top_vnum_room; tvnum++) 
		{
		  if( (room2 = get_room_index(tvnum)) == NULL)
		    continue;
		  room2->linked = FALSE;
		}

	      /* Find all the rooms each room is linked and set the flag */
	      for(tvnum=0; tvnum<=top_vnum_room; tvnum++) 
		{
      
		  if( (room2 = get_room_index(tvnum)) == NULL)
		    continue;
		  for(exit=0; exit<MAX_EXITS; exit++) {
		    if( room2->exit[exit]) {
		      room2->linked = TRUE;
		      room2->exit[exit]->u1.to_room->linked = TRUE;
		    }
		  }
		}
	      /* Find all the portal endings from the objects */
	      for (tvnum = 0; tvnum < top_obj_index; tvnum++)
		if ((pObjIndex = get_obj_index(tvnum)) != NULL)
		  {
		    if (pObjIndex->item_type != ITEM_PORTAL)
		      continue;
		    room2 = get_room_index(pObjIndex->value[3]);
		    if (room2)
		      room2->linked = TRUE;
		  }
	 
	    
	      /* Find all the pet storage rooms */
	      for(tvnum=0; tvnum<=top_vnum_room; tvnum++) 
		{
      
		  if( (room2 = get_room_index(tvnum)) == NULL)
		    continue;

		  if ( !IS_SET(room2->service_flags, SERVICE_PET_SHOP) )
		    continue;
      
		  if( (room2 = get_room_index(tvnum+1)) == NULL)
		    continue;
      
		  room2->linked = TRUE;
		}
	      first = FALSE;
	    }

	    if (room->linked)
	      continue;
	    ++number; /*count it if we found a match */
	    sprintf( buf, "%3d) [%5d] %s (%s)\n\r",
		     number, vnum, room->name, area->name );
	    add_buf( buffer, buf );

	    if ( number >= max_found )
	      break;
	  
	  }else {
	    display_roomcheck_syntax(ch);
	    return;
	  }
	  if ( number >= max_found )
	    break;
	}
    }
  if ( !number )
    send_to_char( "No matching criteria.\n\r", ch );
  else
    page_to_char(buf_string(buffer),ch);
    
  free_buf(buffer);

}

void do_arealinks(CHAR_DATA *ch, char *argument)
{
    FILE *fp;
    BUFFER *buffer;
    AREA_DATA *parea;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *to_room;
    ROOM_INDEX_DATA *from_room;
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int vnum = 0;
    int iHash, door;
    bool found = FALSE;

    /* To provide a convenient way to translate door numbers to words */
    static char * const dir_name[] = {"north","east","south","west","up","down"};

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    /* First, the 'all' option */
    if (!str_cmp(arg1,"all"))
    {
	/*
	 * If a filename was provided, try to open it for writing
	 * If that fails, just spit output to the screen.
	 */
	if (arg2[0] != '\0')
	{
	    fclose(fpReserve);
	    if( (fp = fopen(arg2, "w")) == NULL)
	    {
		send_to_char("Error opening file, printing to screen.\n\r",ch);
		fclose(fp);
		fpReserve = fopen(NULL_FILE, "r");
		fp = NULL;
	    }
	}
	else
	    fp = NULL;

	/* Open a buffer if it's to be output to the screen */
	if (!fp)
	    buffer = new_buf();

	/* Loop through all the areas */
	for (parea = area_first; parea != NULL; parea = parea->next)
	{
	    /* First things, add area name  and vnums to the buffer */
	    sprintf(buf, "*** %s (%d to %d) ***\n\r",
			 parea->name, parea->min_vnum, parea->max_vnum);
	    fp ? fprintf(fp, buf) : add_buf(buffer, buf);

	    /* Now let's start looping through all the rooms. */
	    found = FALSE;
	    for(iHash = 0; iHash < MAX_KEY_HASH; iHash++)
	    {
		for( from_room = room_index_hash[iHash];
		     from_room != NULL;
		     from_room = from_room->next )
		{
		    /*
		     * If the room isn't in the current area,
		     * then skip it, not interested.
		     */
		    if ( from_room->vnum < parea->min_vnum
		    ||   from_room->vnum > parea->max_vnum )
			continue;

		    /* Aha, room is in the area, lets check all directions */
		    for (door = 0; door < 5; door++)
		    {
			/* Does an exit exist in this direction? */
			if( (pexit = from_room->exit[door]) != NULL )
			{
			    to_room = pexit->u1.to_room;

			    /*
			     * If the exit links to a different area
			     * then add it to the buffer/file
			     */
			    if( to_room != NULL
			    &&  (to_room->vnum < parea->min_vnum
			    ||   to_room->vnum > parea->max_vnum) )
			    {
				found = TRUE;
				sprintf(buf, "    (%d) links %s to %s (%d)\n\r",
				    from_room->vnum, dir_name[door],
				    to_room->area->name, to_room->vnum);

				/* Add to either buffer or file */
				if(fp == NULL)
				    add_buf(buffer, buf);
				else
				    fprintf(fp, buf);
			    }
			}
		    }
		}
	    }

	    /* Informative message for areas with no external links */
	    if (!found)
		add_buf(buffer, "    No links to other areas found.\n\r");
	}

	/* Send the buffer to the player */
	if (!fp)
	{
	    page_to_char(buf_string(buffer), ch);
	    free_buf(buffer);
	}
	/* Or just clean up file stuff */
	else
	{
	    fclose(fp);
	    fpReserve = fopen(NULL_FILE, "r");
	}

	return;
    }

    /* No argument, let's grab the char's current area */
    if(arg1[0] == '\0')
    {
	parea = ch->in_room ? ch->in_room->area : NULL;

	/* In case something wierd is going on, bail */
	if (parea == NULL)
	{
	    send_to_char("You aren't in an area right now, funky.\n\r",ch);
	    return;
	}
    }
    /* Room vnum provided, so lets go find the area it belongs to */
    else if(is_number(arg1))
    {
	vnum = atoi(arg1);

	/* Hah! No funny vnums! I saw you trying to break it... */
	if (vnum <= 0 || vnum > 65536)
	{
	    send_to_char("The vnum must be between 1 and 65536.\n\r",ch);
	    return;
	}

	/* Search the areas for the appropriate vnum range */
	for (parea = area_first; parea != NULL; parea = parea->next)
	{
	    if(vnum >= parea->min_vnum && vnum <= parea->max_vnum)
		break;
	}

	/* Whoops, vnum not contained in any area */
	if (parea == NULL)
	{
	    send_to_char("There is no area containing that vnum.\n\r",ch);
	    return;
	}
    }
    /* Non-number argument, must be trying for an area name */
    else
    {
	/* Loop the areas, compare the name to argument */
	for(parea = area_first; parea != NULL; parea = parea->next)
	{
	    if(!str_prefix(arg1, parea->name))
		break;
	}

	/* Sorry chum, you picked a goofy name */
	if (parea == NULL)
	{
	    send_to_char("There is no such area.\n\r",ch);
	    return;
	}
    }

    /* Just like in all, trying to fix up the file if provided */
    if (arg2[0] != '\0')
    {
	fclose(fpReserve);
	if( (fp = fopen(arg2, "w")) == NULL)
	{
	    send_to_char("Error opening file, printing to screen.\n\r",ch);
	    fclose(fp);
	    fpReserve = fopen(NULL_FILE, "r");
	    fp = NULL;
	}
    }
    else
	fp = NULL;

    /* And we loop the rooms */
    for(iHash = 0; iHash < MAX_KEY_HASH; iHash++)
    {
	for( from_room = room_index_hash[iHash];
	     from_room != NULL;
	     from_room = from_room->next )
	{
	    /* Gotta make sure the room belongs to the desired area */
	    if ( from_room->vnum < parea->min_vnum
	    ||   from_room->vnum > parea->max_vnum )
		continue;

	    /* Room's good, let's check all the directions for exits */
	    for (door = 0; door < 5; door++)
	    {
		if( (pexit = from_room->exit[door]) != NULL )
		{
		    to_room = pexit->u1.to_room;

		    /* Found an exit, does it lead to a different area? */
		    if( to_room != NULL
		    &&  (to_room->vnum < parea->min_vnum
		    ||   to_room->vnum > parea->max_vnum) )
		    {
			found = TRUE;
			sprintf(buf, "%s (%d) links %s to %s (%d)\n\r",
				    parea->name, from_room->vnum, dir_name[door],
				    to_room->area->name, to_room->vnum);

			/* File or buffer output? */
			if(fp == NULL)
			    send_to_char(buf, ch);
			else
			    fprintf(fp, buf);
		    }
		}
	    }
	}
    }

    /* Informative message telling you it's not externally linked */
    if(!found)
    {
	send_to_char("No links to other areas found.\n\r",ch);
	/* Let's just delete the file if no links found */
	if (fp)
	    unlink(arg2);
	return;
    }

    /* Close up and clean up file stuff */
    if(fp)
    {
	fclose(fp);
	fpReserve = fopen(NULL_FILE, "r");
    }

}

void do_wrlist( CHAR_DATA *ch, char *argument )
{
  ROOM_INDEX_DATA *room;
  ROOM_INDEX_DATA *in_room;
  MOB_INDEX_DATA *mob;
  OBJ_INDEX_DATA *obj;
  char arg[MAX_STRING_LENGTH];
  char arg1[MAX_STRING_LENGTH];
  char arg2[MAX_STRING_LENGTH];
  int uvnum;
  int lvnum;
  int MR = 60000;
  int type = -1;
  
  argument = one_argument( argument, arg );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  uvnum = ( is_number( arg2 ) ) ? atoi( arg2 ) : 0;
  lvnum = ( is_number( arg1 ) ) ? atoi( arg1 ) : 0;  

  if ( !str_cmp( arg, "o" ) )
    type = 2;
  if ( !str_cmp( arg, "m" ) )
    type = 1;
  if ( !str_cmp( arg, "r" ) )
    type = 0;
  if ( !str_cmp( arg, "p" ) )
    type = 3;

  if ( ( uvnum - lvnum ) > 200 )
  {
    send_to_char( "{WThat range is too large.\n\r{x", ch );
    return;
  }
  
  if ( ( ( uvnum == 0 ) && ( lvnum == 0 ) ) || ( arg[0] == '\0' ) 
   || ( type == -1 ) )
  {
    send_to_char( "{MSyntax: wrlist [type] [lvnum] [uvnum]\n\r{x", ch );
    return;
  }

  if ( uvnum > MR || uvnum < 1 || lvnum > MR || lvnum < 1 || lvnum > uvnum )
  {
    send_to_char( "{WInvalid level(s).\n\r{x", ch );
    return;
  }

  in_room = ch->in_room;  
  if ( type == 0 )
  {
    char_from_room( ch );
  }
  for ( MR = lvnum; MR <= uvnum; MR++ )
  {
    if ( type == 0 )
    {
      if ( ( room = get_room_index( MR ) ) )
      {
        sprintf( log_buf, "{R%-5d  {w%-20s\n\r", room->vnum, room->name );
        send_to_char( log_buf, ch );
        char_to_room( ch, room );
        do_resets( ch, "" );
        char_from_room( ch );
      }
    }
    if ( type == 2 )
    {
      if ( ( obj = get_obj_index( MR ) ) )
      {
        sprintf( log_buf, "{R%-5d  {w%-20s\n\r",  obj->vnum, obj->name );
        send_to_char( log_buf, ch );
      }
    }
    if ( type == 1 )
    {
      if ( ( mob = get_mob_index( MR ) ) )
      {
        sprintf( log_buf, "{R%-5d  {w%-20s\n\r", mob->vnum, mob->player_name );
        send_to_char( log_buf, ch );
      }
    }
    if ( type == 3 )
    {
      if ( ( mob = get_mob_index( MR ) ) )
      {
      }
  }  
  }
  if ( type == 0 )
    char_to_room( ch, in_room );
  return;
 }

extern AREA_DATA* area_first; /* snarf the local fro db.c */
typedef enum {on_ground, inside, on_mob} where;
typedef struct obj_reset OBJ_RESET;

DECLARE_DO_FUN(do_help);

#define MAX_NUMBER_APP   (sizeof(number_app) / sizeof (number_app[0]))
#define MAX_EXTRA_FLAG (sizeof(flag_names) / sizeof (flag_names[0]))

struct obj_reset
{
	sh_int vnum;
	sh_int level;
	OBJ_RESET* next;
};

struct extra_flag_names
{
	char name;
	int value;
};

/* applys where value matters. They are displayed in exactly this order: */
const sh_int number_app [] =
{
	APPLY_HITROLL,
	APPLY_DAMROLL,
	APPLY_HIT,
	APPLY_MANA,
	APPLY_MOVE,
	APPLY_AC,
	APPLY_STR,
	APPLY_DEX,
	APPLY_INT,
	APPLY_WIS,
	APPLY_CON,
};

/* extra flags and corresponding characters in the flag list */
const struct extra_flag_names flag_names [] =
{
	/* important flags, in uppercase */
	{ 'G', ITEM_ANTI_GOOD},
	{ 'E', ITEM_ANTI_EVIL},
	{ 'N', ITEM_ANTI_NEUTRAL},
	{ 'V', ITEM_INVIS},
	{ 'M', ITEM_MAGIC},
	{ 'D', ITEM_NODROP},
	{ 'R', ITEM_NOREMOVE},
	/* less important flags, in lowercase */
	{ 'g', ITEM_GLOW },
	{ 'h', ITEM_HUM  },
	{ 'd', ITEM_DARK }, /* Diku? */
	{ 'l', ITEM_LOCK },
	{ 'v', ITEM_EVIL },
	{ 'b', ITEM_BLESS},
	{ 'P', ITEM_NOPURGE},
	{ 'V', ITEM_VIS_DEATH},
	{ 'R', ITEM_ROT_DEATH},
	{ 'L', ITEM_NOLOCATE},
	{ 'S', ITEM_SELL_EXTRACT},
	{ 'B', ITEM_BURN_PROOF},
	{ 'U', ITEM_NOUNCURSE}
	
};

/* return other information relevant to your MUD */
char * get_other_information (OBJ_INDEX_DATA *obj)
{
	return "";
}
	

/* manipulation of a small list to hold vnums & levels they were reset at
   to avoid redundancy */   
void dispose_list (OBJ_RESET *list)
{
	OBJ_RESET* next;
	
	for ( ; list != NULL; list = next)
	{
		next = list->next;
		free (list);
	}
}

OBJ_RESET * new_list (void) /* create a new list */
{
	OBJ_RESET * list;
	/* create an empty list head */	
	list = (OBJ_RESET*) malloc (sizeof(OBJ_RESET));	
	list->next = NULL;	
	return list;	
}

void add_to_list (OBJ_RESET *list, sh_int vnum, sh_int level)
{
	OBJ_RESET * new_item;
	new_item = (OBJ_RESET*) malloc (sizeof(OBJ_RESET));
	new_item->next = list->next;
	list->next = new_item;
	new_item->vnum = vnum;
	new_item->level = level;
}

bool is_in_list (OBJ_RESET *list, sh_int vnum, sh_int level)
{
	for (list = list->next ; list != NULL; list = list->next)
		if ((list->vnum == vnum) && (list->level == level))
			return TRUE;
	return FALSE;
}


/* standard item_type_names accepts only OBJ_DATA* */
char *item_index_type_name (sh_int item_type)
{
	switch (item_type)
	{
    case ITEM_LIGHT:		return "light";
    case ITEM_SCROLL:		return "scroll";
    case ITEM_WAND:			return "wand";
    case ITEM_STAFF:		return "staff";
    case ITEM_WEAPON:		return "weapon";
    case ITEM_TREASURE:		return "treasure";
    case ITEM_ARMOR:		return "armor";
    case ITEM_POTION:		return "potion";
    case ITEM_FURNITURE:	return "furniture";
    case ITEM_TRASH:		return "trash";
    case ITEM_CONTAINER:	return "container";
    case ITEM_DRINK_CON:	return "drink container";
    case ITEM_KEY:			return "key";
    case ITEM_FOOD:			return "food";
    case ITEM_MONEY:		return "money";
    case ITEM_BOAT:			return "boat";
    case ITEM_CORPSE_NPC:	return "npc corpse";
    case ITEM_CORPSE_PC:	return "pc corpse";
    case ITEM_FOUNTAIN:		return "fountain";
    case ITEM_PILL:			return "pill";
    case ITEM_PORTAL:		return "portal";
    case ITEM_CLOTHING:		return "clothing";
    case ITEM_MAP:			return "map";
    case ITEM_WARP_STONE:	return "warpstone";
    case ITEM_ROOM_KEY:		return "room key";
    case ITEM_GEM:			return "gem";
    case ITEM_JEWELRY:		return "jewelry";
    case ITEM_JUKEBOX:		return "jukebox";
    
    }

    bug( "Item_type_name: unknown type %d.", item_type );
    return "(unknown)";
}

/* how much affect_num is there in pAffectr */
int get_affect_total (AFFECT_DATA* pAffect, int affect_num)
{
	int total = 0;
	
	for ( ; pAffect != NULL; pAffect = pAffect->next)
		if (pAffect->location == affect_num)
			total = total + pAffect->modifier;
			
	return total;
}

/* is affect_num contained in pAffect somewhere ? */
bool is_affect (AFFECT_DATA* pAffect, int affect_num)
{
	for ( ; pAffect != NULL; pAffect = pAffect->next)
		if (pAffect->location == affect_num)
			return TRUE;
	
	return FALSE;
}

/* shows all the extra flags like imm(dam_type), res(dam_type), DetInvis, DetMagic etc. */
char * get_bit_flags (AFFECT_DATA* pAffect)
{
	static char buffer[512];
	static char buf2[512];
	
	strcpy (buffer, "");
	
	for ( ; pAffect != NULL; pAffect = pAffect->next)
	{
		if (pAffect->bitvector)
		{
		switch (pAffect->where)
		{
        case TO_AFFECTS:
        	sprintf (buf2,"%s", affect_bit_name(pAffect->bitvector));
        	break;
        case TO_OBJECT:
            sprintf(buf2,"obj(%s)",
                     extra_bit_name(pAffect->bitvector));
            break;
        case TO_IMMUNE:
            sprintf(buf2,"imm(%s)",
                     imm_bit_name(pAffect->bitvector));
            break;
        case TO_RESIST:
            sprintf(buf2,"res(%s)",
                     imm_bit_name(pAffect->bitvector));
            break;
        case TO_VULN:
            sprintf(buf2,"vul(%s)",
                     imm_bit_name(pAffect->bitvector));
            break;
        default:
            sprintf(buf2,"unknown(%d,%d)",
                     pAffect->where,pAffect->bitvector);
            break;
		
		} /* switch */

		strcat (buffer,buf2);
		strcat (buffer, ",");
		} /* if */
	} /* for */
	
	if (strlen(buffer))
		buffer[strlen(buffer)-1] = '\0'; /* cut off the trailing comma */
	

	return buffer;		
}

char * get_damage_string (OBJ_INDEX_DATA* obj, int level)
{
	static char buffer[10];
	
	if (obj->new_format) /* new format: damage given in file */
		sprintf (buffer, "%dd%d", obj->value[1], obj->value[2]);
	else
		sprintf (buffer, "%d-%d", 1 * level/4 +2, 3*level/4 + 6);
	
	return buffer;
}


/* returns a string of extra flags */
/* strings looks like this: -----V--b-E, i.e.. set flags have their letter,
   flags not set, have just a '-'
 */
  
char * extra_flag_string (int extra)
{
	int i;
	static char buffer [MAX_EXTRA_FLAG+1]; /* 1 for each flag + term NUL */

	for (i = 0; i < MAX_EXTRA_FLAG; i++)
		if (IS_SET (extra, flag_names[i].value))
			buffer[i] = flag_names[i].name; /* it's my buffer and I'll do whatever I want - nyah! */
		else
			buffer[i] = '-'; /* flag not there */

	/* insert the terminating NUL ! */
	
	buffer[MAX_EXTRA_FLAG] = '\0';
	return buffer;			
}

/* returns a string with all relevant (i.e. contained in number_app/bool_app
   affects in pAffect. Only three first letters of location-name are used.
   Example: +4dam,+1dex,-1str
*/
void affect_str (AFFECT_DATA* pAffect, char* buffer)
{
	int i,total;
	char buf1 [20]; /* just a small buffer, should not hold more than +9999999999nam, */
	
	strcpy (buffer, ""); /* clear the buffer */

	for (i = 0; i < MAX_NUMBER_APP; i++)
	{	
		total = get_affect_total(pAffect, number_app[i]);
		if (total) /* total != 0 */
		{
			/* i hope this works on non-linux? */
			/* 'arm' abbrev for armor class is not good BTW */
			sprintf (buf1, "%+d%-2.3s,", total,affect_loc_name(number_app[i])); /* there is no itoa on unix ? */
			strcat (buffer, buf1);
		}
	}

	if (strlen(buffer) > 0) /* if we actually have printed something */
		buffer[strlen(buffer)-1] = '\0';		
} /* affect_str */


/* print a comma-delimetered line into a file */
void printobj (FILE *fp, OBJ_RESET *list, where location, sh_int vnum_what, 
               sh_int vnum_where, sh_int vnum_who, sh_int vnum_inside,
               bool reboot)
{
	char buf[MAX_STRING_LENGTH];
	char buf1[MAX_STRING_LENGTH];
	MOB_INDEX_DATA *mob;
	int level,i;
	bool shopkeeper = FALSE;

	OBJ_INDEX_DATA *obj = get_obj_index (vnum_what);

if (!obj) /* could not find that vnum! */
{
	log_string(LOG_BUG,"printobj: could not find obj %d\n",vnum_what);
	return;
} /* if */

switch (location) /* write something about WHERE the objects is reset */
{

default:
	bug ("printobj: invalid location:%d",location);
	return;
	
case on_ground: /* where is used */
{		
	ROOM_INDEX_DATA* room = get_room_index (vnum_where);
	
	if (room) /* room != NULL */
		sprintf (buf1, "\"in room: %s\"", room->name);
	else
		sprintf (buf1, "\"nowhere(!)\"");
	break;
}

case inside:    /* inside is used. where is derived */
{
	OBJ_INDEX_DATA* obj_inside_index;
	OBJ_DATA* obj_inside;
	
	
	obj_inside_index = get_obj_index (vnum_inside);
	
	if (!obj_inside_index) /* could not even find the obj index?! */
	{
		bug ("printobj: container-obj %d non-existant",vnum_inside);
		return;
	}
	
	/* find an instance of the object. Note we can risk finding one on a player.. :( */
	
	obj_inside = get_obj_type (obj_inside_index);
	
	if (!obj_inside) /* couldnt find where one of those is */
		vnum_where = 0; /* could not locate */
	else
		if (obj_inside->in_room) /* check for valid room */
			vnum_where = obj_inside->in_room->vnum;
		else /* not in a room.. check inventory of a mob ? */
			if (obj_inside->carried_by && obj_inside->carried_by->in_room)
				vnum_where = obj_inside->carried_by->in_room->vnum;
			
	if (vnum_where)
	{
		ROOM_INDEX_DATA* room = get_room_index (vnum_where);
		if (room)
			if (obj_inside->carried_by) /* the container is carried by a mob */
				sprintf (buf1, "\"inside %s on %s\"", obj_inside->short_descr, 
								IS_NPC (obj_inside->carried_by)
								? obj_inside->carried_by->short_descr
								: "(a player)" );
			else
				sprintf (buf1, "\"inside %s in %s\"", obj_inside->short_descr, room->name);
		else
			sprintf (buf1, "\"inside %s somewhere(!)\"", obj_inside->short_descr);
	} /* if vnum_where */
	else
		sprintf (buf1, "\"inside something(!), somewhere(!)\"");				
				
	break;
} /* case inside */

case on_mob:    /* where, who is used */
{
	mob = get_mob_index (vnum_who);	
	if (mob)
		sprintf (buf1, "\"on %s\"", mob->short_descr);
	else
		sprintf (buf1, "\"on someone(!)\"");

	break;
} /* on mob */

} /* switch where */

mob = get_mob_index (vnum_who); /* vnum_who */
	
level = obj->level;

/* check if item is the part of a shop's inventory, and if so adjust level */
/* snarfed from reset_area, changed to only average level */
/* note that we can have a valid mob pointer even if the item is not to be 
   given to a shopkeeper, therefore check for location
   Problem: Are items equipped on shopkeeper set to shop-level or normal level?
   Seems to be the former..
 */
    
if ( (location == on_mob) &&  
	 (mob != NULL) && 
	 (mob->pShop != NULL)) /* loaded on mob, valid mob, valid shopkeeper */
{

	shopkeeper = TRUE;
	
}
/* check if we already have written an object like this of the same level */
if (is_in_list (list, vnum_what, level))
	return; /* only write obj of same level once */
	
add_to_list (list, vnum_what, level);

/* short descr, keywords, level, vnum_what, vnum_where */

sprintf (buf, "\"%s\",\"%s\",%d,%d,%d,", 
	    obj->short_descr, obj->name, level, vnum_what, vnum_where);
strcat (buf, buf1);
strcat (buf, ","); /* append textual description of place where it is reset */

/* append extra-flags. if item on a shopkeeper, append the extra # flag,
   if item reboot, append * (asterix) */
sprintf (buf1, "\"%s%c%c\",", extra_flag_string (obj->extra_flags),
		 shopkeeper     ? '#' : '-',
		 reboot         ? '*' : '-' );
strcat (buf, buf1);

/* weight, cost, material, wear flags */
sprintf (buf1, "%d,%d,\"%s\",\"(%s)\",", 
  obj->weight, obj->cost, 
  obj->material ? obj->material : "none", /* material bit */
  wear_bit_name(obj->wear_flags)); /* weight&value of object */
strcat (buf, buf1);

/* data specific for objects */

/* name, AC, charges, level, spell1, spell2, spell3 */
switch (obj->item_type)
{
case ITEM_WEAPON: /* weapon (damage_type), damagegiven, weapon extra flags */	
	sprintf (buf1, "\"weapon(%s)\",\"%s\",0,\"%s\",", 
				attack_table[obj->value[3]].name,
				get_damage_string (obj,level),				
				weapon_bit_name(obj->value[4])); /* charges, spell level, weapon flags */
	break; /* weapon */

case ITEM_ARMOR: /* armor, ACs and bulk */
	sprintf (buf1, "\"armor\",\"%d/%d/%d/%d\",%d,\"\",", 
		 	 obj->value[0], obj->value[1], obj->value[2], obj->value[3],
		 	 obj->value[5]
		 	); /* AC x 4, bulk*/
	break; /* armor */
	
case ITEM_WAND: /* wand|staff, spell-level, charges, spellname */
case ITEM_STAFF:
	sprintf (buf1, "\"%s\",%d,%d,\"%s\",", 
			 item_index_type_name(obj->item_type),
			 obj->value[0],
			 obj->value[1],
			 ((obj->value[3] > 0) && (obj->value[3] < MAX_SKILL)) ?
			 skill_table[obj->value[3]].name
			 : "unknown spell"
	        ); 
	break; /* wand, staff */

case ITEM_PILL:
case ITEM_SCROLL:
case ITEM_POTION: /* pill|scroll|potion, spell-level, 0, spell-name(s) */
	sprintf (buf1, "\"%s\",%d,0,\"", 
			 item_index_type_name(obj->item_type),
			 obj->value[0]
			);
			
	/* pill/scroll/potion, spell level, no charges, */
	for (i = 1; i < 5; i++) /* extra slot for ROM */
	{
		/* valid skills range from 1 .. MAX_SKILL-1 */
		if ( (obj->value[i] > 0) && (obj->value[i] < MAX_SKILL) ) /* spell name, if any */
		{
			strcat (buf1, skill_table[obj->value[i]].name);
			strcat (buf1, ","); /* add a comma after, should be OK inside a string ?? */
		}
	}
		if (buf1[strlen(buf1)-1] == ',')
			buf1[strlen(buf1)-1] = '\0'; /* remove trailing comma */
		strcat (buf1, "\","); /* terminating quote, only one! */
		
	break; /* pill, scroll, potion */

case ITEM_LIGHT: /* number of hours the light lasts */
	if (obj->value[2] == 0)
		sprintf (buf1, "\"light\",0,0,\"(dead)\",");
	else
		if (obj->value[2] == -1)
			sprintf (buf1, "\"light\",0,-1,\"(unlimited)\",");
		else
			sprintf (buf1, "\"light\",0,%d,\"(limited)\",", obj->value[2]);
			
	break; /* forgot */
	
case ITEM_CONTAINER:
{
	sprintf (buf1, "\"container\",%d,\"%dlbs, %d max (%d%% bulk)\",\"", 
			 obj->value[2], obj->value[0],
			 obj->value[3], obj->value[4]); 
	if (IS_SET(obj->value[1], CONT_CLOSEABLE))
		strcat (buf1, "close ");
	if (IS_SET(obj->value[1], CONT_PICKPROOF))
		strcat (buf1, "nopick ");
	if (IS_SET(obj->value[1], CONT_CLOSED))
		strcat (buf1, "closed ");
	if (IS_SET(obj->value[1], CONT_LOCKED))
		strcat (buf1, "locked");
		
	strcat (buf1, "\",");
	break;		
}

case ITEM_DRINK_CON: /* drink container */
{
	/* print out poison factor, number of drinks and liquid type */
	/* only print liq type if it is a valid type */
	/* Valid liquids range from 0 .. LIQ_MAX-1 */
	sprintf (buf1, "\"drink container\",%d,%d,\"%s\",",
		            obj->value[3], obj->value[0],
		            (obj->value[2] >= 0) 
		            ? liq_table[obj->value[2]].liq_name
		            : "uknown liquid"
		    );
	break;		
}

case ITEM_TREASURE:  /* treasure, nothing extra */
case ITEM_FURNITURE: /* do */
case ITEM_TRASH:     /* do */
	sprintf (buf1, "\"%s\",0,0,\"\",", 
		 	 item_index_type_name(obj->item_type)
		 	);
	break;
	
case ITEM_FOOD:
	sprintf (buf1, "\"food\",%d,%d,\"%s\",", 
			 obj->value[0], obj->value[1],
			 obj->value[3] ? "poisoned" : ""
		 	);
	break;
case ITEM_PORTAL:
{
	ROOM_INDEX_DATA* to_room;
	
	to_room = get_room_index (obj->value[3]);
	
	sprintf (buf1, "\"portal to %s\",%d,%d,\"%d\",", 
			 to_room ? to_room->name : "nowhere?",
			 obj->value[1],
			 obj->value[0],
			 obj->value[2]);
	break;
}


default:
	return; /* do not save those (food,key,money,boat,corpse,fountain */
}

strcat (buf, buf1); /* add obj-specific data */
	
/* now: affects */
/* first: a textual list of all affects */

strcat (buf, "\"");
affect_str (obj->affected, buf1);
strcat (buf,buf1);
strcat (buf, "\",");

/* then one field for each numeral affect */

for (i = 0; i < MAX_NUMBER_APP; i++)
{	
	sprintf (buf1, "%d,", get_affect_total (obj->affected, number_app[i]) );
	strcat (buf, buf1);
}
	sprintf (buf1, "\"%s\",", get_bit_flags (obj->affected)); /* NOT TESTED */
	strcat (buf, buf1);

/* add other information. This could be... anything ? */	
	sprintf (buf1, ",\"%s\"", get_other_information(obj));
	strcat (buf,buf1);
	
	fprintf (fp, "%s\n",buf); /* lets gooo! */	
	return;
} /* printobj */


/* print a header */
void printheader (FILE *fp)
{
	int i;
	
	fprintf (fp,
"\"Short desc\",\"Keywords\",\"Level\",\"Vnum\",\"Room\",\"Location\",\
\"Flags\",\"Weight\",\"Value\",\"Material\",\"Worn\",\"Item type\",\"AC/SLevel\",\"Capacity\",\
\"Spells\",\"Affects\","); /* ouch! */

    for (i = 0; i < MAX_NUMBER_APP; i++)
    {	
    	fprintf (fp, "\"%s\",", affect_loc_name(number_app[i]) );
    } /* for i */

	/* bit affects */
    fprintf (fp, "\"Bit affects\","); /* was bool_app before */
    fprintf (fp, "\"Other\"\n"); /* finally, "other" field (?) */
    return;
} /* printheader */


/* print one area to the file fp */
/* file must be open for w and pArea != NULL */
void printarea (FILE *fp, AREA_DATA *pArea)
{
	MOB_INDEX_DATA *last_mob; /* remember the last mob */
	ROOM_INDEX_DATA *last_room;
	ROOM_INDEX_DATA *room;
	RESET_DATA * pReset;
	OBJ_RESET *list;
	bool mob_reboot; /* TRUE if the last loaded mob reboots at reboot only */
	int roomcount;

	list = new_list ();	 /* allocate memory for a new list */
	last_mob = NULL; 
	last_room = NULL;
	mob_reboot = FALSE;

	for( roomcount = pArea->min_vnum ; roomcount <= pArea->max_vnum; roomcount++ )
	{
		room = get_room_index( roomcount );
		if( room )
		{

	/* run through all the resets in that area */
	for (pReset = room->reset_first; pReset; pReset = pReset->next)
	{
    	/* check what kind of reset it is */

    	switch (pReset->command) /* always in uppercase */
    	{
    	case 'M': /* loads a mob */
    	{
    		if ( ( last_mob = get_mob_index( pReset->arg1 ) ) == NULL )
           	{ /* mob doesnt seem to exist ? */
           		bug( "objdump: 'M': bad vnum %d.\n\r", pReset->arg1 );
               	continue; /* next reset */
           	}
           	
            if (pReset->arg2 == -1) /* -1 = load mob/obj at reboot only */
            	mob_reboot = TRUE;

           	if ( (last_room = get_room_index( pReset->arg3 ) ) == NULL )
           	{ /* room doesn't seem to exist ? */
           		bug( "objdump: 'R': bad vnum %d.\n\r", pReset->arg3 );
               	continue;
           	}
    		
    		break; /* case M */
    	}
    	
    	case 'O': /* load an obj on the ground */
    	{
    		/*       file, location,   what,          where,   who    inside what*/	
    		
    		if (last_mob) /* do we have a last mob ? */
    			printobj (fp,list,on_ground, pReset->arg1, pReset->arg3, 
    			          last_mob->vnum, 0, (pReset->arg2 == -1));
    		else
    			printobj (fp,list,on_ground, pReset->arg1, pReset->arg3, 0, 0, (pReset->arg2 == -1));
    	
    		break; /* case O */
    	}
    	
    	case 'P': /* put it inside some obj */
    	{
    		/* current version is limited, it can only find the vnum of a room
    		   if the object that this one is inside exists */
    					
    		/*      file,         locat,  what,         where, who,            inside what */
    		if (last_mob)
    			printobj (fp,list,inside, pReset->arg1, 0,     last_mob->vnum, pReset->arg3,(pReset->arg2 == -1));
    		else
    			printobj (fp,list,inside, pReset->arg1, 0,     0,              pReset->arg3,(pReset->arg2 == -1));			
    	
    		break; /* case P */
    	}
    	
    	case 'G': /* create then give to mob */
    	case 'E': /* with an optional equip, but we don't really care here */
    	{
    		if ((last_mob != NULL) && (last_room != NULL))
    			printobj (fp,list,on_mob, pReset->arg1, last_room->vnum, last_mob->vnum, 0, 
    			          mob_reboot );
    		else
    			printf ("objdump: 'G' or 'E': no last mob\n\r");
    	
    		break; /* case G,E */
    	}
	}    	
	} /* for pReset */

	}
	}

	dispose_list (list); /* dispose of the temporary list of vnum/level pairs */
	return;
} /* printarea */

/* 
   objdump dumps a (commadelimetered) list of _reset_ objs in either the
   world or in one specific area. Not much feedback to user.
*/ 
void do_objdump (CHAR_DATA *ch, char *argument)
{
	AREA_DATA  *pArea;
	FILE *fp;

	/* for parsing the command line */	
	char filename[MAX_INPUT_LENGTH];
	char dumptype[MAX_INPUT_LENGTH];

	argument = one_argument(argument, dumptype); /* parse */

	/* check for valid arguments */

	if ( 
	     (!str_cmp (dumptype, "all")) ||
	     (!str_cmp (dumptype, "area"))
	   ) /* valid argument */
	{

	sprintf( filename, "../doc/objdump.csv");

    	fp = fopen (filename, "w"); /* open for writing */
    	
    	if (!fp) /* could not create file */
    	{
			send_to_char ("Could not open the specified file for output.\n\r",ch);
    		return;
    	}

    	printheader (fp); /* print ... the header */

    	/* now print whatever the player wants */    	
    	if (!str_cmp (dumptype, "all"))
			for (pArea = area_first; pArea != NULL; pArea = pArea->next)
				printarea (fp, pArea);
		else
			printarea (fp, ch->in_room->area);

		fclose (fp);
		
		send_to_char ("Probably successful.\n\r",ch);
	} /* if */
	else /* not 'all' or 'area' */
		do_help (ch, "objdump");
	return;			
} /* do_objdump */

void do_skillstat(CHAR_DATA *ch, char *argument)
{
    BUFFER *buffer;
    char arg[MAX_INPUT_LENGTH];
    char skill_list[MAX_LEVEL + 1][MAX_STRING_LENGTH];
    char skill_columns[MAX_LEVEL + 1];
    int sn, level, min_lev = 1, max_lev = MAX_LEVEL;
    bool found = FALSE;
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    if (IS_NPC(ch))
    return;

    if ( arg[0] == '\0' )
    {
        send_to_char( "List skills for whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if (IS_NPC(victim))
    {
        send_to_char( "Use this for skills on players.\n\r", ch );
        return;
    }

    /* initialize data */
    for (level = 0; level <= MAX_LEVEL; level++)
    {
        skill_columns[level] = 0;
        skill_list[level][0] = '\0';
    }

    for (sn = 0; sn < MAX_SKILL; sn++)
    {
        if (skill_table[sn].name == NULL )
        break;

        if ((level = skill_table[sn].skill_level[victim->class]) <= MAX_LEVEL
        &&  level >= min_lev && level <= max_lev
        &&  (skill_table[sn].spell_fun == spell_null)
        &&  victim->pcdata->learned[sn] > 0)
        {
            found = TRUE;
            level = skill_table[sn].skill_level[victim->class];
            if (victim->level < level)
                sprintf(buf,"%-18s n/a      ", skill_table[sn].name);
            else
                sprintf(buf,"%-18s %3d%%      ",skill_table[sn].name,
                victim->pcdata->learned[sn]);

            if (skill_list[level][0] == '\0')
                sprintf(skill_list[level],"\n\rLevel %2d: %s",level,buf);
            else /* append */
            {
                if ( ++skill_columns[level] % 2 == 0)
                    strcat(skill_list[level],"\n\r          ");
                    strcat(skill_list[level],buf);
            }
        }
    }

    /* return results */

    if (!found)
    {
        send_to_char("No skills found.\n\r",ch);
        return;
    }

    buffer = new_buf();
    for (level = 0; level <= MAX_LEVEL; level++)
        if (skill_list[level][0] != '\0')
            add_buf(buffer,skill_list[level]);
            add_buf(buffer,"\n\r");
            page_to_char(buf_string(buffer),ch);
            free_buf(buffer);
}

void do_spellstat(CHAR_DATA *ch, char *argument)
{
    BUFFER *buffer;
    char buff[100];
    char arg[MAX_INPUT_LENGTH];
    char spell_list[MAX_LEVEL + 1][MAX_STRING_LENGTH];
    char spell_columns[MAX_LEVEL + 1];
    int sn, gn, col, level, min_lev = 1, max_lev = MAX_LEVEL, mana;
    bool found = FALSE;
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;


    if (IS_NPC(ch))
    return;

    if ( arg[0] == '\0' )
    {
        send_to_char( "List spells for whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if (IS_NPC(victim))
    {
        send_to_char( "Use this for skills on players.\n\r", ch );
        return;
    }

    /* groups */

    col = 0;

    for (gn = 0; gn < MAX_GROUP; gn++)
    {
        if (group_table[gn].name == NULL)
        break;
        if (victim->pcdata->group_known[gn])
        {
            sprintf(buff,"%-20s ",group_table[gn].name);
            send_to_char(buff,ch);
            if (++col % 3 == 0)
                send_to_char("\n\r",ch);
        }
    }
    if ( col % 3 != 0 )
    {
        send_to_char( "\n\r", ch );
        sprintf(buff,"Creation points: %d\n\r",victim->pcdata->points);
        send_to_char(buff,ch);
    }
    
    /* initialize data */
    for (level = 0; level <= MAX_LEVEL; level++)
    {
        spell_columns[level] = 0;
        spell_list[level][0] = '\0';
    }

    for (sn = 0; sn < MAX_SKILL; sn++)
    {
        if (skill_table[sn].name == NULL )
        break;

        if ((level = skill_table[sn].skill_level[victim->class]) <= MAX_LEVEL
        &&  level >= min_lev && level <= max_lev
        &&  skill_table[sn].spell_fun != spell_null
        &&  victim->pcdata->learned[sn] > 0)
        {
            found = TRUE;
            level = skill_table[sn].skill_level[victim->class];

            if (victim->level < level)
                sprintf(buf,"%-18s n/a      ", skill_table[sn].name);
            else
            {
                mana = UMAX(skill_table[sn].min_mana,
                100/(2 + victim->level - level));
                sprintf(buf,"%-18s  %3d mana  ",skill_table[sn].name,mana);
            }

            if (spell_list[level][0] == '\0')
                sprintf(spell_list[level],"\n\rLevel %2d: %s",level,buf);
            else /* append */
            {
                if ( ++spell_columns[level] % 2 == 0)
                strcat(spell_list[level],"\n\r          ");
                strcat(spell_list[level],buf);
            }
        }
    }

    /* return results */

    if (!found)
    {
        send_to_char("No spells found.\n\r",ch);
        return;
    }

    buffer = new_buf();
    for (level = 0; level <= MAX_LEVEL; level++)
        if (spell_list[level][0] != '\0')
            add_buf(buffer,spell_list[level]);
            add_buf(buffer,"\n\r");
            page_to_char(buf_string(buffer),ch);
            free_buf(buffer);
}

void do_olevel(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_INPUT_LENGTH];
    char level[MAX_INPUT_LENGTH];
    char name[MAX_INPUT_LENGTH];
    BUFFER *buffer;
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    bool found;
    int number = 0, max_found;

    found = FALSE;
    number = 0;
    max_found = 200;

    buffer = new_buf();

    argument = one_argument(argument, level);
    if (level[0] == '\0')
    {
        send_to_char("Syntax: olevel <level>\n\r",ch);
        send_to_char("        olevel <level> <name>\n\r",ch);
        return;
    }
 
    argument = one_argument(argument, name);
    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
        if ( obj->level != atoi(level) )
            continue;

	if ( name[0] != '\0' && !is_name(name, obj->name) )
	    continue;

        found = TRUE;
        number++;
 
        for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj );
 
        if ( in_obj->carried_by != NULL && can_see(ch,in_obj->carried_by)
        &&   in_obj->carried_by->in_room != NULL)
            sprintf( buf, "%3d) %s is carried by %s [Room %d]\n\r",
                number, obj->short_descr,PERS(in_obj->carried_by, ch),
                in_obj->carried_by->in_room->vnum );
        else if (in_obj->in_room != NULL && can_see_room(ch,in_obj->in_room))
            sprintf( buf, "%3d) %s is in %s [Room %d]\n\r",
                number, obj->short_descr,in_obj->in_room->name, 
                in_obj->in_room->vnum);
        else
            sprintf( buf, "%3d) %s is somewhere\n\r",number, obj->short_descr);
 
        buf[0] = UPPER(buf[0]);
        add_buf(buffer,buf);
 
        if (number >= max_found)
            break;
    }
 
    if ( !found )
        send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
    else
        page_to_char(buf_string(buffer),ch);

    free_buf(buffer);
}

void do_mlevel( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];
    BUFFER *buffer;
    CHAR_DATA *victim;
    bool found;
    int count = 0;

    if ( argument[0] == '\0' )
    {
	send_to_char("Syntax: mlevel <level>\n\r",ch);
	return;
    }

    found = FALSE;
    buffer = new_buf();
    for ( victim = char_list; victim != NULL; victim = victim->next )
    {
        if ( victim->in_room != NULL
        &&   atoi(argument) == victim->level )
        {
            found = TRUE;
            count++;
            sprintf( buf, "%3d) [%5d] %-28s [%5d] %s\n\r", count,
                IS_NPC(victim) ? victim->pIndexData->vnum : 0,
                IS_NPC(victim) ? victim->short_descr : victim->name,
                victim->in_room->vnum,
                victim->in_room->name );
            add_buf(buffer,buf);
        }
    }

    if ( !found )
        act( "You didn't find any mob of level $T.", ch, NULL, argument, TO_CHAR );
    else
        page_to_char(buf_string(buffer),ch);

    free_buf(buffer);

    return;
}

void do_randomize( CHAR_DATA *ch, char *argument )
{
    generate_mobs();
    generate_items();
}
