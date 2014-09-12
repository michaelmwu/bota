/* -----------------------------------------------------------------------
The following snippet was written by Gary McNickle (dharvest) for
Rom 2.4 specific MUDs and is released into the public domain. My thanks to
the originators of Diku, and Rom, as well as to all those others who have
released code for this mud base.  Goes to show that the freeware idea can
actually work. ;)  In any case, all I ask is that you credit this code
properly, and perhaps drop me a line letting me know it's being used.

from: gary@dharvest.com
website: http://www.dharvest.com
or http://www.dharvest.com/resource.html (rom related)

Send any comments, flames, bug-reports, suggestions, requests, etc... 
to the above email address.
----------------------------------------------------------------------- */

/***************************************************************************
*       ROM 2.4 is copyright 1993-1995 Russ Taylor			   *
*       ROM has been brought to you by the ROM consortium		   *
*           Russ Taylor (rtaylor@pacinfo.com)				   *
*           Gabrielle Taylor (gtaylor@pacinfo.com)			   *
*           Brian Moore (rom@rom.efn.org)				   *
*       By using this code, you have agreed to follow the terms of the     *
*       ROM license, in the file Rom24/doc/rom.license                     *
***************************************************************************/

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


/**  file   : guild.c
  *  author : Gary McNickle (gary@dharvest.com)
  *  purpose: Meant to provide a guild system configurable without 
  *           recompiling. This is done via an ASCII file called "guild.dat"  
  *           that you can edit or create with any text editor 
  *           (vi/pico/jed/jaret/etc).  All gulid/clan related commands from 
  *           ROM 2.4 have been moved to this file in an effort to keep 
  *           related code located all in one spot.
  */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"

/** Function: guild_bit_name
  * Descr   : Returns the ascii name of a GUILD_ bit vector.
  * Returns : (char *)
  * Syntax  : (n/a- called by system)
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */
char *guild_bit_name( int guild_flags )
{
  static char buf[512];
  
  buf[0] = '\0';
  if ( guild_flags & GUILD_INDEPENDENT	) strcat( buf, " independent"	);
  if ( guild_flags & GUILD_CHANGED	) strcat( buf, " changed"	);
  if ( guild_flags & GUILD_DELETED	) strcat( buf, " deleted"	);
  if ( guild_flags & GUILD_IMMORTAL	) strcat( buf, " immortal"	);
  if ( guild_flags & GUILD_PKILL	) strcat( buf, " pkill"	);
  
  return ( buf[0] != '\0' ) ? buf+1 : "none";
}

/** Function: is_leader
  * Descr   : Validates guild leadership of (ch).
  * Returns : True (yes, ch is a mortal leader) or False
  * Syntax  : (n/a)
  * Written : v1.1 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

bool is_leader(CHAR_DATA * ch)
{
    /* update by: Zak Johnson (jakj@usa.net) Thanks Zak! */
    return IS_SET(ch->act, PLR_MORTAL_LEADER) ? 1 : 0;
} /* end: is_leader */



/** Function: can_guild
  * Descr   : Validates ability of (ch) to guild other players into
  *         : a specific (or generic, if implementor) guild.
  * Returns : TRUE/FALSE
  * Syntax  : (n/a)
  * Written : v1.1 2/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

bool can_guild(CHAR_DATA * ch)
{
    /* ok if ch is a SUPREME or higher */
    if (ch->level >= SUPREME || ch->trust >= SUPREME)
	return TRUE;

    /* not ok if ch is not guilded or is not a mortal leader */
    if (ch->clan == 0 || !is_leader(ch))
	return FALSE;

    return clan_table[ch->clan].ml[0];
} /* end: can_guild */



/** Function: can_deguild
  * Descr   : Validates ability of (ch) to de-guild players from a 
  *         : particular guild, or generic (if ch=implementor)
  * Returns : TRUE/FALSE
  * Syntax  : (n/a)
  * Written : v1.1 2/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

bool can_deguild(CHAR_DATA * ch)
{
    /* ok if ch is a SUPREME or higher */
    if (ch->level >= SUPREME || ch->trust >= SUPREME)
	return TRUE;

    /* not ok if ch is not guilded or is not a mortal leader */
    if (ch->clan == 0 || !is_leader(ch))
	return FALSE;

    return clan_table[ch->clan].ml[1];
} /* end: can_deguild */



/** Function: can_promote
  * Descr   : Validates ability of (ch) to promote players in their guilds
  * Returns : True/False
  * Syntax  : (n/a)
  * Written : v1.1 2/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

bool can_promote(CHAR_DATA * ch)
{
    /* ok if ch is a SUPREME or higher */
    if (ch->level >= SUPREME || ch->trust >= SUPREME)
	return TRUE;

    /* not ok if ch is not guilded or is not a mortal leader */
    if (ch->clan == 0 || !is_leader(ch))
	return FALSE;

    /* is a mortal leader, but do they have the right? */
    return clan_table[ch->clan].ml[2];
} /* end: can_promote */



/** Function: can_demote
  * Descr   : Validates ability of (ch) to demote a player within their guild.
  * Returns : True/False
  * Syntax  : (n/a)
  * Written : v1.1 2/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

bool can_demote(CHAR_DATA * ch)
{
    /* ok if ch is a SUPREME or higher */
    if (ch->level >= SUPREME || ch->trust >= SUPREME)
	return TRUE;

    /* not ok if ch is not guilded or is not a mortal leader */
    if (ch->clan == 0 || !is_leader(ch))
	return FALSE;

    return clan_table[ch->clan].ml[3];
} /* end: can_demote */




/** Function: is_clan
  * Descr   : Returns wether or not (ch) is a member of any guild/clan
  * Returns : True/False
  * Syntax  : (n/a)
  * Written : v1.0 ?
  * Author  : Rom Consortium. 
  */

bool is_clan(CHAR_DATA * ch)
{
    return ch->clan;
} /* end: is_clan */




/** Function: is_same_clan
  * Descr   : Returns wether or not (ch) and (victim) are members of
  *         : the same guild/clan
  * Returns : True/False
  * Syntax  : (n/a)
  * Written : v1.2 1/98
  * Author  : Rom Consortium. Updated by Gary McNickle <gary@dharvest.com>
  */

bool is_same_clan(CHAR_DATA * ch, CHAR_DATA * victim)
{
    if (is_independent(ch))
	return FALSE;
    else
	return (ch->clan == victim->clan);
} /* end: is_same_clan */



/** Function: clan_lookup
  * Descr   : Returns the numeric position of a clan name within the
  *         : clan_table 
  * Returns : (int)
  * Syntax  : (n/a)
  * Written : v1.0 
  * Author  : Rom Consortium
  */

int clan_lookup(const char *name)
{
    int clan;

    for (clan = 0; clan < MAX_CLAN; clan++)
    {
	if (!str_prefix(name, clan_table[clan].name))
	    return clan;
    }

    return 0;
} /* end: clan_lookup */

/** Function: player_rank
  * Descr   : Returns the players (ch) rank as a string.
  * Returns : char*
  * Syntax  : (n/a)
  * Written : v1.0 1/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

char *player_rank(CHAR_DATA * ch)
{
    if (ch->clan == 0)
	return '\0';
    return clan_table[ch->clan].rank[ch->rank].rankname;
} /* end: player_rank */


/** Function: player_clan
  * Descr   : Returns the name of the players (ch) clan as a string
  * Returns : char*
  * Syntax  : (n/a)
  * Written : v1.0 1/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

char *player_clan(CHAR_DATA * ch)
{
    if (ch->clan == 0)
	return '\0';
    return clan_table[ch->clan].name;
} /* end: player_clan */


/** Function: do_promote
  * Descr   : Promotes (or demotes) a character within their own guild
  * Returns : List of ranks if given no argument
  * Syntax  : promote <who> <rank #>
  * Written : v1.5 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

void do_promote(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int cnt;
    int sn = 0;

    argument = one_argument(argument, arg1);

    if (!can_promote(ch))
    {
	send_to_char("Huh?\n\r", ch);
	return;
    }

    if (arg1[0] == '\0' || argument[0] == '\0')
    {
	/*
	 * --------------------------------------------------------------
	 * Keep in mind that we are displaying the ranks as 1 - MAX_RANK,
	 * so, since the structure is actually 0 - MAX_RANK-1, we need to
	 * set "cnt" to cnt-1.
	 * -------------------------------------------------------------- 
	 */

	send_to_char("Syntax: promote <who> <rank #>\n\r", ch);
	send_to_char("where rank is one of the following:\n\r", ch);

	for (cnt = 0; cnt < MAX_RANK; cnt++)
	{
	  sprintf(buf, "%2d] %s\n\r", cnt + 1,
 	    is_clan(ch) ? clan_table[ch->clan].rank[cnt].rankname : "(None)");
	  send_to_char(buf, ch);
	}
	send_to_char("\n\r", ch);
	return;
    }				/* end syntax */

    if ((victim = get_char_world(ch, arg1)) == NULL)
    {
	send_to_char("They aren't playing.\n\r", ch);
	return;
    }

	if(IS_NPC(victim))
    {
	send_to_char("Not on NPCs.\n\r", ch);
	return;
    }

    if (!is_clan(victim))
    {
        send_to_char("They are not a member of any guilds!\n\r", ch);
        return;
    }

    if (!is_same_clan(ch, victim) &&
	(ch->level < SUPREME))
    {
	send_to_char("They are a member of a guild different than yours!\n\r", ch);
	return;
    }

    if (!str_cmp(argument, "leader"))
    {
	SET_BIT(victim->act, PLR_MORTAL_LEADER);
	send_to_char("They are now a mortal leader.\n\r", ch);
	send_to_char("You have just been promoted to a leader of your guild!\n\r", victim);
	return;
    }

    cnt = atoi(argument) - 1;
    if (cnt < 0 ||
	cnt > MAX_RANK -1 ||
	clan_table[victim->clan].rank[cnt].rankname == NULL)
    {
	send_to_char("That rank does not exist!", ch);
	return;
    }
    if (cnt > victim->rank && ((ch == victim) & (!IS_IMMORTAL(ch))))
    {
	send_to_char("Heh. I dont think so...", ch);
	return;
    }

  /** bug report by: Zak Jonhnson (zakj@usa.net)
    * we were checking ch->rank here..*sigh* Thanks Zak!
    */
    if (cnt > victim->rank)
    {
	int i;

	sprintf(buf, "You have been promoted to %s!\n\r",
		clan_table[victim->clan].rank[cnt].rankname);
	send_to_char(buf, victim);

	sprintf(buf, "%s has been promoted to %s!\n\r",
	      victim->name, clan_table[victim->clan].rank[cnt].rankname);
	send_to_char(buf, ch);

	for (i = victim->rank; i < cnt; i++)
	    if (clan_table[victim->clan].rank[i].skillname != NULL)
	    {
		sn = skill_lookup(clan_table[victim->clan].rank[i].skillname);
		if (sn < 0)
		{
		    sprintf(buf, "Bug: Add skill [%s] is not a valid skill",
			    clan_table[victim->clan].rank[cnt].skillname);
		    log_string(LOG_ERR,buf);
		} else if (!victim->pcdata->learned[sn])
		    victim->pcdata->learned[sn] = 20 + (victim->level / 4);
	    }
    } 
    
    else if (cnt < victim->rank)
    {
	if (IS_SET(victim->act, PLR_MORTAL_LEADER))
	    REMOVE_BIT(victim->act, PLR_MORTAL_LEADER);

	sprintf(buf, "You have been demoted to %s!\n\r",
		clan_table[victim->clan].rank[cnt].rankname);

	send_to_char(buf, victim);
	sprintf(buf, "%s has been demoted to %s!\n\r",
	      victim->name, clan_table[victim->clan].rank[cnt].rankname);

	send_to_char(buf, ch);
	/*
	 * ---------------------------------------------------------------
	 * Note: I dont think it would be fair here to take away any skills
	 * the victim may have earned at a higher rank. It makes no RP sense
	 * to do so and only hurts the player (loss of practices etc). Imms
	 * may want to keep an eye on this, as we dont want players jumping
	 * guilds just to gain new skills.
	 * -------------------------------------------------------------- 
	 */
    }				/* else no change */
    victim->rank = cnt;
    return;
} /* end: do_promote */


/** Function: do_guild
  * Descr   : Adds a character to the membership of a guild, or optionally,
  *         : removes them from a guild.
  * Returns : (n/a)
  * Syntax  : guild <who> <clan name>
  * Written : v1.3 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

void do_guild(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int clan;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (!can_guild(ch))
    {
	send_to_char("Huh?\n\r", ch);
	return;
    }
    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
	send_to_char("Syntax: guild <char> <clan name>\n\r", ch);
	return;
    }
    if ((victim = get_char_world(ch, arg1)) == NULL)
    {
	send_to_char("They aren't playing.\n\r", ch);
	return;
    }

	if(IS_NPC(victim))
    {
	send_to_char("Not on NPCs.\n\r", ch);
	return;
    }	

    /** thanks to Zanthras for the bug fix here...*/
    if (is_clan(victim) && !is_same_clan(ch, victim) &&
	((ch->level < SUPREME) & (ch->trust < SUPREME)))
    {
	send_to_char("They are a member of a guild other than your own.\n\r", ch);
	return;
    }

    if (!str_prefix(arg2, "none"))
    {
	send_to_char("They are no longer a member of any guild.\n\r", ch);
	send_to_char("You are no longer a member of any guild!\n\r", victim);
	victim->clan = 0;
	victim->rank = 0;

	/* add by: Zak Jonhson (zakj@usa.net) */
	if (IS_SET(victim->act, PLR_MORTAL_LEADER))
	    REMOVE_BIT(victim->act, PLR_MORTAL_LEADER);

	return;
    }
    if ((clan = clan_lookup(arg2)) == 0)
    {
	send_to_char("No such guild exists.\n\r", ch);
	return;
    }
    
    if( (IS_SET(victim->act, PLR_NOCLAN) && is_clan_pkill(clan)) && !IS_IMMORTAL(ch) )
    {
	send_to_char("They aren't allowed to be in a pkill clan",ch);
		return;
    }

    sprintf(buf, "They are now a %s of the %s.\n\r",
	    clan_table[clan].rank[0].rankname, clan_table[clan].name);
    send_to_char(buf, ch);

    sprintf(buf, "You are now a %s of the %s.\n\r",
	    clan_table[clan].rank[0].rankname, clan_table[clan].name);
    send_to_char(buf, victim);

    if (IS_SET(victim->act, PLR_MORTAL_LEADER))
	REMOVE_BIT(victim->act, PLR_MORTAL_LEADER);

    victim->clan = clan;
    victim->rank = 0;		/* lowest, default */
} /* end: do_guild */

void do_noclan( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
 
    one_argument( argument, arg );
 
    if ( arg[0] == '\0' )
    {
        send_to_char( "Noclan whom?\n\r", ch );
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
       
    if ( IS_SET(victim->act, PLR_NOCLAN) )
    {
        REMOVE_BIT(victim->act, PLR_NOCLAN);
        send_to_char( "NOCLAN removed.\n\r", ch );
        sprintf(buf,"$N allows %s to join pkill clans.",victim->name);
        wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
    else
    {
        SET_BIT(victim->act, PLR_NOCLAN);

        if(ch->clan)
        {
	    victim->clan = 0;
	    victim->rank = 0;
        }

        send_to_char( "NOCLAN set.\n\r", ch );
        sprintf(buf,"$N forbids %s to join pkill clans.",victim->name);
        wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
       
    save_char_obj( victim );
 
    return;
}

void do_lone(CHAR_DATA *ch, char *argument)
{
    send_to_char("If you want to become a loner, SPELL IT OUT.\n\r",ch);
    return;
}

void do_loner(CHAR_DATA *ch, char *argument)
{
    if(IS_NPC(ch)) return;

    if(ch->in_arena)
    {
        send_to_char("Now is ... not a good time.\n\r",ch);
        return;
    }

    if(ch->clan)
    {
        if(is_independent(ch))
            send_to_char("You're already an independent pkiller.\n\r", ch);
        else
            send_to_char("You're already in a clan.\n\r", ch);
        return;
    }

    if(ch->level < 25)
    {
        send_to_char("You must be level 25 or higher to become a loner.\n\r",ch);
        return;
    }

    if(argument[0] == '\0')
    {
        send_to_char("Syntax: loner confirm\n\r"
            "\n\rWarning! This will make you PK-able!!!\n\r", ch);
        return;
    }

    if(!str_cmp(argument, "confirm"))
    {
        send_to_char("You are now a loner. Watch out!\n\r", ch);
        ch->clan = clan_lookup("loner");
	 ch->rank = 0;
        return;
    }
    return;
}
