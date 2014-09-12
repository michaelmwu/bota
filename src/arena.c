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

/***************************************************************************
*       Arena code copyright 2003-2005 Jason Lai                           *
*       Please keep this notice intact.                                    *
***************************************************************************/

/**
*
* Game types:
*  DeathMatch       - Basic team deathmatch. 20 * num players to win.
*  Vampire Tag      - Zombie slayer combat. Sort of.
*  Capture The Flag - Classic game. Not implemented.
*  Siege            - Need 450 points to win, objectives are best bet.
*
* Maps:
*  forest - Standard team deathmatch map.
*  castle - Siege map. Green attacks, red defends, one HQ up for grabs.
*
***/

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

/* command procedures needed */
DECLARE_DO_FUN(do_look		);
DECLARE_DO_FUN(do_echo		);

EVENT_FUN( event_start_arena );

void enter_arena(CHAR_DATA *ch, int team);
void end_arena args((bool normal));
void setup_stats(CHAR_DATA *victim, int level);
CHAR_DATA *reload_char(CHAR_DATA *ch);
void arena_broadcast(int team, char *message, CHAR_DATA *ch);
void check_score(int team);
void load_map(int map);
void reset_area (AREA_DATA * pArea, bool force);

PyObject *load_module(char *moduleloc, bool reload);

#define OTHER_TEAM(team)  (team == 0 ? 1 : 0)

struct arena_map_type
{
    char *name;
    int vnum_start;
    int vnum_end;
    int hqs [MAX_ARENA_TEAMS]; /* starting HQ's */
    struct
    {
        int ctf_obj;      /* this team wins by capturing this object  */
        int capture_vnum; /* this team goes here to drop off the flag */
    } ctf_data [MAX_ARENA_TEAMS];
    char *pymodule_loc;
} maps [] =
{
  {"forest", 200, 249, {200, 222}, {{201, 200}, {200, 222}}, "arena.forest"},
  {"caine", 250, 299, {250, 276}, {}, "arena.caine"},
  {"castle", 400, 499, {417, 400}, {}, "arena.castle"},
  {NULL}
};

void do_arena (CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    if(!IS_IMMORTAL(ch))
    {
        send_to_char("No player commands yet.\r\n",ch);
        return;
    }

    if(argument[0] == '\0')
    {
        send_to_char("Syntax:\r\n",ch);
        send_to_char("  arena init [map]  - prepares arena\r\n",ch);
        send_to_char("  arena show        - display current setup\r\n",ch);
        send_to_char("  arena begin       - starts the arena match\n\r",ch);
        send_to_char("  arena end         - end the arena match\n\r",ch);

        send_to_char("  arena set game <type>      - types: dm, vt, siege\n\r",ch);
        send_to_char("  arena set timer <minutes>  - game ends if no team wins\n\r",ch);
        send_to_char("  arena set equalize <level> - players set to this level\n\r",ch);
        send_to_char("  arena set numguards <num>  - guards per HQ\n\r",ch);
        send_to_char("  arena add [player list]    - adds players to random teams\n\r",ch);
        send_to_char("  arena add room             - adds players in current room\n\r",ch);
        send_to_char("  arena red [player list]    - adds to red only\n\r",ch);
        send_to_char("  arena green [player list]  - adds to green only\n\r",ch);
        return;
    }

    argument = one_argument(argument, arg);
    if(!str_cmp(arg,"init"))
    {
        if(arena.status != ARENA_STATUS_OPEN)
        {
            send_to_char("Wait for the arena to end, first.\n\r",ch);
            return;
        }

        send_to_char("Initializing arena...\n\r",ch);
        arena.status = ARENA_STATUS_WAITING;
        arena.teams_used = 2;

        if(argument[0] != '\0')
        {
            int i;

            for(i = 0; maps[i].name != NULL; i++)
            {
                if(!str_cmp(argument, maps[i].name))
                {
                    sprintf(buf, "Using map %s.\n\r", maps[i].name);
                    send_to_char(buf, ch);
                    load_map(i);
                    break;
                }
            }

            if(maps[i].name == NULL)
                load_map(0);
        }
        else
        {
            send_to_char("Using default map.\n\r", ch);
            load_map(0);
        }

        arena.timer = 30;
        arena.totalplayers = 0;
        arena.numguards = 2;
        arena.game_type = ARENA_DM;

        arena.teams[0].name = "{RRed{x";
        arena.teams[0].keywords = "red";
        arena.teams[0].colour = 'R';
        arena.teams[0].score = 0;
        arena.teams[0].players = 0;

        arena.teams[1].name = "{GGreen{x";
        arena.teams[1].keywords = "green";
        arena.teams[1].colour = 'g';
        arena.teams[1].score = 0;
        arena.teams[1].players = 0;
    }

    if(!str_cmp(arg,"end"))
    {
        if(arena.status != ARENA_STATUS_GAME)
        {
            send_to_char("There's no game running right now.\r\n",ch);
            return;
        }

        arena.status = ARENA_STATUS_FINISHING;
        send_to_char("Ending arena.\r\n",ch);
        return;
    }

    if(!str_cmp(arg,"show"))
    {
        if(arena.status != ARENA_STATUS_GAME &&
           arena.status != ARENA_STATUS_WAITING)
        {
            send_to_char("Please run 'arena init' first.\r\n",ch);
            return;
        }

        switch(arena.game_type)
        {
            case ARENA_DM:
                send_to_char("Game type: Deathmatch\r\n",ch); break;
            case ARENA_CTF:
                send_to_char("Game type: Capture the Flag\r\n",ch); break;
            case ARENA_VT:
                send_to_char("Game type: Vampire Tag\r\n",ch); break;
            case ARENA_SIEGE:
                send_to_char("Game type: Siege\r\n",ch); break;
        }

        sprintf(buf, "Arena Map: %s\n\r", maps[arena.map].name);
        send_to_char(buf, ch);

        if(arena.equalize_level)
            sprintf(buf, "Levels equalized to %d\r\n", arena.equalize_level);
        else
            sprintf(buf, "Level equalizing disabled.\n\r");
        send_to_char(buf,ch);

        sprintf(buf, "HQs: %d red, %d green.\n\r", arena.teams[0].hq->vnum,
                                                   arena.teams[1].hq->vnum);
        send_to_char(buf,ch);

        sprintf(buf, "Number of guards in each HQ: %d\n\r", arena.numguards);
        send_to_char(buf,ch);

        sprintf(buf, "Game length: %d min\n\r", arena.timer);
        send_to_char(buf,ch);

        return;
    }

    if(arena.status != ARENA_STATUS_WAITING)
    {
        send_to_char("Please run 'arena init' first.\r\n",ch);
        return;
    }

    if(!str_cmp(arg,"begin"))
    {
        MOB_INDEX_DATA *guard_index = get_mob_index(201);
        CHAR_DATA *guard;
        int team, gnum;

        for(team = 0; team < arena.teams_used; team++)
        {
            if(arena.game_type == ARENA_CTF)
            {
                /* code not written */
            }

            /* spawn guards */
            for(gnum = 0; gnum < arena.numguards; gnum++)
            {
                guard = create_mobile(guard_index);
                guard->arena_team = team;
                guard->in_arena = TRUE;
                char_to_room(guard, arena.teams[team].hq);

                act_new_2( "$n materializes into the room.", guard, NULL, NULL,
                           TO_ROOM, POS_RESTING, 0, ACTION_MUST_SEE );
            }
        }

        send_to_char("Starting arena...\n\r",ch);
        arena.status = ARENA_STATUS_GAME;

        script_call_noarg(arena.pymodule, "on_begin");

        arena_broadcast(-1, "Arena match started!", NULL);
    }

    if(!str_cmp(arg,"map"))
    {
        int i;

        argument = one_argument(argument, arg);

        if(arena.totalplayers > 0)
        {
            send_to_char("Not while players are in the arena.\n\r", ch);
            return;
        }

        if(arg[0] == '\0')
        {
            send_to_char("Syntax: arena map <mapname>\n\r", ch);
            return;
        }

        for(i = 0; maps[i].name != NULL; i++)
        {
            if(!str_cmp(arg, maps[i].name))
            {
                sprintf(buf, "Switching map to %s.\n\r", maps[i].name);
                send_to_char(buf, ch);
                load_map(i);
                return;
            }
        }

        send_to_char("Map not found.\n\r", ch);
        return;
    }

    if(!str_cmp(arg,"set"))
    {
        argument = one_argument(argument, arg);

        if(arg[0] == '\0')
        {
            send_to_char("Options: game equalize timer numguards\r\n", ch);
            return;
        }

        if(!str_cmp(arg,"game"))
        {
            if(!str_cmp(argument,"dm"))
            {
                arena.game_type = ARENA_DM;
                send_to_char("Game type set to Deathmatch.\r\n", ch);

                arena.teams[0].name = "{RRed{x";
                arena.teams[1].name = "{GGreen{x";
            }
            else if(!str_cmp(argument,"ctf")) /* CTF code not written! */
            {
                arena.game_type = ARENA_CTF;
                send_to_char("Game type set to Capture the Flag.\r\n",ch);

                arena.teams[0].name = "{RRed{x";
                arena.teams[1].name = "{GGreen{x";
            }
            else if(!str_cmp(argument,"siege"))
            {
                arena.game_type = ARENA_SIEGE;
                send_to_char("Game type set to Siege.\r\n",ch);
                send_to_char("(Make sure to choose a Siege map, e.g 'castle'.)\r\n",ch);

                arena.teams[0].name = "{RRed{x";
                arena.teams[1].name = "{GGreen{x";
            }
            else if(!str_cmp(argument,"vt"))
            {
                arena.game_type = ARENA_VT;
                send_to_char("Game type set to Vampire Tag.\r\n",ch);

                arena.teams[0].name = "{RVampires{x";
                arena.teams[1].name = "{GSlayers{x";
            }
            else
                send_to_char("Invalid game type.\r\n",ch);

            return;
        }

        if(!str_cmp(arg,"timer") || !str_cmp(arg, "equalize")
        || !str_cmp(arg,"numguards"))
        {
            int num;

            if(argument[0] == '\0' || !is_number(argument))
            {
                send_to_char("Need a number.\r\n",ch);
                return;
            }

            num = atoi(argument);

            if(num < 0 || num > 101)
            {
                send_to_char("No.\r\n",ch);
                return;
            }

            if(!str_cmp(arg,"timer"))
                arena.timer = num;
            else if(!str_cmp(arg,"equalize"))
                arena.equalize_level = num;
            else if(!str_cmp(arg,"numguards"))
                arena.numguards = num;

            sprintf(buf, "Value for %s set to %d.\r\n", arg, num);
            send_to_char(buf,ch);
            return;
        }
    }

    if(!str_cmp(arg,"add") || !str_cmp(arg, "red") || !str_cmp(arg,"green"))
    {
        int team;
        bool force_team = FALSE;
        CHAR_DATA *victim;
        CHAR_DATA *to_add[20];
        int v_pos = 0, i;

        if(!str_cmp(arg, "red"))   { team = 0; force_team = TRUE; }
        if(!str_cmp(arg, "green")) { team = 1; force_team = TRUE; }

        if(!str_cmp(argument, "room")) /* arena add room */
        {
            argument = "";
            for(victim = ch->in_room->people; victim != NULL;
                victim = victim->next_in_room)
            {
                if(IS_IMMORTAL(victim) || IS_NPC(victim) || victim->in_arena)
                    continue;

                to_add[v_pos++] = victim;

                if(v_pos > 18) { send_to_char("Too many.\r\n", ch); return; }
            }

        }

        while(argument[0] != '\0')
        {
            argument = one_argument(argument, arg);

            if( ( victim = get_char_world( ch, arg ) ) == NULL)
            {
                sprintf(buf, "Skipping '%s': could not find that person.\r\n", arg);
                send_to_char(buf,ch);
                continue;
            }

            if(IS_IMMORTAL(victim) || IS_NPC(victim) || victim->in_arena)
            {
                sprintf(buf, "Skipping %s: not allowed.\r\n", PERS(victim, ch));
                send_to_char(buf, ch);
                continue;
            }

            to_add[v_pos++] = victim;

            if(v_pos > 18) { send_to_char("Too many.\r\n", ch); return; }
        }

        for(i = 0; i < v_pos; i++) /* lame hack-ish shuffle */
        {
            int pos = number_range(i, v_pos - 1);
            victim = to_add[pos];
            to_add[pos] = to_add[i];
            to_add[i] = victim;
        }

        for(i = 0; i < v_pos; i++)
        {
            victim = to_add[i];

            if(!force_team)
            {
                if(arena.game_type == ARENA_VT && arena.teams[0].players >= 1)
                    team = 1; /* only one vampire at the start, by default */
                else if(arena.teams[0].players == arena.teams[1].players)
                    team = number_range(0,1);
                else
                    team = (arena.teams[0].players > arena.teams[1].players) ? 1 : 0;
            }

            sprintf(buf, "Added %s to the %s team.\r\n", PERS(victim, ch), arena.teams[team].name);
            send_to_char(buf,ch);

            send_to_char("Entering the Arena!\n\r", victim);

            enter_arena( victim, team );

            sprintf(buf, "You join the %s team.\n\r", arena.teams[team].name);
            send_to_char(buf,victim);

            do_look(victim,"auto");
        }
        return;
    }
}

void enter_arena(CHAR_DATA *ch, int team)
{
    if(arena.status != ARENA_STATUS_WAITING)
        return;

    if(IS_NPC(ch)) return;

    save_char_obj(ch);

    /* Okay, start messing with the stats ... */

    ch->in_arena = TRUE; /* prevents saving */
    ch->arena_team = team;

    arena.teams[ch->arena_team].players++;
    arena.totalplayers++;

    char_from_room(ch);
    char_to_room(ch,arena.teams[ch->arena_team].hq);

    ch->clan = 0;
    ch->rank = 0;
    REMOVE_BIT(ch->act,PLR_MORTAL_LEADER);

    ch->platinum = 0;
    ch->gold = 0;
    ch->silver = 25;

    ch->pcdata->confirm_delete = 0;
    ch->pcdata->confirm_reroll = 0;

    ch->on_quest = FALSE;

    if(arena.equalize_level)
        setup_stats(ch, arena.equalize_level);

    if(arena.game_type == ARENA_VT && team == 0)
    {
        ch->max_hit *= 0.95; /* weaker, but better than converts */
        ch->max_mana *= 1.15;
    }

    nuke_pets(ch);
    ch->pet = NULL;

    die_follower(ch);
}

void setup_stats(CHAR_DATA *victim, int level)
{
    int oldlevel, iLevel, sn;

    if(level < victim->level)
    {
        oldlevel         = victim->level;
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
    }

    for ( iLevel = victim->level ; iLevel < level; iLevel++ )
    {
        victim->level += 1;
        advance_level( victim, TRUE );
    }
    victim->exp   = exp_per_level(victim,victim->pcdata->points)
                  * UMAX( 1, victim->level );
    victim->trust = 0;

    for(sn = 0; sn < MAX_SKILL; sn++)
    {
        if( skill_table[sn].name == NULL )
            break;

        if ( victim->level < skill_table[sn].skill_level[victim->class]
          || victim->pcdata->learned[sn] < 1 /* skill is not known */ )
            continue;

        victim->pcdata->learned[sn] = 90;
    }
}

void arena_death (CHAR_DATA *ch, CHAR_DATA *killer)
{
    char buf[MAX_STRING_LENGTH];

    if(IS_NPC(ch)) return;

    if(killer && killer->in_arena)
    {
        if(ch->arena_team == killer->arena_team)
        {
            arena.teams[killer->arena_team].score -= 10;
        }
        else
        {
            arena.teams[killer->arena_team].score += 10;
            sprintf(buf, "Excellent! $n killed the enemy %s.", ch->name);
            arena_broadcast(killer->arena_team, buf, killer);

            sprintf(buf, "Alas, %s was struck down by $n.", ch->name);
            arena_broadcast(ch->arena_team, buf, killer);
        }
    }
    else /* Died of poison, or a neutral NPC, or something like that */
    {
        arena.teams[ch->arena_team].score -= 10;
    }

    /* Vampire Tag code */
    if(arena.game_type == ARENA_VT && killer->arena_team == 0
    && ch->arena_team != 0)
    {
        /* insert some kind of blood-sucking message here */

        if(arena.teams[ch->arena_team].players > 1 &&
           arena.teams[0].score >= (19 * arena.teams[0].players))
        { /* you are now a vampire! */
            OBJ_DATA *obj, *obj_next;

            for(obj = ch->in_room->contents; obj; obj = obj_next)
            {
                obj_next = obj->next;

                if(obj->item_type == ITEM_CORPSE_PC
                && !str_cmp(obj->owner,ch->name) )
                {
                    /* move corpse(s) to new HQ */
                    obj_from_room(obj);
                    obj_to_room(obj, arena.teams[0].hq);
                }
            }

            char_from_room(ch);
            char_to_room(ch, arena.teams[0].hq);

            arena.teams[ch->arena_team].players--;
            ch->arena_team = 0;
            send_to_char("You have become a {RVAMPIRE{x!\r\n",ch);
            ch->max_hit  *= 0.75; /* vampires are weaker */
            ch->max_mana *= 1.15; /* but have more magic */

            ch->pcdata->learned[gsn_feed] = 90;

            arena_broadcast(-1, "$n succumbed and has become a vampire!", ch);

            if(arena.teams[ch->arena_team].players == 1) /* to the surivor */
                arena_broadcast(1, "You're the only one left!", ch);
        }
    }

    sprintf(buf, "{W*** Score: "
                 "Your team's is {R%d{W, other team's is {G%d{x ***{x",
                 arena.teams[0].score, arena.teams[1].score);
    arena_broadcast(0, buf, NULL);

    sprintf(buf, "{W*** Score: "
                 "Your team's is {R%d{W, other team's is {G%d{x ***{x",
                 arena.teams[1].score, arena.teams[0].score);
    arena_broadcast(1, buf, NULL);

    /* restore partially */
    ch->hit        = ch->max_hit * 0.9;
    ch->mana       = ch->max_mana * 0.9;
    ch->move       = ch->max_move;
    update_pos( ch);

    check_score(killer->arena_team);
}

void check_score(int team)
{
    int needed;

    switch(arena.game_type)
    {
        case ARENA_SIEGE: needed = 450; break;
        default: needed = arena.totalplayers * 20;
    }

    if(arena.teams[team].score >= needed)
    {
        char buf[MAX_STRING_LENGTH];
        char buf2[MAX_STRING_LENGTH];
        CHAR_DATA *wch;

        sprintf(buf, "%s wins the arena match!", arena.teams[team].name);
        arena_broadcast(-1, buf, NULL);

        strcpy(buf,  "The victors: ");
        strcpy(buf2, "The defeated:");

        for ( wch = plr_list; wch != NULL; wch = wch->plr_next )
        {
            if(!wch->in_arena) continue;

            if(wch->arena_team == team)
            {
                strcat(buf, " ");
                strcat(buf, wch->name);
            }
            else
            {
                strcat(buf2, " ");
                strcat(buf2, wch->name);
            }
        }

        arena_broadcast(-1, buf, NULL);
        arena_broadcast(-1, buf2, NULL);

        arena.status = ARENA_STATUS_FINISHING;
    }

}

/* use -1 for team to broadcast to all */
/* will not broadcast to ch */
void arena_broadcast(int team, char *message, CHAR_DATA *ch)
{
    CHAR_DATA *wch;
    char buf[MAX_STRING_LENGTH];

    sprintf(buf, "{Y[{BARENA{Y]{x %s{x", message);

    if(!ch) strcat(buf, "\n\r");

    for ( wch = plr_list; wch != NULL; wch = wch->plr_next )
    {
        if(!wch->in_arena) continue;
        if(team >= 0 && wch->arena_team != team) continue;

        if(ch)
            act_new(buf, ch, NULL, wch, TO_VICT, POS_DEAD);
        else
            send_to_char(buf, wch);
    }
}

void end_arena(bool normal)
{
    CHAR_DATA *ch, *ch_next_in_room;
    OBJ_DATA *obj, *obj_next;
    int tcvnum;
    ROOM_INDEX_DATA *room;

    arena.timer = 0;

    /* Clean up the place, loop through arena rooms */
    for(tcvnum = arena.vnum_start;
       tcvnum <= arena.vnum_end; tcvnum++ )
    {
        room = get_room_index(tcvnum);

        if(!room) continue;

        /* Toast the people */
        for(ch = room->people; ch; ch = ch_next_in_room)
        {
            ch_next_in_room = ch->next_in_room;

            stop_fighting( ch, TRUE );

            if(IS_NPC(ch))
            {
                if(ch->master)
                    stop_follower(ch);

                extract_char(ch, TRUE);
                continue;
            }

            if(!ch->in_arena)
            { /* Get people out of here */
                send_to_char("{YArena is being reset.{x\n\r",ch);
                send_to_char("{YYou have been moved out.{x\n\r",ch);
                char_from_room(ch);
                char_to_room(ch,get_room_index(3008));
                continue;
            }

            ch = reload_char(ch);
            if(!ch)
            {
                bug_f("Failed to reload char in arena.");
                continue;
            }

            send_to_char("\n\r\n\r{YThe arena match has ended!{x\n\r",ch);

            send_to_char("You are transferred out of the arena.\n\r",ch);
            char_to_room( ch, ch->in_room );

            act2("$n has re-entered the game.",ch,NULL,NULL,TO_ROOM, ACTION_MUST_SEE);

            if (ch->pet != NULL)
            {
                char_to_room(ch->pet,ch->in_room);
                act2("$n has re-entered the game.",ch->pet,NULL,NULL,TO_ROOM, ACTION_MUST_SEE);
            }

            do_look(ch,"auto");
        }

        /* Toast any items */
        for(obj = room->contents; obj; obj = obj_next)
        {
            obj_next = obj->next_content;
            extract_obj(obj);
        }
    }

    /* Reset the arena structure data */
    arena.status = ARENA_STATUS_OPEN;
}

CHAR_DATA *reload_char(CHAR_DATA *ch)
{
    char name[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;

    if(IS_NPC(ch) || IS_IMMORTAL(ch)) return NULL;

    strcpy(name,ch->name);
    d = ch->desc;

    if(ch->pcdata->in_progress)
        free_note (ch->pcdata->in_progress);

    extract_char(ch, TRUE);

    ch = NULL;

    if(d == NULL)
        return NULL;

    /* Reload from the saved character file */
    load_char_obj(d, name);
    if(!d->character)
    {
        send_to_char("Reload recovery failed!\n\r",ch);
        close_socket(d);
        return NULL;
    }

    ch = d->character;

    ch->next = char_list;
    char_list = ch;

    ch->plr_next = plr_list;
    plr_list = ch;

    d->connected = CON_PLAYING;
    reset_char(ch);

    ch->pcdata->socket = str_dup(d->host);

    return ch;
}


void event_start_arena(EVENT *e) /* not used; for starting countdown */
{
    arena.status = ARENA_STATUS_GAME;

    return;
}

void do_mparena(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument(argument, arg);

    if(!ch->in_arena) return;

    if(!str_cmp(arg, "sethq")) /* arena hqset <teamid> <vnum> */
    {
        int team, vnum;
        argument = one_argument(argument, arg);
        if(!is_number(arg)) return;
        team = atoi(arg);
        if(team < 0 || team >= MAX_ARENA_TEAMS) return;

        if(!is_number(argument)) return;
        vnum = atoi(argument);
        if(!ENTRE_INC(arena.vnum_start, vnum, arena.vnum_end)) return;

        arena.teams[team].hq = get_room_index(vnum);
    }
    else if(!str_cmp(arg, "award")) /* arena award <team> <points> */
    {
        int team, points;
        argument = one_argument(argument, arg);
        if(!is_number(arg)) return;
        team = atoi(arg);
        if(team < 0 || team >= MAX_ARENA_TEAMS) return;

        if(!is_number(argument)) return;
        points = atoi(argument);
        arena.teams[team].score += points;

        check_score(team);
    }
    else if(!str_cmp(arg, "mload")) /* arena mload <team> <vnum> */
    {
        int team, vnum;
        MOB_INDEX_DATA *pMobIndex;
        CHAR_DATA      *victim;

        argument = one_argument(argument, arg);
        if(!is_number(arg)) return;
        team = atoi(arg);
        if(team < 0 || team >= MAX_ARENA_TEAMS) return;

        argument = one_argument( argument, arg );

        if ( ch->in_room == NULL || arg[0] == '\0' || !is_number(arg) )
            return;

        vnum = atoi(arg);
        if ( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
        {
            sprintf( arg, "arena-mload: bad mob index (%d) from mob %d",
                vnum, IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
            bug( arg, 0 );
            return;
        }
        victim = create_mobile( pMobIndex );
        char_to_room( victim, ch->in_room );

        victim->arena_team = team;
        victim->in_arena = TRUE;

        return;
    }
    else if(!str_cmp(arg, "broadcast") || !str_cmp(arg, "bcast"))
    {
        int team;

        argument = one_argument(argument, arg);
        if(!is_number(arg)) return;
        team = atoi(arg);
        if(team < -1 || team >= MAX_ARENA_TEAMS) return;

        arena_broadcast(team, argument, NULL);
        return;
    }
}

void load_map(int map)
{
    AREA_DATA *area;
    int team;

    arena.map = map;
    arena.vnum_start = maps[map].vnum_start;
    arena.vnum_end = maps[map].vnum_end;

    area = get_room_index(arena.vnum_start)->area;
    reset_area(area, TRUE);

    for(team = 0; team < MAX_ARENA_TEAMS; team++)
        arena.teams[team].hq = get_room_index(maps[map].hqs[team]);

    if(arena.pymodule)
    {
        Py_DECREF(arena.pymodule);
        arena.pymodule = NULL;
    }

    if(!IS_NULLSTR(maps[map].pymodule_loc))
        arena.pymodule = load_module(maps[map].pymodule_loc, TRUE);
    else
        arena.pymodule = NULL;
}
