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

void do_heal(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *mob;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int cost,sn;
    SPELL_FUN *spell;
    char *words;

    /* check for healer */
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
        if ( IS_NPC(mob) && IS_SET(mob->act, ACT_IS_HEALER) )
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
	act("$N says '{aI offer the following spells:{x'",ch,NULL,mob,TO_CHAR);
	send_to_char("  light: cure light wounds      10 gold\n\r",ch);
	send_to_char("  serious: cure serious wounds  15 gold\n\r",ch);
	send_to_char("  critic: cure critical wounds  25 gold\n\r",ch);
	send_to_char("  heal: healing spell	        50 gold\n\r",ch);
	send_to_char("  blind: cure blindness         20 gold\n\r",ch);
	send_to_char("  disease: cure disease         15 gold\n\r",ch);
	send_to_char("  poison:  cure poison	        25 gold\n\r",ch); 
	send_to_char("  cancel:  cancels affects      80 gold\n\r",ch); 
	send_to_char("  uncurse: remove curse	        50 gold\n\r",ch);
	send_to_char("  refresh: restore movement      5 gold\n\r",ch);
	send_to_char("  mana:  restore mana	        10 gold\n\r",ch);
	send_to_char("  all hp: restores ALL hp    1 gold/8hp\n\r",ch);
	send_to_char("  all mana: \"\"\"\"   ALL mana  1 gold/10mana\n\r",ch);
        if(ch->max_hit - ch->hit > 0)
        {
            sprintf(buf,"\n\rIt will cost %d gold to heal all hit points.\n\r",
                (ch->max_hit - ch->hit)/8 );
	    send_to_char(buf,ch);
            sprintf(buf,"You presently have %d%% of your total hit points.\n\r",
                ((ch->hit*100)/ch->max_hit));
            send_to_char(buf,ch);
        }
	send_to_char(" Type heal <type> to be healed.\n\r",ch);
	return;
    }

    if (!str_prefix(arg,"light"))
    {
        spell = spell_cure_light;
	sn    = skill_lookup("cure light");
	words = "judicandus dies";
	 cost  = 1000;
    }

    else if (!str_prefix(arg,"serious"))
    {
	spell = spell_cure_serious;
	sn    = skill_lookup("cure serious");
	words = "judicandus gzfuajg";
	cost  = 1600;
    }

    else if (!str_prefix(arg,"critical"))
    {
	spell = spell_cure_critical;
	sn    = skill_lookup("cure critical");
	words = "judicandus qfuhuqar";
	cost  = 2500;
    }

    else if (!str_prefix(arg,"heal"))
    {
	spell = spell_heal;
	sn = skill_lookup("heal");
	words = "pzar";
	cost  = 5000;
    }

    else if (!str_prefix(arg,"blindness"))
    {
	spell = spell_cure_blindness;
	sn    = skill_lookup("cure blindness");
      	words = "judicandus noselacri";		
        cost  = 2000;
    }

    else if (!str_prefix(arg,"disease"))
    {
	spell = spell_cure_disease;
	sn    = skill_lookup("cure disease");
	words = "judicandus eugzagz";
	cost = 1500;
    }

    else if (!str_prefix(arg,"poison"))
    {
	spell = spell_cure_poison;
	sn    = skill_lookup("cure poison");
	words = "judicandus sausabru";
	cost  = 2500;
    }

    else if (!str_prefix(arg,"cancellation"))
    {
	spell = spell_cancellation;
	sn    = skill_lookup("cancellation");
	words = "judicandus desparasus";
	cost  = 8000;
    }
	
    else if (!str_prefix(arg,"uncurse") || !str_prefix(arg,"curse"))
    {
	spell = spell_remove_curse; 
	sn    = skill_lookup("remove curse");
	words = "candussido judifgz";
	cost  = 5000;
    }

    else if (!str_prefix(arg,"mana") || !str_prefix(arg,"energize"))
    {
        spell = NULL;
        sn = -2;
        words = "energizer";
        cost = 1000;
    }

	
    else if (!str_prefix(arg,"refresh") || !str_prefix(arg,"moves"))
    {
	spell =  spell_refresh;
	sn    = skill_lookup("refresh");
	words = "candusima"; 
	cost  = 500;
    }

    else if (!str_prefix(arg,"all hp"))
    {
        spell = NULL;
        sn = -3;
        words = "relax";
        if(ch->max_hit - ch->hit <= 10)
        {
            send_to_char("You really don't need to.\n\r",ch);
            return;
        }
        cost = 100 * ((ch->max_hit - ch->hit)/8);
    }

    else if (!str_prefix(arg,"all mana"))
    {
        spell = NULL;
        sn = -4;
        words = "omeditae";
        if(ch->max_mana - ch->mana <= 15)
        {
            send_to_char("You really don't need to.\n\r",ch);
            return;
        }
        cost = 100 * ((ch->max_mana - ch->mana)/10);
    }

    else 
    {
	act("$N says '{aType 'heal' for a list of spells.{x'",
	    ch,NULL,mob,TO_CHAR);
	return;
    }

    if (cost > ((ch->platinum * 10000) + (ch->gold * 100) + ch->silver))
    {
	act("$N says '{aYou do not have enough gold for my services.{x'",
	    ch,NULL,mob,TO_CHAR);
	return;
    }

    WAIT_STATE(ch,PULSE_VIOLENCE);

    deduct_cost(ch,cost,VALUE_SILVER);
    act("$n utters the words '{a$T{x'.",mob,NULL,words,TO_ROOM);
  
    if (sn == -2)  /* restore mana trap...kinda hackish */
    {
	ch->mana += dice(2,8) + mob->level / 3;
	ch->mana = UMIN(ch->mana,ch->max_mana);
	send_to_char("A warm {Yglow{x passes through you.\n\r",ch);
	return;
    }

    if (sn == -3)  /* Restore all HP */
    {
        ch->hit = ch->max_hit;
        update_pos(ch);
        send_to_char("You feel completely {brelaxed{x.\n\r",ch);
    }

    if (sn == -4)  /* Restore all mana */
    {
        ch->mana = ch->max_mana;
        update_pos(ch);
        send_to_char("Your mind feels at {bease{x.\n\r",ch);
    }

     if (sn <= -1 || !spell)
	return;
    
     spell(sn,mob->level,mob,ch,TARGET_CHAR);
}

void do_spellup_auto(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *mob;
    char arg[MAX_INPUT_LENGTH];
    int cost,sn;
    SPELL_FUN *spell;
    char *words;

    /* check for spellup mob */
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
        if ( IS_NPC(mob) && IS_SET(mob->act, ACT_IS_SPELLWIZ) )
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
	act("$N says '{aI offer the following spells:{x'",ch,NULL,mob,TO_CHAR);
	send_to_char("  fireshield: shield of fire      100 gold\n\r",ch);
	send_to_char("  iceshield: shield of ice        100 gold\n\r",ch);
	send_to_char("  shockshield: shield of static   100 gold\n\r",ch);
	send_to_char("  shield: shield of energy        80 gold\n\r",ch);
	send_to_char(" Type spellup <type> to be enchanted.\n\r",ch);
	return;
    }

    if (!str_prefix(arg,"fireshield"))
    {
        spell = spell_fireshield;
	sn    = skill_lookup("fireshield");
	words = "defens fury";
	cost  = 10000;
    }

    else if (!str_prefix(arg,"iceshield"))
    {
        spell = spell_iceshield;
	sn    = skill_lookup("iceshield");
	words = "defens articus";
	cost  = 10000;
    }
    else if (!str_prefix(arg,"shockshield"))
    {
        spell = spell_shockshield;
	sn    = skill_lookup("shockshield");
	words = "defens electrus";
	cost  = 10000;
    }
    else if (!str_prefix(arg,"shield"))
    {
        spell = spell_shield;
	sn    = skill_lookup("shield");
	words = "defens protectus";
	cost  = 8000;
    }

    else
    {
	act("$N says '{aType 'spellup' for a list of spells.{x'",
	    ch,NULL,mob,TO_CHAR);
	return;
    }

    if (cost > ((ch->platinum * 10000) + (ch->gold * 100) + ch->silver))
    {
	act("$N says '{aYou do not have enough gold for my services.{x'",
	    ch,NULL,mob,TO_CHAR);
	return;
    }

    WAIT_STATE(ch,PULSE_VIOLENCE);

    deduct_cost(ch,cost,VALUE_SILVER);
    act("$n utters the words '{a$T{x'.",mob,NULL,words,TO_ROOM);
  
    if (sn <= -1 || !spell)
	return;
    
    spell(sn,mob->level,mob,ch,TARGET_CHAR);
}

