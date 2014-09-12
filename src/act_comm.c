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
 **************************************************************************/

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
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"

/* command procedures needed */
DECLARE_DO_FUN(do_quit	);
DECLARE_DO_FUN(do_clanlist	);

void foreign_say	args( ( CHAR_DATA *ch, char *argument, int lang ) );

/* RT code to delete yourself */

void do_delet( CHAR_DATA *ch, char *argument)
{
    send_to_char("You must type the full command to delete yourself.\n\r",ch);
}

void do_clear (CHAR_DATA * ch, char *argument)
{
    send_to_char ("{WClear{x doesn't seem to work with your client. :(\n\r",ch);
    send_to_char ("\x01B[2J", ch);
}

void do_delete( CHAR_DATA *ch, char *argument)
{
   char strsave[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];

   if (IS_NPC(ch))
	return;

   if (ch->in_room->vnum == ROOM_VNUM_CORNER)
   {
        send_to_char("Stay in the corner where you belong.\n\r",ch);
        return;
   }
  
   if (ch->in_arena)
   {
        send_to_char("Now is not a good time.\n\r",ch);
        return;
   }

   if (ch->pcdata->confirm_delete)
   {
	if (argument[0] == '\0')
	{
	    send_to_char("Delete status removed.\n\r",ch);
	    ch->pcdata->confirm_delete = 0;
	    return;
	}
	else
	{
            int num = atoi(argument);
            if(num < 1000 || num != ch->pcdata->confirm_delete)
            {
	        send_to_char("Invalid confirmation number.\n\r",ch);
	        send_to_char("Delete status removed.\n\r",ch);
	        ch->pcdata->confirm_delete = 0;

                return;
            }

	    if (ch->clan && !is_independent(ch))
            {
                ch->clan = 0;
                ch->rank = 0;
            }
    	    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
	    wiznet("$N turns $Mself into line noise.",ch,NULL,0,0,0);
 	    stop_fighting(ch,TRUE);
	    if (ch->level > HERO)
	    {
		update_wizlist(ch, 1);
	    }
	    do_quit(ch,"");
	    unlink(strsave);
	    return;
 	}
    }

    if (argument[0] != '\0')
    {
	send_to_char("Just type delete. No argument.\n\r",ch);
	return;
    }

    ch->pcdata->confirm_delete = number_range(1200,9999);    
    sprintf(buf, "Type \"delete %d\" to confirm this command.\n\r",
            ch->pcdata->confirm_delete);

    send_to_char(buf,ch);

    send_to_char("WARNING: this command is irreversible.\n\r",ch);
    send_to_char("Typing delete without an argument will undo delete status.\n\r",
	ch);
    wiznet("$N is contemplating deletion.",ch,NULL,0,0,get_trust(ch));
}

void do_rerol( CHAR_DATA *ch, char *argument)
{
    send_to_char("You must type the full command to reroll yourself.\n\r",ch);
}

void do_reroll( CHAR_DATA *ch, char *argument)
{
   char strsave[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];

   if (IS_NPC(ch))
	return;

   if ((ch->class < MAX_CLASS/2) && (!IS_HERO(ch))) {
	send_to_char("You must be a Hero in the first tier to reroll.\n\r",ch);
	return;
   }

   if (ch->in_room->vnum == ROOM_VNUM_CORNER)
   {
        send_to_char("Stay in the corner where you belong.\n\r",ch);
        return;
   }

   if (ch->in_arena)
   {
        send_to_char("Now is not a good time.\n\r",ch);
        return;
   }

   if (ch->pcdata->confirm_reroll)
   {
	if (argument[0] == '\0')
	{
	    send_to_char("Reroll status removed.\n\r",ch);
	    ch->pcdata->confirm_reroll = 0;
	    return;
	}
	else
	{
            int num = atoi(argument);
            if(num < 1000 || num != ch->pcdata->confirm_reroll)
            {
	        send_to_char("Invalid confirmation number.\n\r",ch);
	        send_to_char("Reroll status removed.\n\r",ch);
	        ch->pcdata->confirm_reroll = 0;

                return;
            }

	    if (ch->clan && !is_independent(ch))
            {
                ch->clan = 0;
                ch->rank = 0;
            }

    	    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
	    wiznet("$N turns $Mself into line noise.",ch,NULL,0,0,0);
 	    stop_fighting(ch,TRUE);
	    SET_BIT( ch->act, PLR_REROLL );
	    ch->pcdata->tier = 1;
	    if (ch->level > HERO)
	    {
		update_wizlist(ch, 1);
	    }
	    do_quit(ch,"");
	    return;
 	}
    }

    if (argument[0] != '\0')
    {
	send_to_char("Just type reroll. No argument.\n\r",ch);
	return;
    }

    ch->pcdata->confirm_reroll = number_range(1200,9999);    
    sprintf(buf, "Type \"reroll %d\" to confirm this command.\n\r",
            ch->pcdata->confirm_reroll);

    send_to_char(buf,ch);

    send_to_char("All EQ, quest points, etc, on you will be deleted.\n\r",ch);
    send_to_char("See HELP REROLL.\n\r",ch);
    send_to_char("WARNING: this command is irreversible.\n\r",ch);
    send_to_char("Typing reroll with an argument will undo reroll status.\n\r",
	ch);
    wiznet("$N is contemplating rerolling.",ch,NULL,0,0,get_trust(ch));
}


/* RT code to display channel status */

void do_channels( CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    /* lists all channels and their status */
    send_to_char("   channel     status\n\r",ch);
    send_to_char("---------------------\n\r",ch);
 
    send_to_char("gossip         ",ch);
    if (!IS_SET(ch->comm,COMM_NOGOSSIP))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    send_to_char("clan gossip    ",ch);
    if (!IS_SET(ch->comm,COMM_NOCGOSSIP))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    send_to_char("ooc            ",ch);
    if (!IS_SET(ch->comm,COMM_NOOOC))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    send_to_char("quest gossip   ",ch);
    if (!IS_SET(ch->comm,COMM_NOCGOSSIP))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    send_to_char("auction        ",ch);
    if (!IS_SET(ch->comm,COMM_NOAUCTION))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    send_to_char("music          ",ch);
    if (!IS_SET(ch->comm,COMM_NOMUSIC))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    send_to_char("Q/A            ",ch);
    if (!IS_SET(ch->comm,COMM_NOASK))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    send_to_char("Quote          ",ch);
    if (!IS_SET(ch->comm,COMM_NOQUOTE))
	send_to_char("ON\n\r",ch);
    else
	send_to_char("OFF\n\r",ch);

    send_to_char("grats          ",ch);
    if (!IS_SET(ch->comm,COMM_NOGRATS))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    if (IS_IMMORTAL(ch))
    {
      send_to_char("god channel    ",ch);
      if(!IS_SET(ch->comm,COMM_NOWIZ))
        send_to_char("ON\n\r",ch);
      else
        send_to_char("OFF\n\r",ch);
    }

    if (ch->level >= LEVEL_HERO)
    {
      send_to_char("hero channel    ",ch);
      if(!IS_SET(ch->comm,COMM_NOHERO))
        send_to_char("ON\n\r",ch);
      else
        send_to_char("OFF\n\r",ch);
    }

    send_to_char("shouts         ",ch);
    if (!IS_SET(ch->comm,COMM_SHOUTSOFF))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    send_to_char("tells          ",ch);
    if (!IS_SET(ch->comm,COMM_DEAF))
	send_to_char("ON\n\r",ch);
    else
	send_to_char("OFF\n\r",ch);

    send_to_char("quiet mode     ",ch);
    if (IS_SET(ch->comm,COMM_QUIET))
      send_to_char("ON\n\r",ch);
    else
      send_to_char("OFF\n\r",ch);

    if (IS_SET(ch->comm,COMM_AFK))
	send_to_char("You are AFK.\n\r",ch);

    if (IS_SET(ch->comm,COMM_STORE))
	send_to_char("You store tells during fights.\n\r",ch);

    if (IS_SET(ch->comm,COMM_SNOOP_PROOF))
	send_to_char("You are immune to snooping.\n\r",ch);
   
    if (ch->lines != PAGELEN)
    {
	if (ch->lines)
	{
	    sprintf(buf,"You display %d lines of scroll.\n\r",ch->lines+2);
	    send_to_char(buf,ch);
 	}
	else
	    send_to_char("Scroll buffering is off.\n\r",ch);
    }

    if (ch->prompt != NULL)
    {
	sprintf(buf,"Your current prompt is: %s\n\r",ch->prompt);
	send_to_char(buf,ch);
    }

    if (IS_SET(ch->comm,COMM_NOSHOUT))
      send_to_char("You cannot shout.\n\r",ch);
  
    if (IS_SET(ch->comm,COMM_NOTELL))
      send_to_char("You cannot use tell.\n\r",ch);
 
    if (IS_SET(ch->comm,COMM_NOCHANNELS))
     send_to_char("You cannot use channels.\n\r",ch);

    if (IS_SET(ch->comm,COMM_NOEMOTE))
      send_to_char("You cannot show emotions.\n\r",ch);

}

/*
 * Public channel system (2). Allows more control than public_ch().
 * Format: character, argument, message to ch, message to all, type, comm bit
 * Use $t for name and $T for in msg strings
 * Created by Thyrr
 */

void public_ch ( CHAR_DATA *ch, char *arg, char *ymsg, char *msg, char *type, const int bitname)
{
    char buf[MAX_INPUT_LENGTH + 200];
    char cname[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    int pos;
    bool found = FALSE;

    smash_tilde( arg );

    if (arg[0] == '\0' && ch )
    {

      if (IS_SET(ch->comm,(bitname)))
      {
        sprintf( buf, "%s{x channel is now ON.\n\r", type);
        send_to_char(buf, ch);
        REMOVE_BIT(ch->comm,(bitname));
        return;
      }
      else
      {
        sprintf( buf, "%s{x channel is now OFF.\n\r", type);
        send_to_char(buf, ch);
        SET_BIT(ch->comm,(bitname));
        return;
      }
    }
    else if (ch)
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }

        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
        }

      if ((ch->in_room->vnum == ROOM_VNUM_CORNER)
      && (!IS_IMMORTAL(ch)))
      {
	send_to_char("Just keep your nose in the corner like a good little player.\n\r",ch);
	return;
      }


/*
        if ((!str_infix(" fuck ", arg))
        ||  (!str_prefix("fuck ", arg))
        ||  (!str_suffix(" fuck", arg))
        ||  (!str_suffix(" fuck.", arg))
        ||  (!str_suffix(" fuck!", arg))
        ||  (!str_cmp("fuck", arg))
        ||  (!str_cmp("fuck.", arg))
        ||  (!str_cmp("fuck!", arg))
        ||  (!str_cmp("shit", arg))
        ||  (!str_cmp("shit.", arg))
        ||  (!str_cmp("shit!", arg))
        ||  (!str_infix(" shit ", arg))
        ||  (!str_prefix("shit ", arg))
        ||  (!str_suffix(" shit", arg))
        ||  (!str_suffix(" shit.", arg))
        ||  (!str_suffix(" shit!", arg)))
        {
            if (!IS_IMMORTAL(ch)) {
                send_to_char("Don't let the IMM's catch you talking like that!\n\r",ch);
                return;
            }
        }
*/

        if(!IS_IMMORTAL(ch))
        {
	    str_replace(arg,"fuck","pineapple");
            str_replace(arg,"fark","salmon");
	    str_replace(arg,"fvck","fruitbread");
	    str_replace(arg,"shit","doodoo");
	    str_replace(arg,"sh!t","raisins");
	    str_replace(arg,"sh|t","toilet");
	    str_replace(arg,"sh*t","munchkin");
	    str_replace(arg,"asshole","George Bush");
	    str_replace(arg,"idiot","chimpanzee");
	    str_replace(arg,"@sshole","idiot");
	    str_replace(arg,"wtf","what the cheesecake");
	    str_replace(arg,"wth","Barney");
	    str_replace(arg,"bitch","I love men");
	    str_replace(arg,"bioch","KFC chicken");
	    str_replace(arg,"biotch","deer");
	    str_replace(arg,"rape","Nanking");
	    str_replace(arg,"faggot","Nerf football");
	    str_replace(arg,"fag","bundle of moose");
	    str_replace(arg,"gay","roast beef");
	    str_replace(arg,"homosexual","I'm so horny");
	    str_replace(arg,"homersexual","mm.. Flanders");
        }

        if ((ch->in_room->vnum == ROOM_VNUM_CORNER)
        && (!IS_IMMORTAL(ch)))
        {
          send_to_char("Just keep your nose in the corner like a good little player",ch);
          return;
        }
    }
        /* if they did have it off, using it turns it back on... */
        if(ch)
          REMOVE_BIT(ch->comm,(bitname));

        if (ch) /* For automation? */
          act_new_2(ymsg, ch, "you", arg, TO_CHAR, POS_SLEEPING,
              0, ACTION_GLOBAL);

  	for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *victim;
            victim = d->original ? d->original : d->character;
            if ( d->connected == CON_PLAYING &&
                 d->character != ch &&
                 !IS_SET(victim->comm,(bitname)) &&
                 !IS_SET(victim->comm,COMM_QUIET) )
            {
                for (pos = 0; pos < MAX_FORGET; pos++)
         	{
                    if (victim->pcdata->forget[pos] == NULL)
                       break;
                    if (ch && !str_cmp(ch->name,victim->pcdata->forget[pos]))
                       found = TRUE;
		}
                if(ch && !can_see_global(victim,ch))
                    strcpy(cname,(IS_IMMORTAL(ch) ? "{BA{bn {BI{bm{Bm{bo{Br{bt{Bal{x" : "Someone") );
                else if (ch)
                    strcpy(cname, !IS_NPC(ch) ? ch->name : ch->short_descr);
                else
                    strcpy(cname, "[!{RError-{Y^{R-Police{x!]");

		if ((!ch) || ((bitname != COMM_NOCLAN || is_same_clan(ch, victim))
		 && (bitname != COMM_NOWIZ || IS_IMMORTAL(victim) ) && !found))
                act_new_2(msg, victim, cname, arg, TO_CHAR, POS_SLEEPING, 0, ACTION_GLOBAL);
            }
        }        
}


void do_auction( CHAR_DATA *ch, char *argument )
{

    if (!ch) /* Automated Auctions */
    {
        public_ch(NULL, argument,
            "#ERROR(You Auction)",
            "{D({rA{wu{rc{wt{ri{wo{rn{D) {g$T{x",
            "#ERROR(Auction Toggle)#",
            COMM_NOAUCTION);
            return;
    }

    public_ch(ch, argument,
            "{gY{Go{gu {GAuction{x: {g$T{x",
            "$t {gAuction{x: {g$T{x",
            "Auction",
            COMM_NOAUCTION);

}

void do_gossip( CHAR_DATA *ch, char *argument )
{
	/* Make the words drunk if needed */
	if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 3)
	argument = makedrunk(argument,ch);

  public_ch(ch, argument,
            "You gossip '{H{+$T{x'",
            "$t gossips '{H{+$T{x'",
            "Gossip Channel ({rgossip{x)",
            COMM_NOGOSSIP);

}

void do_ooc( CHAR_DATA *ch, char *argument )
{

  public_ch(ch, argument,
            "You {rO{DO{rC{B: {M{+$T{x",
            "$t {rO{DO{rCs{B: {M{+$T{x",
            "Out Of Character ({rO{DO{rC{*{x)",
            COMM_NOGOSSIP);

/* Usage note: use $t for name, $T where the message is */

}

void do_grats( CHAR_DATA *ch, char *argument )
{

    if(!ch)
    {
        public_ch(ch, argument,
            "[Error] (nobody) {Jgrats{x '$T{x'",
            "Blackmage {Jgrats{x '$T{x'",
            "!Grats-Error!",
            COMM_NOGRATS);

        return;

    }

    public_ch(ch, argument,
            "You {Jgrats{x '$T{x'",
            "$t {Jgrats{x '$T{x'",
            "Congrats Channel ({Jgrats{x)",
            COMM_NOGRATS);
}


/* RT deaf blocks out all shouts */

void do_deaf( CHAR_DATA *ch, char *argument)
{
    
   if (IS_SET(ch->comm,COMM_DEAF))
   {
     send_to_char("You can now hear tells again.\n\r",ch);
     REMOVE_BIT(ch->comm,COMM_DEAF);
   }
   else 
   {
     send_to_char("From now on, you won't hear tells.\n\r",ch);
     SET_BIT(ch->comm,COMM_DEAF);
   }
}

/* RT quiet blocks out all communication */

void do_quiet ( CHAR_DATA *ch, char * argument)
{
    if (IS_SET(ch->comm,COMM_QUIET))
    {
      send_to_char("Quiet mode removed.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_QUIET);
    }
   else
   {
     send_to_char("From now on, you will only hear says and emotes.\n\r",ch);
     SET_BIT(ch->comm,COMM_QUIET);
   }
}

/* afk command */

void do_afk ( CHAR_DATA *ch, char * argument)
{
    char buf[MAX_STRING_LENGTH];

    if (IS_SET(ch->comm,COMM_AFK))
    {
      if(ch->tells)
      {
         sprintf( buf, "AFK mode removed.  You have {R%d{x tells waiting.\n\r", ch->tells );
         send_to_char( buf, ch );
         send_to_char("Type 'replay' to see tells.\n\r",ch);
      }
      else
      {
         send_to_char("AFK mode removed.  You have no tells waiting.\n\r",ch);
      }
      REMOVE_BIT(ch->comm,COMM_AFK);
    }
   else
   {
     send_to_char("You are now in AFK mode.\n\r",ch);
     SET_BIT(ch->comm,COMM_AFK);
   }
}

void do_autostore ( CHAR_DATA *ch, char * argument)
{
   if (IS_SET(ch->comm,COMM_STORE))
   {
     send_to_char("You will no longer store tells during fights.\n\r",ch);
     REMOVE_BIT(ch->comm,COMM_STORE);
   }
   else
   {
     send_to_char("You will now store tells during fights.\n\r",ch);
     SET_BIT(ch->comm,COMM_STORE);
   }
}

void do_replay (CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
    {
	send_to_char("You can't replay.\n\r",ch);
	return;
    }

    if (buf_string(ch->pcdata->buffer)[0] == '\0')
    {
	send_to_char("You have no tells to replay.\n\r",ch);
	return;
    }

    page_to_char(buf_string(ch->pcdata->buffer),ch);
    clear_buf(ch->pcdata->buffer);
    ch->tells = 0;
}

void do_qgossip( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int wtime;

    if (!ch->on_quest && !IS_IMMORTAL(ch))
    {   
        send_to_char("You are not on a quest!\n\r",ch);
        return;
    }   
    if (argument[0] == '\0' )
    {   
      if (IS_SET(ch->comm,COMM_NOQGOSSIP))
      { 
        send_to_char("Quest gossip channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOQGOSSIP);
      } 
      else
      {   
        send_to_char("Quest gossip channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOQGOSSIP);
      } 
    }  
    else  /* qgossip message sent, turn qgossip on if it isn't already */
    {   
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }

        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
        }
 
        if ((!str_infix(" fuck ", argument))
        ||  (!str_prefix("fuck ", argument))
        ||  (!str_suffix(" fuck", argument))
        ||  (!str_suffix(" fuck.", argument))
        ||  (!str_suffix(" fuck!", argument))
        ||  (!str_cmp("fuck", argument))
        ||  (!str_cmp("fuck.", argument))
        ||  (!str_cmp("fuck!", argument))
        ||  (!str_cmp("shit", argument))
        ||  (!str_cmp("shit.", argument))
        ||  (!str_cmp("shit!", argument))
        ||  (!str_infix(" shit ", argument))
        ||  (!str_prefix("shit ", argument))
        ||  (!str_suffix(" shit", argument))
        ||  (!str_suffix(" shit.", argument))
        ||  (!str_suffix(" shit!", argument)))
        {
            if (!IS_IMMORTAL(ch)) {
                send_to_char("Don't let the IMM's catch you talking like that!\n\r",ch);
                return;
            }
        }

      if ((ch->in_room->vnum == ROOM_VNUM_CORNER)
      && (!IS_IMMORTAL(ch)))
      {
	send_to_char("Just keep your nose in the corner like a good little player.\n\r",ch);
	return;
      }

      REMOVE_BIT(ch->comm,COMM_NOQGOSSIP);
 
      sprintf( buf, "You qgossip '{l%s{x'\n\r", argument );
      send_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
        int pos;
        bool found = FALSE;

        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOQGOSSIP) &&
             !IS_SET(victim->comm,COMM_QUIET) &&
             ((victim->on_quest) || IS_IMMORTAL(victim)))
        {
            for (pos = 0; pos < MAX_FORGET; pos++)
            {
                if (victim->pcdata->forget[pos] == NULL)
                    break;
                if (!str_cmp(ch->name,victim->pcdata->forget[pos]))
                    found = TRUE;
            }
            if (!found)
            {
                act_new( "$n qgossips '{l$t{x'",
                    ch,argument, d->character, TO_VICT,POS_SLEEPING );
            }
        }
      }
    }
    wtime = UMAX(2, 9-(ch->level));
    WAIT_STATE( ch, wtime );
}

void do_quote( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int wtime;
 
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOQUOTE))
      {
        send_to_char("Quote channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOQUOTE);
      }
      else
      {
        send_to_char("Quote channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOQUOTE);
      }
    }
    else  /* quote message sent, turn quote on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
        }
 
        if ((!str_infix(" fuck ", argument))
        ||  (!str_prefix("fuck ", argument))
        ||  (!str_suffix(" fuck", argument))
        ||  (!str_suffix(" fuck.", argument))
        ||  (!str_suffix(" fuck!", argument))
        ||  (!str_cmp("fuck", argument))
        ||  (!str_cmp("fuck.", argument))
        ||  (!str_cmp("fuck!", argument))
        ||  (!str_cmp("shit", argument))
        ||  (!str_cmp("shit.", argument))
        ||  (!str_cmp("shit!", argument))
        ||  (!str_infix(" shit ", argument))
        ||  (!str_prefix("shit ", argument))
        ||  (!str_suffix(" shit", argument))
        ||  (!str_suffix(" shit.", argument))
        ||  (!str_suffix(" shit!", argument)))
        { 
            if (!IS_IMMORTAL(ch)) {
                send_to_char("Don't let the IMM's catch you talking like that!\n\r",ch);
                return; 
            } 
        } 

      if ((ch->in_room->vnum == ROOM_VNUM_CORNER)
      && (!IS_IMMORTAL(ch)))
      {
	send_to_char("Just keep your nose in the corner like a good little player.\n\r",ch);
	return;
      }

      REMOVE_BIT(ch->comm,COMM_NOQUOTE);
 
      sprintf( buf, "You quote '{Q%s{x'\n\r", argument );
      send_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
	int pos;
	bool found = FALSE;

        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOQUOTE) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
	    for (pos = 0; pos < MAX_FORGET; pos++)
	    {
		if (victim->pcdata->forget[pos] == NULL)
		    break;
		if (!str_cmp(ch->name,victim->pcdata->forget[pos]))
		    found = TRUE;
	    }
	    if (!found)
	    {
		act_new( "$n quotes '{Q$t{x'",
		    ch,argument, d->character, TO_VICT,POS_SLEEPING );
	    }
        }
      }
    }
    wtime = UMAX(2, 9-(ch->level));
    WAIT_STATE( ch, wtime );
}

/* RT ask channel */
void do_ask( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int wtime;
 
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOASK))
      {
        send_to_char("Q/A channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOASK);
      }
      else
      {
        send_to_char("Q/A channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOASK);
      }
    }
    else  /* ask sent, turn Q/A on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
	}
 
        if ((!str_infix(" fuck ", argument))
        ||  (!str_prefix("fuck ", argument))
        ||  (!str_suffix(" fuck", argument))
        ||  (!str_suffix(" fuck.", argument))
        ||  (!str_suffix(" fuck!", argument))
        ||  (!str_cmp("fuck", argument))
        ||  (!str_cmp("fuck.", argument))
        ||  (!str_cmp("fuck!", argument))
        ||  (!str_cmp("shit", argument))
        ||  (!str_cmp("shit.", argument))
        ||  (!str_cmp("shit!", argument))
        ||  (!str_infix(" shit ", argument))
        ||  (!str_prefix("shit ", argument))
        ||  (!str_suffix(" shit", argument))
        ||  (!str_suffix(" shit.", argument))
        ||  (!str_suffix(" shit!", argument)))
        { 
            if (!IS_IMMORTAL(ch)) {
                send_to_char("Don't let the IMM's catch you talking like that!\n\r",ch);
                return; 
            } 
        } 

	if ((ch->in_room->vnum == ROOM_VNUM_CORNER)
	&& (!IS_IMMORTAL(ch)))
	{
	    send_to_char("Just keep your nose in the corner like a good little player.\n\r",ch);
	    return;
	}

        REMOVE_BIT(ch->comm,COMM_NOASK);
 
      sprintf( buf, "You ask '{P%s{x'\n\r", argument );
      send_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
	int pos;
	bool found = FALSE;

        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOASK) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
	    for (pos = 0; pos < MAX_FORGET; pos++)
	    {
		if (victim->pcdata->forget[pos] == NULL)
		    break;
		if (!str_cmp(ch->name,victim->pcdata->forget[pos]))
		    found = TRUE;
	    }
	    if (!found)
	    {
		act_new("$n asks '{P$t{x'",
		    ch,argument,d->character,TO_VICT,POS_SLEEPING);
	    }
        }
      }
    }
    wtime = UMAX(2, 9-(ch->level));
    WAIT_STATE( ch, wtime );
}

/* RT answer channel - uses same line as asks */
void do_answer( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int wtime;
 
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOASK))
      {
        send_to_char("Q/A channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOASK);
      }
      else
      {
        send_to_char("Q/A channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOASK);
      }
    }
    else  /* answer sent, turn Q/A on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
	}
 
        if ((!str_infix(" fuck ", argument))
        ||  (!str_prefix("fuck ", argument))
        ||  (!str_suffix(" fuck", argument))
        ||  (!str_suffix(" fuck.", argument))
        ||  (!str_suffix(" fuck!", argument))
        ||  (!str_cmp("fuck", argument))
        ||  (!str_cmp("fuck.", argument))
        ||  (!str_cmp("fuck!", argument))
        ||  (!str_cmp("shit", argument))
        ||  (!str_cmp("shit.", argument))
        ||  (!str_cmp("shit!", argument))
        ||  (!str_infix(" shit ", argument))
        ||  (!str_prefix("shit ", argument))
        ||  (!str_suffix(" shit", argument))
        ||  (!str_suffix(" shit.", argument))
        ||  (!str_suffix(" shit!", argument)))
        { 
            if (!IS_IMMORTAL(ch)) {
                send_to_char("Don't let the IMM's catch you talking like that!\n\r",ch);
                return; 
            } 
        } 

	if ((ch->in_room->vnum == ROOM_VNUM_CORNER)
	&& (!IS_IMMORTAL(ch)))
	{
	    send_to_char("Just keep your nose in the corner like a good little player.\n\r",ch);
	    return;
	}

        REMOVE_BIT(ch->comm,COMM_NOASK);
 
      sprintf( buf, "You answer '{P%s{x'\n\r", argument );
      send_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
	int pos;
	bool found = FALSE;

        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOASK) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
	    for (pos = 0; pos < MAX_FORGET; pos++)
	    {
		if (victim->pcdata->forget[pos] == NULL)
		    break;
		if (!str_cmp(ch->name,victim->pcdata->forget[pos]))
		    found = TRUE;
	    }
	    if (!found)
	    {
		act_new("$n answers '{P$t{x'",
		    ch,argument,d->character,TO_VICT,POS_SLEEPING);
	    }
        }
      }
    }
    wtime = UMAX(2, 9-(ch->level));
    WAIT_STATE( ch, wtime );
}

/* RT music channel */
void do_music( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int wtime;
 
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOMUSIC))
      {
        send_to_char("Music channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOMUSIC);
      }
      else
      {
        send_to_char("Music channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOMUSIC);
      }
    }
    else  /* music sent, turn music on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
	}
 
        if ((!str_infix(" fuck ", argument))
        ||  (!str_prefix("fuck ", argument))
        ||  (!str_suffix(" fuck", argument))
        ||  (!str_suffix(" fuck.", argument))
        ||  (!str_suffix(" fuck!", argument))
        ||  (!str_cmp("fuck", argument))
        ||  (!str_cmp("fuck.", argument))
        ||  (!str_cmp("fuck!", argument))
        ||  (!str_cmp("shit", argument))
        ||  (!str_cmp("shit.", argument))
        ||  (!str_cmp("shit!", argument))
        ||  (!str_infix(" shit ", argument))
        ||  (!str_prefix("shit ", argument))
        ||  (!str_suffix(" shit", argument))
        ||  (!str_suffix(" shit.", argument))
        ||  (!str_suffix(" shit!", argument)))
        { 
            if (!IS_IMMORTAL(ch)) {
                send_to_char("Don't let the IMM's catch you talking like that!\n\r",ch);
                return; 
            } 
        } 

	if ((ch->in_room->vnum == ROOM_VNUM_CORNER)
	&& (!IS_IMMORTAL(ch)))
	{
	    send_to_char("Just keep your nose in the corner like a good little player.\n\r",ch);
	    return;
	}

        REMOVE_BIT(ch->comm,COMM_NOMUSIC);
 
      sprintf( buf, "You SING (global): '{N%s{x'\n\r", argument );
      send_to_char( buf, ch );
      sprintf( buf, "$n SINGS (global): '{N%s{x'", argument );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
	int pos;
	bool found = FALSE;

        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOMUSIC) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
	    for (pos = 0; pos < MAX_FORGET; pos++)
	    {
		if (victim->pcdata->forget[pos] == NULL)
		    break;
		if (!str_cmp(ch->name,victim->pcdata->forget[pos]))
		    found = TRUE;
	    }
	    if (!found)
	    {
		act_new_2("$n MUSIC: '{N$t{x'",
		    ch,argument,d->character,TO_VICT,POS_SLEEPING, 0,
                    ACTION_GLOBAL);
	    }
        }
      }
    }
    wtime = UMAX(2, 9-(ch->level));
    WAIT_STATE( ch, wtime );
}

void do_herotalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOHERO))
      {
        send_to_char("Hero channel is now ON\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOHERO);
      }
      else
      {
        send_to_char("Hero channel is now OFF\n\r",ch);
        SET_BIT(ch->comm,COMM_NOHERO);
      }
      return;
    }

    REMOVE_BIT(ch->comm,COMM_NOHERO);

    sprintf( buf, "$n {YH{we{Yr{wo{x: {W%s{x", argument );
    act_new("$n {YH{we{Yr{wo{x: {W$t{x",ch,argument,NULL,TO_CHAR,POS_DEAD);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING &&
             d->character->level >= LEVEL_HERO &&
             !IS_SET(d->character->comm,COMM_NOHERO) )
        {
            act_new_2("$n {YH{we{Yr{wo{x: {W$t{x",ch,argument,d->character,TO_VICT,POS_DEAD,
                0, ACTION_GLOBAL);
        }
    }

    return;
}


void do_immtalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOWIZ))
      {
	send_to_char("Immortal channel is now ON\n\r",ch);
	REMOVE_BIT(ch->comm,COMM_NOWIZ);
      }
      else
      {
	send_to_char("Immortal channel is now OFF\n\r",ch);
	SET_BIT(ch->comm,COMM_NOWIZ);
      } 
      return;
    }

    REMOVE_BIT(ch->comm,COMM_NOWIZ);

    sprintf( buf, "$n: {L%s{x", argument );
    act_new("$n: {L$t{x",ch,argument,NULL,TO_CHAR,POS_DEAD);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected == CON_PLAYING && 
	     IS_IMMORTAL(d->character) && 
             !IS_SET(d->character->comm,COMM_NOWIZ) )
	{
	    act_new_2("$n: {L$t{x",ch,argument,d->character,TO_VICT,POS_DEAD,
                0, ACTION_GLOBAL);
	}
    }

    return;
}

void do_imptalk( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm2,COMM_NOIMP))
      {
	send_to_char("Implementor channel is now ON\n\r",ch);
	REMOVE_BIT(ch->comm2,COMM_NOIMP);
      }
      else
      {
	send_to_char("Implementor channel is now OFF\n\r",ch);
	SET_BIT(ch->comm2,COMM_NOIMP);
      } 
      return;
    }

    REMOVE_BIT(ch->comm2,COMM_NOIMP);

    act_new("{g[IMP{G $n{g] {B$t{x",ch,argument,NULL,TO_CHAR,POS_DEAD);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->connected == CON_PLAYING && 
	     get_trust(d->character) == MAX_LEVEL && 
             !IS_SET(d->character->comm2,COMM_NOIMP) )
	{
	    act_new("{g[IMP $n{g] {B$t{x",ch,argument,
             d->character,TO_VICT,POS_DEAD);
	}
    }

    return;
}


void do_say( CHAR_DATA *ch, char *argument )
{
	/* Make the words drunk if needed */
    if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 3)
	argument = makedrunk(argument,ch);

    foreign_say(ch,argument,ch->primary_lang);
    return;
}

void do_elvish( CHAR_DATA *ch, char *argument )
{
    if(!CAN_SPEAK(ch,LANGUAGE_ELVISH))
    {
        send_to_char("You don't know enough Elvish to say that.",ch);
        return;
    }

    foreign_say(ch,argument,LANGUAGE_ELVISH);
    return;
}

void do_dwarvish( CHAR_DATA *ch, char *argument )
{
    if(!CAN_SPEAK(ch,LANGUAGE_DWARVISH))
    {
        send_to_char("You don't know enough Dwarvish to say that.",ch);
        return;
    }

    foreign_say(ch,argument,LANGUAGE_DWARVISH);
    return;
}

void do_orcish( CHAR_DATA *ch, char *argument )
{
    if(!CAN_SPEAK(ch,LANGUAGE_ORCISH))
    {
        send_to_char("You don't know enough Dwarvish to say that.",ch);
        return;
    }

    foreign_say(ch,argument,LANGUAGE_ORCISH);
    return;
}

void do_common( CHAR_DATA *ch, char *argument )
{
    if(!CAN_SPEAK(ch,LANGUAGE_COMMON))
    {
        send_to_char("You don't know enough Common to say that.",ch);
        return;
    }

    foreign_say(ch,argument,LANGUAGE_COMMON);
    return;
}


void foreign_say( CHAR_DATA *ch, char *argument, int lang )
{
    char buf[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];

    if ( argument[0] == '\0' )
    {
	send_to_char( "Say what?\n\r", ch );
	return;
    }

    foreign_speech(ch,argument,arg2,lang);

    sprintf(buf,"[%s] $n says '{S$T{x'",language_table[lang].lname);
    act_lang( buf, ch, NULL, arg2, TO_ROOM, POS_RESTING, lang );
    sprintf(buf,"[%s] You say '{S$T{x'",language_table[lang].lname);
    act_lang( buf, ch, NULL, arg2, TO_CHAR, POS_RESTING, lang );

     if ( !IS_NPC(ch) )
     {
       CHAR_DATA *mob, *mob_next;
       for ( mob = ch->in_room->people; mob != NULL; mob = mob_next )
       {
           mob_next = mob->next_in_room;
           if ( IS_NPC(mob) && HAS_TRIGGER( mob, TRIG_SPEECH )
           &&   mob->position == mob->pIndexData->default_pos )
               mp_act_trigger( argument, mob, ch, NULL, NULL, TRIG_SPEECH );
       }
     }
    return;
}



void do_shout_old( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0' )
    {
      	if (IS_SET(ch->comm,COMM_SHOUTSOFF))
      	{
            send_to_char("You can hear shouts again.\n\r",ch);
            REMOVE_BIT(ch->comm,COMM_SHOUTSOFF);
      	}
      	else
      	{
            send_to_char("You will no longer hear shouts.\n\r",ch);
            SET_BIT(ch->comm,COMM_SHOUTSOFF);
      	}
      	return;
    }

    if ( IS_SET(ch->comm, COMM_NOSHOUT) )
    {
        send_to_char( "You can't shout.\n\r", ch );
        return;
    }

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER)
    && (!IS_IMMORTAL(ch)))
    {
	send_to_char("Just keep your nose in the corner like a good little player.\n\r",ch);
	return;
    }
 
    REMOVE_BIT(ch->comm,COMM_SHOUTSOFF);

    WAIT_STATE( ch, 12 );

    act( "You shout '{T$T{x'", ch, NULL, argument, TO_CHAR );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	CHAR_DATA *victim;
	int pos;
	bool found = FALSE;

	victim = d->original ? d->original : d->character;

	if ( d->connected == CON_PLAYING &&
	     d->character != ch &&
	     !IS_SET(victim->comm, COMM_SHOUTSOFF) &&
	     !IS_SET(victim->comm, COMM_QUIET) ) 
	{
	    for (pos = 0; pos < MAX_FORGET; pos++)
	    {
		if (victim->pcdata->forget[pos] == NULL)
		    break;
		if (!str_cmp(ch->name,victim->pcdata->forget[pos]))
		    found = TRUE;
	    }
	    if (!found)
	    {
		act("$n shouts '{T$t{x'",ch,argument,d->character,TO_VICT);
	    }
	}
    }

    return;
}



void do_tell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int pos;
    bool found = FALSE;
    int wtime;

	/* Make the words drunk if needed */
	if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 3)
	argument = makedrunk(argument,ch);

    if ( IS_SET(ch->comm, COMM_NOTELL) || IS_SET(ch->comm,COMM_DEAF))
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }

    if ( IS_SET(ch->comm, COMM_QUIET) )
    {
	send_to_char( "You must turn off quiet mode first.\n\r", ch);
	return;
    }

    if (IS_SET(ch->comm,COMM_DEAF))
    {
	send_to_char("You must turn off deaf mode first.\n\r",ch);
	return;
    }

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER)
    && (!IS_IMMORTAL(ch)))
    {
	send_to_char("Just keep your nose in the corner like a good little player.\n\r",ch);
	return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Tell whom what?\n\r", ch );
	return;
    }

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
    if ( ( victim = get_char_world( ch, arg ) ) == NULL
    || ( IS_NPC(victim) && victim->in_room != ch->in_room ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->desc == NULL && !IS_NPC(victim))
    {
	act("$N seems to have misplaced $S link...try again later.",
	    ch,NULL,victim,TO_CHAR);
        sprintf(buf,"%s tells you '{U%s{x'\n\r",PERS(ch,victim),argument);
        buf[0] = UPPER(buf[0]);
        add_buf(victim->pcdata->buffer,buf);
	victim->tells++;
	return;
    }

    if (!IS_NPC(victim))
    {
	for (pos = 0; pos < MAX_FORGET; pos++)
	{
	    if (victim->pcdata->forget[pos] == NULL)
		break;
	    if (!str_cmp(ch->name,victim->pcdata->forget[pos]))
		found = TRUE;
	}
    }
    if (found)
    {
	act("$N doesn't seem to be listening to you.",ch,NULL,victim,TO_CHAR);
	return;
    }

/* Removed..Vengeance */
/*
    if ( !(IS_IMMORTAL(ch) && ch->level > LEVEL_IMMORTAL) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }
*/
  
    if ((IS_SET(victim->comm,COMM_QUIET) || IS_SET(victim->comm,COMM_DEAF))
    && !IS_IMMORTAL(ch))
    {
	act( "$E is not receiving tells.", ch, 0, victim, TO_CHAR );
  	return;
    }

    if (IS_SET(victim->comm,COMM_AFK))
    {
	if (IS_NPC(victim))
	{
	    act("$E is AFK, and not receiving tells.",ch,NULL,victim,TO_CHAR);
	    return;
	}

	act("$E is AFK, but your tell will go through when $E returns.",
	    ch,NULL,victim,TO_CHAR);
	sprintf(buf,"%s tells you '{U%s{x'\n\r",PERS(ch,victim),argument);
	buf[0] = UPPER(buf[0]);
	add_buf(victim->pcdata->buffer,buf);
	victim->tells++;
	return;
    }

    if (IS_SET(victim->comm,COMM_STORE)
	&& (victim->fighting != NULL) )
    {
	if (IS_NPC(victim))
	{
	    act("$E is fighting, and not receiving tells.",ch,NULL,victim,TO_CHAR);
	    return;
	}

	act("$E is fighting, but your tell will go through when $E finishes.",
	    ch,NULL,victim,TO_CHAR);
	sprintf(buf,"%s tells you '{U%s{x'\n\r",PERS(ch,victim),argument);
	buf[0] = UPPER(buf[0]);
	add_buf(victim->pcdata->buffer,buf);
	victim->tells++;
	return;
    }

    act_new( "You tell $N '{U$t{x'", ch, argument, victim, TO_CHAR,POS_DEAD );
    act_new("$n tells you '{U$t{x'",ch,argument,victim,TO_VICT,POS_DEAD);
    victim->reply	= ch;

    wtime = UMAX(2, 9-(ch->level));
    WAIT_STATE( ch, wtime );

    return;
}



void do_reply( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    int pos;
    bool found = FALSE;
    int wtime;

	/* Make the words drunk if needed */
	if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 3)
	argument = makedrunk(argument,ch);

    if ( IS_SET(ch->comm, COMM_NOTELL) )
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER)
    && (!IS_IMMORTAL(ch)))
    {
	send_to_char("Just keep your nose in the corner like a good little player.\n\r",ch);
	return;
    }

    if ( ( victim = ch->reply ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim->desc == NULL && !IS_NPC(victim))
    {
        act("$N seems to have misplaced $S link...try again later.",
            ch,NULL,victim,TO_CHAR);
        sprintf(buf,"%s tells you '{U%s{x'\n\r",PERS(ch,victim),argument);
        buf[0] = UPPER(buf[0]);
        add_buf(victim->pcdata->buffer,buf);
	victim->tells++;
        return;
    }

    if (!IS_NPC(victim))
    {
	for (pos = 0; pos < MAX_FORGET; pos++)
	{
	    if (victim->pcdata->forget[pos] == NULL)
		break;
	    if (!str_cmp(ch->name,victim->pcdata->forget[pos]))
		found = TRUE;
	}
    }
    if (found)
    {
	act("$N doesn't seem to be listening to you.",ch,NULL,victim,TO_CHAR);
	return;
    }

/* Removed...Vengeance */
/*
    if ( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    {
	act( "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }
*/

    if ((IS_SET(victim->comm,COMM_QUIET) || IS_SET(victim->comm,COMM_DEAF))
    &&  !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim))
    {
        act_new( "$E is not receiving tells.", ch, 0, victim, TO_CHAR,POS_DEAD);
        return;
    }

/* Removed....Vengeance */
/*
    if (!IS_IMMORTAL(victim) && !IS_AWAKE(ch))
    {
	send_to_char( "In your dreams, or what?\n\r", ch );
	return;
    }
*/

    if (IS_SET(victim->comm,COMM_AFK))
    {
        if (IS_NPC(victim))
        {
            act_new("$E is AFK, and not receiving tells.",
		ch,NULL,victim,TO_CHAR,POS_DEAD);
            return;
        }
 
        act_new("$E is AFK, but your tell will go through when $E returns.",
            ch,NULL,victim,TO_CHAR,POS_DEAD);
        sprintf(buf,"%s tells you '{U%s{x'\n\r",PERS(ch,victim),argument);
	buf[0] = UPPER(buf[0]);
        add_buf(victim->pcdata->buffer,buf);
	victim->tells++;
        return;
    }

    act_new("You tell $N '{U$t{x'",ch,argument,victim,TO_CHAR,POS_DEAD);
    act_new("$n tells you '{U$t{x'",ch,argument,victim,TO_VICT,POS_DEAD);
    victim->reply	= ch;

    wtime = UMAX(2, 9-(ch->level));
    WAIT_STATE( ch, wtime );

    return;
}



void do_yell( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

	/* Make the words drunk if needed */
	if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 3)
	argument = makedrunk(argument,ch);

    if ( IS_SET(ch->comm, COMM_NOSHOUT) )
    {
        send_to_char( "You can't yell.\n\r", ch );
        return;
    }
 
    if ( argument[0] == '\0' )
    {
	send_to_char( "Yell what?\n\r", ch );
	return;
    }

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER)
    && (!IS_IMMORTAL(ch)))
    {
	send_to_char("Just keep your nose in the corner like a good little player.\n\r",ch);
	return;
    }

    act("You yell '{T$t{x'",ch,argument,NULL,TO_CHAR);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	int pos;
	bool found = FALSE;

	if ( d->connected == CON_PLAYING
        &&   d->character
	&&   d->character != ch
	&&   d->character->in_room != NULL
	&&   d->character->in_room->area == ch->in_room->area 
        &&   !IS_SET(d->character->comm,COMM_QUIET) )
	{
            if(!IS_NPC(d->character))
	    for (pos = 0; pos < MAX_FORGET; pos++)
	    {
		if (d->character->pcdata->forget[pos] == NULL)
		    break;
		if (!str_cmp(ch->name,d->character->pcdata->forget[pos]))
		    found = TRUE;
	    }
	    if (!found)
	    {
		act("$n yells '{T$t{x'",ch,argument,d->character,TO_VICT);
	    }
	}
    }

    return;
}


void do_emote( CHAR_DATA *ch, char *argument )
{
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
 
    MOBtrigger = FALSE;
    if(IS_NPC(ch))
    {
        act( "{B$n $T{x", ch, NULL, argument, TO_ROOM );
        act( "{B$n $T{x", ch, NULL, argument, TO_CHAR );
    }
    else
    {
        act( "{W[{Demote{W] {B$n $T{x", ch, NULL, argument, TO_ROOM );
        act( "{W[{Demote{W] {B$n $T{x", ch, NULL, argument, TO_CHAR );
    }

    MOBtrigger = TRUE;
    return;
}


void do_pmote( CHAR_DATA *ch, char *argument )
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
 
    act( "{W[{Demote{W] {c$n $t{x", ch, argument, NULL, TO_CHAR );

    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (vch->desc == NULL || vch == ch)
	    continue;

	if ((letter = strstr(argument,vch->name)) == NULL)
	{
	    MOBtrigger = FALSE;	
	    act("{W[{Demote{W] {c$N $t{x",vch,argument,ch,TO_CHAR);
	    MOBtrigger = TRUE;
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

	MOBtrigger = FALSE;
	act("{W[{Demote{W] {c$N $t{x",vch,temp,ch,TO_CHAR);
	MOBtrigger = TRUE;
    }
	
    return;
}


/*
 * All the posing stuff.
 */
struct	pose_table_type
{
    char *	message[2*MAX_CLASS];
};

const	struct	pose_table_type	pose_table	[]	=
{
    {
	{
	    "You sizzle with {Be{bn{Be{br{Bg{by{x.",
	    "$n sizzles with {Be{bn{Be{br{Bg{by{x.",
	    "You feel very {Wholy{x.",
	    "$n looks very {Wholy{x.",
	    "You perform a small card trick.",
	    "$n performs a small card trick.",
	    "You show your bulging muscles.",
	    "$n shows $s pitiful muscles.",
	    "You nonchalantly turn wine into water.",
	    "$n nonchalantly turns wine into water.",
	    "... 98, 99, 100 ... you do pushups.",
	    "..1....mmph!.. 2 .. $n does pushups.",
	    "You deftly steal everyone's weapon.",
	    "$n deftly steals your weapon."
	}
    },

    {
	{
	    "You turn into a butterfly, then return to your normal shape.",
	    "$n turns into a butterfly, then returns to $s normal shape.",
	    "You nonchalantly turn wine into water.",
	    "$n nonchalantly turns wine into water.",
	    "You wiggle your ears alternately.",
	    "$n wiggles $s ears alternately.",
	    "You crack nuts between your fingers.",
	    "$n cracks nuts between $s fingers.",
	    "A {Whalo{x appears over your head.",
	    "A {Whalo{x appears over $n's head.",
	    "Deep in prayer, you levitate.",
	    "Deep in prayer, $n levitates.",
	    "You steal the underwear off every person in the room.",
	    "Your underwear is gone!  $n stole it!"
	}
    },

    {
	{
	    "{BBlue sparks{x fly from your fingers.",
	    "{BBlue sparks{x fly from $n's fingers.",
	    "A {Whalo{x appears over your head.",
	    "A {Whalo{x appears over $n's head.",
	    "You nimbly tie yourself into a knot.",
	    "$n nimbly ties $mself into a knot.",
	    "You grizzle your teeth and look mean.",
	    "$n grizzles $s teeth and looks mean.",
	    "Crunch, crunch -- you munch a bottle.",
	    "Crunch, crunch -- $n munches a bottle.",
	    "You juggle with daggers, apples, and eyeballs.",
	    "$n juggles with daggers, apples, and eyeballs.",
	    "Little {rred{x lights dance in your {z{Re{xy{z{Re{xs.",
	    "Little {rred{x lights dance in $n's {z{Re{xy{z{Re{xs."
	}
    },

    {
	{
	    "Little {rred{x lights dance in your {z{Re{xy{z{Re{xs.",
	    "Little {rred{x lights dance in $n's {z{Re{xy{z{Re{xs.",
	    "You recite words of wisdom.",
	    "$n recites words of wisdom.",
	    "You juggle with daggers, apples, and eyeballs.",
	    "$n juggles with daggers, apples, and eyeballs.",
	    "You hit your head, and your eyes roll.",
	    "$n hits $s head, and $s eyes roll.",
	    "You nimbly tie yourself into a knot.",
	    "$n nimbly ties $mself into a knot.",
	    "A spot light hits you.",
	    "A spot light hits $n.",
	    "You count the money in everyone's pockets.",
	    "Check your money, $n is counting it."
	}
    },

    {
	{
	    "A slimy {ggreen monster{x appears before you and bows.",
	    "A slimy {ggreen monster{x appears before $n and bows.",
	    "Deep in prayer, you levitate.",
	    "Deep in prayer, $n levitates.",
	    "You steal the underwear off every person in the room.",
	    "Your underwear is gone!  $n stole it!",
	    "Crunch, crunch -- you munch a bottle.",
	    "Crunch, crunch -- $n munches a bottle.",
	    "A fire elemental singes your hair.",
	    "A fire elemental singes $n's hair.",
	    "The sun pierces through the clouds to illuminate you.",
	    "The sun pierces through the clouds to illuminate $n.",
	    "Your eyes dance with greed.",
	    "$n's eyes dance with greed."
	}
    },

    {
	{
	    "You turn everybody into a little pink elephant.",
	    "You are turned into a little pink elephant by $n.",
	    "An {Wangel{x consults you.",
	    "An {Wangel{x consults $n.",
	    "The dice roll ... and you win again.",
	    "The dice roll ... and $n craps out.",
	    "... 98, 99, 100 ... you do pushups.",
	    "..1....mmph!.. 2 .. $n does pushups.",
	    "Smoke and fumes leak from your nostrils.",
	    "Smoke and fumes leak from $n's nostrils.",
	    "You balance a pocket knife on your tongue.",
	    "$n balances a pocket knife on your tongue.",
	    "Watch your feet, you are juggling granite boulders.",
	    "Watch your feet, $n is juggling granite boulders."
	}
    },

    {
	{
	    "A small ball of light dances on your fingertips.",
	    "A small ball of light dances on $n's fingertips.",
	    "Your body {Yglows with an unearthly light{x.",
	    "$n's body {Yglows with an unearthly light{x.",
	    "You count the money in everyone's pockets.",
	    "Check your money, $n is counting it.",
	    "Arnold Schwarzenegger admires your physique.",
	    "PeeWee Herman laughs at $n's physique.",
	    "Everyone levitates as you pray.",
	    "You levitate as $n prays.",
	    "You produce a coin from everyone's ear.",
	    "$n produces a coin from your ear.",
	    "Oomph!  You squeeze water out of a granite boulder.",
	    "Oomph!  $n squeezes water out of a granite boulder."
	}
    },

    {
	{
	    "Smoke and fumes leak from your nostrils.",
	    "Smoke and fumes leak from $n's nostrils.",
	    "A spot light hits you.",
	    "A spot light hits $n.",
	    "You balance a pocket knife on your tongue.",
	    "$n balances a pocket knife on your tongue.",
	    "Watch your feet, you are juggling granite boulders.",
	    "Watch your feet, $n is juggling granite boulders.",
	    "A {Ccool breeze{x refreshes you.",
	    "A {Ccool breeze{x refreshes $n.",
	    "You step behind your {Dshadow{x.",
	    "$n steps behind $s {Dshadow{x.",
	    "Arnold Schwarzenegger admires your physique.",
	    "PeeWee Herman laughs at $n's physique."
	}
    },

    {
	{
	    "The light flickers as you rap in magical languages.",
	    "The light flickers as $n raps in magical languages.",
	    "Everyone levitates as you pray.",
	    "You levitate as $n prays.",
	    "You produce a coin from everyone's ear.",
	    "$n produces a coin from your ear.",
	    "Oomph!  You squeeze water out of a granite boulder.",
	    "Oomph!  $n squeezes water out of a granite boulder.",
	    "The sky changes {Rc{Co{Gl{Bo{Yr{x to match your eyes.",
	    "The sky changes {Rc{Co{Gl{Bo{Yr{x to match $n's eyes.",
	    "The ocean parts before you.",
	    "The ocean parts before $n.",
	    "Your head disappears.",
	    "$n's head disappears. {D(I was tired of looking at it anyway){x"
	}
    },

    {
	{
	    "Your head disappears.",
	    "$n's head disappears. {D(I was tired of looking at it anyway){x",
	    "A {Ccool breeze{x refreshes you.",
	    "A {Ccool breeze{x refreshes $n.",
	    "You step behind your {Dshadow{x.",
	    "$n steps behind $s {Dshadow{x.",
	    "You pick your teeth with a spear.",
	    "$n picks $s teeth with a spear.",
	    "The stones dance to your command.",
	    "The stones dance to $n's command.",
	    "A thunder cloud kneels to you.",
	    "A thunder cloud kneels to $n.",
	    "The Grey Mouser buys you a beer.",
	    "The Grey Mouser buys $n a beer."
	}
    },

    {
	{
	    "A fire elemental singes your hair.",
	    "A fire elemental singes $n's hair.",
	    "The sun pierces through the clouds to illuminate you.",
	    "The sun pierces through the clouds to illuminate $n.",
	    "Your eyes dance with greed.",
	    "$n's eyes dance with greed.",
	    "Everyone is swept off their foot by your hug.",
	    "You are swept off your feet by $n's hug.",
	    "Your head disappears.",
	    "$n's head disappears. {D(I was tired of looking at it anyway){x",
	    "A {Ccool breeze{x refreshes you.",
	    "A {Ccool breeze{x refreshes $n.",
	    "You step behind your {Dshadow{x.",
	    "$n steps behind $s {Dshadow{x."
	}
    },

    {
	{
	    "The sky changes {Rc{Co{Gl{Bo{Yr{x to match your eyes.",
	    "The sky changes {Rc{Co{Gl{Bo{Yr{x to match $n's eyes.",
	    "The ocean parts before you.",
	    "The ocean parts before $n.",
	    "You deftly steal everyone's weapon.",
	    "$n deftly steals your weapon.",
	    "Your karate chop splits a tree.",
	    "$n's karate chop breaks $s hand.",
	    "A thunder cloud kneels to you.",
	    "A thunder cloud kneels to $n.",
	    "The Grey Mouser buys you a beer.",
	    "The Grey Mouser buys $n a beer.",
	    "A strap of your armor breaks over your mighty thews.",
	    "A strap of $n's armor breaks over $s rolls of fat."
	}
    },

    {
	{
	    "The stones dance to your command.",
	    "The stones dance to $n's command.",
	    "A thunder cloud kneels to you.",
	    "A thunder cloud kneels to $n.",
	    "The Grey Mouser buys you a beer.",
	    "The Grey Mouser buys $n a beer.",
	    "A strap of your armor breaks over your mighty thews.",
	    "A strap of $n's armor breaks over $s rolls of fat.",
	    "The heavens and grass change colour as you smile.",
	    "The heavens and grass change colour as $n smiles.",
	    "The Burning Man speaks to you.",
	    "The Burning Man speaks to $n.",
	    "Everyone's pocket explodes with your fireworks.",
	    "Your pocket explodes with $n's fireworks."
	}
    },

    {
	{
	    "The heavens and grass change colour as you smile.",
	    "The heavens and grass change colour as $n smiles.",
	    "The Burning Man speaks to you.",
	    "The Burning Man speaks to $n.",
	    "Everyone's pocket explodes with your fireworks.",
	    "Your pocket explodes with $n's fireworks.",
	    "A boulder cracks at your frown.",
	    "A boulder cracks at $n's frown.",
	    "Everyone's clothes are transparent, and you are laughing.",
	    "Your clothes are transparent, and $n is laughing.",
	    "An eye in a pyramid w{zi{xnks at you.",
	    "An eye in a pyramid w{zi{xnks at $n.",
	    "Everyone discovers your dagger a centimeter from their eye.",
	    "You discover $n's dagger a centimeter from your eye."
	}
    },

    {
	{
	    "Everyone's clothes are transparent, and you are laughing.",
	    "Your clothes are transparent, and $n is laughing.",
	    "An eye in a pyramid w{zi{xnks at you.",
	    "An eye in a pyramid w{zi{xnks at $n.",
	    "Everyone discovers your dagger a centimeter from their eye.",
	    "You discover $n's dagger a centimeter from your eye.",
	    "Mercenaries arrive to do your bidding.",
	    "Mercenaries arrive to do $n's bidding.",
	    "A black hole swallows you.",
	    "A black hole swallows $n.",
	    "Valentine Michael Smith offers you a glass of water.",
	    "Valentine Michael Smith offers $n a glass of water.",
	    "Where did you go?",
	    "Where did $n go?"
	}
    },

    {
	{
	    "A black hole swallows you.",
	    "A black hole swallows $n.",
	    "Valentine Michael Smith offers you a glass of water.",
	    "Valentine Michael Smith offers $n a glass of water.",
	    "Where did you go?",
	    "Where did $n go?",
	    "Four matched Percherons bring in your chariot.",
	    "Four matched Percherons bring in $n's chariot.",
	    "The world shimmers in time with your whistling.",
	    "The world shimmers in time with $n's whistling.",
	    "The great god $G gives you a staff.",
	    "The great god $G gives $n a staff.",
	    "Everyone's clothes are transparent, and you are laughing.",
	    "Your clothes are transparent, and $n is laughing."
	}
    },

    {
	{
	    "The world shimmers in time with your whistling.",
	    "The world shimmers in time with $n's whistling.",
	    "The great god $G gives you a staff.",
	    "The great god $G gives $n a staff.",
	    "Click.",
	    "Click.",
	    "Atlas asks you to relieve him.",
	    "Atlas asks $n to relieve him.",
	    "Four matched Percherons bring in your chariot.",
	    "Four matched Percherons bring in $n's chariot.",
	    "Mercenaries arrive to do your bidding.",
	    "Mercenaries arrive to do $n's bidding.",
	    "A boulder cracks at your frown.",
	    "A boulder cracks at $n's frown."
	}
    }
};



void do_pose( CHAR_DATA *ch, char *argument )
{
    int level;
    int pose;

    if ( IS_NPC(ch) )
	return;

    level = UMIN( ch->level, sizeof(pose_table) / sizeof(pose_table[0]) - 1 );
    pose  = number_range(0, level);

    if (ch->class < MAX_CLASS/2)
    {
	act( pose_table[pose].message[2*ch->class+0], ch, NULL, NULL, TO_CHAR );
	act( pose_table[pose].message[2*ch->class+1], ch, NULL, NULL, TO_ROOM );
    } else {
	act( pose_table[pose].message[2*(ch->class-(MAX_CLASS/2))+0], ch, NULL, NULL, TO_CHAR );
	act( pose_table[pose].message[2*(ch->class-(MAX_CLASS/2))+1], ch, NULL, NULL, TO_ROOM );
    }
    return;
}



void do_bug( CHAR_DATA *ch, char *argument )
{
    append_file( ch, BUG_FILE, argument );
    send_to_char( "Bug logged.\n\r", ch );
    return;
}

void do_typo( CHAR_DATA *ch, char *argument )
{
    append_file( ch, TYPO_FILE, argument );
    send_to_char( "Typo logged.\n\r", ch );
    return;
}

void do_rent( CHAR_DATA *ch, char *argument )
{
    send_to_char( "There is no rent here.  Just save and quit.\n\r", ch );
    return;
}


void do_qui( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to QUIT, you have to spell it out.\n\r", ch );
    return;
}



void do_quit( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d,*d_next;
    int id;

    if ( IS_NPC(ch) )
	return;

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "No way! You are fighting.\n\r", ch );
	return;
    }

    if ( ch->position  < POS_STUNNED  )
    {
	send_to_char( "You're not DEAD yet.\n\r", ch );
	return;
    }

    send_to_char( "{rReality is attacking you!{x\n\r",ch);
    send_to_char( "{cYou hit Reality hard!{x\n\r",ch);
    send_to_char( "{BReality's truth does UNSPEAKABLE things to you!{x\n\r\n\r",ch);

    WAIT_STATE(ch, 25 * PULSE_VIOLENCE);
    act_new_2( "$n has left the game.", ch, NULL, NULL, TO_ROOM, POS_RESTING, 0, ACTION_MUST_SEE );
    sprintf( log_buf, "%s has quit.", ch->name );
    log_string( LOG_GAME, log_buf );
     wiznet("$N rejoins the real world.",ch,NULL,WIZ_LOGINS,0,get_trust(ch));

    /*
     * After extract_char the ch is no longer valid!
     */
    save_char_obj( ch );

    /* Free note that might be there somehow */
    if (ch->pcdata->in_progress)
	free_note (ch->pcdata->in_progress);

    id = ch->id;
    d = ch->desc;
    extract_char( ch, TRUE );
    if ( d != NULL )
	close_socket( d );

    /* toast evil cheating bastards */
    for (d = descriptor_list; d != NULL; d = d_next)
    {
	CHAR_DATA *tch;

	d_next = d->next;
	tch = d->original ? d->original : d->character;
	if (tch && tch->id == id)
	{
	    extract_char(tch,TRUE);
	    close_socket(d);
	} 
    }

    return;
}

void force_quit( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d,*d_next;
    int id;
    int lose;

    if ( IS_NPC(ch) )
	return;

    if ( ch->level >= LEVEL_IMMORTAL )
	return;


    if ( ch->position == POS_FIGHTING )
    {
	lose = (ch->desc != NULL) ? 25 : 50;
	gain_exp( ch, 0 - lose );
	stop_fighting( ch, TRUE );
    }

    if ( ch->position  < POS_STUNNED  )
    {
	lose = (ch->desc != NULL) ? 50 : 100;
	gain_exp( ch, 0 - lose );
    }
    send_to_char( "{rReality is attacking you!{x\n\r",ch);
    send_to_char( "{cYou hit Reality hard!{x\n\r",ch);
    send_to_char( "{BReality's truth does UNSPEAKABLE things to you!{x\n\r\n\r",ch);
    WAIT_STATE(ch,25 * PULSE_VIOLENCE);
    act( "$n has left the game.", ch, NULL, NULL, TO_ROOM );
    sprintf( log_buf, "%s has quit.", ch->name );
    log_string( LOG_GAME, log_buf );
     wiznet("$N rejoins the real world.",ch,NULL,WIZ_LOGINS,0,get_trust(ch));

    /*
     * After extract_char the ch is no longer valid!
     */
    save_char_obj( ch );
    id = ch->id;
    d = ch->desc;
    extract_char( ch, TRUE );
    if ( d != NULL )
	close_socket( d );

    /* toast evil cheating bastards */
    for (d = descriptor_list; d != NULL; d = d_next)
    {
	CHAR_DATA *tch;

	d_next = d->next;
	tch = d->original ? d->original : d->character;
	if (tch && tch->id == id)
	{
	    extract_char(tch,TRUE);
	    close_socket(d);
	} 
    }

    return;
}


void do_save( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    save_char_obj( ch );
    send_to_char("Saving. Remember that ROT has automatic saving.\n\r", ch);
    WAIT_STATE(ch,PULSE_VIOLENCE/2);
    return;
}



void do_follow( CHAR_DATA *ch, char *argument )
{
/* RT changed to allow unlimited following and follow the NOFOLLOW rules */
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Follow whom? (Type {Bfollow self{x to stop following someone.)\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL )
    {
	act( "But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR );
	return;
    }

    if ( victim == ch )
    {
	if ( ch->master == NULL )
	{
	    send_to_char( "You already follow yourself.\n\r", ch );
	    return;
	}
	stop_follower(ch);
	return;
    }

    if (!IS_NPC(victim) && IS_SET(victim->act,PLR_NOFOLLOW) && !IS_IMMORTAL(ch))
    {
	act("$N doesn't seem to want any followers.\n\r",
             ch,NULL,victim, TO_CHAR);
        return;
    }

    if ( ( !IS_NPC( ch ) && !IS_NPC( victim ) )
    && ( !IS_IMMORTAL( ch ) )
    && ( !IS_IMMORTAL( victim ) )
    && ( ch != victim )
    && ( !strcmp(ch->pcdata->socket, victim->pcdata->socket ) ) )
    {
	act("You can't seem to follow $N.\n\r",ch,NULL,victim,TO_CHAR);
	return;
    }

    if(ch->in_arena && victim->in_arena
    && ch->arena_team != victim->arena_team)
    {
        send_to_char("You're not on the same team.\n\r",ch);
        return;
    }

    REMOVE_BIT(ch->act,PLR_NOFOLLOW);
    
    if ( ch->master != NULL )
	stop_follower( ch );

    add_follower( ch, victim );
    return;
}


void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
    if ( ch->master != NULL )
    {
	bug( "Add_follower: non-null master.", 0 );
	return;
    }

    ch->master        = master;
    ch->leader        = NULL;

    if ( can_see( master, ch ) )
	act( "$n now follows you.", ch, NULL, master, TO_VICT );

    act( "You now follow $N.",  ch, NULL, master, TO_CHAR );

    return;
}

void stop_follower( CHAR_DATA *ch )
{
    if ( ch->master == NULL )
    {
	bug( "Stop_follower: null master.", 0 );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) )
    {
	REMOVE_BIT( ch->affected_by, AFF_CHARM );
	affect_strip( ch, gsn_charm_person );
    }
    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_PET) )
    {
	REMOVE_BIT(ch->act, ACT_PET);
	ch->timer = 0;
	ch->hastimer = TRUE;
    }

    if ( can_see( ch->master, ch ) && ch->in_room != NULL)
    {
	act( "$n stops following you.",     ch, NULL, ch->master, TO_VICT    );
    	act( "You stop following $N.",      ch, NULL, ch->master, TO_CHAR    );
    }
    if (ch->master->pet == ch)
	ch->master->pet = NULL;

    ch->master = NULL;
    ch->leader = NULL;
    return;
}

/* nukes charmed monsters and pets */
void nuke_pets( CHAR_DATA *ch )
{    
    CHAR_DATA *pet;

    if ((pet = ch->pet) != NULL)
    {
    	stop_follower(pet);
    	if (pet->in_room != NULL)
    	    act("$N slowly fades away.",ch,NULL,pet,TO_NOTVICT);
    	extract_char(pet,TRUE);
    }
    ch->pet = NULL;

    return;
}



void die_follower( CHAR_DATA *ch )
{
    CHAR_DATA *fch;

    if ( ch->master != NULL )
    {
    	if (ch->master->pet == ch)
    	    ch->master->pet = NULL;
	stop_follower( ch );
    }

    ch->leader = NULL;

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
	if ( fch->master == ch )
	    stop_follower( fch );
	if ( fch->leader == ch )
	    fch->leader = fch;
    }

    return;
}



void do_order( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    bool found;
    bool fAll;

    argument = one_argument( argument, arg );
    one_argument(argument,arg2);

    if (!str_cmp(arg2,"delete") || !str_cmp(arg2,"mob"))
    {
        send_to_char("That will NOT be done.\n\r",ch);
        return;
    }

    if (!str_cmp(arg2,"reroll"))
    {
	send_to_char("That will NOT be done.\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Order whom to do what?\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You feel like taking, not giving, orders.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	fAll   = TRUE;
	victim = NULL;
    }
    else
    {
	fAll   = FALSE;
	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\n\r", ch );
	    return;
	}

	if (!IS_AFFECTED(victim, AFF_CHARM) || victim->master != ch 
	||  (IS_IMMORTAL(victim) && get_trust(victim) >= get_trust(ch))
        ||  IS_IMMORTAL(CH_ORIGINAL(ch)))
	{
	    send_to_char( "Do it yourself!\n\r", ch );
	    return;
	}
    }

    found = FALSE;
    for ( och = ch->in_room->people; och != NULL; och = och_next )
    {
	och_next = och->next_in_room;

	if ( IS_AFFECTED(och, AFF_CHARM)
	&&   och->master == ch
	&& ( fAll || och == victim ) )
	{
	    found = TRUE;
	    sprintf( buf, "$n orders you to '%s'.", argument );
	    act( buf, ch, NULL, och, TO_VICT );
	    interpret( och, argument );
	}
    }

    if ( found )
    {
	WAIT_STATE(ch,PULSE_VIOLENCE);
	send_to_char( "Ok.\n\r", ch );
    }
    else
	send_to_char( "You have no followers here.\n\r", ch );
    return;
}

void do_group( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	CHAR_DATA *gch;
	CHAR_DATA *leader;

	leader = (ch->leader != NULL) ? ch->leader : ch;
	sprintf( buf, "%s's group:\n\r", PERS(leader, ch) );
	send_to_char( buf, ch );

	for ( gch = char_list; gch != NULL; gch = gch->next )
	{
	    if ( is_same_group( gch, ch ) )
	    {
		sprintf( buf,
		"[%2d %s] %-16s %4d/%4d hp %4d/%4d mana %4d/%4d mv %5ld xp\n\r",
		    gch->level,
		    IS_NPC(gch) ? "Mob" : class_table[gch->class].who_name,
		    capitalize( PERS(gch, ch) ),
		    gch->hit,   gch->max_hit,
		    gch->mana,  gch->max_mana,
		    gch->move,  gch->max_move,
		    gch->exp    );
		send_to_char( buf, ch );
	    }
	}
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch->master != NULL || ( ch->leader != NULL && ch->leader != ch ) )
    {
	send_to_char( "But you are following someone else!\n\r", ch );
	return;
    }

    if ( victim->master != ch && ch != victim )
    {
	act_new( "$N isn't following you.", ch, NULL, victim, TO_CHAR, POS_SLEEPING );
	return;
    }
    
    if (IS_AFFECTED(victim,AFF_CHARM))
    {
        send_to_char("You can't remove charmed mobs from your group.\n\r",ch);
        return;
    }
    
    if (IS_AFFECTED(ch,AFF_CHARM))
    {
    	act("You like your master too much to leave $m!",ch,NULL,victim,TO_VICT);
    	return;
    }

    if (victim->level - ch->level > 14)
    {
	send_to_char("They are to high of a level for your group.\n\r",ch);
	return;
    }

    if (victim->level - ch->level < -14)
    {
	send_to_char("They are to low of a level for your group.\n\r",ch);
	return;
    }

    if ( is_same_group( victim, ch ) && ch != victim )
    {
	victim->leader = NULL;
	act( "$n removes $N from $s group.",   ch, NULL, victim, TO_NOTVICT );
	act( "$n removes you from $s group.",  ch, NULL, victim, TO_VICT    );
	act( "You remove $N from your group.", ch, NULL, victim, TO_CHAR    );
	return;
    }

    if ( ( !IS_NPC( ch ) && !IS_NPC( victim ) )
    && ( !IS_IMMORTAL( ch ) )
    && ( !IS_IMMORTAL( victim ) )
    && ( ch != victim )
    && ( !strcmp(ch->pcdata->socket, victim->pcdata->socket ) ) )
    {
	send_to_char("They are unable to join your group.\n\r",ch);
	return;
    }

    victim->leader = ch;
    act( "$N joins $n's group.", ch, NULL, victim, TO_NOTVICT );
    act( "You join $n's group.", ch, NULL, victim, TO_VICT    );
    act( "$N joins your group.", ch, NULL, victim, TO_CHAR    );
    return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    int members;
    int amount_platinum = 0, amount_gold = 0, amount_silver = 0;
    int share_platinum, share_gold, share_silver;
    int extra_platinum, extra_gold, extra_silver;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
               one_argument( argument, arg3 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Split how much?\n\r", ch );
	return;
    }
    
    amount_silver = atoi( arg1 );

    if (arg2[0] != '\0')
	amount_gold = atoi(arg2);

    if (arg3[0] != '\0')
	amount_platinum = atoi(arg3);

    if ( amount_platinum < 0 || amount_gold < 0 || amount_silver < 0)
    {
	send_to_char( "Your group wouldn't like that.\n\r", ch );
	return;
    }

    if ( amount_platinum == 0 && amount_gold == 0 && amount_silver == 0 )
    {
	send_to_char( "You hand out zero coins, but no one notices.\n\r", ch );
	return;
    }

    if ( (ch->silver + (ch->gold * 100) + (ch->platinum * 10000) )
       < (amount_silver + (amount_gold * 100) + (amount_platinum * 10000) ) )
    {
	send_to_char( "You don't have that much to split.\n\r", ch );
	return;
    }
  
    members = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) && !IS_AFFECTED(gch,AFF_CHARM))
	    members++;
    }

    if ( members < 2 )
    {
	send_to_char( "Just keep it all.\n\r", ch );
	return;
    }
	    
    share_platinum   = amount_platinum / members;
    extra_platinum   = amount_platinum % members;

    amount_gold += (extra_platinum * 100);
    share_gold   = amount_gold / members;
    extra_gold   = amount_gold % members;

    amount_silver += (extra_gold * 100);
    share_silver = amount_silver / members;
    extra_silver = amount_silver % members;

    if ( share_platinum == 0 && share_gold == 0 && share_silver == 0 )
    {
	send_to_char( "Don't even bother, cheapskate.\n\r", ch );
	return;
    }

    deduct_cost(ch,amount_platinum-extra_platinum,VALUE_PLATINUM);
    add_cost(ch,share_platinum,VALUE_PLATINUM);
    deduct_cost(ch,amount_gold-extra_gold,VALUE_GOLD);
    add_cost(ch,share_gold,VALUE_GOLD);
    deduct_cost(ch,amount_silver,VALUE_SILVER);
    add_cost(ch,share_silver+extra_silver,VALUE_SILVER);

    if (share_platinum > 0)
    {
	sprintf(buf,
	    "You split %d platinum coins. Your share is %d platinum.\n\r",
	     amount_platinum-extra_platinum,share_platinum);
	send_to_char(buf,ch);
    }
    if (share_gold > 0)
    {
	sprintf(buf,
	    "You split %d gold coins. Your share is %d gold.\n\r",
	     amount_gold-extra_gold,share_gold);
	send_to_char(buf,ch);
    }
    if (share_silver > 0)
    {
	sprintf(buf,
	    "You split %d silver coins. Your share is %d silver.\n\r",
 	    amount_silver,share_silver + extra_silver);
	send_to_char(buf,ch);
    }

    if (share_gold == 0 && share_silver == 0)
    {
	sprintf(buf,"$n splits %d platinum coins. Your share is %d platinum.",
		amount_platinum-extra_platinum,share_platinum);
    }
    else if (share_platinum == 0 && share_silver == 0)
    {
	sprintf(buf,"$n splits %d gold coins. Your share is %d gold.",
		amount_gold-extra_gold,share_gold);
    }
    else if (share_platinum == 0 && share_gold == 0)
    {
	sprintf(buf,"$n splits %d silver coins. Your share is %d silver.",
		amount_silver,share_silver);
    }
    else if (share_silver == 0)
    {
	sprintf(buf,
"$n splits %d platinum and %d gold coins. giving you %d platinum and %d gold.\n\r",
	 amount_platinum-extra_platinum, amount_gold-extra_gold,
	 share_platinum, share_gold);
    }
    else if (share_gold == 0)
    {
	sprintf(buf,
"$n splits %d platinum and %d silver coins. giving you %d platinum and %d silver.\n\r",
	 amount_platinum-extra_platinum, amount_silver,
	 share_platinum, share_silver);
    }
    else if (share_platinum == 0)
    {
	sprintf(buf,
"$n splits %d gold and %d silver coins. giving you %d gold and %d silver.\n\r",
	 amount_gold-extra_gold, amount_silver,
	 share_gold, share_silver);
    }
    else
    {
	sprintf(buf,
"$n splits %d platinum, %d gold and %d silver coins. giving you %d platinum, %d gold and %d silver.\n\r",
	 amount_platinum-extra_platinum, amount_gold-extra_gold, amount_silver,
	 share_platinum, share_gold, share_silver);
    }

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( gch != ch && is_same_group(gch,ch) && !IS_AFFECTED(gch,AFF_CHARM))
	{
	    act( buf, ch, NULL, gch, TO_VICT );
	    add_cost(gch,share_platinum,VALUE_PLATINUM);
	    add_cost(gch,share_gold,VALUE_GOLD);
	    add_cost(gch,share_silver,VALUE_SILVER);
	}
    }

    return;
}



void do_gtell( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Tell your group what?\n\r", ch );
	return;
    }

    if ( IS_SET( ch->comm, COMM_NOTELL ) )
    {
	send_to_char( "Your message didn't get through!\n\r", ch );
	return;
    }

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER)
    && (!IS_IMMORTAL(ch)))
    {
	send_to_char("Just keep your nose in the corner like a good little player.\n\r",ch);
	return;
    }

    /*
     * Note use of send_to_char, so gtell works on sleepers.
     */
    sprintf( buf, "%s tells the group '{K%s{x'\n\r", ch->name, argument );
    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
	if ( is_same_group( gch, ch ) )
	    send_to_char( buf, gch );
    }

    return;
}



/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch )
{
    if ( ach == NULL || bch == NULL)
	return FALSE;

    if( ach->in_arena && bch->in_arena && ach->arena_team != bch->arena_team )
        return FALSE;

    if ( ach->leader != NULL ) ach = ach->leader;
    if ( bch->leader != NULL ) bch = bch->leader;
    return ach == bch;
}

/*
 * Colour setting and unsetting, way cool, Lope Oct '94
 */
void do_colour( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_STRING_LENGTH ];
    int  ccolor;

    argument = one_argument( argument, arg );

    if( !*arg )
    {
	if( !IS_SET( ch->act, PLR_COLOUR ) )
	{
	    SET_BIT( ch->act, PLR_COLOUR );
	    send_to_char( "{bC{ro{yl{co{mu{gr{x is now {rON{x, Way Cool!\n\r", ch );
	}
	else
	{
	    send_to_char_bw( "Colour is now OFF, <sigh>\n\r", ch );
	    REMOVE_BIT( ch->act, PLR_COLOUR );
	}
    }
    else if (!str_prefix(arg,"list"))
    {
	send_to_char( "\n\rColors:\n\r",ch);
	send_to_char( "     0 - Reset           9 - Bright Red\n\r",ch);
	send_to_char( "     1 - Red            10 - Bright Green\n\r",ch);
	send_to_char( "     2 - Green          11 - Yellow\n\r",ch);
	send_to_char( "     3 - Brown          12 - Bright Blue\n\r",ch);
	send_to_char( "     4 - Blue           13 - Bright Magenta\n\r",ch);
	send_to_char( "     5 - Magenta        14 - Bright Cyan\n\r",ch);
	send_to_char( "     6 - Cyan           15 - Bright White\n\r",ch);
	send_to_char( "     7 - White          16 - Black\n\r",ch);
	send_to_char( "     8 - Grey           17 - None\n\r",ch);
	send_to_char( "Channels:\n\r",ch);
	send_to_char( "     auction    cgossip    clan\n\r",ch);
	send_to_char( "     gossip     grats      gtell\n\r",ch);
	send_to_char( "     immtalk    music      ask\n\r",ch);
	send_to_char( "     quote      say        shout\n\r",ch);
	send_to_char( "     tell       wiznet     mobsay\n\r",ch);
	send_to_char( "     room       condition  fight\n\r",ch);
	send_to_char( "     opponent   witness    disarm\n\r",ch);
	send_to_char( "     qgossip\n\r",ch);
	send_to_char( "For a more detailed list, see HELP COLORS\n\r",ch);
	send_to_char( "For a list of current settings, see HELP SETTINGS\n\r",ch);
    }
    else if (!strcmp(arg,"0"))
    {
	ch->color = 0;
	send_to_char( "{xOK\n\r",ch);
    }
    else if (!strcmp(arg,"1")) 
    { 
        ch->color = 1; 
        send_to_char( "{xOK\n\r",ch); 
    } 
    else if (!strcmp(arg,"2")) 
    { 
        ch->color = 2; 
        send_to_char( "{xOK\n\r",ch); 
    } 
    else if (!strcmp(arg,"3")) 
    { 
        ch->color = 3; 
        send_to_char( "{xOK\n\r",ch); 
    } 
    else if (!strcmp(arg,"4")) 
    { 
        ch->color = 4; 
        send_to_char( "{xOK\n\r",ch); 
    } 
    else if (!strcmp(arg,"5")) 
    { 
        ch->color = 5; 
        send_to_char( "{xOK\n\r",ch); 
    } 
    else if (!strcmp(arg,"6")) 
    { 
        ch->color = 6; 
        send_to_char( "{xOK\n\r",ch); 
    } 
    else if (!strcmp(arg,"7")) 
    { 
        ch->color = 7; 
        send_to_char( "{xOK\n\r",ch); 
    } 
    else if (!strcmp(arg,"8")) 
    { 
        ch->color = 8; 
        send_to_char( "{xOK\n\r",ch); 
    } 
    else if (!strcmp(arg,"9")) 
    { 
        ch->color = 9; 
        send_to_char( "{xOK\n\r",ch); 
    } 
    else if (!strcmp(arg,"10")) 
    { 
        ch->color = 10; 
        send_to_char( "{xOK\n\r",ch); 
    } 
    else if (!strcmp(arg,"11")) 
    { 
        ch->color = 11; 
        send_to_char( "{xOK\n\r",ch); 
    } 
    else if (!strcmp(arg,"12")) 
    { 
        ch->color = 12; 
        send_to_char( "{xOK\n\r",ch); 
    } 
    else if (!strcmp(arg,"13")) 
    { 
        ch->color = 13; 
        send_to_char( "{xOK\n\r",ch); 
    } 
    else if (!strcmp(arg,"14")) 
    { 
        ch->color = 14; 
        send_to_char( "{xOK\n\r",ch); 
    } 
    else if (!strcmp(arg,"15")) 
    { 
        ch->color = 15; 
        send_to_char( "{xOK\n\r",ch); 
    } 
    else if (!strcmp(arg,"16")) 
    { 
        ch->color = 16; 
        send_to_char( "{xOK\n\r",ch); 
    } 
    else if (!strcmp(arg,"17")) 
    { 
        ch->color = 0; 
        send_to_char( "{xOK\n\r",ch); 
    } 
    else if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax: color {{list|#|<channel> #}\n\r", ch );
    }
    else if (!is_number( argument ) )
    {
	send_to_char( "Syntax: color {{list|#|<channel> #}\n\r", ch );
    }
    else
    {
	ccolor = atoi(argument);
	if (ccolor >= 18)
	{
	    send_to_char( "Color number must be 0-17\n\r", ch );
	    return;
	}
	if (!str_prefix(arg,"auction"))
	{
	    ch->color_auc = ccolor;
	    send_to_char( "auction channel set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"cgossip"))
	{
	    ch->color_cgo = ccolor;
	    send_to_char( "clan gossip channel set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"clan"))
	{
	    ch->color_cla = ccolor;
	    send_to_char( "clan talk channel set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"gossip"))
	{
	    ch->color_gos = ccolor;
	    send_to_char( "gossip channel set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"grats"))
	{
	    ch->color_gra = ccolor;
	    send_to_char( "grats channel set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"gtell"))
	{
	    ch->color_gte = ccolor;
	    send_to_char( "group tell channel set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"immtalk"))
	{
	    ch->color_imm = ccolor;
	    send_to_char( "immortal talk channel set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"music"))
	{
	    ch->color_mus = ccolor;
	    send_to_char( "music channel set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"ask"))
	{
	    ch->color_que = ccolor;
	    send_to_char( "question/answer channel set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"quote"))
	{
	    ch->color_quo = ccolor;
	    send_to_char( "quote channel set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"say"))
	{
	    ch->color_say = ccolor;
	    send_to_char( "say channel set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"shout"))
	{
	    ch->color_sho = ccolor;
	    send_to_char( "shout/yell channel set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"tell"))
	{
	    ch->color_tel = ccolor;
	    send_to_char( "tell/reply channel set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"wiznet"))
	{
	    ch->color_wiz = ccolor;
	    send_to_char( "wiznet channel set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"mobsay"))
	{
	    ch->color_mob = ccolor;
	    send_to_char( "mobile talk channel set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"room"))
	{
	    ch->color_roo = ccolor;
	    send_to_char( "room name display set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"condition"))
	{
	    ch->color_con = ccolor;
	    send_to_char( "character condition display set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"fight"))
	{
	    ch->color_fig = ccolor;
	    send_to_char( "your fight actions set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"opponent"))
	{
	    ch->color_opp = ccolor;
	    send_to_char( "opponents fight actions set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"disarm"))
	{
	    ch->color_dis = ccolor;
	    send_to_char( "disarm display set.\n\r",ch); 
	}
	else if (!str_prefix(arg,"witness"))
	{
	    ch->color_wit = ccolor;
	    send_to_char( "witness fight actions set.\n\r",ch); 
	}
        else if (!str_prefix(arg,"qgossip"))
        {
            ch->color_qgo = ccolor;
            send_to_char( "quest gossip channel set.\n\r",ch);
        }
	else
	{
	    send_to_char( "Syntax: color {{list|#|<channel> #}\n\r", ch );
	}

    }
    return;
}

/* Global Social ( Gossip Channel ) */

void do_gocial(CHAR_DATA *ch, char *argument)
{
    char command[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int cmd;
    bool found;
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];
/*
    int counter;
    int count;
    char buf2[MAX_STRING_LENGTH];

    send_to_char("Disabled due to bugs.\n\r",ch);
    if(!IS_IMMORTAL(ch))
     return;

*/

    argument = one_argument(argument,command);

    if (command[0] == '\0')
    {
	send_to_char("What do you wish to gocial?\n\r",ch);
	return;
    }

      if ((ch->in_room->vnum == ROOM_VNUM_CORNER)
      && (!IS_IMMORTAL(ch)))
      {
	send_to_char("Just keep your nose in the corner like a good little player.\n\r",ch);
	return;
      }


    found = FALSE;
    for (cmd = 0; social_table[cmd].name[0] != '\0'; cmd++)
    {
	if (command[0] == social_table[cmd].name[0]
	&& !str_prefix( command,social_table[cmd].name ) )
	{
	    found = TRUE;
	    break;
	}
    }

    if (!found)
    {
	send_to_char("What kind of social is that?!?!\n\r",ch);
	return;
    }

    if (!IS_NPC(ch) && IS_SET(ch->comm,   COMM_QUIET))
    {
	send_to_char("You must turn off quiet mode first.\n\r",ch);
	return;
    }

    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOGOSSIP))
    {
	send_to_char("But you have the gossip channel turned off!\n\r",ch);
	return;
    }

    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOCHANNELS))
    {
	send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
	return;
    }

    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE))
    {
	send_to_char("You have no emotions.\n\r",ch);
	return;
    }



    switch (ch->position)
    {
    case POS_DEAD:
	send_to_char("Lie still; you are DEAD!\n\r",ch);
	return;
    case POS_INCAP:
    case POS_MORTAL:
	send_to_char("You are hurt far too bad for that.\n\r",ch);
	return;
    case POS_STUNNED:
	send_to_char("You are too stunned for that.\n\r",ch);
	return;
    }

    one_argument(argument,arg);
    victim = NULL;
    if (arg[0] == '\0')
    {
	sprintf(buf, "{B[{GGoc{B]{x %s", social_table[cmd].char_no_arg );
	act_new_2(buf,ch,NULL,NULL,TO_CHAR,POS_DEAD,0,ACTION_GLOBAL);
        if(social_table[cmd].others_no_arg)
        {
	sprintf(buf, "{B[{GGoc{B]{x %s", social_table[cmd].others_no_arg );
	for (d = descriptor_list; d != NULL; d = d->next)
	{
	    CHAR_DATA *vch;
	    vch = d->original ? d->original : d->character;
	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(vch->comm,COMM_NOGOSSIP) &&
		!IS_SET(vch->comm,COMM_QUIET))
	    {
		act_new_2(buf,ch,NULL,vch,TO_VICT,POS_DEAD,0,ACTION_GLOBAL);
	    }
	}
        }
    }
    else if ((victim = get_char_world(ch,arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }
    else if (victim == ch)
    {
	sprintf(buf,"{B[{GGoc{B]{x %s", social_table[cmd].char_auto);
	act_new_2(buf,ch,NULL,NULL,TO_CHAR,POS_DEAD,0,ACTION_GLOBAL);
        if(social_table[cmd].others_auto)
        {
	sprintf(buf,"{B[{GGoc{B]{x %s", social_table[cmd].others_auto);
	for (d = descriptor_list; d != NULL; d = d->next)
	{
	    CHAR_DATA *vch;
	    vch = d->original ? d->original : d->character;
	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(vch->comm,COMM_NOGOSSIP) &&
		!IS_SET(vch->comm,COMM_QUIET))
	    {
		act_new_2(buf,ch,NULL,vch,TO_VICT,POS_DEAD, 0, ACTION_GLOBAL);
	    }
	}
        }
    }	        	
    else
    {
	sprintf(buf,"{B[{GGoc{B]{x %s", social_table[cmd].char_found);
	act_new_2(buf,ch,NULL,victim,TO_CHAR,POS_DEAD, 0, ACTION_GLOBAL);
	sprintf(buf,"{B[{GGoc{B]{x %s", social_table[cmd].vict_found);
	act_new_2(buf,ch,NULL,victim,TO_VICT,POS_DEAD, 0, ACTION_GLOBAL);

        if(social_table[cmd].char_no_arg)
        {
	sprintf(buf,"{B[{GGoc{B]{x %s", social_table[cmd].others_found);
/*	for (counter = 0; buf[counter+1] != '\0'; counter++)
	{
	    if (buf[counter] == '$' && buf[counter + 1] == 'N')
	    {
		strcpy(buf2,buf);
		buf2[counter] = '\0';
		strcat(buf2,!IS_NPC(victim) ? victim->name : victim->short_descr);
		for (count = 0; buf[count] != '\0'; count++)
		{
		    buf[count] = buf[count+counter+2];
		}
		strcat(buf2,buf);
		strcpy(buf,buf2);

	    }
	    else if (buf[counter] == '$' && buf[counter + 1] == 'E')
	    {
		switch (victim->sex)
		{
		default:
		    strcpy(buf2,buf);
		    buf2[counter] = '\0';
		    strcat(buf2,"it");
		    for (count = 0; buf[count] != '\0'; count ++)
		    {
			buf[count] = buf[count+counter+2];
		    }
		    strcat(buf2,buf);
		    strcpy(buf,buf2);
		    break;
		case 1:
		    strcpy(buf2,buf);
		    buf2[counter] = '\0';
		    strcat(buf2,"it");
		    for (count = 0; buf[count] != '\0'; count++)
		    {
			buf[count] = buf[count+counter+2];
		    }
		    strcat(buf2,buf);
		    strcpy(buf,buf2);
		    break;
		case 2:
		    strcpy(buf2,buf);
		    buf2[counter] = '\0';
		    strcat(buf2,"it");
		    for (count = 0; buf[count] != '\0'; count++)
		    {
			buf[count] = buf[count+counter+2];
		    }
		    strcat(buf2,buf);
		    strcpy(buf,buf2);
		    break;
		}
	    }    
	    else if (buf[counter] == '$' && buf[counter + 1] == 'M')
	    {
		buf[counter] = '%';
		buf[counter + 1] = 's';
		switch (victim->sex)
		{
		default:
  		    strcpy(buf2,buf);
		    buf2[counter] = '\0';
		    strcat(buf2,"it");
		    for (count = 0; buf[count] != '\0'; count++)
		    {
			buf[count] = buf[count+counter+2];
		    }
		    strcat(buf2,buf);
		    strcpy(buf,buf2);
		    break;
		case 1:
		    strcpy(buf2,buf);
		    buf2[counter] = '\0';
		    strcat(buf2,"him");
		    for (count = 0; buf[count] != '\0'; count++)
		    {
			buf[count] = buf[count+counter+2];
		    }
		    strcat(buf2,buf);
		    strcpy(buf,buf2);
		    break;
		case 2:
		    strcpy(buf2,buf);
		    buf2[counter] = '\0';
		    strcat(buf2,"her");
		    for (count = 0; buf[count] != '\0'; count++);
		    {
			buf[count] = buf[count+counter+2];
		    }
		    strcat(buf2,buf);
		    strcpy(buf,buf2);
		    break;
		}
	    }
	    else if (buf[counter] == '$' && buf[counter + 1] == 'S')
	    {
		switch (victim->sex)
		{
		default:
		strcpy(buf2,buf);
		buf2[counter] = '\0';
		strcat(buf2,"its");
		for (count = 0;buf[count] != '\0'; count++)
		{
		    buf[count] = buf[count+counter+2];
		}
		strcat(buf2,buf);
		strcpy(buf,buf2);
		break;
		case 1:
		    strcpy(buf2,buf);
		    buf2[counter] = '\0';
		    strcat(buf2,"his");
		    for (count = 0; buf[count] != '\0'; count++)
		    {
			buf[count] = buf[count+counter+2];
		    }
		    strcat(buf2,buf);
		    strcpy(buf,buf2);
		    break;
		case 2:
		    strcpy(buf2,buf);
		    buf2[counter] = '\0';
		    strcat(buf2,"hers");
		    for (count = 0; buf[count] != '\0'; count++)
		    {
			buf[count] = buf[count+counter+2];
		    }
		    strcat(buf2,buf);
		    strcpy(buf,buf2);
		    break;
		}
	    }

	}*/
/*
	for (d=descriptor_list; d != NULL; d = d->next)
	{
	    CHAR_DATA *vch;
	    vch = d->original ? d->original : d->character;
	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		d->character != victim &&
		!IS_SET(vch->comm, COMM_NOGOSSIP) &&
		!IS_SET(vch->comm,COMM_QUIET))
	    {
		act_new(buf,ch,NULL,vch,TO_VICT,POS_DEAD);
	    }
	}
*/
        }
    }
    return;
}

void do_titlelock (CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm2,COMM_TITLELOCK))
    {
      send_to_char("You will change your title as you level.\n\r",ch);
      REMOVE_BIT(ch->comm2,COMM_TITLELOCK);
    }
    else
    {
      send_to_char("You will keep your title as you level.\n\r",ch);
      SET_BIT(ch->comm2,COMM_TITLELOCK);
    }
}

void do_shout( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in_room;
    int door;
    char buf[MAX_STRING_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *from_name;

	/* Make the words drunk if needed */
	if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 3)
	argument = makedrunk(argument,ch);

/*
    if ( IS_SET(ch->comm, COMM_NOSHOUT) )
    {
        send_to_char( "You can't shout.\n\r", ch );
        return;
    }*/
 
    if ( argument[0] == '\0' )
    {
	send_to_char( "Shout what?\n\r", ch );
	return;
    }

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER)
    && (!IS_IMMORTAL(ch)))
    {
	send_to_char("Just keep your nose in the corner like a good little player.\n\r",ch);
	return;
    }

    foreign_speech(ch,argument,arg2,ch->primary_lang);
    sprintf(buf,"[%s] You shout '{S$T{x'",language_table[ch->primary_lang].lname);
    act_lang( buf, ch, NULL, arg2, TO_CHAR, POS_RESTING, ch->primary_lang );

    sprintf(buf,"[%s] $n shouts '{S$T{x'",language_table[ch->primary_lang].lname);
    act_lang( buf, ch, NULL, arg2, TO_ROOM, POS_RESTING, ch->primary_lang );


    was_in_room = ch->in_room;
    for ( door = 0; door <= 5; door++ )
    {
        EXIT_DATA *pexit;


        if ( ( pexit = was_in_room->exit[door] ) != NULL
        &&   pexit->u1.to_room != NULL
        &&   !IS_SET(pexit->exit_info, EX_SOUNDPROOF)
        &&   pexit->u1.to_room != was_in_room )
        {
            ch->in_room = pexit->u1.to_room;

        if(ch->in_room->exit[rev_dir[door]] == NULL)
            from_name = "somewhere";
        else
            from_name = dir_name_rel[rev_dir[door]];

    sprintf(buf,"[%s] From $t, $n shouts '{S$T{x'",language_table[ch->primary_lang].lname);
    act_lang( buf, ch, from_name, arg2,
              TO_ROOM, POS_RESTING, ch->primary_lang );

        }
    }
    ch->in_room = was_in_room;

    return;
}

void do_battlespam ( CHAR_DATA *ch, char * argument)
{
    if (IS_SET(ch->comm2,COMM_NOBATTLESPAM))
    {
      send_to_char("You will now see battle spam.\n\r",ch);
      REMOVE_BIT(ch->comm2,COMM_NOBATTLESPAM);
    }
   else
   {
     send_to_char("You will now see reduced battlespam.\n\r",ch);
     SET_BIT(ch->comm2,COMM_NOBATTLESPAM);
   }
}

void do_telnetga ( CHAR_DATA *ch, char * argument)
{
    if (IS_SET(ch->comm,COMM_TELNET_GA))
    {
      send_to_char("Telnet GA disabled.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_TELNET_GA);
    }
   else
   {
     send_to_char("Telnet GA enabled (you probably don't need this.)\n\r",ch);
     SET_BIT(ch->comm,COMM_TELNET_GA);
   }
}

void do_clantalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;

    if (!str_prefix(argument,"list") && argument[0] != '\0')
    {
        argument = one_argument(argument,arg);
        do_clanlist(ch,argument);
        return;
    }

    if (!is_clan(ch) || is_independent(ch))
    {
        send_to_char("You aren't in a clan.\n\r",ch);
        return;
    }
    if ( argument[0] == '\0' )
    {
        if (IS_SET(ch->comm,COMM_NOCLAN))
        {
            send_to_char("Clan channel is now ON\n\r",ch);
            REMOVE_BIT(ch->comm,COMM_NOCLAN);
        }
        else
        {
            send_to_char("Clan channel is now OFF\n\r",ch);
            SET_BIT(ch->comm,COMM_NOCLAN);
        }
        return;
    }
    if (IS_SET(ch->comm,COMM_NOCHANNELS))
    {
        send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
        return;
    }

    REMOVE_BIT(ch->comm,COMM_NOCLAN);

    sprintf( buf, "{B%s {W%s{x: %s\n\r", player_rank(ch),
             ch->name, argument );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING &&
             is_same_clan(ch,d->character) &&
             !IS_SET(d->character->comm,COMM_NOCLAN) &&
             !IS_SET(d->character->comm,COMM_QUIET) )
        {
            send_to_char(buf, d->character);
        }
    }
    return;
}

void do_clanlist(CHAR_DATA *ch, char *argument)
{
    sh_int clan;
    char buf[MAX_STRING_LENGTH];

    for(clan = 1; clan_table[clan].name[0] != '\0'; clan++)
    {
        if(!IS_IMMORTAL(ch) && is_clan_independent(clan)) continue;

        send_to_char(" -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-"
            "\n\r",ch);

        if(!IS_IMMORTAL(ch))
            sprintf(buf,"%s %s\n\r"
                    "\n\rDescription:\n\r%s",
                    clan_table[clan].who_name, (is_clan_pkill(clan)) ? "({RPKILL{x)" : "", clan_table[clan].description);
/*        else if(is_clead(ch) && ch->clan == clan)
            sprintf(buf,"%s %s    Members/Max %d/%d\n\r"
                    "\n\rDescription:\n\r%s",
                    clan->who_name, (is_clan_pkill(clan)) ? "({RPKILL{x)" : "",
		    clan->members, clan->max_members,
*/
        else
            sprintf(buf,"Who: %-10s  %s"
                    "\n\rName: %-10s"
                    "\n\rDescription: %s"
                    "\n\rHall: %-5d\n\r",
                    clan_table[clan].who_name,
                    (is_clan_pkill(clan)) ? "PK" : "Non-PK",
                    clan_table[clan].name,
                    clan_table[clan].description,
                    clan_table[clan].room[0]);
        send_to_char(buf,ch);
    }
	return;
}

void do_cgossip( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int wtime;
 
    if (!is_clan(ch) && !IS_IMMORTAL(ch))
    {
        send_to_char("You are not in a clan!\n\r",ch);
	return;
    }
    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOCGOSSIP))
      {
        send_to_char("Clan gossip channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOCGOSSIP);
      }
      else
      {
        send_to_char("Clan gossip channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOCGOSSIP);
      }
    }
    else  /* cgossip message sent, turn cgossip on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }
 
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
       	}

        if ((!str_infix(" fuck ", argument))
        ||  (!str_prefix("fuck ", argument))
        ||  (!str_suffix(" fuck", argument))
        ||  (!str_suffix(" fuck.", argument))
        ||  (!str_suffix(" fuck!", argument))
        ||  (!str_cmp("fuck", argument))
        ||  (!str_cmp("fuck.", argument))
        ||  (!str_cmp("fuck!", argument))
        ||  (!str_cmp("shit", argument))
        ||  (!str_cmp("shit.", argument))
        ||  (!str_cmp("shit!", argument))
        ||  (!str_infix(" shit ", argument))
        ||  (!str_prefix("shit ", argument))
        ||  (!str_suffix(" shit", argument))
        ||  (!str_suffix(" shit.", argument))
        ||  (!str_suffix(" shit!", argument)))
        { 
            if (!IS_IMMORTAL(ch)) {
                send_to_char("Don't let the IMM's catch you talking like that!\n\r",ch);
                return; 
            } 
        } 


      REMOVE_BIT(ch->comm,COMM_NOCGOSSIP);
 
      sprintf( buf, "You cgossip '{E%s{x'\n\r", argument );
      send_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;
	int pos;
	bool found = FALSE;
 
        victim = d->original ? d->original : d->character;
 
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOCGOSSIP) &&
             !IS_SET(victim->comm,COMM_QUIET) &&
	     (is_clan(victim) || IS_IMMORTAL(victim)))
        {
	    for (pos = 0; pos < MAX_FORGET; pos++)
	    {
		if (victim->pcdata->forget[pos] == NULL)
		    break;
		if (!str_cmp(ch->name,victim->pcdata->forget[pos]))
		    found = TRUE;
	    }
	    if (!found)
	    {
		act_new( "$n cgossips '{E$t{x'", 
		    ch,argument, d->character, TO_VICT,POS_SLEEPING );
	    }
        }
      }
    }
    wtime = UMAX(2, 9-(ch->level));
    WAIT_STATE( ch, wtime );
}
