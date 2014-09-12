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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "event.h"
#include "interp.h"

bool	check_social	args( ( CHAR_DATA *ch, char *command,
			    char *argument ) );

char last_command[MAX_STRING_LENGTH];
unsigned int  last_hostip;

/*
 * Command logging types.
 */
#define LOG_NEVER	-1
#define LOG_NORMAL	 0     /* don't log unless specified */
#define LOG_SIMPLE	 5
#define LOG_COMM	 6
#define LOG_ALWAYS	 9



/*
 * Log-all switch.
 */
bool				fLogAll		= FALSE;
bool				global_quest	= FALSE;

#define GEN CMD_GENERAL
#define CFG CMD_CONFIG
#define COM CMD_COMM
#define NFO CMD_INFO
#define CBT CMD_COMBAT

#define NS CMD_NOSHOW
#define ST CMD_STIER

const char *cmd_type_names[] =
{
 "", "general", "config", "comm", "info"
};

/*
 * Command table.
 */
const	struct	cmd_type	cmd_table	[] =
{
    /*
     * Common movement commands.
     */
{ "north",	do_north,	POS_STANDING,    0,  LOG_NEVER, GEN, NS },
{ "east",	do_east,	POS_STANDING,	 0,  LOG_NEVER, GEN, NS },
{ "south",	do_south,	POS_STANDING,	 0,  LOG_NEVER, GEN, NS },
{ "west",	do_west,	POS_STANDING,	 0,  LOG_NEVER, GEN, NS },
{ "up",		do_up,		POS_STANDING,	 0,  LOG_NEVER, GEN, NS },
{ "down",	do_down,	POS_STANDING,	 0,  LOG_NEVER, GEN, NS },
{ "run",       do_run,        POS_STANDING,    0,  LOG_NEVER, GEN, NS },

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
{ "at",		do_at,          POS_DEAD,       L6,  LOG_SIMPLE, GEN, 0 },
{ "cast",	do_cast,	POS_FIGHTING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "auction",	do_auction,     POS_SLEEPING,    0,  LOG_SIMPLE, GEN, 0 },
{ "buy",	do_buy,		POS_RESTING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "channels",	do_channels,    POS_DEAD,        0,  LOG_NORMAL, GEN, 0 },
{ "exits",	do_exits,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "get",	do_get,		POS_RESTING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "goto",	do_goto,        POS_DEAD,       IM,  LOG_SIMPLE, GEN, 0 },
{ "group",	do_group,       POS_SLEEPING,    0,  LOG_SIMPLE, GEN, 0 },
{ "guild",	do_guild,	POS_DEAD,	90,  LOG_ALWAYS, GEN, 0 },
{ "hit",	do_kill,	POS_FIGHTING,	 0,  LOG_SIMPLE, CBT, NS },
{ "inventory",	do_inventory,	POS_DEAD,	 0,  LOG_SIMPLE, GEN, 0 },
{ "kill",	do_kill,	POS_FIGHTING,	 0,  LOG_SIMPLE, CBT, 0 },
{ "look",	do_look,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "glance",	do_glance,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "clan",	do_clantalk,	POS_SLEEPING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "music",	do_music,   	POS_SLEEPING,    0,  LOG_SIMPLE, GEN, 0 }, 
{ "mock",	do_mock,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "order",	do_order,	POS_RESTING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "practice",	do_practice,	POS_SLEEPING,    0,  LOG_NORMAL, GEN, 0 },
{ "rest",	do_rest,	POS_SLEEPING,	 0,  LOG_NORMAL, GEN, 0 },
{ "sit",	do_sit,		POS_SLEEPING,    0,  LOG_NORMAL, GEN, 0 },
{ "sockets",	do_sockets,	POS_DEAD,       ML,  LOG_NORMAL, GEN, 0 },
{ "stand",	do_stand,	POS_SLEEPING,	 0,  LOG_NORMAL, GEN, 0 },
{ "tell",	do_tell,	POS_SLEEPING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "unlock",     do_unlock,      POS_RESTING,     0,  LOG_NORMAL, GEN, 0 },
{ "wield",	do_wear,	POS_RESTING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "wizhelp",	do_wizhelp,	POS_DEAD,	IM,  LOG_NORMAL, GEN, 0 },

    /*
     * Informational commands.
     */
{ "affects",	do_affects,	POS_DEAD,	 0,  LOG_NORMAL, NFO, 0 },
{ "areas",	do_areas,	POS_DEAD,	 0,  LOG_SIMPLE, NFO, 0 },
{ "bug",	do_bug,		POS_DEAD,	 0,  LOG_SIMPLE, GEN, 0 },
{ "board",	do_board,	POS_SLEEPING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "clear",	do_clear,	POS_DEAD,	 0,  LOG_NORMAL, GEN, 1 }, /*Leviticus*/
{ "clanlist",   do_clanlist,    POS_DEAD,        0,  LOG_NORMAL, GEN, 0 },
{ "commands",	do_commands,	POS_DEAD,	 0,  LOG_NORMAL, GEN, 0 },
{ "compare",	do_compare,	POS_RESTING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "consider",	do_consider,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "count",	do_count,	POS_SLEEPING,	 0,  LOG_NORMAL, NFO, 0 },
{ "credits",	do_credits,	POS_DEAD,	 0,  LOG_NORMAL, GEN, 0 },
{ "equipment",	do_equipment,	POS_DEAD,	 0,  LOG_SIMPLE, GEN, 0 },
{ "examine",	do_examine,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "?",		do_help,	POS_DEAD,	 0,  LOG_NORMAL, GEN, NS },
{ "help",	do_help,	POS_DEAD,	 0,  LOG_SIMPLE, GEN, 0 },
{ "info",	do_groups,      POS_SLEEPING,    0,  LOG_NORMAL, GEN, 0 },
{ "peek",	do_peek,	POS_STANDING,	 0,  LOG_NORMAL, GEN, 0 },
{ "read",	do_read,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "report",	do_report,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "rules",	do_rules,	POS_DEAD,	 0,  LOG_NORMAL, GEN, 0 },
{ "score",	do_score,	POS_DEAD,	 0,  LOG_NORMAL, NFO, 0 },
{ "scan",	do_scan,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "skills",	do_skills,	POS_DEAD,	 0,  LOG_SIMPLE, NFO, 0 },
{ "socials",	do_socials,	POS_DEAD,	 0,  LOG_NORMAL, NFO, 0 },
{ "show",	do_show,	POS_DEAD,	 0,  LOG_NORMAL, GEN, 0 },
{ "spells",	do_spells,	POS_DEAD,	 0,  LOG_SIMPLE, NFO, 0 },
{ "story",	do_story,	POS_DEAD,	 0,  LOG_NORMAL, NFO, 0 },
{ "time",	do_time,	POS_DEAD,	 0,  LOG_NORMAL, NFO, 0 },
{ "typo",	do_typo,	POS_DEAD,	 0,  LOG_NORMAL, GEN, 0 },
{ "weather",	do_weather,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "who",	do_nwho,	POS_DEAD,	 0,  LOG_NORMAL, GEN, 0 },
{ "whois",	do_whois,	POS_DEAD,	 0,  LOG_NORMAL, GEN, 0 },
{ "wizlist",	do_wizlist,	POS_DEAD,        0,  LOG_NORMAL, GEN, 0 },
{ "worth",	do_worth,	POS_SLEEPING,	 0,  LOG_NORMAL, GEN, 0 },
{ "pkcount",	do_pkcount,	POS_SLEEPING,	 0,  LOG_NORMAL, NFO, 0 },

    /*
     * Configuration commands.
     */
{ "alia",	do_alia,	POS_DEAD,	 0,  LOG_NORMAL, CFG, NS },
{ "alias",	do_alias,	POS_DEAD,	 0,  LOG_SIMPLE, CFG, 0 },
{ "autolist",	do_autolist,	POS_DEAD,	 0,  LOG_NORMAL, CFG, 0 },
{ "autoassist",	do_autoassist,	POS_DEAD,        0,  LOG_NORMAL, CFG, 0 },
{ "autoafk",	do_autoaway,	POS_DEAD,        0,  LOG_NORMAL, CFG, 0 },
{ "autoexit",	do_autoexit,	POS_DEAD,        0,  LOG_NORMAL, CFG, 0 },
{ "autogold",	do_autogold,	POS_DEAD,        0,  LOG_NORMAL, CFG, 0 },
{ "autoloot",	do_autoloot,	POS_DEAD,        0,  LOG_NORMAL, CFG, 0 },
{ "autosac",	do_autosac,	POS_DEAD,        0,  LOG_NORMAL, CFG, 0 },
{ "autosplit",	do_autosplit,	POS_DEAD,        0,  LOG_NORMAL, CFG, 0 },
{ "autostore",	do_autostore,	POS_DEAD,        0,  LOG_NORMAL, CFG, 0 },
{ "autopeek",	do_autopeek,	POS_DEAD,	 0,  LOG_NORMAL, CFG, 0 },
{ "brief",	do_brief,	POS_DEAD,        0,  LOG_NORMAL, CFG, 0 },
{ "colour",	do_colour,      POS_DEAD,        0,  LOG_NORMAL, CFG, 0 },
{ "color",	do_colour,      POS_DEAD,        0,  LOG_NORMAL, CFG, 0 },
{ "combine",	do_combine,	POS_DEAD,        0,  LOG_SIMPLE, CFG, 0 },
{ "compact",	do_compact,	POS_DEAD,        0,  LOG_NORMAL, CFG, 0 },
{ "description",do_description,	POS_DEAD,	 0,  LOG_SIMPLE, CFG, 0 },
{ "long",	do_long,	POS_DEAD,        0,  LOG_NORMAL, CFG, 0 },
{ "nofollow",	do_nofollow,	POS_DEAD,        0,  LOG_NORMAL, CFG, 0 },
{ "noloot",	do_noloot,	POS_DEAD,        0,  LOG_NORMAL, CFG, 0 },
{ "nosummon",	do_nosummon,	POS_DEAD,        0,  LOG_SIMPLE, CFG, 0 },
{ "notran",	do_notran,	POS_DEAD,	 0,  LOG_NORMAL, CFG, 0 },
{ "password",	do_password,	POS_DEAD,	 0,  LOG_NEVER,  CFG, 0 },
{ "prompt",	do_prompt,	POS_DEAD,        0,  LOG_NORMAL, CFG, 0 },
{ "telnetga",	do_telnetga,	POS_DEAD,        0,  LOG_NORMAL, CFG, 0 },
{ "scroll",	do_scroll,	POS_DEAD,	 0,  LOG_NORMAL, CFG, 0 },
{ "title",	do_title,	POS_DEAD,	 0,  LOG_SIMPLE, CFG, 0 },
{ "titlelock",	do_titlelock,	POS_DEAD,	 0,  LOG_NORMAL, CFG, 0 },
{ "autotitle",	do_titlelock,	POS_DEAD,	 0,  LOG_NORMAL, CFG, 0 },
{ "unalias",	do_unalias,	POS_DEAD,	 0,  LOG_SIMPLE, CFG, 0 },
{ "wimpy",	do_wimpy,	POS_DEAD,	 0,  LOG_NORMAL, GEN, 0 },

    /*
     * Communication commands.
     */
{ "afk",	do_afk,		POS_SLEEPING,	 0,  LOG_NORMAL, CFG, 0 },
{ "answer",	do_answer,	POS_SLEEPING,	 0,  LOG_SIMPLE, COM, 0 },
{ "ansi",	do_colour,	POS_SLEEPING,	 0,  LOG_NORMAL, COM, 0 },
{ "deaf",	do_deaf,	POS_DEAD,	 0,  LOG_NORMAL, CFG, 0 },
{ "delay",	do_delay,	POS_DEAD,	 0,  LOG_NORMAL, CFG, 1 },
{ "emote",	do_emote,	POS_RESTING,	 0,  LOG_SIMPLE, COM, 0 },
{ "pmote",	do_pmote,	POS_RESTING,	 0,  LOG_SIMPLE, COM, 0 },
{ ".",		do_gossip,	POS_SLEEPING,	 0,  LOG_SIMPLE, COM, NS },
{ "gossip",	do_gossip,	POS_SLEEPING,	 0,  LOG_SIMPLE, COM, 0 },
{ "ooc",	do_ooc,		POS_DEAD,	 0,  LOG_SIMPLE, COM, 0 },
{ "oco",	do_ooc,		POS_DEAD,	 0,  LOG_SIMPLE, COM, NS },
{ "gocial",	do_gocial,	POS_STANDING,	 0,  LOG_SIMPLE, COM, 0 },
{ "gsocial",	do_gocial,	POS_STANDING,	 0,  LOG_SIMPLE, COM, 0 },
{ "=",		do_cgossip,	POS_SLEEPING,	 0,  LOG_SIMPLE, COM, NS },
{ "cgossip",	do_cgossip,	POS_SLEEPING,	 0,  LOG_SIMPLE, COM, 0 },
{ "-",		do_qgossip,	POS_SLEEPING,	 0,  LOG_SIMPLE, COM, NS },
{ "qgossip",	do_qgossip,	POS_SLEEPING,	 0,  LOG_SIMPLE, COM, 0 },
{ ",",		do_emote,	POS_RESTING,	 0,  LOG_SIMPLE, COM, NS },
{ "grats",	do_grats,	POS_SLEEPING,	 0,  LOG_SIMPLE, COM, 0 },
{ "gtell",	do_gtell,	POS_DEAD,	 0,  LOG_SIMPLE, COM, 0 },
{ ";",		do_gtell,	POS_DEAD,	 0,  LOG_SIMPLE, COM, NS },
{ "note",	do_note,	POS_SLEEPING,	 0,  LOG_SIMPLE, COM, 0 },
{ "pose",	do_pose,	POS_RESTING,	 0,  LOG_NORMAL, COM, 0 },
{ "iquest",	do_iquest,	POS_SLEEPING,	 0,  LOG_SIMPLE, COM, 0 },
{ "quest",	do_quest,	POS_STANDING,	 0,  LOG_SIMPLE, COM, 0 },
{ "aquest",	do_quest,	POS_STANDING,	 0,  LOG_SIMPLE, COM, NS },
{ "ask",	do_ask,		POS_SLEEPING,	 0,  LOG_SIMPLE, COM, 0 },
{ "quote",	do_quote,	POS_SLEEPING,	 0,  LOG_SIMPLE, COM, 0 },
{ "quiet",	do_quiet,	POS_SLEEPING, 	 0,  LOG_SIMPLE, COM, 0 },
{ "reply",	do_reply,	POS_SLEEPING,	 0,  LOG_SIMPLE, COM, 0 },
{ "replay",	do_replay,	POS_SLEEPING,	 0,  LOG_NORMAL, COM, 0 },
{ "say",	do_say,		POS_RESTING,	 0,  LOG_SIMPLE, COM, 0 },
{ "elvish",	do_elvish,	POS_RESTING,	 0,  LOG_SIMPLE, COM, 0 },
{ "dwarvish",	do_dwarvish,	POS_RESTING,	 0,  LOG_SIMPLE, COM, 0 },
{ "orcish",	do_orcish,	POS_RESTING,	 0,  LOG_SIMPLE, COM, 0 },
{ "common",	do_common,	POS_RESTING,	 0,  LOG_SIMPLE, COM, 0 },
{ "'",		do_say,		POS_RESTING,	 0,  LOG_SIMPLE, COM, NS },
{ "shout",	do_shout,	POS_RESTING,	 0,  LOG_SIMPLE, COM, 0 },
{ "yell",	do_yell,	POS_RESTING,	 0,  LOG_SIMPLE, COM, 0 },
{ "battlespam",	do_battlespam,	POS_SLEEPING, 	 0,  LOG_SIMPLE, CFG, 0 },
{ "languages",	do_languages,	POS_SLEEPING,	 0,  LOG_NORMAL, CFG, 0 },

    /*
     * Object manipulation commands.
     */
{ "brandish",	do_brandish,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "close",	do_close,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "drink",	do_drink,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "drop",	do_drop,	POS_RESTING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "eat",	do_eat,		POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "envenom",	do_envenom,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "fill",	do_fill,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "give",	do_give,	POS_RESTING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "heal",	do_heal,	POS_RESTING,	 0,  LOG_SIMPLE, GEN, 0 }, 
{ "repent",	do_repent,	POS_RESTING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "curse",	do_curse,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "hold",	do_wear,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "list",	do_list,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "lock",	do_lock,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "open",	do_open,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "pick",	do_pick,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "pour",	do_pour,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "put",	do_put,		POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "quaff",	do_quaff,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "recite",	do_recite,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "remove",	do_remove,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "sell",	do_sell,	POS_RESTING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "second",	do_second,	POS_RESTING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "study",	do_study,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 1 },
{ "take",	do_get,		POS_RESTING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "sacrifice",	do_sacrifice,	POS_RESTING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "junk",	do_junk,        POS_RESTING,     0,  LOG_SIMPLE, GEN, NS },
{ "tap",	do_sacrifice,   POS_RESTING,     0,  LOG_SIMPLE, GEN, NS },   
{ "value",	do_value,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "wear",	do_wear,	POS_RESTING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "zap",	do_zap,		POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },

    /*
     * Combat commands.
     */
{ "backstab",	do_backstab,	POS_FIGHTING,	 0,  LOG_NORMAL, CBT, 0 },
{ "bash",	do_bash,	POS_FIGHTING,    0,  LOG_NORMAL, GEN, 0 },
{ "bs",		do_backstab,	POS_FIGHTING,	 0,  LOG_NORMAL, CBT, NS },
{ "berserk",	do_berserk,	POS_FIGHTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "circle",	do_circle,	POS_FIGHTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "feed",	do_feed,	POS_FIGHTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "dirt",	do_dirt,	POS_FIGHTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "disarm",	do_disarm,	POS_FIGHTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "flee",	do_flee,	POS_FIGHTING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "gouge",	do_gouge,	POS_FIGHTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "kick",	do_kick,	POS_FIGHTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "strike",	do_strike,	POS_FIGHTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "murde",	do_murde,	POS_FIGHTING,	 0,  LOG_NORMAL, CBT, NS },
{ "murder",	do_murder,	POS_FIGHTING,	 0,  LOG_SIMPLE, CBT, 0 },
{ "surrender",	do_surrender,	POS_FIGHTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "rescue",	do_rescue,	POS_FIGHTING,	 0,  LOG_NORMAL, GEN, NS },
{ "trip",	do_trip,	POS_FIGHTING,    0,  LOG_NORMAL, GEN, 0 },

    /*
     * Mob command interpreter (placed here for faster scan...)
     */
    { "mob",	do_mob,         POS_DEAD,        0,  LOG_NEVER,  GEN, 0 },

    /*
     * Miscellaneous commands.
     */
{ "account",	do_account,	POS_STANDING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "enter", 	do_enter, 	POS_STANDING,	 0,  LOG_NORMAL, GEN, 0 },
{ "follow",	do_follow,	POS_RESTING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "fish",	do_fish,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "lore",	do_lore,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "cook",	do_cook,	POS_RESTING,	ML,  LOG_NORMAL, GEN, 0 },
{ "rsave",	do_rsave,	POS_RESTING,	ML,  LOG_NORMAL, GEN, 0 },
{ "owho",	do_who,		POS_DEAD,	 0,  LOG_NORMAL, GEN, NS },
{ "nscore",	do_nscore,	POS_SLEEPING,	ML,  LOG_NORMAL, GEN, NS },
{ "gain",	do_gain,	POS_STANDING,	 0,  LOG_NORMAL, GEN, 0 },
{ "go",		do_enter,	POS_STANDING,	 0,  LOG_NORMAL, GEN, NS },
{ "groups",	do_groups,	POS_SLEEPING,    0,  LOG_NORMAL, GEN, 0 },
{ "hide",	do_hide,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "manashield", do_manashield,  POS_FIGHTING,    0,  LOG_ALWAYS, GEN, NS },
// { "member",	do_member,	POS_RESTING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "promote",	do_promote,	POS_DEAD,	 90, LOG_NORMAL, GEN, 0 },
{ "play",	do_play,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "qui",	do_qui,		POS_DEAD,	 0,  LOG_NORMAL, GEN, NS },
{ "quit",	do_quit,	POS_DEAD,	 0,  LOG_NORMAL, GEN, 0 },
{ "recall",	do_recall,	POS_FIGHTING,	 0,  LOG_SIMPLE, GEN, 0 },
{ "/",		do_recall,	POS_FIGHTING,	 0,  LOG_SIMPLE, GEN, NS },
{ "rent",	do_rent,	POS_DEAD,	 0,  LOG_NORMAL, GEN, NS },
{ "save",	do_save,	POS_DEAD,	 0,  LOG_NORMAL, GEN, 0 },
{ "sleep",	do_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL, GEN, 0 },
{ "sneak",	do_sneak,	POS_STANDING,	 0,  LOG_NORMAL, GEN, 0 },
{ "split",	do_split,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "steal",	do_steal,	POS_STANDING,	 0,  LOG_NORMAL, GEN, 0 },
{ "train",	do_train,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "track",	do_track,	POS_STANDING,	 0,  LOG_NORMAL, GEN, 0 },
{ "visible",	do_visible,	POS_SLEEPING,	 0,  LOG_NORMAL, GEN, 0 },
{ "wake",	do_wake,	POS_SLEEPING,	 0,  LOG_NORMAL, GEN, 0 },
{ "where",	do_where,	POS_RESTING,	 0,  LOG_NORMAL, GEN, 0 },
{ "donate",	do_donate,	POS_STANDING,	 5,  LOG_NORMAL, GEN, 0 },
{ "cdonate",	do_cdonate,	POS_STANDING,	 5,  LOG_NORMAL, GEN, 0 },
{ "class",	do_class,	POS_SLEEPING,    0,  LOG_NORMAL, GEN, 0 },
{ "forge",	do_forge,	POS_SLEEPING,    0,  LOG_NORMAL, GEN, NS },
{ "forget",	do_forget,	POS_SLEEPING,    0,  LOG_SIMPLE, GEN, 0 },
{ "remembe",	do_remembe,	POS_SLEEPING,    0,  LOG_NORMAL, GEN, NS },
{ "remember",	do_remember,	POS_SLEEPING,    0,  LOG_SIMPLE, GEN, 0 },
{ "voodoo",	do_voodoo,	POS_STANDING,	20,  LOG_SIMPLE, GEN, 0 },

{ "search",	do_search,	POS_STANDING,	ML,  LOG_SIMPLE, GEN, 0 },
{ "compress",	do_compress,	POS_DEAD,        0,  LOG_NORMAL, CFG, 0 },


    /*
     * Immortal commands.
     */
{ "advance",	do_advance,	POS_DEAD,	ML,  LOG_ALWAYS, GEN, 0 },
{ "arena",      do_arena,       POS_DEAD,       L6,  LOG_NORMAL, GEN, 0 },
{ "dump",	do_dump,	POS_DEAD,	ML,  LOG_ALWAYS, GEN, NS },
{ "trust",	do_trust,	POS_DEAD,	ML,  LOG_ALWAYS, GEN, 0 },
{ "violate",	do_violate,	POS_DEAD,	ML,  LOG_ALWAYS, GEN, 0 },

{ "addlag",     do_addlag,      POS_DEAD,       ML,  LOG_ALWAYS, GEN, 0 },
{ "allow",	do_allow,	POS_DEAD,	L2,  LOG_ALWAYS, GEN, 0 },
{ "arealinks",  do_arealinks,   POS_DEAD,       ML,  LOG_NORMAL, GEN, 1 },
{ "avatar",     do_avatar,      POS_DEAD,       IM,  LOG_ALWAYS, GEN, 0 },
{ "ban",	do_ban,		POS_DEAD,	L2,  LOG_ALWAYS, GEN, 0 },
{ "bonus",	do_bonus,	POS_DEAD,	L1,  LOG_ALWAYS, GEN, 0 },
{ "deny",	do_deny,	POS_DEAD,	L1,  LOG_ALWAYS, GEN, 0 },
{ "disconnect",	do_disconnect,	POS_DEAD,	L3,  LOG_ALWAYS, GEN, 0 },
{ "dupe",	do_dupe,	POS_DEAD,	L1,  LOG_SIMPLE, GEN, 0 },
{ "finger",	do_finger,	POS_DEAD,	 0,  LOG_SIMPLE, GEN, 0 },
{ "flag",	do_flag,	POS_DEAD,	L2,  LOG_ALWAYS, GEN, 0 },
{ "freeze",	do_freeze,	POS_DEAD,	L4,  LOG_ALWAYS, GEN, 0 },
{ "permban",	do_permban,	POS_DEAD,	L1,  LOG_ALWAYS, GEN, 0 },
{ "protect",	do_protect,	POS_DEAD,	L1,  LOG_ALWAYS, GEN, 0 },
{ "reboo",	do_reboo,	POS_DEAD,	L1,  LOG_NORMAL, GEN, NS },
{ "reboot",	do_reboot,	POS_DEAD,	L1,  LOG_ALWAYS, GEN, 0 },
{ "copyove",	do_copyove,	POS_DEAD,	ML,  LOG_NORMAL, GEN, NS },
{ "copyover",	do_copyover,	POS_DEAD,	ML,  LOG_ALWAYS, GEN, 0 },
{ "award",	do_award,	POS_DEAD,	ML,  LOG_ALWAYS, GEN, 0 },
{ "set",	do_set,		POS_DEAD,	L2,  LOG_ALWAYS, GEN, 0 },
{ "security",   do_security,    POS_DEAD,       L2,  LOG_ALWAYS, GEN, 0 },
{ "shutdow",	do_shutdow,	POS_DEAD,	ML,  LOG_NORMAL, GEN, NS },
{ "shutdown",	do_shutdown,	POS_DEAD,	ML,  LOG_ALWAYS, GEN, 0 },
{ "wedpost",	do_wedpost,	POS_DEAD,	ML,  LOG_SIMPLE, GEN, 0 },
{ "repop",	do_repop,	POS_DEAD,	L2,  LOG_SIMPLE, GEN, 0 },
{ "pathfind",	do_pathfind,	POS_DEAD,	L2,  LOG_SIMPLE, NFO, 0 },
{ "vmfind",	do_vmfind,	POS_DEAD,	L2,  LOG_SIMPLE, NFO, 0 },
{ "wizlock",	do_wizlock,	POS_DEAD,	L2,  LOG_ALWAYS, GEN, 0 },

{ "force",	do_force,	POS_DEAD,	L7,  LOG_ALWAYS, GEN, 0 },
{ "load",	do_load,	POS_DEAD,	L6,  LOG_ALWAYS, GEN, 0 },
{ "newlock",	do_newlock,	POS_DEAD,	L1,  LOG_ALWAYS, GEN, 0 },
{ "nochannels",	do_nochannels,	POS_DEAD,	L6,  LOG_ALWAYS, GEN, 0 },
{ "noemote",	do_noemote,	POS_DEAD,	L5,  LOG_ALWAYS, GEN, 0 },
{ "noclan",	do_noclan,	POS_DEAD,	L3,  LOG_ALWAYS, GEN, 0 },
{ "norestore",	do_norestore,	POS_DEAD,	L3,  LOG_ALWAYS, GEN, 0 },
{ "noshout",	do_noshout,	POS_DEAD,	L6,  LOG_ALWAYS, GEN, 0 },
{ "notell",	do_notell,	POS_DEAD,	L5,  LOG_ALWAYS, GEN, 0 },
{ "notitle",	do_notitle,	POS_DEAD,	L3,  LOG_ALWAYS, GEN, 0 },
{ "pack",	do_pack,	POS_DEAD,     101,  LOG_SIMPLE, GEN, ST },
{ "pecho",	do_pecho,	POS_DEAD,	L5,  LOG_SIMPLE, GEN, 0 }, 
{ "pload",	do_pload,	POS_DEAD,	ML,  LOG_SIMPLE, GEN, 0 },
{ "punload",	do_punload,	POS_DEAD,	ML,  LOG_SIMPLE, GEN, 0 },
{ "purge",	do_purge,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, 0 },
{ "restore",	do_restore,	POS_DEAD,	L4,  LOG_SIMPLE, GEN, 0 },
{ "sla",	do_sla,		POS_DEAD,	L3,  LOG_SIMPLE, GEN, NS },
{ "slay",	do_slay,	POS_DEAD,	L3,  LOG_ALWAYS, GEN, 0 },
{ "teleport",	do_transfer,    POS_DEAD,	L6,  LOG_ALWAYS, GEN, 0 },	
{ "transfer",	do_transfer,	POS_DEAD,	L6,  LOG_ALWAYS, GEN, 0 },
{ "corner",	do_corner,	POS_DEAD,	IM,  LOG_ALWAYS, GEN, 0 },
{ "twit",	do_twit,	POS_DEAD,	L7,  LOG_NORMAL, GEN, 0 },
{ "pardon",	do_pardon,	POS_DEAD,	L7,  LOG_NORMAL, GEN, 0 },
{ "omni",	do_omni,	POS_DEAD,	L8,  LOG_NORMAL, GEN, 0 },

{ "poofin",	do_bamfin,	POS_DEAD,	L8,  LOG_SIMPLE, GEN, 0 },
{ "poofout",	do_bamfout,	POS_DEAD,	L8,  LOG_SIMPLE, GEN, 0 },
{ "gecho",	do_echo,	POS_DEAD,	L4,  LOG_SIMPLE, GEN, 0 },
{ "ghost",	do_ghost,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, 0 },
{ "holylight",	do_holylight,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, 0 },
{ "incognito",	do_incognito,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, 0 },
{ "invis",	do_invis,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, NS },
{ "log",	do_log,		POS_DEAD,	L1,  LOG_ALWAYS, GEN, 0 },
{ "memory",	do_memory,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, 0 },
{ "mwhere",	do_mwhere,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, 0 },
{ "owhere",	do_owhere,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, 0 },
{ "peace",	do_peace,	POS_DEAD,	L2,  LOG_NORMAL, GEN, 0 },
{ "allpeace",	do_allpeace,	POS_DEAD,	L1,  LOG_SIMPLE, GEN, 0 },
{ "wpeace",	do_allpeace,	POS_DEAD,	L1,  LOG_SIMPLE, GEN, NS },
{ "echo",	do_recho,	POS_DEAD,	L8,  LOG_SIMPLE, GEN, 0 },
{ "return",	do_return,      POS_DEAD,       L7,  LOG_SIMPLE, GEN, 0 },
{ "snoop",	do_snoop,	POS_DEAD,	L4,  LOG_ALWAYS, GEN, 0 },
{ "spellup",	do_spellup,     POS_DEAD,       L8,  LOG_SIMPLE, GEN, 0 },
{ "stat",	do_stat,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, 0 },
{ "string",	do_string,	POS_DEAD,	L6,  LOG_ALWAYS, GEN, 0 },
{ "switch",	do_switch,	POS_DEAD,	L7,  LOG_ALWAYS, GEN, 0 },
{ "wizinvis",	do_invis,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, 0 },
{ "wecho",	do_wecho,	POS_DEAD,	ML,  LOG_SIMPLE, GEN, 0 },
{ "message",	do_message,	POS_DEAD,	ML,  LOG_SIMPLE, GEN, 0 },
{ "vnum",	do_vnum,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, 0 },
{ "zecho",	do_zecho,	POS_DEAD,	L4,  LOG_SIMPLE, GEN, 0 },
// { "clead",	do_clead,	POS_DEAD,	L1,  LOG_ALWAYS, GEN, 0 },

{ "spellup",	do_spellup_auto,POS_DEAD,        0,  LOG_SIMPLE, GEN, 0 },

{ "clone",	do_clone,	POS_DEAD,	L6,  LOG_SIMPLE, GEN, 0 },
{ "coinflip",	do_coinflip,	POS_DEAD,	L6,  LOG_SIMPLE, GEN, 0 },

{ "wiznet",	do_wiznet,	POS_DEAD,	 0,  LOG_SIMPLE, GEN, ST },
{ "herotalk",   do_herotalk,    POS_DEAD, LEVEL_HERO,  LOG_SIMPLE, GEN, 0 },
{ "immtalk",	do_immtalk,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, 0 },
{ "imptalk",	do_imptalk,	POS_DEAD,	ML,  LOG_SIMPLE, GEN, 0 },
{ "imotd",	do_imotd,       POS_DEAD,       IM,  LOG_NORMAL, GEN, 0 },
{ ":",		do_immtalk,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, NS },
{ "smote",	do_smote,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, 0 },
{ "prefi",	do_prefi,	POS_DEAD,	IM,  LOG_NORMAL, GEN, NS },
{ "prefix",	do_prefix,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, 0 },
{ "mpoint",	do_mpoint,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, 0 },
{ "mquest",	do_mquest,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, 0 },
// { "randclan",	do_randclan,	POS_DEAD,	ML,  LOG_SIMPLE, GEN, 0 },
{ "immkiss",	do_immkiss,	POS_DEAD,	L4,  LOG_SIMPLE, GEN, 0 },
{ "squire",	do_squire,	POS_DEAD,	ML,  LOG_ALWAYS, GEN, 0 },
{ "knight",	do_knight,	POS_DEAD,	ML,  LOG_ALWAYS, GEN, 0 },
{ "recover",	do_recover,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, 0 },
{ "gset",	do_gset,	POS_DEAD,	IM,  LOG_SIMPLE, GEN, 0 },
{ "wizslap",	do_wizslap,	POS_DEAD,	L5,  LOG_ALWAYS, GEN, 0 },
{ "beep",	do_beep,	POS_DEAD,	L3,  LOG_NORMAL, GEN, 0 },
{ "wipe",	do_wipe,	POS_DEAD,	L1,  LOG_ALWAYS, GEN, 0 },
{ "mpdump",	do_mpdump,      POS_DEAD,       IM,  LOG_NEVER, GEN, 0 },
{ "mpstat",	do_mpstat,      POS_DEAD,       IM,  LOG_NEVER, GEN, 0 },

{ "etest",	do_etest,	POS_DEAD,       ML,  LOG_SIMPLE, GEN, NS },
{ "map",	do_map, 	POS_DEAD,       ML,  LOG_SIMPLE, GEN, NS },

    /*
     * OLC
     */
{ "edit",	do_olc,         POS_DEAD,       IM,  LOG_NORMAL, GEN, 0 },
{ "asave",	do_asave,       POS_DEAD,       IM,  LOG_NORMAL, GEN, 0 },
{ "alist",	do_alist,       POS_DEAD,       IM,  LOG_NORMAL, GEN, 0 },
// { "clanedit",   do_clanedit,    POS_DEAD,       L1,  LOG_ALWAYS, GEN, 0 },
{ "gedit",   do_gedit,	POS_DEAD,	ML,  LOG_ALWAYS, GEN, 0 },
{ "resets",	do_resets,      POS_DEAD,    	IM,  LOG_NORMAL, GEN, 0 },
{ "redit",	do_redit,       POS_DEAD,    	IM,  LOG_NORMAL, GEN, 0 },
{ "medit",	do_medit,       POS_DEAD,    	IM,  LOG_NORMAL, GEN, 0 },
{ "aedit",	do_aedit,       POS_DEAD,    	IM,  LOG_NORMAL, GEN, 0 },
{ "oedit",	do_oedit,       POS_DEAD,    	IM,  LOG_NORMAL, GEN, 0 },
{ "olevel",     do_olevel,      POS_DEAD,       IM,  LOG_NORMAL, GEN, 0 },
{ "objdump",    do_objdump,     POS_DEAD,       ML,  LOG_ALWAYS, GEN, 0 },
{ "mlevel",     do_mlevel,      POS_DEAD,       IM,  LOG_NORMAL, GEN, 0 },
{ "mpedit",	do_mpedit,      POS_DEAD,       IM,  LOG_NORMAL, GEN, 0 },
{ "hedit",	do_hedit,	POS_DEAD,       IM,  LOG_NORMAL, GEN, 1 },
{ "mstat",      do_mstat,       POS_DEAD,       IM,  LOG_NORMAL, GEN, 0 },
{ "mset",       do_mset,        POS_DEAD,       ML,  LOG_ALWAYS, GEN, 0 },
{ "ostat",      do_ostat,       POS_DEAD,       IM,  LOG_NORMAL, GEN, 0 },
{ "oset",       do_oset,        POS_DEAD,       L4,  LOG_ALWAYS, GEN, 0 },
{ "randomize",	do_randomize,	POS_DEAD,	L4,  LOG_NORMAL, GEN, 0 },
{ "saveguilds",       save_guilds,        POS_DEAD,       ML,  LOG_ALWAYS, GEN, 0 },
{ "skillstat",  do_skillstat,   POS_DEAD,       IM,  LOG_NORMAL, GEN, 0 },
{ "spellstat",  do_spellstat,   POS_DEAD,       IM,  LOG_NORMAL, GEN, 0 },
{ "roomcheck",	do_roomcheck,	POS_RESTING,	IM,  LOG_NORMAL, GEN, 1 },
{ "wrlist",     do_wrlist,      POS_DEAD,       IM,  LOG_NORMAL, GEN, 1 },

    /*
     * Commands needed last on list
     */
{ "restring",	do_restring,	POS_STANDING,	 0,  LOG_NORMAL, GEN, 0 },
{ "delet",	do_delet,	POS_DEAD,	 0,  LOG_NORMAL, GEN, NS },
{ "delete",	do_delete,	POS_STANDING,	 0,  LOG_NORMAL, GEN, 0 },
{ "vap",	do_vap,		POS_DEAD,	 ML, LOG_NORMAL, GEN, NS },
{ "vape",	do_vape,	POS_DEAD,	 ML, LOG_ALWAYS, GEN, 0 },
{ "rerol",	do_rerol,	POS_DEAD,	 0,  LOG_NORMAL, GEN, NS|ST },
{ "reroll",	do_reroll,	POS_STANDING,	 0,  LOG_ALWAYS, GEN, ST },

{ "lone",	do_lone,	POS_DEAD,	15,  LOG_NORMAL, GEN, NS },
{ "loner",	do_loner,	POS_STANDING,	15,  LOG_ALWAYS, GEN, 0 },

{ "gran",	do_gran,	POS_STANDING,	ML,  LOG_ALWAYS, GEN, NS },
{ "grant",	do_grant,	POS_STANDING,	ML,  LOG_ALWAYS, GEN, NS },

{ "revok",	do_revok,	POS_DEAD,	ML,  LOG_ALWAYS, GEN, NS },
{ "revoke",	do_revoke,	POS_DEAD,	ML,  LOG_ALWAYS, GEN, NS },

{ "gstat",	do_gstat,	POS_DEAD,	L1,  LOG_ALWAYS, GEN, NS },

{ "rusage",	do_rusage,	POS_DEAD,	ML,  LOG_NORMAL, GEN, NS },
{ "lastcomm",	do_lastcomm,	POS_DEAD,	ML,  LOG_NORMAL, GEN, 0 },
{ "renam",	do_renam,	POS_DEAD,	ML,  LOG_NORMAL, GEN, NS },
{ "rename",	do_rename,	POS_DEAD,	ML,  LOG_ALWAYS, GEN, NS },

{ "python",	do_python,	POS_DEAD,	ML,  LOG_ALWAYS, GEN, NS },
{ "runscript",	do_runscript,	POS_DEAD,	ML,  LOG_ALWAYS, GEN, NS },
{ "scriptsetup",do_scriptsetup,	POS_DEAD,	ML,  LOG_ALWAYS, GEN, NS },
{ "pycall",	do_pycall,	POS_DEAD,	ML,  LOG_ALWAYS, GEN, NS },

    /*
     * End of list.
     */
    { "",	0,		POS_DEAD,	 0,  LOG_NORMAL, 0, 0 }
};




/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret( CHAR_DATA *ch, char *argument )
{
    char command[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    extern char last_command[MAX_STRING_LENGTH];
    extern unsigned int  last_hostip;

    extern int nAllocString;
    extern int nAllocPerm;

    int string_count = nAllocString;
    int perm_count = nAllocPerm;
    char cmd_copy[MAX_INPUT_LENGTH];

    int cmd;
    int trust;
    bool found;

    /*
     * Placed here, so we don't have to worry about tildes, period.
     * RW
     */
    smash_tilde( argument );

    /*
     * Strip leading spaces.
     */
    while ( isspace(*argument) )
	argument++;
    if ( argument[0] == '\0' )
	return;

	strcpy(cmd_copy, argument) ;

    /*
     * No hiding.
     */
/*    REMOVE_BIT( ch->affected_by, AFF_HIDE ); */

    /*
     * Implement freeze command.
     */
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_FREEZE) )
    {
	send_to_char( "You're totally frozen!\n\r", ch );
	return;
    }

    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    strcpy( logline, argument );

    if ( ch->desc != NULL )
        last_hostip = ch->desc->hostip;
    sprintf(last_command,"%s: room [%d]: <%s>",ch->name,
            (ch->in_room) ? ch->in_room->vnum : -1,
            argument );

    if(!IS_NPC(ch))
        ch->pcdata->last_command_timestamp = current_time;

    if ( !isalpha(argument[0]) && !isdigit(argument[0]) )
    {
	command[0] = argument[0];
	command[1] = '\0';
	argument++;
	while ( isspace(*argument) )
	    argument++;
    }
    else
    {
	argument = one_argument( argument, command );
    }

    /*
     * Look for command in command table.
     */
    found = FALSE;
    trust = get_trust( ch );
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == cmd_table[cmd].name[0]
	&&   !str_prefix( command, cmd_table[cmd].name ) )
	{
            if (cmd_table[cmd].level <= trust || is_granted(ch,cmd_table[cmd].do_fun))
	    {
		found = TRUE;
		break;
            }
            else
	    {
                continue;
	    }

	    if (!IS_SET(cmd_table[cmd].flags,CMD_STIER))
	    {
		found = TRUE;
		break;
	    } else if (ch->class >= MAX_CLASS/2)
	    {
		found = TRUE;
		break;
	    } else if (ch->level >= LEVEL_HERO)
	    {
		found = TRUE;
		break;
	    }
	}
    }

    /*
     * Log and snoop.
     */
    if ( cmd_table[cmd].log == LOG_NEVER )
	strcpy( logline, "" );

    if ( ( !IS_NPC(ch) && cmd_table[cmd].log >= LOG_SIMPLE
         && cmd_table[cmd].log != LOG_NEVER )
    || ( IS_NPC(ch) && ch->desc && ch->desc->original
         && cmd_table[cmd].log >= ch->desc->original->log_level )
    ||   fLogAll
    ||   cmd_table[cmd].log == LOG_ALWAYS )
    {
	sprintf( log_buf, "Log %s: %s: %s",
            (ch->desc && ch->desc->original)
            ? ch->desc->original->name : ch->name,
            found ? cmd_table[cmd].name : "?", logline );
	log_string(LOG_COMMAND, log_buf );

        if(get_trust(ch) < MAX_LEVEL)
        {
            doubleup_char('$', log_buf, buf);
	    wiznet(buf,ch,NULL,WIZ_SECURE,0,
	        UMAX(MAX_LEVEL,get_trust(ch)+1));
        }
    }

    if ( ch->desc != NULL && ch->desc->snoop_by != NULL )
    {
	write_to_buffer( ch->desc->snoop_by, "% ",    2 );
	write_to_buffer( ch->desc->snoop_by, logline, 0 );
	write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
    }

    if ( !found )
    {
	/*
	 * Look for command in socials table.
	 */
	if ( !check_social( ch, command, argument ) )
	    send_to_char( "Huh?\n\r", ch );
	return;
    }

    /*
     * Character not in position for command?
     */
    if ( ch->position < cmd_table[cmd].position )
    {
	switch( ch->position )
	{
	case POS_DEAD:
	    send_to_char( "Lie still; you are DEAD.\n\r", ch );
	    break;

	case POS_MORTAL:
	case POS_INCAP:
	    send_to_char( "You are hurt far too bad for that.\n\r", ch );
	    break;

	case POS_STUNNED:
	    send_to_char( "You are too stunned to do that.\n\r", ch );
	    break;

	case POS_SLEEPING:
	    send_to_char( "In your dreams, or what?\n\r", ch );
	    break;

	case POS_RESTING:
	    send_to_char( "Nah... You feel too relaxed...\n\r", ch);
	    break;

	case POS_SITTING:
	    send_to_char( "Better stand up first.\n\r",ch);
	    break;

	case POS_FIGHTING:
	    send_to_char( "No way!  You are still fighting!\n\r", ch);
	    break;

	}
	return;
    }

    if(cmd_table[cmd].position > POS_SLEEPING &&
       cmd_table[cmd].type != CMD_COMM &&
       cmd_table[cmd].type != CMD_CONFIG &&
       cmd_table[cmd].type != CMD_COMBAT)
    {
        REMOVE_BIT( ch->affected_by, AFF_HIDE );
        interrupt_char(ch, EVENT_INTERRUPT_ON_COMMAND);
    }


    /*
     * Dispatch the command.
     */
    (*cmd_table[cmd].do_fun) ( ch, argument );

if (string_count < nAllocString)
{
sprintf(buf,"Memcheck : Increase in strings : %d : %s : %s : %d", nAllocString, ch->name, cmd_copy, nAllocString - string_count) ;
wiznet(buf, NULL, NULL, WIZ_MEMCHECK,0,0) ;
log_string( LOG_ERR, "Memcheck : Increase in strings : %d : %s : %s : %d", nAllocString, ch->name, cmd_copy, nAllocString - string_count);
}

if (perm_count < nAllocPerm)
{
sprintf(buf,"Increase in perms : %d : %s : %s : %d", nAllocPerm, ch->name, cmd_copy, nAllocPerm - perm_count) ;
wiznet(buf, NULL, NULL, WIZ_MEMCHECK, 0,0) ;
log_string(LOG_ERR,"Increase in perms : %d : %s : %s : %d", nAllocPerm, ch->name, cmd_copy, nAllocPerm - perm_count);
}

    last_hostip = 0;

    tail_chain( );
    return;
}



bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int cmd;
    bool found;

    found  = FALSE;
    for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == social_table[cmd].name[0]
	&&   !str_prefix( command, social_table[cmd].name ) )
	{
	    found = TRUE;
	    break;
	}
    }

    if ( !found )
	return FALSE;

    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
	send_to_char( "You are anti-social!\n\r", ch );
	return TRUE;
    }

    switch ( ch->position )
    {
    case POS_DEAD:
	send_to_char( "Lie still; you are DEAD.\n\r", ch );
	return TRUE;

    case POS_INCAP:
    case POS_MORTAL:
	send_to_char( "You are hurt far too bad for that.\n\r", ch );
	return TRUE;

    case POS_STUNNED:
	send_to_char( "You are too stunned to do that.\n\r", ch );
	return TRUE;

    case POS_SLEEPING:
	/*
	 * I just know this is the path to a 12" 'if' statement.  :(
	 * But two players asked for it already!  -- Furey
	 */
/*
	if ( !str_cmp( social_table[cmd].name, "snore" ) )
	    break;*/
	send_to_char( "In your dreams, or what?\n\r", ch );
	return TRUE;

    }

    one_argument( argument, arg );
    victim = NULL;
    if ( arg[0] == '\0' )
    {
	act( social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM    );
	act( social_table[cmd].char_no_arg,   ch, NULL, victim, TO_CHAR    );
    }
    else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
    }
    else if ( victim == ch )
    {
	act( social_table[cmd].others_auto,   ch, NULL, victim, TO_ROOM    );
	act( social_table[cmd].char_auto,     ch, NULL, victim, TO_CHAR    );
    }
    else
    {
	act( social_table[cmd].others_found,  ch, NULL, victim, TO_NOTVICT );
	act( social_table[cmd].char_found,    ch, NULL, victim, TO_CHAR    );
	act( social_table[cmd].vict_found,    ch, NULL, victim, TO_VICT    );

	if ( !IS_NPC(ch) && IS_NPC(victim)
	&&   !IS_AFFECTED(victim, AFF_CHARM)
	&&   IS_AWAKE(victim) 
	&&   victim->desc == NULL)
	{
	    switch ( number_bits( 4 ) )
	    {
	    case 0:

	    case 1: case 2: case 3: case 4:
	    case 5: case 6: case 7: case 8:
		act( social_table[cmd].others_found,
		    victim, NULL, ch, TO_NOTVICT );
		act( social_table[cmd].char_found,
		    victim, NULL, ch, TO_CHAR    );
		act( social_table[cmd].vict_found,
		    victim, NULL, ch, TO_VICT    );
		break;

	    case 9: case 10: case 11: case 12:
		act( "$n slaps $N.",  victim, NULL, ch, TO_NOTVICT );
		act( "You slap $N.",  victim, NULL, ch, TO_CHAR    );
		act( "$n slaps you.", victim, NULL, ch, TO_VICT    );
		break;
	    }
	}
    }

    return TRUE;
}



/*
 * Return true if an argument is completely numeric.
 */
bool is_number ( char *arg )
{
 
    if ( *arg == '\0' )
        return FALSE;
 
    if ( *arg == '+' || *arg == '-' )
        arg++;
 
    for ( ; *arg != '\0'; arg++ )
    {
        if ( !isdigit( *arg ) )
            return FALSE;
    }
 
    return TRUE;
}


/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument( char *argument, char *arg )
{
    char *pdot;
    int number;
    
    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
	if ( *pdot == '.' )
	{
	    *pdot = '\0';
	    number = atoi( argument );
	    *pdot = '.';
	    strcpy( arg, pdot+1 );
	    return number;
	}
    }

    strcpy( arg, argument );
    return 1;
}

/* 
 * Given a string like 14*foo, return 14 and 'foo'
*/
int mult_argument(char *argument, char *arg)
{
    char *pdot;
    int number;

    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
        if ( *pdot == '*' )
        {
            *pdot = '\0';
            number = atoi( argument );
            *pdot = '*';
            strcpy( arg, pdot+1 );
            return number;
        }
    }
 
    strcpy( arg, argument );
    return 1;
}

void separate_char_end(char *argument, char ch, char *farg, char *endarg)
{
    char *pdot;
    for ( pdot = argument; *pdot != '\0'; pdot++, farg++ )
    {
        if(*pdot == ch)
        {
            *farg = '\0';

            strcpy(endarg, pdot + 1);
            return;
        }
        *farg = *pdot;
    }

    *farg = '\0';
    strcpy(endarg, "");
    return;
}

/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument( char *argument, char *arg_first )
{
    char cEnd;

    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*arg_first = LOWER(*argument);
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( isspace(*argument) )
	argument++;

    return argument;
}

char *one_case_argument( char *argument, char *arg_first )
{
    char cEnd;

    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*arg_first = *argument;
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( isspace(*argument) )
	argument++;

    return argument;
}


void do_commands( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;
 
    col = 0;
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level <  LEVEL_HERO
        &&   cmd_table[cmd].level <= get_trust( ch ) 
	&&   !IS_SET(cmd_table[cmd].flags, CMD_NOSHOW))
	{
	    if (!IS_SET(cmd_table[cmd].flags, CMD_STIER))
	    {
		sprintf( buf, "%-12s", cmd_table[cmd].name );
		send_to_char( buf, ch );
		if ( ++col % 6 == 0 )
		    send_to_char( "\n\r", ch );
	    } else if (ch->class >= MAX_CLASS/2)
	    {
		sprintf( buf, "%-12s", cmd_table[cmd].name );
		send_to_char( buf, ch );
		if ( ++col % 6 == 0 )
		    send_to_char( "\n\r", ch );
	    } else if (ch->level >= LEVEL_HERO)
	    {
		sprintf( buf, "%-12s", cmd_table[cmd].name );
		send_to_char( buf, ch );
		if ( ++col % 6 == 0 )
		    send_to_char( "\n\r", ch );
	    }
	}
    }
 
    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
    return;
}

void do_wizhelp( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;
    int clevel;
    col = 0;

    for( clevel = LEVEL_HERO + 1; clevel < MAX_LEVEL + 1; clevel++ ) 
    {
        for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
	{ 
            if ( cmd_table[cmd].level >= LEVEL_HERO
            &&   cmd_table[cmd].level <= get_trust( ch ) 
	    &&   cmd_table[cmd].level == clevel)
	    {
	        sprintf( buf, "{C[{x%-3d{C] {w%-12s{x", cmd_table[cmd].level, cmd_table[cmd].name );
	        send_to_char( buf, ch );
	        if ( ++col % 4 == 0 )
		    send_to_char( "\n\r", ch );
	    }
	}
    }

    if ( ++col % 4 == 0 )
       send_to_char( "\n\r", ch );
 
    return;
}

void do_bonus( CHAR_DATA *ch, char *argument)
{
	 CHAR_DATA *victim;
	 char       buf  [ MAX_STRING_LENGTH ];
	 char       arg1 [ MAX_INPUT_LENGTH ];
	 char       arg2 [ MAX_INPUT_LENGTH ];
	 int      value;

	 argument = one_argument( argument, arg1 );
	 argument = one_argument( argument, arg2 );

	 if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
	 {
	send_to_char( "{wSyntax: bonus <char> <Exp>.{x\n\r", ch );
	return;

	 }
	 if (( victim = get_char_world ( ch, arg1 ) ) == NULL )
	 {
		send_to_char("{wThat Player is not here.{x\n\r", ch);
		return;
	 }

	 if ( IS_NPC( victim ) )
	 {
	send_to_char( "{wNot on NPC's.{x\n\r", ch );
	return;
	 }

	 if ( ch == victim )
	 {
	send_to_char( "{wYou may not bonus yourself.{x\n\r", ch );
	return;
	 }

	 if (IS_IMMORTAL(victim) || victim->level >= LEVEL_IMMORTAL)
	 {
	 send_to_char("{wYou can't bonus immortals silly!{x\n\r", ch);
	 return;
	 }

	 value = atoi( arg2 );
	 if ( value < -5000 || value > 5000 )
	 {
	send_to_char( "{wValue range is -5000 to 5000.{x\n\r", ch );
	return;
	 }

	 if ( value == 0 )
	 {
	send_to_char( "{wThere is no point in giving 0 experience.{x\n\r", ch );
	return;
	 }

	 gain_exp(victim, value);
	 sprintf( buf,"{wYou have bonused %s a whopping %d experience points.{x\n\r",
			victim->name, value);
			send_to_char(buf, ch);

	 if ( value > 0 )
	 {
		sprintf( buf,"{wYou have been bonused %d experience points.{x\n\r", value );
		send_to_char( buf, victim );
	 }

	 else
	 {
		sprintf( buf,"{wYou have been penalized %d experience points.{x\n\r", value );
		send_to_char( buf, victim );
	 }

	 return;
}

void python_command(CHAR_DATA *ch, int cmd, char *argument)
{
    
}

