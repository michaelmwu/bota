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
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"


char *	const	dir_name	[]		=
{
    "north", "east", "south", "west", "up", "down",
    "north", "east", "south", "west", "up", "down"
};

char *	const	dir_name_rel	[]		=
{
    "north", "east", "south", "west", "above", "below",
    "north", "east", "south", "west", "above", "below"
};

const	sh_int	rev_dir		[]		=
{
    2, 3, 0, 1, 5, 4, 2, 3, 0, 1, 5, 4
};

const	sh_int	movement_loss	[SECT_MAX]	=
{
    1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 8, 5, 5
};

const	char *	stage_names	[MAX_STAGE]	=
{
  "novice", "intermediate", "expert", "master", "grand master"
};

const struct
{
    char *name;
    char *keywords;
    int recall;
} hometowns [2] =
{ /* umm, this does nothing. Incomplete code. */
  {"Midgaard", "Midgaard", 3001},
  {"NewThalos", "NewThalos", 9609}
};

/* item type list */
const struct item_type		item_table	[]	=
{
    {	ITEM_LIGHT,		"light"		},
    {	ITEM_SCROLL,		"scroll"	},
    {	ITEM_WAND,		"wand"		},
    {   ITEM_STAFF,		"staff"		},
    {   ITEM_WEAPON,		"weapon"	},
    {   ITEM_TREASURE,		"treasure"	},
    {   ITEM_ARMOR,		"armor"		},
    {	ITEM_POTION,		"potion"	},
    {	ITEM_CLOTHING,		"clothing"	},
    {   ITEM_FURNITURE,		"furniture"	},
    {	ITEM_TRASH,		"trash"		},
    {	ITEM_CONTAINER,		"container"	},
    {	ITEM_DRINK_CON, 	"drink"		},
    {	ITEM_KEY,		"key"		},
    {	ITEM_FOOD,		"food"		},
    {	ITEM_MONEY,		"money"		},
    {	ITEM_BOAT,		"boat"		},
    {	ITEM_CORPSE_NPC,	"npc_corpse"	},
    {	ITEM_CORPSE_PC,		"pc_corpse"	},
    {   ITEM_FOUNTAIN,		"fountain"	},
    {	ITEM_PILL,		"pill"		},
    {	ITEM_PROTECT,		"protect"	},
    {	ITEM_MAP,		"map"		},
    {	ITEM_PORTAL,		"portal"	},
    {	ITEM_WARP_STONE,	"warp_stone"	},
    {	ITEM_ROOM_KEY,		"room_key"	},
    {	ITEM_GEM,		"gem"		},
    {	ITEM_JEWELRY,		"jewelry"	},
    {   ITEM_JUKEBOX,		"jukebox"	},
    {	ITEM_DEMON_STONE,	"demon_stone"	},
    {	ITEM_EXIT,		"exit"		},
    {	ITEM_PIT,		"pit"		},
    {	ITEM_IKEEP,		"ikeeper"	},
    {	ITEM_MAGIC_CLOUD,	"magic_cloud"	},
    {	ITEM_FISHING_POLE,	"fishing_pole"	},
    {	ITEM_UNCOOKED_FOOD,	"uncooked_food"	},
    {	ITEM_TRAP,		"trap"		},
    {	ITEM_GENERIC,		"generic"	},
    {   0,			NULL		}
};


/* weapon selection table */
const	struct	weapon_type	weapon_table	[]	=
{
   { "sword",	OBJ_VNUM_SCHOOL_SWORD,	WEAPON_SWORD,	&gsn_sword	},
   { "mace",	OBJ_VNUM_SCHOOL_MACE,	WEAPON_MACE,	&gsn_mace 	},
   { "dagger",	OBJ_VNUM_SCHOOL_DAGGER,	WEAPON_DAGGER,	&gsn_dagger	},
   { "axe",	OBJ_VNUM_SCHOOL_AXE,	WEAPON_AXE,	&gsn_axe	},
   { "staff",	OBJ_VNUM_SCHOOL_STAFF,	WEAPON_SPEAR,	&gsn_spear	},
   { "flail",	OBJ_VNUM_SCHOOL_FLAIL,	WEAPON_FLAIL,	&gsn_flail	},
   { "whip",	OBJ_VNUM_SCHOOL_WHIP,	WEAPON_WHIP,	&gsn_whip	},
   { "polearm",	OBJ_VNUM_SCHOOL_POLEARM,WEAPON_POLEARM,	&gsn_polearm	},
   { NULL,	0,				0,	NULL		}
};


 
/* wiznet table and prototype for future flag setting */
const   struct wiznet_type      wiznet_table    []              =
{
   {    "on",           WIZ_ON,         0  },
   {    "prefix",	WIZ_PREFIX,	0  },
   {    "ticks",        WIZ_TICKS,      IM },
   {    "log",		WIZ_LOG,	IM },
   {    "logins",       WIZ_LOGINS,     IM },
   {    "sites",        WIZ_SITES,      IM },
   {    "links",        WIZ_LINKS,      L5 },
   {	"newbies",	WIZ_NEWBIE,	0  },
   {	"spam",		WIZ_SPAM,	L5 },
   {    "deaths",       WIZ_DEATHS,     IM },
   {    "resets",       WIZ_RESETS,     L4 },
   {    "mobdeaths",    WIZ_MOBDEATHS,  L4 },
   {    "flags",	WIZ_FLAGS,	L5 },
   {	"penalties",	WIZ_PENALTIES,	L5 },
   {	"saccing",	WIZ_SACCING,	L5 },
   {	"levels",	WIZ_LEVELS,	0  },
   {	"load",		WIZ_LOAD,	L2 },
   {	"restore",	WIZ_RESTORE,	L2 },
   {	"snoops",	WIZ_SNOOPS,	L2 },
   {	"switches",	WIZ_SWITCHES,	L2 },
   {	"secure",	WIZ_SECURE,	ML },
   {	"bugs",		WIZ_BUGS,	ML },
   {	"numbers",	WIZ_NUMBERS,	ML },
   {	"transfers",	WIZ_TRANSFERS,	ML },
   {	"memorycheck",	WIZ_MEMCHECK,	ML },
   {	NULL,		0,		0  }
};

/* attack table  -- not very organized :(
 * Be sure to keep MAX_DAMAGE_MESSAGE up
 * to date in merc.h
 */
const 	struct attack_type	attack_table	[]		=
{
    { 	"none",		"hit",		-1		},  /*  0 */
    {	"slice",	"slice", 	DAM_SLASH	},	
    {   "stab",		"stab",		DAM_PIERCE	},
    {	"slash",	"slash",	DAM_SLASH	},
    {	"whip",		"whip",		DAM_SLASH	},
    {   "claw",		"claw",		DAM_SLASH	},  /*  5 */
    {	"blast",	"blast",	DAM_BASH	},
    {   "pound",	"pound",	DAM_BASH	},
    {	"crush",	"crush",	DAM_BASH	},
    {   "grep",		"grep",		DAM_SLASH	},
    {	"bite",		"bite",		DAM_PIERCE	},  /* 10 */
    {   "pierce",	"pierce",	DAM_PIERCE	},
    {   "suction",	"suction",	DAM_BASH	},
    {	"beating",	"beating",	DAM_BASH	},
    {   "digestion",	"digestion",	DAM_ACID	},
    {	"charge",	"charge",	DAM_BASH	},  /* 15 */
    { 	"slap",		"slap",		DAM_BASH	},
    {	"punch",	"punch",	DAM_BASH	},
    {	"wrath",	"wrath",	DAM_ENERGY	},
    {	"magic",	"magic",	DAM_ENERGY	},
    {   "divine",	"divine power",	DAM_HOLY	},  /* 20 */
    {	"cleave",	"cleave",	DAM_SLASH	},
    {	"scratch",	"scratch",	DAM_PIERCE	},
    {   "peck",		"peck",		DAM_PIERCE	},
    {   "peckb",	"peck",		DAM_BASH	},
    {   "chop",		"chop",		DAM_SLASH	},  /* 25 */
    {   "sting",	"sting",	DAM_PIERCE	},
    {   "smash",	 "smash",	DAM_BASH	},
    {   "shbite",	"shocking bite",DAM_LIGHTNING	},
    {	"flbite",	"flaming bite", DAM_FIRE	},
    {	"frbite",	"freezing bite", DAM_COLD	},  /* 30 */
    {	"acbite",	"acidic bite", 	DAM_ACID	},
    {	"chomp",	"chomp",	DAM_PIERCE	},
    {  	"drain",	"life drain",	DAM_NEGATIVE	},
    {   "thrust",	"thrust",	DAM_PIERCE	},
    {   "slime",	"slime",	DAM_ACID	},
    {	"shock",	"shock",	DAM_LIGHTNING	},
    {   "thwack",	"thwack",	DAM_BASH	},
    {   "flame",	"flame",	DAM_FIRE	},
    {   "chill",	"chill",	DAM_COLD	},
    {   "typo",		"typo",		DAM_SLASH	}, /* 40 */
    {   NULL,		NULL,		0		}
};

/* race table */
const 	struct	race_type	race_table	[]		=
{
/*
    {
	name,		pc_race?,
	act bits,
	aff_by bits,
	off bits,
	imm,
	res,
	vuln,
	shd bits,
	form,		parts 
    },
*/
    { "unique",		FALSE, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    { 
	"human",		TRUE, 
	0,
	0, 
	0,
	0, 
	0,
	0,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"elf",			TRUE,
	0,
	AFF_INFRARED,
	0,
	0,
	RES_CHARM,
	VULN_IRON,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"dwarf",		TRUE,
	0,
	AFF_INFRARED,
	0,
	0,
	RES_POISON|RES_DISEASE,
	VULN_DROWNING,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"giant",		TRUE,
	0,
	0,
	0,
	0,
	RES_FIRE|RES_COLD,
	VULN_MENTAL|VULN_LIGHTNING,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    { 
	"pixie",		TRUE,
	0,
	AFF_FLYING|AFF_DETECT_MAGIC|AFF_INFRARED,
	0,
	0,
	RES_CHARM|RES_MENTAL,
	VULN_IRON,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|P
    },

    { 
	"halfling",		TRUE,
	0,
	AFF_PASS_DOOR,
	0,
	0,
	RES_POISON|RES_DISEASE,
	VULN_LIGHT,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    { 
	"halforc",		TRUE,
	0,
	AFF_BERSERK,
	0,
	0,
	RES_MAGIC|RES_WEAPON,
	VULN_MENTAL,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    { 
	"goblin",		TRUE,
	0,
	AFF_INFRARED,
	0,
	0,
	RES_MENTAL,
	VULN_SILVER|VULN_LIGHT|VULN_WOOD|VULN_HOLY,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    { 
	"halfelf",		TRUE,
	0,
	AFF_FARSIGHT,
	0,
	0,
	0,
	0,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"avian",		TRUE,
	0,
    AFF_FLYING|AFF_DETECT_INVIS|AFF_DETECT_HIDDEN|AFF_FARSIGHT|AFF_INFRARED,
	0,
	0,
	RES_LIGHTNING,
	VULN_DISEASE|VULN_POISON,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|P
    },

    {
	"gnome",		TRUE,
	0,
	AFF_INFRARED,
	0,
	0,
	RES_MENTAL,
	VULN_DROWNING,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"draconian",		TRUE,
	0,
	AFF_FLYING,
	0,
	IMM_POISON|IMM_DISEASE,
	RES_FIRE|RES_COLD,
	VULN_SLASH|VULN_PIERCE|VULN_LIGHTNING,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|P|Q
    },

    {
        "centaur",              TRUE,
        0,
	0,
	0,
        0,
	0,
	0,
        0,
        A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
    },

    {
        "gnoll",                TRUE,
        0,
	AFF_DETECT_HIDDEN|AFF_DARK_VISION,
	0,
        0,
	0,
	VULN_SILVER,
        0,
        A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|U|V
    },

    {
        "heucuva",              TRUE,
        0,
	AFF_DARK_VISION,
	0,
        IMM_NEGATIVE|IMM_POISON|IMM_DISEASE,
	RES_WEAPON,
	VULN_HOLY|VULN_LIGHT|VULN_SILVER,
        0,
        A|H|M|V,        A|B|C|G|H|I|J|K
    },

    {
        "kenku",                TRUE,
        0,
	AFF_FARSIGHT|AFF_DETECT_HIDDEN|AFF_INFRARED|AFF_FLYING,
        0,
        0,
	RES_COLD,
	0,
        0,
        A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|P
    },

    {
        "minotaur",             TRUE,
        0,
	AFF_FARSIGHT,
	0,
        IMM_POISON,
	0,
	VULN_BASH,
        0,
        A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|W
    },

    {
        "satyr",                TRUE,
        0,
	AFF_DETECT_HIDDEN|AFF_DETECT_EVIL|AFF_DETECT_GOOD,
        0,
	IMM_FIRE,
	RES_NEGATIVE,
	VULN_HOLY|VULN_LIGHT,
        0,
        A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
    },

    {
        "titan",                TRUE,
        0,
	AFF_DETECT_INVIS|AFF_BERSERK,
	0,
        IMM_CHARM,
	0,
	0,
        0,
        A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"bat",			FALSE,
	0,
	AFF_FLYING|AFF_DARK_VISION,
	OFF_DODGE|OFF_FAST,
	0,
	0,
	VULN_LIGHT,
	0,
	A|G|V,		A|C|D|E|F|H|J|K|P
    },

    {
	"bear",			FALSE,
	0,
	0,
	OFF_CRUSH|OFF_DISARM|OFF_BERSERK,
	0,
	RES_BASH|RES_COLD,
	0,
	0,
	A|G|V,		A|B|C|D|E|F|H|J|K|U|V
    },

    {
	"cat",			FALSE,
	0,
	AFF_DARK_VISION,
	OFF_FAST|OFF_DODGE,
	0,
	0,
	0,
	0,
	A|G|V,		A|C|D|E|F|H|J|K|Q|U|V
    },

    {
	"centipede",		FALSE,
	0,
	AFF_DARK_VISION,
	0,
	0,
	RES_PIERCE|RES_COLD,
	VULN_BASH,
	0,
 	A|B|G|O,		A|C|K	
    },

    {
	"dog",			FALSE,
	0,
	0,
	OFF_FAST,
	0,
	0,
	0,
	0,
	A|G|V,		A|C|D|E|F|H|J|K|U|V
    },

    {
	"doll",			FALSE,
	0,
	0,
	0,
 IMM_COLD|IMM_POISON|IMM_HOLY|IMM_NEGATIVE|IMM_MENTAL|IMM_DISEASE|IMM_DROWNING,
	RES_BASH|RES_LIGHT,
	VULN_SLASH|VULN_FIRE|VULN_ACID|VULN_LIGHTNING|VULN_ENERGY,
	0,
	E|J|M|cc,	A|B|C|G|H|K
    },

    { 	"dragon", 		FALSE, 
	0,
	AFF_INFRARED|AFF_FLYING,
	0,
	0,
	RES_FIRE|RES_BASH|RES_CHARM, 
	VULN_PIERCE|VULN_COLD,
	0,
	A|H|Z,		A|C|D|E|F|G|H|I|J|K|P|Q|U|V|X
    },

    {
	"fido",			FALSE,
	0,
	0,
	OFF_DODGE|ASSIST_RACE,
	0,
	0,
	VULN_MAGIC,
	0,
	A|B|G|V,	A|C|D|E|F|H|J|K|Q|V
    },		
   
    {
	"fox",			FALSE,
	0,
	AFF_DARK_VISION,
	OFF_FAST|OFF_DODGE,
	0,
	0,
	0,
	0,
	A|G|V,		A|C|D|E|F|H|J|K|Q|V
    },

    {
	"hobgoblin",		FALSE,
	0,
	AFF_INFRARED,
	0,
	0,
	RES_DISEASE|RES_POISON,
	0,
	0,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|Y
    },

    {
	"kobold",		FALSE,
	0,
	AFF_INFRARED,
	0,
	0,
	RES_POISON,
	VULN_MAGIC,
	0,
	A|B|H|M|V,	A|B|C|D|E|F|G|H|I|J|K|Q
    },

    {
	"lizard",		FALSE,
	0,
	0,
	0,
	0,
	RES_POISON,
	VULN_COLD,
	0,
	A|G|X|cc,	A|C|D|E|F|H|K|Q|V
    },

    {
	"modron",		FALSE,
	0,
	AFF_INFRARED,
	ASSIST_RACE|ASSIST_ALIGN,
	IMM_CHARM|IMM_DISEASE|IMM_MENTAL|IMM_HOLY|IMM_NEGATIVE,
	RES_FIRE|RES_COLD|RES_ACID,
	0,
	0,
	H,		A|B|C|G|H|J|K
    },

    {
	"orc",			FALSE,
	0,
	AFF_INFRARED,
	0,
	0,
	RES_DISEASE,
	VULN_LIGHT,
	0,
	A|H|M|V,	A|B|C|D|E|F|G|H|I|J|K
    },

    {
	"pig",			FALSE,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	A|G|V,	 	A|C|D|E|F|H|J|K
    },	

    {
	"rabbit",		FALSE,
	0,
	0,
	OFF_DODGE|OFF_FAST,
	0,
	0,
	0,
	0,
	A|G|V,		A|C|D|E|F|H|J|K
    },
    
    {
	"school monster",	FALSE,
	ACT_NOALIGN,
	0,
	0,
	IMM_CHARM|IMM_SUMMON,
	0,
	VULN_MAGIC,
	0,
	A|M|V,		A|B|C|D|E|F|H|J|K|Q|U
    },	

    {
	"snake",		FALSE,
	0,
	0,
	0,
	0,
	RES_POISON,
	VULN_COLD,
	0,
	A|G|X|Y|cc,	A|D|E|F|K|L|Q|V|X
    },
 
    {
	"song bird",		FALSE,
	0,
	AFF_FLYING,
	OFF_FAST|OFF_DODGE,
	0,
	0,
	0,
	0,
	A|G|W,		A|C|D|E|F|H|K|P
    },

    {
	"troll",		FALSE,
	0,
	AFF_REGENERATION|AFF_INFRARED|AFF_DETECT_HIDDEN,
	OFF_BERSERK,
 	0,
	RES_CHARM|RES_BASH,
	VULN_FIRE|VULN_ACID,
	0,
	A|B|H|M|V,		A|B|C|D|E|F|G|H|I|J|K|U|V
    },

    {
	"water fowl",		FALSE,
	0,
	AFF_SWIM|AFF_FLYING,
	0,
	0,
	RES_DROWNING,
	0,
	0,
	A|G|W,		A|C|D|E|F|H|K|P
    },		
  
    {
	"wolf",			FALSE,
	0,
	AFF_DARK_VISION,
	OFF_FAST|OFF_DODGE,
	0,
	0,
	0,	
	0,
	A|G|V,		A|C|D|E|F|J|K|Q|V
    },

    {
	"wyvern",		FALSE,
	0,
	AFF_FLYING|AFF_DETECT_INVIS|AFF_DETECT_HIDDEN,
	OFF_BASH|OFF_FAST|OFF_DODGE,
	IMM_POISON,
	0,
	VULN_LIGHT,
	0,
	A|B|G|Z,		A|C|D|E|F|H|J|K|Q|V|X
    },

    { "unique",		FALSE, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

    {
	NULL, 0, 0, 0, 0, 0, 0
    }
};

const	struct	pc_race_type	pc_race_table	[]	=
{
    {
	"null race",	"",
	0,
	{ 100, 100, 100, 100, 100, 100, 100,
	  100, 100, 100, 100, 100, 100, 100 },
	{ "" },
	{ 13, 13, 13, 13, 13 },
	{ 18, 18, 18, 18, 18 },
	0,
	{
	  { -1,  0,  0 }
	}
    },
 
/*
    {
	"race name", 	short name,
	points,
	{ class multipliers },
	{ bonus skills },
	{ base stats },
	{ max stats },
	size 
    },
*/
    {
	"human",	"Human",
	0,
	{ 100, 100, 100, 100, 100, 100, 100,
	  90, 90, 90, 90, 90, 90, 90 },
	{ "" },
	{ 17, 16, 16, 17, 17 },
	{ 22, 21, 21, 22, 22 },
	SIZE_MEDIUM,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { -1			, 0		,    0 }
	}
    },

    { 	
	"elf",		"Elf",
	5,
	{ 100, 125,  100, 120, 120, 105, 115, 
	  90, 113,  90, 108, 95, 95, 104 }, 
	{ "sneak", "hide", "invisibility" },
	{ 12, 18, 14, 20, 13 },
	{ 17, 23, 19, 25, 18 },
	SIZE_SMALL,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { LANGUAGE_ELVISH	, KSKILL_INITIAL,  100 },
	  { -1			, 0		,    0 }

	}
    },

    {
	"dwarf",	"Dwarf",
	8,
	{ 150, 100, 125, 100, 110, 110, 110,
	  135, 90, 113, 90, 113, 99, 99 },
	{ "berserk" },
	{ 18, 12, 18, 12, 16 },
	{ 24, 17, 24, 17, 23 },
	SIZE_MEDIUM,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { LANGUAGE_DWARVISH	, KSKILL_INITIAL,  100 },
	  { -1			, 0		,    0 }
	}
    },

    {
	"giant",	"Giant",
	6,
	{ 200, 125, 150, 105, 125, 150, 120,
	  180, 113, 135, 95, 135, 135, 108 },
	{ "bash", "fast healing" },
	{ 20, 11, 11, 15, 20 },
	{ 25, 16, 16, 20, 25 },
	SIZE_LARGE,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { -1			, 0		,    0 }
	}
    },

    {
	"pixie",	"Pixie",
	6,
	{ 100, 100, 120, 200, 150, 100, 150,
	  90, 90, 108, 180, 95, 90, 135 },
	{ "" },
	{ 13, 20, 20, 20, 13 },
	{ 17, 25, 25, 25, 17 },
	SIZE_TINY,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { -1			, 0		,    0 }
	}
    },

    {
	"halfling",	"Hfling",
	5,
	{ 105, 120, 100, 150, 150, 120, 120,
	  95, 108, 90, 135, 108, 108, 108 },
	{ "sneak", "hide" },
	{ 13, 17, 17, 20, 15 },
	{ 18, 22, 22, 25, 20 },
	SIZE_SMALL,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { -1			, 0		,    0 }
	}
    },

    {
	"halforc",	"Hf-Orc",
	6,
	{ 200, 200, 120, 100, 125, 150, 105,
	  180, 180, 108, 90, 135, 135, 95 },
	{ "fast healing" },
	{ 18, 11, 11, 19, 18 },
	{ 23, 15, 15, 24, 23 },
	SIZE_MEDIUM,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { LANGUAGE_ORCISH	, KSKILL_INITIAL,   95 },
	  { -1			, 0		,    0 }
	}
    },

    {
	"goblin",	"Goblin",
	5,
	{ 105, 125, 110, 125, 120, 120, 110,
	  95, 113, 99, 113, 99, 108, 99 },
	{ "sneak", "hide" },
	{ 11, 19, 14, 20, 14 },
	{ 16, 24, 20, 25, 19 },
	SIZE_SMALL,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { -1			, 0		,    0 }
	}
    },

    {
	"halfelf",	"Hf-Elf",
	2,
	{ 105, 105, 105, 105, 105, 105, 105,
	  95, 95, 95, 95, 95, 95, 95 },
	{ "sneak", "hide" },
	{ 13, 16, 16, 15, 14 },
	{ 18, 23, 23, 20, 19 },
	SIZE_MEDIUM,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { LANGUAGE_ELVISH	, KSKILL_INITIAL,  95 },

	  { -1			, 0		,    0 }
	}
    },

    {
	"avian",	"Avian",
	5,
	{ 110, 105, 150, 125, 120, 100, 120,
	  99, 95, 135, 113, 108, 90, 108 },
	{ "fly" },
	{ 18, 16, 12, 18, 16 },
	{ 24, 21, 17, 23, 22 },
	SIZE_LARGE,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { -1			, 0		,    0 }
	}
    },

    {
	"gnome",	"Gnome",
	4,
	{ 100, 110, 150, 150, 125, 105, 150,
	  90, 99, 135, 135, 99, 95, 135 },
	{ "" },
	{ 12, 20, 20, 15, 12 },
	{ 17, 25, 25, 22, 17 },
	SIZE_SMALL,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { -1			, 0		,    0 }
	}
    },

    {
	"draconian",	"Dracon",
	11,
	{ 125, 150, 200, 100, 110, 125, 150,
	  113, 135, 180, 90, 108, 113, 135 },
	{ "fast healing" },
	{ 20, 17, 14, 11, 20 },
	{ 25, 23, 19, 16, 25 },
	SIZE_HUGE,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { -1			, 0		,    0 }
	}
    },

    {
	"centaur",	"Centr",
	9,
	{ 100, 110, 100, 175, 110, 110, 95,
	  90, 100, 90, 165, 100, 100, 85 },
	{ "enhanced damage" },
	{ 16, 18, 12, 14, 19 },
	{ 21, 23, 17, 19, 24 },
	SIZE_LARGE,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { -1			, 0		,    0 }
        }
    },

    {
	"gnoll",	"Gnoll",
	7,
	{ 110, 110, 125, 110, 175, 110, 110,
	  100, 100, 115, 100, 165, 100, 100 },
	{ "" },
	{ 15, 20, 20, 20, 15 },
	{ 20, 25, 25, 25, 19 },
	SIZE_LARGE,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { -1			, 0		,    0 }
	}
    },

    {
	"heucuva",	"Heucuv",
	10,
	{ 110, 110, 110, 100, 110, 110, 100,
          100, 100, 100, 90, 100, 100, 90 },
	{ "second attack" },
	{ 20, 12, 10, 20, 20 },
	{ 25, 17, 15, 25, 25 },
	SIZE_MEDIUM,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { -1			, 0		,    0 }
	}
    },

    {
	"kenku",	"Kenku",
	5,
	{ 125, 110, 150, 150, 110, 125, 180,
	  115, 100, 140, 140, 100, 115, 170 },
	{ "meditation" },
	{ 13, 15, 17, 20, 17 },
	{ 18, 20, 22, 25, 22 },
	SIZE_MEDIUM,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { -1			, 0		,    0 }
	}
    },

    {
	"minotaur",	"Minotr",
	7,
	{ 110, 110, 110, 95, 110, 110, 110,
	  100, 100, 100, 85, 100, 100, 100 },
	{ "enhanced damage" },
	{ 20, 12, 12, 12, 20 },
	{ 25, 17, 17, 17, 25 },
	SIZE_LARGE,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { -1			, 0		,    0 }
	}
    },

    {
	"satyr",	"Satyr",
	6,
	{ 110, 110, 110, 175, 110, 110, 150,
	  100, 100, 100, 165, 100, 100, 140 },
	{ "" },
	{ 20, 20, 10, 12, 17 },
	{ 25, 25, 15, 17, 23 },
	SIZE_LARGE,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { -1			, 0		,    0 }
	}
    },

    {
	"titan",	"Titan",
	11,
	{ 180, 105, 130, 105, 105, 130, 100,
	  170, 93, 115, 95, 95, 115, 98 },
	{ "fast healing" },
	{ 20, 15, 13, 12, 20 },
	{ 25, 20, 18, 17, 25 },
	SIZE_GIANT,
	{
	  { LANGUAGE_COMMON	, KSKILL_INITIAL,  100 },
	  { -1			, 0		,    0 }
	}
    }
};

	
      	

/*
 * Class table.
 */
const	struct	class_type	class_table	[MAX_CLASS]	=
{

/*
    {
	name, who, attr_prime, weapon,
	{guild1, guild2, guild3}, max_skill, thac0_00, thac0_32, hp_min, hp_max, mana?,
	base_group, default_group
    }
*/

    {
	"mage", "Mage",  STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,
	{ 3018, 9618, 18113 },  75,  20, 6,  9, 12, 1.2,
	"mage basics", "mage default"
    },

    {
	"cleric", "Cleric",  STAT_WIS,  OBJ_VNUM_SCHOOL_MACE,
	{ 3003, 9619, 5699 },  75,  20, 2,  10, 13, 1.4,
	"cleric basics", "cleric default"
    },

    {
	"thief", "Thief",  STAT_DEX,  OBJ_VNUM_SCHOOL_DAGGER,
	{ 3028, 9639, 5633 },  75,  20,  -4,  12, 14, 0.2,
	"thief basics", "thief default"
    },

    {
	"warrior", "Warrior",  STAT_STR,  OBJ_VNUM_SCHOOL_SWORD,
	{ 3022, 9633, 5613 },  75,  20,  -10,  20, 36, 0,
	"warrior basics", "warrior default"
    },

    {
	"ranger", "Ranger",  STAT_STR,  OBJ_VNUM_SCHOOL_SPEAR,
	{ 3372, 9752, 18111 },  75,  20,  -4,  14,  19, 1,
	"ranger basics", "ranger default"
    },

    {
	"druid", "Druid",  STAT_WIS,  OBJ_VNUM_SCHOOL_POLEARM,
	{ 3369, 9755, 18111 },  75,  20,  0,  10,  14,  1.1,
	"druid basics", "druid default"
    },

    {
	"vampire", "Vampire",  STAT_CON,  OBJ_VNUM_SCHOOL_DAGGER,
	{ 3375, 9758, 18113 },  75,  20, -30,  10, 13, 1.1,
	"vampire basics", "vampire default"
    },

    {
	"wizard", "Wizard",  STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,
	{ 3018, 9618, 18113 },  75,  10, -4,  12,  22, 1.3,
	"wizard basics", "wizard default"
    },

    {
	"priest", "Priest",  STAT_WIS,  OBJ_VNUM_SCHOOL_MACE,
	{ 3003, 9619, 5699 },  75,  10, 2,  13, 23, 1.4,
	"priest basics", "priest default"
    },

    {
	"mercenary", "Mercnry",  STAT_DEX,  OBJ_VNUM_SCHOOL_DAGGER,
	{ 3028, 9639, 5633 },  75,  10,  -14,  14, 24, 0.3,
	"mercenary basics", "mercenary default"
    },

    {
	"gladiator", "Gldiatr",  STAT_STR,  OBJ_VNUM_SCHOOL_SWORD,
	{ 3022, 9633, 5613 },  75,  10,  -20,  28, 40, 0,
	"gladiator basics", "gladiator default"
    },

    {
	"strider", "Strider",  STAT_DEX,  OBJ_VNUM_SCHOOL_DAGGER,
	{ 3372, 9752, 18111 },  75,  10,  -14,  20,  30, 0.5,
	"strider basics", "strider default"
    },

    {
	"sage", "Sage",  STAT_WIS,  OBJ_VNUM_SCHOOL_POLEARM,
	{ 3369, 9755, 18111 },  75,  10,  -10,  13,  23,  1.3,
	"sage basics", "sage default"
    },

    {
	"lich", "Lich",  STAT_CON,  OBJ_VNUM_SCHOOL_DAGGER,
	{ 3375, 9758, 18113 },  75,  10, -40,  14,  24, 1,
	"lich basics", "lich default"
    }
};


/*
 * Titles.
 */
char *	const			title_table	[MAX_CLASS][MAX_LEVEL+1][2] =
{

/*	Mage	*/

    {
        { "Man",                        "Woman"                         },
 
        { "Novice of Magic",            "Novice of Magic"               },
        { "Apprentice of Magic",        "Apprentice of Magic"           },
        { "Spell Novice",               "Spell Novice"                  },
        { "Spell Student",              "Spell Student"                 },
        { "Scholar of Magic",           "Scholar of Magic"              },
        { "Scholar of Wizardry",        "Scholar of Wizardry"           },
        { "Delver in Scrolls",          "Delveress in Scrolls"          },
        { "Delver in Spells",           "Delveress in Spells"           },
        { "Medium of Magic",            "Medium of Magic"               },
        { "Medium of Wizardry",         "Medium of Wizardry"            },
        { "Scribe of Magic",            "Scribess of Magic"             },
        { "Scribe of Wizardry",         "Scribess of Wizardry"          },
        { "Minor Seer",                 "Minor Seeress"                 },
        { "Seer",                       "Seeress"                       },
        { "Minor Sage",                 "Minor Sage"                    },
        { "Sage",                       "Sage"                          },
        { "Minor Illusionist",          "Minor Illusionist"             },
        { "Illusionist",                "Illusionist"                   },
        { "Minor Abjurer",              "Minor Abjuress"                },
        { "Abjurer",                    "Abjuress"                      },
        { "Minor Invoker",              "Minor Invoker"                 },
        { "Invoker",                    "Invoker"                       },
        { "Minor Enchanter",            "Minor Enchantress"             },
        { "Enchanter",                  "Enchantress"                   },
        { "Minor Conjurer",             "Minor Conjuress"               },
        { "Conjurer",                   "Conjuress"                     },
        { "Minor Magician",             "Minor Witch"                   },
        { "Magician",                   "Witch"                         },
        { "Minor Creator",              "Minor Creator"                 },
        { "Creator",                    "Creator"                       },
        { "Minor Savant",               "Minor Savant"                  },
        { "Savant",                     "Savant"                        },
        { "Minor Magus",                "Minor Craftess"                },
        { "Magus",                      "Craftess"                      },
        { "Minor Wizard",               "Minor Wizard"                  },
        { "Wizard",                     "Wizard"                        },
        { "Minor Warlock",              "Minor War Witch"               },
        { "Warlock",                    "War Witch"                     },
        { "Minor Sorcerer",             "Minor Sorceress"               },
        { "Sorcerer",                   "Sorceress"                     },
        { "Elder Sorcerer",             "Elder Sorceress"               },
        { "Elder Sorcerer",             "Elder Sorceress"               },
        { "Grand Sorcerer",             "Grand Sorceress"               },
        { "Grand Sorcerer",             "Grand Sorceress"               },
        { "Great Sorcerer",             "Great Sorceress"               },
        { "Great Sorcerer",             "Great Sorceress"               },
        { "Golem Maker",                "Golem Maker"                   },
        { "Golem Maker",                "Golem Maker"                   },
        { "Greater Golem Maker",        "Greater Golem Maker"           },
        { "Greater Golem Maker",        "Greater Golem Maker"           },
        { "Maker of Stones",            "Maker of Stones",              },
        { "Maker of Stones",            "Maker of Stones",              },
        { "Maker of Potions",           "Maker of Potions",             },
        { "Maker of Potions",           "Maker of Potions",             },
        { "Maker of Scrolls",           "Maker of Scrolls",             },
        { "Maker of Scrolls",           "Maker of Scrolls",             },
        { "Maker of Wands",             "Maker of Wands",               },
        { "Maker of Wands",             "Maker of Wands",               },
        { "Maker of Staves",            "Maker of Staves",              },
        { "Maker of Staves",            "Maker of Staves",              },
        { "Demon Summoner",             "Demon Summoner"                },
        { "Demon Summoner",             "Demon Summoner"                },
        { "Greater Demon Summoner",     "Greater Demon Summoner"        },
        { "Greater Demon Summoner",     "Greater Demon Summoner"        },
        { "Dragon Charmer",             "Dragon Charmer"                },
        { "Dragon Charmer",             "Dragon Charmer"                },
        { "Greater Dragon Charmer",     "Greater Dragon Charmer"        },
        { "Greater Dragon Charmer",     "Greater Dragon Charmer"        },
        { "Master of all Magic",        "Master of all Magic"           },
        { "Master of all Magic",        "Master of all Magic"           },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Master Mage",                "Master Mage"                   },
        { "Mage Hero",                  "Mage Heroine"                  },
        { "Squire of Magic",            "Squire of Magic"               },
        { "Knight of Magic",            "Dame of Magic"			},
        { "Demigod of Magic",           "Demigoddess of Magic"          },
        { "Immortal of Magic",          "Immortal of Magic"             },
        { "God of Magic",               "Goddess of Magic"              },
        { "Deity of Magic",             "Deity of Magic"                },
        { "Supremity of Magic",         "Supremity of Magic"            },
        { "Creator",                    "Creator"                       },
        { "Supreme Deity",              "Supreme Deity"                 }
    },

/*	Cleric	*/

    {
	{ "Man",			"Woman"				},

	{ "Believer",			"Believer"			},
	{ "Believer",			"Believer"			},
	{ "Attendant",			"Attendant"			},
	{ "Attendant",			"Attendant"			},
	{ "Acolyte",			"Acolyte"			},
	{ "Acolyte",			"Acolyte"			},
	{ "Novice",			"Novice"			},
	{ "Novice",			"Novice"			},
	{ "Missionary",			"Missionary"			},
	{ "Missionary",			"Missionary"			},
	{ "Adept",			"Adept"				},
	{ "Adept",			"Adept"				},
	{ "Deacon",			"Deaconess"			},
	{ "Deacon",			"Deaconess"			},
	{ "Vicar",			"Vicaress"			},
	{ "Vicar",			"Vicaress"			},
	{ "Priest",			"Priestess"			},
	{ "Priest",			"Priestess"			},
	{ "Minister",			"Lady Minister"			},
	{ "Minister",			"Lady Minister"			},
	{ "Canon",			"Canon"				},
	{ "Canon",			"Canon"				},
	{ "Levite",			"Levitess"			},
	{ "Levite",			"Levitess"			},
	{ "Curate",			"Curess"			},
	{ "Curate",			"Curess"			},
	{ "Monk",			"Nun"				},
	{ "Monk",			"Nun"				},
	{ "Healer",			"Healess"			},
	{ "Healer",			"Healess"			},
	{ "Chaplain",			"Chaplain"			},
	{ "Chaplain",			"Chaplain"			},
	{ "Expositor",			"Expositress"			},
	{ "Expositor",			"Expositress"			},
	{ "Bishop",			"Bishop"			},
	{ "Bishop",			"Bishop"			},
	{ "Arch Bishop",		"Arch Lady of the Church"	},
	{ "Arch Bishop",		"Arch Lady of the Church"	},
	{ "Patriarch",			"Matriarch"			},
	{ "Patriarch",			"Matriarch"			},
	{ "Elder Patriarch",		"Elder Matriarch"		},
	{ "Elder Patriarch",		"Elder Matriarch"		},
	{ "Grand Patriarch",		"Grand Matriarch"		},
	{ "Grand Patriarch",		"Grand Matriarch"		},
	{ "Great Patriarch",		"Great Matriarch"		},
	{ "Great Patriarch",		"Great Matriarch"		},
	{ "Demon Killer",		"Demon Killer"			},
	{ "Demon Killer",		"Demon Killer"			},
	{ "Greater Demon Killer",	"Greater Demon Killer"		},
	{ "Greater Demon Killer",	"Greater Demon Killer"		},
	{ "Cardinal of the Sea",	"Cardinal of the Sea"		},
	{ "Cardinal of the Sea",	"Cardinal of the Sea"		},
	{ "Cardinal of the Earth",	"Cardinal of the Earth"		},
	{ "Cardinal of the Earth",	"Cardinal of the Earth"		},
	{ "Cardinal of the Air",	"Cardinal of the Air"		},
	{ "Cardinal of the Air",	"Cardinal of the Air"		},
	{ "Cardinal of the Ether",	"Cardinal of the Ether"		},
	{ "Cardinal of the Ether",	"Cardinal of the Ether"		},
	{ "Cardinal of the Heavens",	"Cardinal of the Heavens"	},
	{ "Cardinal of the Heavens",	"Cardinal of the Heavens"	},
	{ "Avatar of an Immortal",	"Avatar of an Immortal"		},
	{ "Avatar of an Immortal",	"Avatar of an Immortal"		},
	{ "Avatar of a Deity",		"Avatar of a Deity"		},
	{ "Avatar of a Deity",		"Avatar of a Deity"		},
	{ "Avatar of a Supremity",	"Avatar of a Supremity"		},
	{ "Avatar of a Supremity",	"Avatar of a Supremity"		},
	{ "Avatar of an Implementor",	"Avatar of an Implementor"	},
	{ "Avatar of an Implementor",	"Avatar of an Implementor"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},	
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Master Cleric",		"Master Cleric"			},
	{ "Holy Hero",			"Holy Heroine"			},
	{ "Holy Squire",		"Holy Squire"			},
	{ "Holy Knight",		"Holy Dame"			},
	{ "Demigod",			"Demigoddess",			},
	{ "Immortal",			"Immortal"			},
	{ "God",			"Goddess"			},
	{ "Deity",			"Deity"				},
	{ "Supreme Master",		"Supreme Mistress"		},
        { "Creator",                    "Creator"                       },
	{ "Supreme Deity",		"Supreme Deity"			}
    },

/*	Thief	*/

    {
	{ "Man",			"Woman"				},

	{ "Pilferer",			"Pilferess"			},
	{ "Pilferer",			"Pilferess"			},
	{ "Footpad",			"Footpad"			},
	{ "Footpad",			"Footpad"			},
	{ "Filcher",			"Filcheress"			},
	{ "Filcher",			"Filcheress"			},
	{ "Pick-Pocket",		"Pick-Pocket"			},
	{ "Pick-Pocket",		"Pick-Pocket"			},
	{ "Sneak",			"Sneak"				},
	{ "Sneak",			"Sneak"				},
	{ "Pincher",			"Pincheress"			},
	{ "Pincher",			"Pincheress"			},
	{ "Cut-Purse",			"Cut-Purse"			},
	{ "Cut-Purse",			"Cut-Purse"			},
	{ "Snatcher",			"Snatcheress"			},
	{ "Snatcher",			"Snatcheress"			},
	{ "Sharper",			"Sharpress"			},
	{ "Sharper",			"Sharpress"			},
	{ "Rogue",			"Rogue"				},
	{ "Rogue",			"Rogue"				},
	{ "Robber",			"Robber"			},
	{ "Robber",			"Robber"			},
	{ "Magsman",			"Magswoman"			},
	{ "Magsman",			"Magswoman"			},
	{ "Highwayman",			"Highwaywoman"			},
	{ "Highwayman",			"Highwaywoman"			},
	{ "Burglar",			"Burglaress"			},
	{ "Burglar",			"Burglaress"			},
	{ "Thief",			"Thief"				},
	{ "Thief",			"Thief"				},
	{ "Knifer",			"Knifer"			},
	{ "Knifer",			"Knifer"			},
	{ "Quick-Blade",		"Quick-Blade"			},
	{ "Quick-Blade",		"Quick-Blade"			},
	{ "Killer",			"Murderess"			},
	{ "Killer",			"Murderess"			},
	{ "Brigand",			"Brigand"			},
	{ "Brigand",			"Brigand"			},
	{ "Cut-Throat",			"Cut-Throat"			},
	{ "Cut-Throat",			"Cut-Throat"			},
	{ "Spy",			"Spy"				},
	{ "Spy",			"Spy"				},
	{ "Grand Spy",			"Grand Spy"			},
	{ "Grand Spy",			"Grand Spy"			},
	{ "Master Spy",			"Master Spy"			},
	{ "Master Spy",			"Master Spy"			},
	{ "Assassin",			"Assassin"			},
	{ "Assassin",			"Assassin"			},
	{ "Greater Assassin",		"Greater Assassin"		},
	{ "Greater Assassin",		"Greater Assassin"		},
	{ "Master of Vision",		"Mistress of Vision"		},
	{ "Master of Vision",		"Mistress of Vision"		},
	{ "Master of Hearing",		"Mistress of Hearing"		},
	{ "Master of Hearing",		"Mistress of Hearing"		},
	{ "Master of Smell",		"Mistress of Smell"		},
	{ "Master of Smell",		"Mistress of Smell"		},
	{ "Master of Taste",		"Mistress of Taste"		},
	{ "Master of Taste",		"Mistress of Taste"		},
	{ "Master of Touch",		"Mistress of Touch"		},
	{ "Master of Touch",		"Mistress of Touch"		},
	{ "Crime Lord",			"Crime Mistress"		},
	{ "Crime Lord",			"Crime Mistress"		},
	{ "Infamous Crime Lord",	"Infamous Crime Mistress"	},
	{ "Infamous Crime Lord",	"Infamous Crime Mistress"	},
	{ "Greater Crime Lord",		"Greater Crime Mistress"	},
	{ "Greater Crime Lord",		"Greater Crime Mistress"	},
	{ "Master Crime Lord",		"Master Crime Mistress"		},
	{ "Master Crime Lord",		"Master Crime Mistress"		},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
        { "Master Thief",               "Master Thief"                  },
	{ "Assassin Hero",		"Assassin Heroine"		},
	{ "Squire of Death",		"Squire of Death",		},
	{ "Knight of Death",		"Dame of Death"			},
	{ "Demigod of Assassins",	"Demigoddess of Assassins"	},
	{ "Immortal Assasin",		"Immortal Assassin"		},
	{ "God of Assassins",		"God of Assassins",		},
	{ "Deity of Assassins",		"Deity of Assassins"		},
	{ "Supreme Master",		"Supreme Mistress"		},
        { "Creator",                    "Creator"                       },
	{ "Supreme Deity",		"Supreme Deity"			}
    },

/*	Warrior	*/

    {
	{ "Man",			"Woman"				},

	{ "Swordpupil",			"Swordpupil"			},
	{ "Swordpupil",			"Swordpupil"			},
	{ "Recruit",			"Recruit"			},
	{ "Recruit",			"Recruit"			},
	{ "Sentry",			"Sentress"			},
	{ "Sentry",			"Sentress"			},
	{ "Fighter",			"Fighter"			},
	{ "Fighter",			"Fighter"			},
	{ "Soldier",			"Soldier"			},
	{ "Soldier",			"Soldier"			},
	{ "Warrior",			"Warrior"			},
	{ "Warrior",			"Warrior"			},
	{ "Veteran",			"Veteran"			},
	{ "Veteran",			"Veteran"			},
	{ "Swordsman",			"Swordswoman"			},
	{ "Swordsman",			"Swordswoman"			},
	{ "Fencer",			"Fenceress"			},
	{ "Fencer",			"Fenceress"			},
	{ "Combatant",			"Combatess"			},
	{ "Combatant",			"Combatess"			},
	{ "Hero",			"Heroine"			},
	{ "Hero",			"Heroine"			},
	{ "Myrmidon",			"Myrmidon"			},
	{ "Myrmidon",			"Myrmidon"			},
	{ "Swashbuckler",		"Swashbuckleress"		},
	{ "Swashbuckler",		"Swashbuckleress"		},
	{ "Mercenary",			"Mercenaress"			},
	{ "Mercenary",			"Mercenaress"			},
	{ "Swordmaster",		"Swordmistress"			},
	{ "Swordmaster",		"Swordmistress"			},
	{ "Lieutenant",			"Lieutenant"			},
	{ "Lieutenant",			"Lieutenant"			},
	{ "Champion",			"Lady Champion"			},
	{ "Champion",			"Lady Champion"			},
	{ "Dragoon",			"Lady Dragoon"			},
	{ "Dragoon",			"Lady Dragoon"			},
	{ "Cavalier",			"Lady Cavalier"			},
	{ "Cavalier",			"Lady Cavalier"			},
	{ "Knight",			"Lady Knight"			},
	{ "Knight",			"Lady Knight"			},
	{ "Grand Knight",		"Grand Knight"			},
	{ "Grand Knight",		"Grand Knight"			},
	{ "Master Knight",		"Master Knight"			},
	{ "Master Knight",		"Master Knight"			},
	{ "Strider",			"Strider"			},
	{ "Strider",			"Strider"			},
	{ "Grand Strider",		"Grand Strider"			},
	{ "Grand Strider",		"Grand Strider"			},
	{ "Demon Slayer",		"Demon Slayer"			},
	{ "Demon Slayer",		"Demon Slayer"			},
	{ "Greater Demon Slayer",	"Greater Demon Slayer"		},
	{ "Greater Demon Slayer",	"Greater Demon Slayer"		},
	{ "Dragon Slayer",		"Dragon Slayer"			},
	{ "Dragon Slayer",		"Dragon Slayer"			},
	{ "Greater Dragon Slayer",	"Greater Dragon Slayer"		},
	{ "Greater Dragon Slayer",	"Greater Dragon Slayer"		},
	{ "Underlord",			"Underlord"			},
	{ "Underlord",			"Underlord"			},
	{ "Overlord",			"Overlord"			},
	{ "Overlord",			"Overlord"			},
	{ "Baron of Thunder",		"Baroness of Thunder"		},
	{ "Baron of Thunder",		"Baroness of Thunder"		},
	{ "Baron of Storms",		"Baroness of Storms"		},
	{ "Baron of Storms",		"Baroness of Storms"		},
	{ "Baron of Tornadoes",		"Baroness of Tornadoes"		},
	{ "Baron of Tornadoes",		"Baroness of Tornadoes"		},
	{ "Baron of Hurricanes",	"Baroness of Hurricanes"	},
	{ "Baron of Hurricanes",	"Baroness of Hurricanes"	},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Master Warrior",		"Master Warrior"		},
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
	{ "Master Warrior",		"Master Warrior"		},
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
        { "Master Warrior",             "Master Warrior"                },
	{ "Knight Hero",		"Knight Heroine"		},
	{ "Squire of War",		"Squire of War"			},
	{ "Knight of War",		"Dame of War"			},
	{ "Demigod of War",		"Demigoddess of War"		},
	{ "Immortal Warlord",		"Immortal Warlord"		},
	{ "God of War",			"God of War"			},
	{ "Deity of War",		"Deity of War"			},
	{ "Supreme Master of War",	"Supreme Mistress of War"	},
        { "Creator",                    "Creator"                       },
	{ "Supreme Deity",		"Supreme Deity"			}
    },

/*	Ranger	*/

    {
	{ "Man",			"Woman"				},

	{ "Apprentice of the Hunt",	"Apprentice of the Hunt"	},
	{ "Student of the Hunt",	"Student of the Hunt"		},
	{ "Student of the Hunt",	"Student of the Hunt"		},
	{ "Runner",			"Runner"			},
	{ "Runner",			"Runner"			},
	{ "Strider",			"Strider"			},
	{ "Strider",			"Strider"			},
	{ "Scout",			"Scout"				},
	{ "Scout",			"Scout"				},
	{ "Master Scout",		"Master Scout"			},
	{ "Master Scout",		"Master Scout"			},
	{ "Explorer",			"Explorer"			},
	{ "Explorer",			"Explorer"			},
	{ "Tracker",			"Tracker"			},
	{ "Tracker",			"Tracker"			},
	{ "Survivalist",		"Survivalist"			},
	{ "Survivalist",		"Survivalist"			},
	{ "Watcher",			"Watcher"			},
	{ "Watcher",			"Watcher"			},
	{ "Hunter",			"Hunter"			},
	{ "Hunter",			"Hunter"			},
	{ "Woodsman",			"Woodswoman"			},
	{ "Woodsman",			"Woodswoman"			},
	{ "Guide",			"Guide"				},
	{ "Guide",			"Guide"				},
	{ "Pathfinder",			"Pathfinder"			},
	{ "Pathfinder",			"Pathfinder"			},
	{ "Stalker",			"Stalker"			},
	{ "Stalker",			"Stalker"			},
	{ "Predator",			"Predator"			},
	{ "Predator",			"Predator"			},
	{ "Deerhunter",			"Deerhunter"			},
	{ "Deerhunter",			"Deerhunter"			},
	{ "Elkhunter",			"Elkhunter"			},
	{ "Elkhunter",			"Elkhunter"			},
	{ "Boarhunter",			"Boarhunter"			},
	{ "Boarhunter",			"Boarhunter"			},
	{ "Bearhunter",			"Bearhunter"			},
	{ "Bearhunter",			"Bearhunter"			},
	{ "Falconer",			"Falconer"			},
	{ "Falconer",			"Falconer"			},
	{ "Hawker",			"Hawker"			},
	{ "Hawker",			"Hawker"			},
	{ "Manhunter",			"Manhunter"			},
	{ "Manhunter",			"Manhunter"			},
	{ "Gianthunter",		"Gianthunter"			},
	{ "Gianthunter",		"Gianthunter"			},
	{ "Adept of the Hunt",		"Adept of the Hunt"		},
	{ "Adept of the Hunt",		"Adept of the Hunt"		},
	{ "Lord of the Hunt",		"Lady of the Hunt"		},
	{ "Lord of the Hunt",		"Lady of the Hunt"		},
	{ "Master of the Hunt",		"Mistress of the Hunt"		},
	{ "Master of the Hunt",		"Mistress of the Hunt"		},
	{ "Fox",			"Fox"				},
	{ "Coyote",			"Coyote"			},
	{ "Lynx",			"Lynx"				},
	{ "Wolf",			"Wolf"				},
	{ "Panther",			"Panther"			},
	{ "Cougar",			"Cougar"			},
	{ "Jaguar",			"Jaguar"			},
	{ "Tiger",			"Tiger"				},
	{ "Lion",			"Lion"				},
	{ "Bear",			"Bear"				},
	{ "Beastmaster",		"Beastmaster"			},
	{ "Beastmaster",		"Beastmaster"			},
	{ "Overlord of Beasts",		"Overlord of Beasts"		},
	{ "Overlord of Beasts",		"Overlord of Beasts"		},
	{ "Dragonhunter",		"Dragonhunter"			},
	{ "Dragonhunter",		"Dragonhunter"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Master Ranger",		"Master Ranger"			},
	{ "Ranger Hero",		"Ranger Heroine"		},
	{ "Squire of the Hunt",		"Squire of the Hunt"		},
	{ "Knight of the Hunt",		"Dame of the Hunt"		},
	{ "Demigod of Beasts",		"Demigoddess of Beasts"		},
	{ "Immortal Hunter",		"Immortal Huntress"		},
	{ "God of Beasts",		"Goddess of Beasts"		},
	{ "Deity of Beasts",		"Deity of Beasts"		},
	{ "Supremity of Beasts",	"Supremity of Beasts"		},
	{ "Creator",			"Creator"			},
	{ "Supreme Deity",		"Supreme Deity"			},
    },

/*	Druid	*/

    {
	{ "Man",			"Woman"				},

	{ "Apprentice of Nature",	"Apprentice of Nature"		},
	{ "Student of Nature",		"Student of Nature"		},
	{ "Student of Nature",		"Student of Nature"		},
	{ "Scholar of Nature",		"Scholar of Nature"		},
	{ "Scholar of Nature",		"Scholar of Nature"		},
	{ "Neophyte",			"Neophyte"			},
	{ "Neophyte",			"Neophyte"			},
	{ "Naturalist",			"Naturalist"			},
	{ "Naturalist",			"Naturalist"			},
	{ "Forester",			"Forestress"			},
	{ "Forester",			"Forestress"			},
	{ "Student of Earth",		"Student of Earth"		},
	{ "Student of Earth",		"Student of Earth"		},
	{ "Student of Air",		"Student of Air"		},
	{ "Student of Air",		"Student of Air"		},
	{ "Student of Water",		"Student of Water"		},
	{ "Student of Water",		"Student of Water"		},
	{ "Student of Fire",		"Student of Fire"		},
	{ "Student of Fire",		"Student of Fire"		},
	{ "Student of Life",		"Student of Life"		},
	{ "Student of Life",		"Student of Life"		},
	{ "Student of The Elements",	"Student of The Elements"	},
	{ "Student of The Elements",	"Student of The Elements"	},
	{ "Herbalist",			"Herbalist"			},
	{ "Herbalist",			"Herbalist"			},
	{ "Philosopher",		"Philosopher"			},
	{ "Philosopher",		"Philosopher"			},
	{ "Sage",			"Sage"				},
	{ "Sage",			"Sage"				},
	{ "Prophet",			"Prophetess"			},
	{ "Prophet",			"Prophetess"			},
	{ "Adept of Earth",		"Adept of Earth"		},
	{ "Adept of Earth",		"Adept of Earth"		},
	{ "Adept of Air",		"Adept of Air"			},
	{ "Adept of Air",		"Adept of Air"			},
	{ "Adept of Water",		"Adept of Water"		},
	{ "Adept of Water",		"Adept of Water"		},
	{ "Adept of Fire",		"Adept of Fire"			},
	{ "Adept of Fire",		"Adept of Fire"			},
	{ "Adept of Life",		"Adept of Life"			},
	{ "Adept of Life",		"Adept of Life"			},
	{ "Adept of The Elements",	"Adept of The Elements"		},
	{ "Adept of The Elements",	"Adept of The Elements"		},
	{ "Druid of Earth",		"Druidess of Earth"		},
	{ "Druid of Earth",		"Druidess of Earth"		},
	{ "Druid of Air",		"Druidess of Air"		},
	{ "Druid of Air",		"Druidess of Air"		},
	{ "Druid of Water",		"Druidess of Water"		},
	{ "Druid of Water",		"Druidess of Water"		},
	{ "Druid of Fire",		"Druidess of Fire"		},
	{ "Druid of Fire",		"Druidess of Fire"		},
	{ "Druid of Life",		"Druidess of Life"		},
	{ "Druid of Life",		"Druidess of Life"		},
	{ "Druid of The Elements",	"Druidess of The Elements"	},
	{ "Druid of The Elements",	"Druidess of The Elements"	},
	{ "Sage of Earth",		"Sage of Earth"			},
	{ "Sage of Earth",		"Sage of Earth"			},
	{ "Sage of Air",		"Sage of Air"			},
	{ "Sage of Air",		"Sage of Air"			},
	{ "Sage of Water",		"Sage of Water"			},
	{ "Sage of Water",		"Sage of Water"			},
	{ "Sage of Fire",		"Sage of Fire"			},
	{ "Sage of Fire",		"Sage of Fire"			},
	{ "Sage of Life",		"Sage of Life"			},
	{ "Sage of Life",		"Sage of Life"			},
	{ "Sage of The Elements",	"Sage of The Elements"		},
	{ "Sage of The Elements",	"Sage of The Elements"		},
	{ "Master of Earth",		"Mistress of Earth"		},
	{ "Master of Earth",		"Mistress of Earth"		},
	{ "Master of Air",		"Mistress of Air"		},
	{ "Master of Air",		"Mistress of Air"		},
	{ "Master of Water",		"Mistress of Water"		},
	{ "Master of Water",		"Mistress of Water"		},
	{ "Master of Fire",		"Mistress of Fire"		},
	{ "Master of Fire",		"Mistress of Fire"		},
	{ "Master of Life",		"Mistress of Life"		},
	{ "Master of Life",		"Mistress of Life"		},
	{ "Master of The Elements",	"Mistress of The Elements"	},
	{ "Master of The Elements",	"Mistress of The Elements"	},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Master Druid",		"Master Druid"			},
	{ "Druid Hero",			"Druid Heroine"			},
	{ "Squire of The Elements",	"Squire of The Elements"	},
	{ "Knight of The Elements",	"Dame of The Elements"		},
	{ "Demigod of Nature",		"Demigoddess of Nature"		},
	{ "Immortal Sage",		"Immortal Sage"			},
	{ "God of The Elements",	"Goddess of The Elements"	},
	{ "Deity of The Elements",	"Deity of The Elements"		},
	{ "Supremity of The Elements",	"Supremity of The Elements"	},
	{ "Creator",			"Creator"			},
	{ "Supreme Diety",		"Supreme Diety"			},
    },

/*	Vampire	*/

    {
	{ "Man",			"Woman"				},

	{ "Blood Student",		"Blood Student"			},
	{ "Scholar of Blood",		"Scholar of Blood"		},
	{ "Scholar of Blood",		"Scholar of Blood"		},
	{ "Student of the Night",	"Student of the Night"		},
	{ "Student of the Night",	"Student of the Night"		},
	{ "Student of Death",		"Student of Death"		},
	{ "Student of Death",		"Student of Death"		},
	{ "Lesser Imp",			"Lesser Imp"			},
	{ "Greater Imp",		"Greater Imp"			},
	{ "Illusionist",		"Illusionist"			},
	{ "Illusionist",		"Illusionist"			},
	{ "Seducer",			"Seductress"			},
	{ "Seducer",			"Seductress"			},
	{ "Stalker",			"Stalker"			},
	{ "Stalker",			"Stalker"			},
	{ "Stalker of Blood",		"Stalker of Blood"		},
	{ "Stalker of Blood",		"Stalker of Blood"		},
	{ "Stalker of Night",		"Stalker of Night"		},
	{ "Stalker of Night",		"Stalker of Night"		},
	{ "Stalker of Death",		"Stalker of Death"		},
	{ "Stalker of Death",		"Stalker of Death"		},
	{ "Shadow",			"Shadow"			},
	{ "Shadow",			"Shadow"			},
	{ "Shadow of Blood",		"Shadow of Blood"		},
	{ "Shadow of Blood",		"Shadow of Blood"		},
	{ "Shadow of Night",		"Shadow of Night"		},
	{ "Shadow of Night",		"Shadow of Night"		},
	{ "Shadow of Death",		"Shadow of Death"		},
	{ "Shadow of Death",		"Shadow of Death"		},
	{ "Killer",			"Killer"			},
	{ "Killer",			"Killer"			},
	{ "Murderer",			"Murderer"			},
	{ "Murderer",			"Murderer"			},
	{ "Incubus",			"Succubus"			},
	{ "Incubus",			"Succubus"			},
	{ "Nightstalker",		"Nightstalker"			},
	{ "Nightstalker",		"Nightstalker"			},
	{ "Revenant",			"Revenant"			},
	{ "Revenant",			"Revenant"			},
	{ "Lesser Vrolok",		"Lesser Vrolok"			},
	{ "Vrolok",			"Vrolok"			},
	{ "Greater Vrolok",		"Greater Vrolok"		},
	{ "Demon Servant",		"Demon Servant"			},
	{ "Demon Servant",		"Demon Servant"			},
	{ "Adept of Blood",		"Adept of Blood"		},
	{ "Adept of Blood",		"Adept of Blood"		},
	{ "Adept of Night",		"Adept of Night"		},
	{ "Adept of Night",		"Adept of Night"		},
	{ "Adept of Death",		"Adept of Death"		},
	{ "Adept of Death",		"Adept of Death"		},
	{ "Demon Seeker",		"Demon Seeker"			},
	{ "Demon Seeker",		"Demon Seeker"			},
	{ "Lord of Blood",		"Lady of Blood"			},
	{ "Lord of Blood",		"Lady of Blood"			},
	{ "Lord of Night",		"Lady of Night"			},
	{ "Lord of Night",		"Lady of Night"			},
	{ "Lord of Death",		"Lady of Death"			},
	{ "Lord of Death",		"Lady of Death"			},
	{ "Demon Knight",		"Demon Dame"			},
	{ "Demon Knight",		"Demon Dame"			},
	{ "Master of Blood",		"Mistress of Blood"		},
	{ "Master of Blood",		"Mistress of Blood"		},
	{ "Master of Night",		"Mistress of Night"		},
	{ "Master of Night",		"Mistress of Night"		},
	{ "Master of Death",		"Mistress of Death"		},
	{ "Master of Death",		"Mistress of Death"		},
	{ "Lord of Demons",		"Lady of Demons"		},
	{ "Lord of Demons",		"Lady of Demons"		},
	{ "Baron of Demons",		"Baroness of Demons"		},
	{ "Baron of Demons",		"Baroness of Demons"		},
	{ "Master of Demons",		"Mistress of Demons"		},
	{ "Master of Demons",		"Mistress of Demons"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Master Vampire",		"Master Vampire"		},
	{ "Vampire Hero",		"Vampire Heroine"		},
	{ "Squire of Blood",		"Squire of Blood"		},
	{ "Knight of Blood",		"Dame of Blood"			},
	{ "Demigod of the Night",	"Demigoddess of the Night"	},
	{ "Immortal Undead",		"Immortal Undead"		},
	{ "God of Death",		"Goddess of Death"		},
	{ "Deity of Death",		"Deity of Death"		},
	{ "Supremity of Death",		"Supremity of Death"		},
	{ "Creator",			"Creator"			},
	{ "Supreme Diety",		"Supreme Deity"			},
    },

/*	Wizard	*/

    {
        { "Man",                        "Woman"                         },
 
        { "Novice of Magic",            "Novice of Magic"               },
        { "Apprentice of Magic",        "Apprentice of Magic"           },
        { "Spell Novice",               "Spell Novice"                  },
        { "Spell Student",              "Spell Student"                 },
        { "Scholar of Magic",           "Scholar of Magic"              },
        { "Scholar of Wizardry",        "Scholar of Wizardry"           },
        { "Delver in Scrolls",          "Delveress in Scrolls"          },
        { "Delver in Spells",           "Delveress in Spells"           },
        { "Medium of Magic",            "Medium of Magic"               },
        { "Medium of Wizardry",         "Medium of Wizardry"            },
        { "Scribe of Magic",            "Scribess of Magic"             },
        { "Scribe of Wizardry",         "Scribess of Wizardry"          },
        { "Minor Seer",                 "Minor Seeress"                 },
        { "Seer",                       "Seeress"                       },
        { "Minor Sage",                 "Minor Sage"                    },
        { "Sage",                       "Sage"                          },
        { "Minor Illusionist",          "Minor Illusionist"             },
        { "Illusionist",                "Illusionist"                   },
        { "Minor Abjurer",              "Minor Abjuress"                },
        { "Abjurer",                    "Abjuress"                      },
        { "Minor Invoker",              "Minor Invoker"                 },
        { "Invoker",                    "Invoker"                       },
        { "Minor Enchanter",            "Minor Enchantress"             },
        { "Enchanter",                  "Enchantress"                   },
        { "Minor Conjurer",             "Minor Conjuress"               },
        { "Conjurer",                   "Conjuress"                     },
        { "Minor Magician",             "Minor Witch"                   },
        { "Magician",                   "Witch"                         },
        { "Minor Creator",              "Minor Creator"                 },
        { "Creator",                    "Creator"                       },
        { "Minor Savant",               "Minor Savant"                  },
        { "Savant",                     "Savant"                        },
        { "Minor Magus",                "Minor Craftess"                },
        { "Magus",                      "Craftess"                      },
        { "Minor Wizard",               "Minor Wizard"                  },
        { "Wizard",                     "Wizard"                        },
        { "Minor Warlock",              "Minor War Witch"               },
        { "Warlock",                    "War Witch"                     },
        { "Minor Sorcerer",             "Minor Sorceress"               },
        { "Sorcerer",                   "Sorceress"                     },
        { "Elder Sorcerer",             "Elder Sorceress"               },
        { "Elder Sorcerer",             "Elder Sorceress"               },
        { "Grand Sorcerer",             "Grand Sorceress"               },
        { "Grand Sorcerer",             "Grand Sorceress"               },
        { "Great Sorcerer",             "Great Sorceress"               },
        { "Great Sorcerer",             "Great Sorceress"               },
        { "Golem Maker",                "Golem Maker"                   },
        { "Golem Maker",                "Golem Maker"                   },
        { "Greater Golem Maker",        "Greater Golem Maker"           },
        { "Greater Golem Maker",        "Greater Golem Maker"           },
        { "Maker of Stones",            "Maker of Stones",              },
        { "Maker of Stones",            "Maker of Stones",              },
        { "Maker of Potions",           "Maker of Potions",             },
        { "Maker of Potions",           "Maker of Potions",             },
        { "Maker of Scrolls",           "Maker of Scrolls",             },
        { "Maker of Scrolls",           "Maker of Scrolls",             },
        { "Maker of Wands",             "Maker of Wands",               },
        { "Maker of Wands",             "Maker of Wands",               },
        { "Maker of Staves",            "Maker of Staves",              },
        { "Maker of Staves",            "Maker of Staves",              },
        { "Demon Summoner",             "Demon Summoner"                },
        { "Demon Summoner",             "Demon Summoner"                },
        { "Greater Demon Summoner",     "Greater Demon Summoner"        },
        { "Greater Demon Summoner",     "Greater Demon Summoner"        },
        { "Dragon Charmer",             "Dragon Charmer"                },
        { "Dragon Charmer",             "Dragon Charmer"                },
        { "Greater Dragon Charmer",     "Greater Dragon Charmer"        },
        { "Greater Dragon Charmer",     "Greater Dragon Charmer"        },
        { "Master of all Magic",        "Master of all Magic"           },
        { "Master of all Magic",        "Master of all Magic"           },
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Master Wizard",		"Master Wizard"			},
        { "Wizard Hero",		"Wizard Heroine"		},
        { "Squire of Magic",            "Squire of Magic"               },
        { "Knight of Magic",            "Dame of Magic"			},
        { "Demigod of Magic",           "Demigoddess of Magic"          },
        { "Immortal of Magic",          "Immortal of Magic"             },
        { "God of Magic",               "Goddess of Magic"              },
        { "Deity of Magic",             "Deity of Magic"                },
        { "Supremity of Magic",         "Supremity of Magic"            },
        { "Creator",                    "Creator"                       },
        { "Supreme Deity",              "Supreme Deity"                 }
    },

/*	Priest	*/

    {
	{ "Man",			"Woman"				},

	{ "Believer",			"Believer"			},
	{ "Believer",			"Believer"			},
	{ "Attendant",			"Attendant"			},
	{ "Attendant",			"Attendant"			},
	{ "Acolyte",			"Acolyte"			},
	{ "Acolyte",			"Acolyte"			},
	{ "Novice",			"Novice"			},
	{ "Novice",			"Novice"			},
	{ "Missionary",			"Missionary"			},
	{ "Missionary",			"Missionary"			},
	{ "Adept",			"Adept"				},
	{ "Adept",			"Adept"				},
	{ "Deacon",			"Deaconess"			},
	{ "Deacon",			"Deaconess"			},
	{ "Vicar",			"Vicaress"			},
	{ "Vicar",			"Vicaress"			},
	{ "Priest",			"Priestess"			},
	{ "Priest",			"Priestess"			},
	{ "Minister",			"Lady Minister"			},
	{ "Minister",			"Lady Minister"			},
	{ "Canon",			"Canon"				},
	{ "Canon",			"Canon"				},
	{ "Levite",			"Levitess"			},
	{ "Levite",			"Levitess"			},
	{ "Curate",			"Curess"			},
	{ "Curate",			"Curess"			},
	{ "Monk",			"Nun"				},
	{ "Monk",			"Nun"				},
	{ "Healer",			"Healess"			},
	{ "Healer",			"Healess"			},
	{ "Chaplain",			"Chaplain"			},
	{ "Chaplain",			"Chaplain"			},
	{ "Expositor",			"Expositress"			},
	{ "Expositor",			"Expositress"			},
	{ "Bishop",			"Bishop"			},
	{ "Bishop",			"Bishop"			},
	{ "Arch Bishop",		"Arch Lady of the Church"	},
	{ "Arch Bishop",		"Arch Lady of the Church"	},
	{ "Patriarch",			"Matriarch"			},
	{ "Patriarch",			"Matriarch"			},
	{ "Elder Patriarch",		"Elder Matriarch"		},
	{ "Elder Patriarch",		"Elder Matriarch"		},
	{ "Grand Patriarch",		"Grand Matriarch"		},
	{ "Grand Patriarch",		"Grand Matriarch"		},
	{ "Great Patriarch",		"Great Matriarch"		},
	{ "Great Patriarch",		"Great Matriarch"		},
	{ "Demon Killer",		"Demon Killer"			},
	{ "Demon Killer",		"Demon Killer"			},
	{ "Greater Demon Killer",	"Greater Demon Killer"		},
	{ "Greater Demon Killer",	"Greater Demon Killer"		},
	{ "Cardinal of the Sea",	"Cardinal of the Sea"		},
	{ "Cardinal of the Sea",	"Cardinal of the Sea"		},
	{ "Cardinal of the Earth",	"Cardinal of the Earth"		},
	{ "Cardinal of the Earth",	"Cardinal of the Earth"		},
	{ "Cardinal of the Air",	"Cardinal of the Air"		},
	{ "Cardinal of the Air",	"Cardinal of the Air"		},
	{ "Cardinal of the Ether",	"Cardinal of the Ether"		},
	{ "Cardinal of the Ether",	"Cardinal of the Ether"		},
	{ "Cardinal of the Heavens",	"Cardinal of the Heavens"	},
	{ "Cardinal of the Heavens",	"Cardinal of the Heavens"	},
	{ "Avatar of an Immortal",	"Avatar of an Immortal"		},
	{ "Avatar of an Immortal",	"Avatar of an Immortal"		},
	{ "Avatar of a Deity",		"Avatar of a Deity"		},
	{ "Avatar of a Deity",		"Avatar of a Deity"		},
	{ "Avatar of a Supremity",	"Avatar of a Supremity"		},
	{ "Avatar of a Supremity",	"Avatar of a Supremity"		},
	{ "Avatar of an Implementor",	"Avatar of an Implementor"	},
	{ "Avatar of an Implementor",	"Avatar of an Implementor"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master of all Divinity",	"Mistress of all Divinity"	},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Master Priest",		"Master Priestess"		},
	{ "Divine Hero",		"Divine Heroine"		},
	{ "Divine Squire",		"Divine Squire"			},
	{ "Divine Knight",		"Divine Dame"			},
	{ "Demigod",			"Demigoddess",			},
	{ "Immortal",			"Immortal"			},
	{ "God",			"Goddess"			},
	{ "Deity",			"Deity"				},
	{ "Supreme Master",		"Supreme Mistress"		},
        { "Creator",                    "Creator"                       },
	{ "Supreme Deity",		"Supreme Deity"			}
    },

/*	Mercenary	*/

    {
	{ "Man",			"Woman"				},

	{ "Pilferer",			"Pilferess"			},
	{ "Pilferer",			"Pilferess"			},
	{ "Footpad",			"Footpad"			},
	{ "Footpad",			"Footpad"			},
	{ "Filcher",			"Filcheress"			},
	{ "Filcher",			"Filcheress"			},
	{ "Pick-Pocket",		"Pick-Pocket"			},
	{ "Pick-Pocket",		"Pick-Pocket"			},
	{ "Sneak",			"Sneak"				},
	{ "Sneak",			"Sneak"				},
	{ "Pincher",			"Pincheress"			},
	{ "Pincher",			"Pincheress"			},
	{ "Cut-Purse",			"Cut-Purse"			},
	{ "Cut-Purse",			"Cut-Purse"			},
	{ "Snatcher",			"Snatcheress"			},
	{ "Snatcher",			"Snatcheress"			},
	{ "Sharper",			"Sharpress"			},
	{ "Sharper",			"Sharpress"			},
	{ "Rogue",			"Rogue"				},
	{ "Rogue",			"Rogue"				},
	{ "Robber",			"Robber"			},
	{ "Robber",			"Robber"			},
	{ "Magsman",			"Magswoman"			},
	{ "Magsman",			"Magswoman"			},
	{ "Highwayman",			"Highwaywoman"			},
	{ "Highwayman",			"Highwaywoman"			},
	{ "Burglar",			"Burglaress"			},
	{ "Burglar",			"Burglaress"			},
	{ "Thief",			"Thief"				},
	{ "Thief",			"Thief"				},
	{ "Knifer",			"Knifer"			},
	{ "Knifer",			"Knifer"			},
	{ "Quick-Blade",		"Quick-Blade"			},
	{ "Quick-Blade",		"Quick-Blade"			},
	{ "Killer",			"Murderess"			},
	{ "Killer",			"Murderess"			},
	{ "Brigand",			"Brigand"			},
	{ "Brigand",			"Brigand"			},
	{ "Cut-Throat",			"Cut-Throat"			},
	{ "Cut-Throat",			"Cut-Throat"			},
	{ "Spy",			"Spy"				},
	{ "Spy",			"Spy"				},
	{ "Grand Spy",			"Grand Spy"			},
	{ "Grand Spy",			"Grand Spy"			},
	{ "Master Spy",			"Master Spy"			},
	{ "Master Spy",			"Master Spy"			},
	{ "Assassin",			"Assassin"			},
	{ "Assassin",			"Assassin"			},
	{ "Greater Assassin",		"Greater Assassin"		},
	{ "Greater Assassin",		"Greater Assassin"		},
	{ "Master of Vision",		"Mistress of Vision"		},
	{ "Master of Vision",		"Mistress of Vision"		},
	{ "Master of Hearing",		"Mistress of Hearing"		},
	{ "Master of Hearing",		"Mistress of Hearing"		},
	{ "Master of Smell",		"Mistress of Smell"		},
	{ "Master of Smell",		"Mistress of Smell"		},
	{ "Master of Taste",		"Mistress of Taste"		},
	{ "Master of Taste",		"Mistress of Taste"		},
	{ "Master of Touch",		"Mistress of Touch"		},
	{ "Master of Touch",		"Mistress of Touch"		},
	{ "Crime Lord",			"Crime Mistress"		},
	{ "Crime Lord",			"Crime Mistress"		},
	{ "Infamous Crime Lord",	"Infamous Crime Mistress"	},
	{ "Infamous Crime Lord",	"Infamous Crime Mistress"	},
	{ "Greater Crime Lord",		"Greater Crime Mistress"	},
	{ "Greater Crime Lord",		"Greater Crime Mistress"	},
	{ "Master Crime Lord",		"Master Crime Mistress"		},
	{ "Master Crime Lord",		"Master Crime Mistress"		},
	{ "Godfather",			"Godmother"			},
	{ "Godfather",			"Godmother"			},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
        { "Master Mercenary",		"Master Mercenary"		},
	{ "Assassin Hero",		"Assassin Heroine"		},
	{ "Squire of Death",		"Squire of Death",		},
	{ "Knight of Death",		"Dame of Death"			},
	{ "Demigod of Assassins",	"Demigoddess of Assassins"	},
	{ "Immortal Assasin",		"Immortal Assassin"		},
	{ "God of Assassins",		"God of Assassins",		},
	{ "Deity of Assassins",		"Deity of Assassins"		},
	{ "Supreme Master",		"Supreme Mistress"		},
        { "Creator",                    "Creator"                       },
	{ "Supreme Deity",		"Supreme Deity"			}
    },

/*	Gladiator	*/

    {
	{ "Man",			"Woman"				},

	{ "Swordpupil",			"Swordpupil"			},
	{ "Swordpupil",			"Swordpupil"			},
	{ "Recruit",			"Recruit"			},
	{ "Recruit",			"Recruit"			},
	{ "Sentry",			"Sentress"			},
	{ "Sentry",			"Sentress"			},
	{ "Fighter",			"Fighter"			},
	{ "Fighter",			"Fighter"			},
	{ "Soldier",			"Soldier"			},
	{ "Soldier",			"Soldier"			},
	{ "Warrior",			"Warrior"			},
	{ "Warrior",			"Warrior"			},
	{ "Veteran",			"Veteran"			},
	{ "Veteran",			"Veteran"			},
	{ "Swordsman",			"Swordswoman"			},
	{ "Swordsman",			"Swordswoman"			},
	{ "Fencer",			"Fenceress"			},
	{ "Fencer",			"Fenceress"			},
	{ "Combatant",			"Combatess"			},
	{ "Combatant",			"Combatess"			},
	{ "Hero",			"Heroine"			},
	{ "Hero",			"Heroine"			},
	{ "Myrmidon",			"Myrmidon"			},
	{ "Myrmidon",			"Myrmidon"			},
	{ "Swashbuckler",		"Swashbuckleress"		},
	{ "Swashbuckler",		"Swashbuckleress"		},
	{ "Mercenary",			"Mercenaress"			},
	{ "Mercenary",			"Mercenaress"			},
	{ "Swordmaster",		"Swordmistress"			},
	{ "Swordmaster",		"Swordmistress"			},
	{ "Lieutenant",			"Lieutenant"			},
	{ "Lieutenant",			"Lieutenant"			},
	{ "Champion",			"Lady Champion"			},
	{ "Champion",			"Lady Champion"			},
	{ "Dragoon",			"Lady Dragoon"			},
	{ "Dragoon",			"Lady Dragoon"			},
	{ "Cavalier",			"Lady Cavalier"			},
	{ "Cavalier",			"Lady Cavalier"			},
	{ "Knight",			"Lady Knight"			},
	{ "Knight",			"Lady Knight"			},
	{ "Grand Knight",		"Grand Knight"			},
	{ "Grand Knight",		"Grand Knight"			},
	{ "Master Knight",		"Master Knight"			},
	{ "Master Knight",		"Master Knight"			},
	{ "Strider",			"Strider"			},
	{ "Strider",			"Strider"			},
	{ "Grand Strider",		"Grand Strider"			},
	{ "Grand Strider",		"Grand Strider"			},
	{ "Demon Slayer",		"Demon Slayer"			},
	{ "Demon Slayer",		"Demon Slayer"			},
	{ "Greater Demon Slayer",	"Greater Demon Slayer"		},
	{ "Greater Demon Slayer",	"Greater Demon Slayer"		},
	{ "Dragon Slayer",		"Dragon Slayer"			},
	{ "Dragon Slayer",		"Dragon Slayer"			},
	{ "Greater Dragon Slayer",	"Greater Dragon Slayer"		},
	{ "Greater Dragon Slayer",	"Greater Dragon Slayer"		},
	{ "Underlord",			"Underlord"			},
	{ "Underlord",			"Underlord"			},
	{ "Overlord",			"Overlord"			},
	{ "Overlord",			"Overlord"			},
	{ "Baron of Thunder",		"Baroness of Thunder"		},
	{ "Baron of Thunder",		"Baroness of Thunder"		},
	{ "Baron of Storms",		"Baroness of Storms"		},
	{ "Baron of Storms",		"Baroness of Storms"		},
	{ "Baron of Tornadoes",		"Baroness of Tornadoes"		},
	{ "Baron of Tornadoes",		"Baroness of Tornadoes"		},
	{ "Baron of Hurricanes",	"Baroness of Hurricanes"	},
	{ "Baron of Hurricanes",	"Baroness of Hurricanes"	},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Baron of Meteors",		"Baroness of Meteors"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Centurian Hero",		"Centurian Heroine"		},
	{ "Squire of War",		"Squire of War"			},
	{ "Knight of War",		"Dame of War"			},
	{ "Demigod of War",		"Demigoddess of War"		},
	{ "Immortal Warlord",		"Immortal Warlord"		},
	{ "God of War",			"God of War"			},
	{ "Deity of War",		"Deity of War"			},
	{ "Supreme Master of War",	"Supreme Mistress of War"	},
        { "Creator",                    "Creator"                       },
	{ "Supreme Deity",		"Supreme Deity"			}
    },

/*	Strider	*/

    {
	{ "Man",			"Woman"				},

	{ "Apprentice of the Hunt",	"Apprentice of the Hunt"	},
	{ "Student of the Hunt",	"Student of the Hunt"		},
	{ "Student of the Hunt",	"Student of the Hunt"		},
	{ "Runner",			"Runner"			},
	{ "Runner",			"Runner"			},
	{ "Strider",			"Strider"			},
	{ "Strider",			"Strider"			},
	{ "Scout",			"Scout"				},
	{ "Scout",			"Scout"				},
	{ "Master Scout",		"Master Scout"			},
	{ "Master Scout",		"Master Scout"			},
	{ "Explorer",			"Explorer"			},
	{ "Explorer",			"Explorer"			},
	{ "Tracker",			"Tracker"			},
	{ "Tracker",			"Tracker"			},
	{ "Survivalist",		"Survivalist"			},
	{ "Survivalist",		"Survivalist"			},
	{ "Watcher",			"Watcher"			},
	{ "Watcher",			"Watcher"			},
	{ "Hunter",			"Hunter"			},
	{ "Hunter",			"Hunter"			},
	{ "Woodsman",			"Woodswoman"			},
	{ "Woodsman",			"Woodswoman"			},
	{ "Guide",			"Guide"				},
	{ "Guide",			"Guide"				},
	{ "Pathfinder",			"Pathfinder"			},
	{ "Pathfinder",			"Pathfinder"			},
	{ "Stalker",			"Stalker"			},
	{ "Stalker",			"Stalker"			},
	{ "Predator",			"Predator"			},
	{ "Predator",			"Predator"			},
	{ "Deerhunter",			"Deerhunter"			},
	{ "Deerhunter",			"Deerhunter"			},
	{ "Elkhunter",			"Elkhunter"			},
	{ "Elkhunter",			"Elkhunter"			},
	{ "Boarhunter",			"Boarhunter"			},
	{ "Boarhunter",			"Boarhunter"			},
	{ "Bearhunter",			"Bearhunter"			},
	{ "Bearhunter",			"Bearhunter"			},
	{ "Falconer",			"Falconer"			},
	{ "Falconer",			"Falconer"			},
	{ "Hawker",			"Hawker"			},
	{ "Hawker",			"Hawker"			},
	{ "Manhunter",			"Manhunter"			},
	{ "Manhunter",			"Manhunter"			},
	{ "Gianthunter",		"Gianthunter"			},
	{ "Gianthunter",		"Gianthunter"			},
	{ "Adept of the Hunt",		"Adept of the Hunt"		},
	{ "Adept of the Hunt",		"Adept of the Hunt"		},
	{ "Lord of the Hunt",		"Lady of the Hunt"		},
	{ "Lord of the Hunt",		"Lady of the Hunt"		},
	{ "Master of the Hunt",		"Mistress of the Hunt"		},
	{ "Master of the Hunt",		"Mistress of the Hunt"		},
	{ "Fox",			"Fox"				},
	{ "Coyote",			"Coyote"			},
	{ "Lynx",			"Lynx"				},
	{ "Wolf",			"Wolf"				},
	{ "Panther",			"Panther"			},
	{ "Cougar",			"Cougar"			},
	{ "Jaguar",			"Jaguar"			},
	{ "Tiger",			"Tiger"				},
	{ "Lion",			"Lion"				},
	{ "Bear",			"Bear"				},
	{ "Beastmaster",		"Beastmaster"			},
	{ "Beastmaster",		"Beastmaster"			},
	{ "Overlord of Beasts",		"Overlord of Beasts"		},
	{ "Overlord of Beasts",		"Overlord of Beasts"		},
	{ "Dragonhunter",		"Dragonhunter"			},
	{ "Dragonhunter",		"Dragonhunter"			},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Master Strider",		"Master Strider"		},
	{ "Strider Hero",		"Strider Heroine"		},
	{ "Squire of the Hunt",		"Squire of the Hunt"		},
	{ "Knight of the Hunt",		"Dame of the Hunt"		},
	{ "Demigod of Beasts",		"Demigoddess of Beasts"		},
	{ "Immortal Hunter",		"Immortal Huntress"		},
	{ "God of Beasts",		"Goddess of Beasts"		},
	{ "Deity of Beasts",		"Deity of Beasts"		},
	{ "Supremity of Beasts",	"Supremity of Beasts"		},
	{ "Creator",			"Creator"			},
	{ "Supreme Deity",		"Supreme Deity"			},
    },

/*	Sage	*/

    {
	{ "Man",			"Woman"				},

	{ "Apprentice of Nature",	"Apprentice of Nature"		},
	{ "Student of Nature",		"Student of Nature"		},
	{ "Student of Nature",		"Student of Nature"		},
	{ "Scholar of Nature",		"Scholar of Nature"		},
	{ "Scholar of Nature",		"Scholar of Nature"		},
	{ "Neophyte",			"Neophyte"			},
	{ "Neophyte",			"Neophyte"			},
	{ "Naturalist",			"Naturalist"			},
	{ "Naturalist",			"Naturalist"			},
	{ "Forester",			"Forestress"			},
	{ "Forester",			"Forestress"			},
	{ "Student of Earth",		"Student of Earth"		},
	{ "Student of Earth",		"Student of Earth"		},
	{ "Student of Air",		"Student of Air"		},
	{ "Student of Air",		"Student of Air"		},
	{ "Student of Water",		"Student of Water"		},
	{ "Student of Water",		"Student of Water"		},
	{ "Student of Fire",		"Student of Fire"		},
	{ "Student of Fire",		"Student of Fire"		},
	{ "Student of Life",		"Student of Life"		},
	{ "Student of Life",		"Student of Life"		},
	{ "Student of The Elements",	"Student of The Elements"	},
	{ "Student of The Elements",	"Student of The Elements"	},
	{ "Herbalist",			"Herbalist"			},
	{ "Herbalist",			"Herbalist"			},
	{ "Philosopher",		"Philosopher"			},
	{ "Philosopher",		"Philosopher"			},
	{ "Sage",			"Sage"				},
	{ "Sage",			"Sage"				},
	{ "Prophet",			"Prophetess"			},
	{ "Prophet",			"Prophetess"			},
	{ "Adept of Earth",		"Adept of Earth"		},
	{ "Adept of Earth",		"Adept of Earth"		},
	{ "Adept of Air",		"Adept of Air"			},
	{ "Adept of Air",		"Adept of Air"			},
	{ "Adept of Water",		"Adept of Water"		},
	{ "Adept of Water",		"Adept of Water"		},
	{ "Adept of Fire",		"Adept of Fire"			},
	{ "Adept of Fire",		"Adept of Fire"			},
	{ "Adept of Life",		"Adept of Life"			},
	{ "Adept of Life",		"Adept of Life"			},
	{ "Adept of The Elements",	"Adept of The Elements"		},
	{ "Adept of The Elements",	"Adept of The Elements"		},
	{ "Druid of Earth",		"Druidess of Earth"		},
	{ "Druid of Earth",		"Druidess of Earth"		},
	{ "Druid of Air",		"Druidess of Air"		},
	{ "Druid of Air",		"Druidess of Air"		},
	{ "Druid of Water",		"Druidess of Water"		},
	{ "Druid of Water",		"Druidess of Water"		},
	{ "Druid of Fire",		"Druidess of Fire"		},
	{ "Druid of Fire",		"Druidess of Fire"		},
	{ "Druid of Life",		"Druidess of Life"		},
	{ "Druid of Life",		"Druidess of Life"		},
	{ "Druid of The Elements",	"Druidess of The Elements"	},
	{ "Druid of The Elements",	"Druidess of The Elements"	},
	{ "Sage of Earth",		"Sage of Earth"			},
	{ "Sage of Earth",		"Sage of Earth"			},
	{ "Sage of Air",		"Sage of Air"			},
	{ "Sage of Air",		"Sage of Air"			},
	{ "Sage of Water",		"Sage of Water"			},
	{ "Sage of Water",		"Sage of Water"			},
	{ "Sage of Fire",		"Sage of Fire"			},
	{ "Sage of Fire",		"Sage of Fire"			},
	{ "Sage of Life",		"Sage of Life"			},
	{ "Sage of Life",		"Sage of Life"			},
	{ "Sage of The Elements",	"Sage of The Elements"		},
	{ "Sage of The Elements",	"Sage of The Elements"		},
	{ "Master of Earth",		"Mistress of Earth"		},
	{ "Master of Earth",		"Mistress of Earth"		},
	{ "Master of Air",		"Mistress of Air"		},
	{ "Master of Air",		"Mistress of Air"		},
	{ "Master of Water",		"Mistress of Water"		},
	{ "Master of Water",		"Mistress of Water"		},
	{ "Master of Fire",		"Mistress of Fire"		},
	{ "Master of Fire",		"Mistress of Fire"		},
	{ "Master of Life",		"Mistress of Life"		},
	{ "Master of Life",		"Mistress of Life"		},
	{ "Master of The Elements",	"Mistress of The Elements"	},
	{ "Master of The Elements",	"Mistress of The Elements"	},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Master Sage",		"Master Sage"			},
	{ "Sage Hero",			"Sage Heroine"			},
	{ "Squire of The Elements",	"Squire of The Elements"	},
	{ "Knight of The Elements",	"Dame of The Elements"		},
	{ "Demigod of Nature",		"Demigoddess of Nature"		},
	{ "Immortal Sage",		"Immortal Sage"			},
	{ "God of The Elements",	"Goddess of The Elements"	},
	{ "Deity of The Elements",	"Deity of The Elements"		},
	{ "Supremity of The Elements",	"Supremity of The Elements"	},
	{ "Creator",			"Creator"			},
	{ "Supreme Diety",		"Supreme Diety"			},
    },

/*	Lich	*/
    {
	{ "Man",			"Woman"				},

	{ "Blood Student",		"Blood Student"			},
	{ "Scholar of Blood",		"Scholar of Blood"		},
	{ "Scholar of Blood",		"Scholar of Blood"		},
	{ "Student of the Night",	"Student of the Night"		},
	{ "Student of the Night",	"Student of the Night"		},
	{ "Student of Death",		"Student of Death"		},
	{ "Student of Death",		"Student of Death"		},
	{ "Lesser Imp",			"Lesser Imp"			},
	{ "Greater Imp",		"Greater Imp"			},
	{ "Illusionist",		"Illusionist"			},
	{ "Illusionist",		"Illusionist"			},
	{ "Seducer",			"Seductress"			},
	{ "Seducer",			"Seductress"			},
	{ "Stalker",			"Stalker"			},
	{ "Stalker",			"Stalker"			},
	{ "Stalker of Blood",		"Stalker of Blood"		},
	{ "Stalker of Blood",		"Stalker of Blood"		},
	{ "Stalker of Night",		"Stalker of Night"		},
	{ "Stalker of Night",		"Stalker of Night"		},
	{ "Stalker of Death",		"Stalker of Death"		},
	{ "Stalker of Death",		"Stalker of Death"		},
	{ "Shadow",			"Shadow"			},
	{ "Shadow",			"Shadow"			},
	{ "Shadow of Blood",		"Shadow of Blood"		},
	{ "Shadow of Blood",		"Shadow of Blood"		},
	{ "Shadow of Night",		"Shadow of Night"		},
	{ "Shadow of Night",		"Shadow of Night"		},
	{ "Shadow of Death",		"Shadow of Death"		},
	{ "Shadow of Death",		"Shadow of Death"		},
	{ "Killer",			"Killer"			},
	{ "Killer",			"Killer"			},
	{ "Murderer",			"Murderer"			},
	{ "Murderer",			"Murderer"			},
	{ "Incubus",			"Succubus"			},
	{ "Incubus",			"Succubus"			},
	{ "Nightstalker",		"Nightstalker"			},
	{ "Nightstalker",		"Nightstalker"			},
	{ "Revenant",			"Revenant"			},
	{ "Revenant",			"Revenant"			},
	{ "Lesser Vrolok",		"Lesser Vrolok"			},
	{ "Vrolok",			"Vrolok"			},
	{ "Greater Vrolok",		"Greater Vrolok"		},
	{ "Demon Servant",		"Demon Servant"			},
	{ "Demon Servant",		"Demon Servant"			},
	{ "Adept of Blood",		"Adept of Blood"		},
	{ "Adept of Blood",		"Adept of Blood"		},
	{ "Adept of Night",		"Adept of Night"		},
	{ "Adept of Night",		"Adept of Night"		},
	{ "Adept of Death",		"Adept of Death"		},
	{ "Adept of Death",		"Adept of Death"		},
	{ "Demon Seeker",		"Demon Seeker"			},
	{ "Demon Seeker",		"Demon Seeker"			},
	{ "Lord of Blood",		"Lady of Blood"			},
	{ "Lord of Blood",		"Lady of Blood"			},
	{ "Lord of Night",		"Lady of Night"			},
	{ "Lord of Night",		"Lady of Night"			},
	{ "Lord of Death",		"Lady of Death"			},
	{ "Lord of Death",		"Lady of Death"			},
	{ "Demon Knight",		"Demon Dame"			},
	{ "Demon Knight",		"Demon Dame"			},
	{ "Master of Blood",		"Mistress of Blood"		},
	{ "Master of Blood",		"Mistress of Blood"		},
	{ "Master of Night",		"Mistress of Night"		},
	{ "Master of Night",		"Mistress of Night"		},
	{ "Master of Death",		"Mistress of Death"		},
	{ "Master of Death",		"Mistress of Death"		},
	{ "Lord of Demons",		"Lady of Demons"		},
	{ "Lord of Demons",		"Lady of Demons"		},
	{ "Baron of Demons",		"Baroness of Demons"		},
	{ "Baron of Demons",		"Baroness of Demons"		},
	{ "Master of Demons",		"Mistress of Demons"		},
	{ "Master of Demons",		"Mistress of Demons"		},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Master Lich",		"Master Lich"			},
	{ "Lich Hero",			"Lich Heroine"			},
	{ "Squire of Blood",		"Squire of Blood"		},
	{ "Knight of Blood",		"Dame of Blood"			},
	{ "Demigod of the Night",	"Demigoddess of the Night"	},
	{ "Immortal Undead",		"Immortal Undead"		},
	{ "God of Death",		"Goddess of Death"		},
	{ "Deity of Death",		"Deity of Death"		},
	{ "Supremity of Death",		"Supremity of Death"		},
	{ "Creator",			"Creator"			},
	{ "Supreme Diety",		"Supreme Deity"			},
    }
};



/*
 * Attribute bonus tables.
 */
const	struct	str_app_type	str_app		[26]		=
{
    { -5, -4,   0,  0 },  /* 0  */
    { -5, -4,   3,  1 },  /* 1  */
    { -3, -2,   3,  2 },
    { -3, -1,  10,  3 },  /* 3  */
    { -2, -1,  25,  4 },
    { -2, -1,  55,  5 },  /* 5  */
    { -1,  0,  80,  6 },
    { -1,  0,  90,  7 },
    {  0,  0, 100,  8 },
    {  0,  0, 100,  9 },
    {  0,  0, 115, 10 }, /* 10  */
    {  0,  0, 115, 11 },
    {  0,  0, 130, 12 },
    {  0,  0, 130, 13 }, /* 13  */
    {  0,  1, 140, 14 },
    {  1,  1, 150, 15 }, /* 15  */
    {  1,  2, 165, 16 },
    {  2,  3, 180, 22 },
    {  2,  3, 200, 25 }, /* 18  */
    {  3,  4, 225, 30 },
    {  3,  5, 250, 35 }, /* 20  */
    {  4,  6, 300, 40 },
    {  4,  6, 350, 45 },
    {  5,  7, 400, 50 },
    {  5,  8, 450, 55 },
    {  6,  9, 500, 60 }  /* 25   */
};



const	struct	int_app_type	int_app		[26]		=
{
    {  3 },	/*  0 */
    {  5 },	/*  1 */
    {  7 },
    {  8 },	/*  3 */
    {  9 },
    { 10 },	/*  5 */
    { 11 },
    { 12 },
    { 13 },
    { 15 },
    { 17 },	/* 10 */
    { 19 },
    { 22 },
    { 25 },
    { 28 },
    { 31 },	/* 15 */
    { 34 },
    { 37 },
    { 40 },	/* 18 */
    { 44 },
    { 49 },	/* 20 */
    { 55 },
    { 60 },
    { 70 },
    { 80 },
    { 85 }	/* 25 */
};



const	struct	wis_app_type	wis_app		[26]		=
{
    { 0 },	/*  0 */
    { 0 },	/*  1 */
    { 0 },
    { 0 },	/*  3 */
    { 0 },
    { 1 },	/*  5 */
    { 1 },
    { 1 },
    { 1 },
    { 1 },
    { 1 },	/* 10 */
    { 1 },
    { 1 },
    { 1 },
    { 1 },
    { 2 },	/* 15 */
    { 2 },
    { 2 },
    { 3 },	/* 18 */
    { 3 },
    { 3 },	/* 20 */
    { 3 },
    { 4 },
    { 4 },
    { 4 },
    { 5 }	/* 25 */
};



const	struct	dex_app_type	dex_app		[26]		=
{
    {   60 },   /* 0 */
    {   50 },   /* 1 */
    {   50 },
    {   40 },
    {   30 },
    {   20 },   /* 5 */
    {   10 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 15 },
    { - 20 },
    { - 30 },
    { - 40 },
    { - 50 },   /* 20 */
    { - 60 },
    { - 75 },
    { - 90 },
    { -105 },
    { -120 }    /* 25 */
};


const	struct	con_app_type	con_app		[26]		=
{
    { -4, 20 },   /*  0 */
    { -3, 25 },   /*  1 */
    { -2, 30 },
    { -2, 35 },	  /*  3 */
    { -1, 40 },
    { -1, 45 },   /*  5 */
    { -1, 50 },
    {  0, 55 },
    {  0, 60 },
    {  0, 65 },
    {  0, 70 },   /* 10 */
    {  0, 75 },
    {  0, 80 },
    {  0, 85 },
    {  0, 88 },
    {  1, 90 },   /* 15 */
    {  2, 95 },
    {  2, 97 },
    {  3, 99 },   /* 18 */
    {  3, 99 },
    {  4, 99 },   /* 20 */
    {  4, 99 },
    {  5, 99 },
    {  6, 99 },
    {  7, 99 },
    {  8, 99 }    /* 25 */
};



/*
 * Liquid properties.
 * Used in world.obj.
 * Be sure MAX_LIQUID in merc.h matches
 * the number of entries below.
 */
const	struct	liq_type	liq_table	[]	=
{
/*    name			color	proof, full, thirst, food, ssize */
    { "water",			"clear",	{   0, 1, 10, 0, 16 }	},
    { "beer",			"amber",	{  12, 1,  8, 1, 12 }	},
    { "red wine",		"burgundy",	{  30, 1,  8, 1,  5 }	},
    { "ale",			"brown",	{  15, 1,  8, 1, 12 }	},
    { "dark ale",		"dark",		{  16, 1,  8, 1, 12 }	},

    { "whisky",			"golden",	{ 120, 1,  5, 0,  2 }	},
    { "lemonade",		"pink",		{   0, 1,  9, 2, 12 }	},
    { "firebreather",		"boiling",	{ 190, 0,  4, 0,  2 }	},
    { "local specialty",	"clear",	{ 151, 1,  3, 0,  2 }	},
    { "slime mold juice",	"green",	{   0, 2, -8, 1,  2 }	},

    { "milk",			"white",	{   0, 2,  9, 3, 12 }	},
    { "tea",			"tan",		{   0, 1,  8, 0,  6 }	},
    { "coffee",			"black",	{   0, 1,  8, 0,  6 }	},
    { "blood",			"red",		{   0, 2, -1, 2,  6 }	},
    { "salt water",		"clear",	{   0, 1, -2, 0,  1 }	},

    { "coke",			"brown",	{   0, 2,  9, 2, 12 }	}, 
    { "root beer",		"brown",	{   0, 2,  9, 2, 12 }   },
    { "elvish wine",		"green",	{  35, 2,  8, 1,  5 }   },
    { "white wine",		"golden",	{  28, 1,  8, 1,  5 }   },
    { "champagne",		"golden",	{  32, 1,  8, 1,  5 }   },

    { "mead",			"honey-colored",{  34, 2,  8, 2, 12 }   },
    { "rose wine",		"pink",		{  26, 1,  8, 1,  5 }	},
    { "benedictine wine",	"burgundy",	{  40, 1,  8, 1,  5 }   },
    { "vodka",			"clear",	{ 130, 1,  5, 0,  2 }   },
    { "cranberry juice",	"red",		{   0, 1,  9, 2, 12 }	},

    { "orange juice",		"orange",	{   0, 2,  9, 3, 12 }   }, 
    { "absinthe",		"green",	{ 200, 1,  4, 0,  2 }	},
    { "brandy",			"golden",	{  80, 1,  5, 0,  4 }	},
    { "aquavit",		"clear",	{ 140, 1,  5, 0,  2 }	},
    { "schnapps",		"clear",	{  90, 1,  5, 0,  2 }   },

    { "icewine",		"purple",	{  50, 2,  6, 1,  5 }	},
    { "amontillado",		"burgundy",	{  35, 2,  8, 1,  5 }	},
    { "sherry",			"red",		{  38, 2,  7, 1,  5 }   },	
    { "framboise",		"red",		{  50, 1,  7, 1,  5 }   },
    { "rum",			"amber",	{ 151, 1,  4, 0,  2 }	},

    { "cordial",		"clear",	{ 100, 1,  5, 0,  2 }   },
    { "dr pepper",		"brown",	{   0, 2,  9, 2, 12 }	},
    { NULL,			NULL,		{   0, 0,  0, 0,  0 }	}
};



/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n)	n

const	struct	skill_type	skill_table	[MAX_SKILL]	=
{

/*
 * Magic spells.
 */

/*
    {
	"NAME",
	{ SKILL_LEVELS 1st TIER,
	  SKILL_LEVELS 2nd TIER },
	{ DIFFICULTY 1st TIER,
	  DIFFICULTY 2nd TIER },
	SPELL_FUN,		TARGET,		MIN_POSITION,
	GSN,		SOCKET,	SLOT(#),	MIN_MANA,	BEATS,
	"DAMAGE NOUN",	"OFF MESSAGE",	"OFF MESSAGE (OBJ)"
    }

	NAME		Name of Spell/Skill
	SKILL_LEVELS	Levels to obtain spell/skill per class
	DIFFICULTY	Spells: multiplier for base mana
			Skills: charge for gaining skill
	SPELL_FUN	Routine to call for spells
	TARGET		Legal targets
	MIN_POSTITION	Position for caster/user
	GSN		gsn for skills and some spells
	SOCKET		Can spell be cast on person from same socket
			(to limit multiplaying)
	SLOT(#)		A unique slot number for spells
	MIN_MANA	Base mana for spells (multiplied by DIFFICULTY)
	BEATS		Waiting time after use
	DAMAGE NOUN	Damage Message
	OFF MESSAGE	Wear off message
	OFF MESSAGE (OBJ)	Wear off message for objects
*/

    {
	"reserved",
	{ 199, 199, 199, 199, 199, 199, 199,
	  199, 199, 199, 199, 199, 199, 199 },
	{ 99, 99, 99, 99, 99, 99, 99,
	  99, 99, 99, 99, 99, 99, 99 },
	0,			TAR_IGNORE,	POS_STANDING,
	NULL,		TRUE,	SLOT( 0),	0,	0,
	"",		"",		""
    },

    {
	"acid blast",
	{ 55, 93, 103, 103, 100, 45, 65,
	  45, 83, 103, 103, 90, 35, 55 },
	{ 1, 1, 3, 3, 2, 1, 2,
	  1, 1, 3, 3, 2, 1, 2 },
	spell_acid_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(70),	20,	12,
	"acid blast",	"!Acid Blast!",		""
    },

    {
	"animate",
	{ 103, 103, 103, 103, 103, 103, 45,
	  103, 103, 103, 103, 103, 103, 35 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_animate,		TAR_OBJ_INV,		POS_STANDING,
	NULL,		TRUE,	SLOT(239),	 20,	12,
	"",			"!Animate!",	""
    },

    {
	"armor",
	{ 13, 2, 102, 102, 102, 23, 102,
	  3, 1, 102, 102, 102, 13, 102 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_armor,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT( 1),	 5,	12,
	"",			"You feel less armored.",	""
    },

    {
	"bless",
	{ 90, 13, 102, 102, 102, 16, 102,
	  75, 3, 102, 102, 102, 6, 102 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_bless,		TAR_OBJ_CHAR_DEF,	POS_STANDING,
	NULL,		FALSE,	SLOT( 3),	 5,	12,
	"",			"You feel less righteous.", 
	"$p's holy aura fades."
    },

    {
	"blindness",
	{ 24, 15, 103, 103, 103, 14, 103,
	  14, 5, 103, 103, 103, 4, 103 },
	{ 1, 1, 2, 2, 2, 1, 3,
	  1, 1, 2, 2, 4, 1, 3 },
	spell_blindness,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_blindness,	TRUE,	SLOT( 4),	35,	12,
	"",			"You can see again.",	""
    },

    {
	"burning hands",
	{ 13, 87, 103, 103, 103, 23, 103,
	  3, 77, 103, 103, 103, 13, 85 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_burning_hands,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT( 5),	15,	12,
	"burning hands",	"!Burning Hands!", 	""
    },

    {
	"call lightning",
	{ 103, 36, 103, 103, 30, 48, 103,
	  92, 26, 103, 103, 20, 38, 103 },
	{ 1, 1, 2, 2, 1, 1, 1,
	  1, 1, 2, 2, 1, 1, 1 },
	spell_call_lightning,	TAR_IGNORE,		POS_FIGHTING,
	NULL,		TRUE,	SLOT( 6),	15,	12,
	"lightning bolt",	"!Call Lightning!",	""
    },

    {
	"calm",
	{ 95, 32, 102, 102, 90, 46, 59,
	  85, 22, 102, 102, 80, 36, 49 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_calm,		TAR_IGNORE,		POS_FIGHTING,
	NULL,		TRUE,	SLOT(509),	30,	12,
	"",			"You have lost your peace of mind.",	""
    },

    {
	"cancellation",
	{ 51, 51, 102, 102, 102, 46, 102,
	  41, 41, 102, 102, 102, 36, 102 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_cancellation,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,		FALSE,	SLOT(507),	20,	12,
	""			"!cancellation!",	""
    },

    {
	"cause critical",
	{ 45, 45, 103, 103, 103, 103, 21,
	  35, 35, 103, 103, 103, 103, 11 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_cause_critical,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(63),	20,	12,
	"spell",		"!Cause Critical!",	""
    },

    {
	"cause light",
	{ 5, 2, 103, 103, 103, 103, 5,
	  1, 1, 103, 103, 103, 90, 4 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_cause_light,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(62),	15,	12,
	"spell",		"!Cause Light!",	""
    },

    {
	"cause serious",
	{ 20, 23, 103, 103, 103, 103, 11,
	  10, 13, 103, 103, 103, 100, 9 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_cause_serious,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(64),	17,	12,
	"spell",		"!Cause Serious!",	""
    },

    {   
	"chain lightning",
	{ 66, 103, 103, 103, 103, 103, 103,
	  55, 103, 103, 103, 103, 103, 103 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_chain_lightning,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(500),	25,	12,
	"lightning",		"!Chain Lightning!",	""
    }, 

    {
	"change sex",
	{ 103, 103, 103, 103, 103, 103, 103,
	  103, 103, 103, 103, 103, 103, 103 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_change_sex,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(82),	15,	12,
	"",			"Your body feels familiar again.",	""
    },

    {
	"charm person",
	{ 40, 30, 50, 103, 50, 103, 43,
	  30, 20, 40, 103, 40, 103, 33 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_charm_person,	TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_charm_person,	TRUE,	SLOT( 7),	25,	12,
	"",			"You feel more self-confident.",	""
    },

    {
	"chill touch",
	{ 12, 103, 103, 103, 103, 22, 40,
	  2, 100, 103, 103, 103, 12, 30 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_chill_touch,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT( 8),	15,	12,
	"chilling touch",	"You feel less cold.",	""
    },

    {
	"colour spray",
	{ 32, 103, 103, 103, 103, 20, 103,
	  22, 103, 103, 103, 103, 10, 103 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_colour_spray,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(10),	15,	12,
	"colour spray",		"!Colour Spray!",	""
    },

    { 
        "conjure",
        { 85, 102, 102, 102, 102, 95, 102,
          59, 102, 102, 102, 102, 71, 102 },
        { 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1 },
        spell_conjure,        TAR_IGNORE,             POS_STANDING,
        NULL,          TRUE,         SLOT(280),       100,    12,
        "",                     "!Conjure!",  ""
    },

    { 
        "call beast",
        { 102, 102, 102, 102, 102, 40, 102,
          102, 102, 102, 102, 102, 30, 102 },
        { 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1 },
        spell_call_beast,        TAR_IGNORE,             POS_STANDING,
        NULL,          TRUE,         SLOT(280),       100,    12,
        "",                     "!Call Beast!",  ""
    },

    {
	"continual light",
	{ 11, 8, 102, 102, 7, 5, 102,
	  1, 7, 102, 102, 1, 4, 102 },
	{ 1, 1, 2, 2, 1, 1, 2,
	  1, 1, 2, 2, 1, 1, 2 },
	spell_continual_light,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(57),	 7,	12,
	"",			"!Continual Light!",	""
    },

    {
	"control weather",
	{ 102, 48, 102, 102, 32, 20, 102,
	  102, 38, 102, 102, 22, 10, 102 },
	{ 1, 1, 2, 2, 1, 1, 2,
	  1, 1, 2, 2, 1, 1, 2 },
	spell_control_weather,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(11),	25,	12,
	"",			"!Control Weather!",	""
    },

    {
	"create food",
	{ 19, 10, 102, 102, 8, 7, 102,
	  9, 1, 102, 102, 6, 2, 102 },
	{ 1, 1, 2, 2, 1, 1, 2,
	  1, 1, 2, 2, 1, 1, 2 },
	spell_create_food,	TAR_IGNORE,		POS_RESTING,
	NULL,		TRUE,	SLOT(12),	 5,	12,
	"",			"!Create Food!",	""
    },

    {
	"create rose",
	{ 22, 17, 102, 102, 36, 4, 102,
	  12, 11, 102, 102, 18, 1, 102 },
	{ 1, 1, 2, 2, 1, 1, 2,
	  1, 1, 2, 2, 1, 1, 2 },
	spell_create_rose,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(511),	30, 	12,
	"",			"!Create Rose!",	""
    },  

    {
	"create spring",
	{ 28, 34, 102, 102, 26, 16, 102,
	  18, 24, 102, 102, 16, 8, 102 },
	{ 1, 1, 2, 2, 1, 1, 2,
	  1, 1, 2, 2, 1, 1, 2 },
	spell_create_spring,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(80),	20,	12,
	"",			"!Create Spring!",	""
    },

    {
	"create water",
	{ 15, 5, 102, 102, 8, 3, 102,
	  5, 1, 102, 102, 4, 2, 102 },
	{ 1, 1, 2, 2, 1, 1, 2,
	  1, 1, 2, 2, 1, 1, 2 },
	spell_create_water,	TAR_OBJ_INV,		POS_STANDING,
	NULL,		TRUE,	SLOT(13),	 5,	12,
	"",			"!Create Water!",	""
    },

    {
	"cure blindness",
	{ 102, 11, 102, 102, 13, 16, 102,
	  90, 1, 102, 102, 3, 6, 102 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_cure_blindness,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,		FALSE,	SLOT(14),	 5,	12,
	"",			"!Cure Blindness!",	""
    },

    {
	"cure critical",
	{ 90, 25, 102, 102, 49, 37, 102,
	  80, 15, 102, 102, 39, 27, 102 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_cure_critical,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,		FALSE,	SLOT(15),	45,	12,
	"",			"!Cure Critical!",	""
    },

    {
	"cure disease",
	{ 102, 26, 102, 102, 28, 32, 102,
	  102, 16, 102, 102, 18, 22, 102 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_cure_disease,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(501),	20,	12,
	"",			"!Cure Disease!",	""
    },

    {
	"cure light",
	{ 87, 4, 102, 102, 3, 8, 102,
	  77, 3, 102, 102, 1, 1, 102 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_cure_light,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,		FALSE,	SLOT(16),	5,	12,
	"",			"!Cure Light!",		""
    },

    {
	"cure poison",
	{ 98, 28, 102, 102, 37, 33, 102,
	  88, 18, 102, 102, 27, 23, 102 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_cure_poison,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(43),	 5,	12,
	"",			"!Cure Poison!",	""
    },

    {
	"cure serious",
	{ 102, 14, 102, 102, 34, 24, 102,
	  102, 4, 102, 102, 24, 14, 102 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_cure_serious,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,		FALSE,	SLOT(61),	10,	12,
	"",			"!Cure Serious!",	""
    },

    {
	"curse",
	{ 35, 35, 103, 103, 103, 38, 45,
	  25, 25, 103, 103, 103, 28, 35 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_curse,		TAR_OBJ_CHAR_OFF,	POS_FIGHTING,
	&gsn_curse,	TRUE,	SLOT(17),	20,	12,
	"curse",		"The curse wears off.", 
	"$p is no longer impure."
    },

    {
	"demonfire",
	{ 76, 68, 103, 103, 103, 103, 77,
	  66, 58, 103, 103, 103, 103, 67 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_demonfire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(505),	20,	12,
	"torments",		"!Demonfire!",		""
    },	

    {
	"detect evil",
	{ 22, 22, 102, 102, 10, 102, 17,
	  12, 12, 102, 102, 1, 85, 7 },
	{ 1, 1, 2, 2, 1, 1, 1,
	  1, 1, 2, 2, 1, 1, 1 },
	spell_detect_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,		FALSE,	SLOT(18),	 5,	12,
	"",			"The red in your vision disappears.",	""
    },

    {
        "detect good",
	{ 23, 22, 102, 102, 10, 102, 17,
	  13, 12, 102, 102, 1, 102, 7 },
	{ 1, 1, 2, 2, 1, 1, 1,
	  1, 1, 2, 2, 1, 1, 1 },
        spell_detect_good,      TAR_CHAR_SELF,          POS_STANDING,
        NULL,		FALSE,	SLOT(513),        5,     12,
        "",                     "The gold in your vision disappears.",	""
    },

    {
	"detect hidden",
	{ 24, 22, 24, 102, 29, 26, 18,
	  14, 12, 14, 102, 19, 16, 8 },
	{ 1, 1, 2, 2, 1, 1, 1,
	  1, 1, 2, 2, 1, 1, 1 },
	spell_detect_hidden,	TAR_CHAR_SELF,	POS_STANDING,
	NULL,		FALSE,	SLOT(44),	 5,	12,
	"",			"You feel less aware of your surroundings.",	
	""
    },

    {
	"detect invis",
	{ 25, 16, 11, 102, 25, 25, 4,
	  15, 6, 1, 102, 15, 15, 3 },
	{ 1, 1, 2, 2, 1, 1, 1,
	  1, 1, 2, 2, 1, 1, 1 },
	spell_detect_invis,	TAR_CHAR_SELF,	POS_STANDING,
	NULL,		FALSE,	SLOT(19),	 5,	12,
	"",			"You no longer see invisible objects.",
	""
    },

    {
	"detect magic",
	{ 26, 11, 8, 102, 24, 21, 3,
	  16, 1, 1, 102, 14, 11, 2 },
	{ 1, 1, 2, 2, 1, 1, 1,
	  1, 1, 2, 2, 1, 1, 1 },
	spell_detect_magic,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,		FALSE,	SLOT(20),	 5,	12,
	"",			"The detect magic wears off.",	""
    },

    {
	"detect poison",
	{ 27, 14, 18, 102, 8, 11, 11,
	  17, 4, 8, 102, 1, 1, 1 },
	{ 1, 1, 2, 2, 1, 1, 1,
	  1, 1, 2, 2, 1, 1, 1 },
	spell_detect_poison,	TAR_OBJ_INV,		POS_STANDING,
	NULL,		TRUE,	SLOT(21),	 5,	12,
	"",			"!Detect Poison!",	""
    },

    {
	"dispel evil",
	{ 103, 30, 103, 103, 103, 35, 53,
	  103, 20, 103, 103, 103, 25, 43 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_dispel_evil,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(22),	15,	12,
	"dispel evil",		"!Dispel Evil!",	""
    },

    {
        "dispel good",
	{ 103, 30, 103, 103, 103, 35, 53,
	  103, 20, 103, 103, 103, 25, 43 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
        spell_dispel_good,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,		TRUE,	SLOT(512),      15,     12,
        "dispel good",          "!Dispel Good!",	""
    },

    {
	"dispel magic",
	{ 48, 48, 103, 103, 103, 36, 103,
	  38, 38, 103, 103, 103, 35, 103 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_dispel_magic,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(59),	15,	12,
	"",			"!Dispel Magic!",	""
    },

    {
        "divine wrath",
        { 102, 85, 102, 102, 102, 102, 102,
          102, 75, 102, 102, 102, 101, 102 },
        { 1, 1, 2, 2, 2, 2, 2,
          1, 1, 2, 2, 2, 2, 2 },
        spell_divine_wrath,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_divine_wrath,           FALSE,  SLOT(256),      155,     18,
        "divine wrath",       "!Divine Wrath",
        ""
    },

    {
	"earthquake",
	{ 83, 20, 103, 103, 36, 11, 76,
	  73, 10, 103, 103, 26, 1, 66 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_earthquake,	TAR_IGNORE,		POS_FIGHTING,
	NULL,		TRUE,	SLOT(23),	15,	12,
	"earthquake",		"!Earthquake!",		""
    },

    {
	"empower",
	{ 36, 40, 103, 103, 103, 42, 103,
	  26, 30, 103, 103, 103, 32, 103 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_empower,		TAR_IGNORE,	POS_RESTING,
	NULL,		TRUE,	SLOT(234),	 40,	12,
	"",			"!Empower!",	""
    },

    {
	"enchant armor",
	{ 31, 90, 102, 102, 102, 102, 102,
	  21, 80, 102, 102, 101, 100, 102 },
	{ 2, 2, 4, 4, 4, 2, 4,
	  2, 2, 4, 4, 4, 2, 4 },
	spell_enchant_armor,	TAR_OBJ_INV,		POS_STANDING,
	NULL,		TRUE,	SLOT(510),	50,	24,
	"",			"!Enchant Armor!",	""
    },

    {
	"enchant weapon",
	{ 32, 90, 102, 102, 102, 102, 102,
	  22, 80, 102, 102, 101, 100, 102 },
	{ 2, 2, 4, 4, 4, 2, 4,
	  2, 2, 4, 4, 4, 2, 4 },
	spell_enchant_weapon,	TAR_OBJ_INV,		POS_STANDING,
	NULL,		TRUE,	SLOT(24),	50,	24,
	"",			"!Enchant Weapon!",	""
    },

    {
	"energy drain",
	{ 38, 44, 103, 103, 103, 103, 10,
	  28, 34, 103, 103, 103, 103, 1 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_energy_drain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(25),	35,	12,
	"energy drain",		"!Energy Drain!",	""
    },

    {
	"faerie fire",
	{ 12, 5, 103, 103, 17, 2, 92,
	  2, 4, 103, 103, 7, 1, 82 },
	{ 1, 1, 2, 2, 1, 1, 2,
	  1, 1, 2, 2, 1, 1, 2 },
	spell_faerie_fire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(72),	 5,	12,
	"faerie fire",		"The pink aura around you fades away.",
	""
    },

    {
	"faerie fog",
	{ 28, 41, 103, 103, 35, 18, 103,
	  18, 31, 103, 103, 25, 8, 103 },
	{ 1, 1, 2, 2, 1, 1, 2,
	  1, 1, 2, 2, 1, 1, 2 },
	spell_faerie_fog,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(73),	12,	12,
	"faerie fog",		"!Faerie Fog!",		""
    },

    {
	"farsight",
	{ 4, 8, 103, 103, 15, 9, 6,
	  3, 7, 103, 103, 5, 8, 5 },
	{ 1, 1, 2, 2, 1, 1, 1,
	  1, 1, 2, 2, 1, 1, 1 },
	spell_farsight,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(521),	5,	12,
	"",			"The green in your vision disappears.",
	""
    },	

    {
	"fireball",
	{ 44, 89, 103, 103, 103, 44, 81,
	  34, 79, 103, 103, 103, 34, 71 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_fireball,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(26),	15,	12,
	"fireball",		"!Fireball!",		""
    },
  
    {
	"fireproof",
	{ 25, 24, 102, 102, 102, 21, 102,
	  15, 14, 102, 102, 102, 11, 102 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_fireproof,	TAR_OBJ_INV,		POS_STANDING,
	NULL,		TRUE,	SLOT(523),	10,	12,
	"",			"",	"$p's protective aura fades."
    },

    {
	"fireshield",
	{ 50, 60, 102, 102, 102, 55, 102,
	  40, 50, 102, 102, 102, 45, 102 },
	{ 3, 3, 5, 5, 5, 3, 5,
	  3, 3, 5, 5, 5, 3, 5 },
	spell_fireshield,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(411),	25,	12,
	"fireball",		"Your firey shield gutters out.",
	""
    },

    {
	"flamestrike",
	{ 34, 40, 103, 103, 103, 30, 47,
	  24, 30, 103, 103, 103, 20, 37 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_flamestrike,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(65),	20,	12,
	"flamestrike",		"!Flamestrike!",		""
    },

    {
	"floating disc",
	{ 7, 19, 102, 102, 45, 15, 102,
	  6, 9, 102, 102, 35, 5, 102 },
	{ 1, 1, 2, 2, 1, 1, 2,
	  1, 1, 2, 2, 1, 1, 2 },
	spell_floating_disc,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(522),	40,	24,
	"",			"!Floating disc!",	""
    },

    {
	"fly",
	{ 19, 36, 102, 102, 31, 66, 33,
	  9, 26, 102, 102, 21, 56, 23 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_fly,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(56),	10,	18,
	"",			"You slowly float to the ground.",	""
    },

    {
        "frenzy",
	{ 66, 48, 103, 103, 57, 103, 55,
	  56, 38, 103, 103, 47, 103, 45 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
        spell_frenzy,           TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,		FALSE,	SLOT(504),      30,     24,
        "",                     "Your rage ebbs.",	""
    },

    {
	"gate",
	{ 54, 34, 102, 102, 42, 81, 76,
	  44, 24, 102, 102, 32, 71, 66 },
	{ 1, 1, 2, 2, 4, 1, 2,
	  1, 1, 2, 2, 4, 1, 2 },
	spell_gate,		TAR_IGNORE,		POS_FIGHTING,
	NULL,		FALSE,	SLOT(83),	80,	12,
	"",			"!Gate!",		""
    },

    {
	"giant strength",
	{ 41, 32, 102, 102, 37, 31, 37,
	  31, 22, 102, 102, 27, 21, 27 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_giant_strength,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(39),	20,	12,
	"",			"You feel weaker.",	""
    },

    {
	"harm",
	{ 43, 46, 103, 103, 103, 41, 103,
	  33, 36, 103, 103, 103, 31, 103 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_harm,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(27),	35,	12,
	"harm spell",		"!Harm!,		"""
    },
  
    {
	"haste",
	{ 42, 54, 103, 103, 60, 26, 52,
	  32, 44, 103, 103, 50, 16, 42 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_haste,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,		FALSE,	SLOT(502),	30,	12,
	"",			"You feel yourself slow down.",	""
    },

    {
	"heal",
	{ 102, 32, 102, 102, 80, 55, 102,
	  102, 22, 102, 102, 70, 45, 102 },
	{ 1, 1, 2, 2, 4, 1, 2,
	  1, 1, 2, 2, 3, 1, 2 },
	spell_heal,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,		FALSE,	SLOT(28),	90,	18,
	"",			"!Heal!",		""
    },
  
    {
	"heat metal",
	{ 73, 42, 103, 103, 103, 22, 103,
	  63, 32, 103, 103, 103, 12, 103 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_heat_metal,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(516), 	25,	18,
	"spell",		"!Heat Metal!",		""
    },

    {
	"holy word",
	{ 103, 71, 103, 103, 103, 77, 103,
	  103, 61, 103, 103, 103, 67, 103 },
	{ 2, 2, 4, 4, 4, 2, 4,
	  2, 2, 4, 4, 4, 2, 4 },
	spell_holy_word,	TAR_IGNORE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(506), 	100,	24,
	"divine wrath",		"!Holy Word!",		""
    },

    {
	"iceshield",
	{ 30, 30, 102, 102, 102, 35, 102,
	  20, 20, 102, 102, 102, 25, 102 },
	{ 3, 3, 5, 5, 5, 3, 5,
	  3, 3, 5, 5, 5, 3, 5 },
	spell_iceshield,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(410),	25,	12,
	"chilling touch",	"Your icy shield slowly melts away.",
	""
    },

    {
	"identify",
	{ 30, 32, 35, 102, 46, 42, 28,
	  20, 22, 25, 102, 36, 32, 18 },
	{ 1, 1, 2, 2, 1, 1, 1,
	  1, 1, 2, 2, 1, 1, 1 },
	spell_identify,		TAR_OBJ_INV,		POS_STANDING,
	NULL,		TRUE,	SLOT(53),	12,	24,
	"",			"!Identify!",		""
    },

    {
	"infravision",
	{ 18, 26, 102, 102, 102, 18, 12,
	  8, 16, 102, 102, 102, 8, 2 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_infravision,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(77),	 5,	18,
	"",			"You no longer see in the dark.",	""
    },

    {
	"invisibility",
	{ 9, 37, 17, 102, 12, 39, 13,
	  8, 27, 7, 102, 2, 29, 3 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_invis,		TAR_OBJ_CHAR_DEF,	POS_STANDING,
	&gsn_invis,	FALSE,	SLOT(29),	 5,	12,
	"",			"You are no longer invisible.",		
	"$p fades into view."
    },

    {
	"know alignment",
	{ 24, 17, 102, 102, 36, 27, 33,
	  14, 7, 102, 102, 26, 17, 23 },
	{ 1, 1, 2, 2, 1, 1, 1,
	  1, 1, 2, 2, 1, 1, 1 },
	spell_know_alignment,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,		FALSE,	SLOT(58),	 9,	12,
	"",			"!Know Alignment!",	""
    },

    {
	"lightning bolt",
	{ 25, 46, 103, 103, 40, 42, 91,
	  15, 36, 103, 103, 30, 32, 81 },
	{ 1, 1, 2, 2, 1, 1, 1,
	  1, 1, 2, 2, 1, 1, 1 },
	spell_lightning_bolt,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(30),	15,	12,
	"lightning bolt",	"!Lightning Bolt!",	""
    },

    {
	"locate object",
	{ 17, 30, 102, 102, 38, 35, 24,
	  7, 20, 102, 102, 28, 25, 14 },
	{ 1, 1, 2, 2, 1, 1, 1,
	  1, 1, 2, 2, 1, 1, 1 },
	spell_locate_object,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(31),	20,	18,
	"",			"!Locate Object!",	""
    },

    {
	"magic missile",
	{ 1, 103, 103, 103, 103, 12, 103,
	  1, 103, 103, 103, 103, 2, 103 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_magic_missile,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(32),	15,	12,
	"magic missile",	"!Magic Missile!",	""
    },

    {
        "manashield",
        { 80, 102, 102, 102, 102, 80, 102,
          70, 102, 102, 102, 102, 70, 102 },
        { 2, 3, 5, 5, 5, 2, 5,
          2, 3, 5, 5, 5, 2, 5 },
        spell_manashield,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
        &gsn_manashield,           FALSE,  SLOT(257),      75,     12,
        "",       "Your manashield fades away.",
        ""
    },


    {
	"mass healing",
	{ 102, 75, 102, 102, 85, 85, 102,
	  102, 65, 102, 102, 75, 75, 102 },
	{ 2, 2, 4, 4, 2, 2, 2,
	  2, 2, 4, 4, 2, 2, 2 },
	spell_mass_healing,	TAR_IGNORE,		POS_STANDING,
	NULL,		FALSE,	SLOT(508),	50,	36,
	"",			"!Mass Healing!",	""
    },

    {
	"mass invis",
	{ 43, 49, 103, 103, 55, 103, 54,
	  33, 39, 103, 103, 45, 103, 44 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_mass_invis,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_invis,	FALSE,	SLOT(69),	20,	24,
	"",			"You are no longer invisible.",		""
    },

    {
        "nexus",
	{ 79, 69, 102, 102, 81, 102, 92,
	  69, 59, 102, 102, 71, 102, 82 },
	{ 2, 2, 4, 4, 4, 2, 4,
	  2, 2, 4, 4, 4, 2, 4 },
        spell_nexus,            TAR_IGNORE,             POS_STANDING,
        NULL,		FALSE,	SLOT(520),       75,   36,
        "",                     "!Nexus!",		""
    },

    {
	"pass door",
	{ 48, 63, 49, 102, 60, 56, 42,
	  38, 53, 39, 102, 50, 42, 32 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_pass_door,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,		FALSE,	SLOT(74),	20,	12,
	"",			"You feel solid again.",	""
    },

    {
	"plague",
	{ 46, 34, 103, 103, 59, 39, 65,
	  36, 24, 103, 103, 49, 29, 55 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_plague,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_plague,	TRUE,	SLOT(503),	20,	12,
	"sickness",		"Your sores vanish.",	""
    },

    {
	"poison",
	{ 33, 24, 103, 103, 49, 27, 22,
	  23, 14, 103, 103, 39, 17, 12 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_poison,		TAR_OBJ_CHAR_OFF,	POS_FIGHTING,
	&gsn_poison,	TRUE,	SLOT(33),	10,	12,
	"poison",		"You feel less sick.",	
	"The poison on $p dries up."
    },

    {
	"accelerate poison",
	{ 103, 103, 103, 103, 103, 103, 103,
	  103, 103, 103, 103, 103, 103, 103 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_accelerate_poison,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_accelerate_poison,	TRUE,	SLOT(68),	100,	24,
	"fast poison",	"You feel a lot less sick.",	""
    },

    {
        "portal",
	{ 70, 59, 102, 102, 72, 102, 83,
	  60, 49, 102, 102, 62, 102, 73 },
	{ 2, 2, 4, 4, 4, 2, 4, 
	  2, 2, 4, 4, 4, 2, 4 }, 
        spell_portal,           TAR_IGNORE,             POS_STANDING,
        NULL,		FALSE,	SLOT(519),       50,     24,
        "",                     "!Portal!",		""
    },

    {
	"protection evil",
	{ 24, 18, 102, 102, 64, 19, 66,
	  14, 8, 102, 102, 54, 9, 56 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_protection_evil,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,		FALSE,	SLOT(34), 	5,	12,
	"",			"You feel less protected.",	""
    },

    {
        "protection good",
	{ 24, 18, 102, 102, 64, 19, 66,
	  14, 8, 102, 102, 54, 9, 56 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
        spell_protection_good,  TAR_CHAR_SELF,          POS_STANDING,
        NULL,		FALSE,	SLOT(514),	5,	12,
        "",                     "You feel less protected.",	""
    },

    {
	"protection voodoo",
	{ 102, 102, 102, 102, 102, 102, 102,
	  102, 102, 102, 102, 102, 102, 102 },
	{ 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	spell_protection_voodoo, TAR_CHAR_SELF,		POS_STANDING,
	NULL,		FALSE,	SLOT(345),	5,	12,
	"",			"",	""
    },

    {
	"peace",
	{ 102, 102, 102, 102, 102, 102, 102,
	  102, 102, 102, 102, 102, 102, 102 },
	{ 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	spell_peace, TAR_CHAR_DEFENSIVE,		POS_STANDING,
	&gsn_peace,	FALSE,	SLOT(345),	5,	12,
	"",			"You are now player-killable again.",	""
    },

    {
	"quest pill",
	{ 110, 110, 110, 110, 110, 110, 110,
	  110, 110, 110, 110, 110, 110, 110 },
	{ 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	spell_quest_pill,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,		TRUE,	SLOT(530),	 5,	18,
	"",			"!Quest Pill!",	""
    },

    {
        "ray of truth",
	{ 103, 69, 103, 103, 103, 96, 103,
	  103, 59, 103, 103, 103, 86, 103 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
        spell_ray_of_truth,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,		TRUE,	SLOT(518),      20,     12,
        "ray of truth",         "!Ray of Truth!",	""
    },

    {
	"recharge",
	{ 18, 63, 103, 103, 103, 52, 103,
	  8, 53, 103, 103, 103, 42, 103 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_recharge,		TAR_OBJ_INV,		POS_STANDING,
	NULL,		TRUE,	SLOT(517),	60,	24,
	"",			"!Recharge!",		""
    },

    {
	"refresh",
	{ 16, 9, 102, 102, 18, 10, 16,
	  6, 4, 102, 102, 8, 1, 6 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_refresh,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(81),	6,	18,
	"refresh",		"!Refresh!",		""
    },

    {
	"remove curse",
	{ 96, 35, 102, 102, 93, 49, 102,
	  93, 25, 102, 102, 83, 39, 102 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_remove_curse,	TAR_OBJ_CHAR_DEF,	POS_STANDING,
	NULL,		FALSE,	SLOT(35),	 5,	12,
	"",			"!Remove Curse!",	""
    },

    {
	"restore mana",
	{ 102, 102, 102, 102, 102, 102, 102,
	  102, 102, 102, 102, 102, 102, 102 },
	{ 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	spell_restore_mana,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(251),	1,	18,
	"restore mana",		"!Restore Mana!",	""
    },

    {
	"resurrect",
	{ 103, 103, 103, 103, 103, 103, 28,
	  103, 103, 103, 103, 103, 103, 18 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_resurrect,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(235),	 35,	12,
	"",			"!Resurrect!",	""
    },

    {
	"sanctuary",
	{ 49, 39, 103, 103, 98, 40, 103,
	  40, 29, 103, 103, 57, 30, 103 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_sanctuary,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_sanctuary,	FALSE,	SLOT(36),	70,	12,
	"",			"The white aura around your body fades.",
	""
    },

    {
	"shield",
	{ 40, 60, 102, 102, 88, 80, 102,
	  30, 50, 102, 102, 78, 70, 102 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_shield,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(67),	12,	18,
	"",			"Your force shield shimmers then fades away.",
	""
    },

    {
	"shocking grasp",
	{ 20, 103, 103, 103, 103, 30, 27,
	  10, 103, 103, 103, 103, 20, 17 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_shocking_grasp,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(53),	15,	12,
	"shocking grasp",	"!Shocking Grasp!",	""
    },

    {
	"shockshield",
	{ 60, 40, 102, 102, 102, 90, 102,
	  50, 30, 102, 102, 102, 80, 102 },
	{ 3, 3, 5, 5, 5, 3, 5,
	  3, 3, 5, 5, 5, 3, 5 },
	spell_shockshield,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,		FALSE,	SLOT(412),	25,	12,
	"lightning bolt",	"Your crackling shield sizzles and fades.",
	""
    },

    {
        "shockwave",
        { 85, 102, 102, 102, 102, 90, 102,
          75, 102, 102, 102, 102, 80, 102 },
        { 1, 1, 2, 2, 2, 2, 2,
          1, 1, 2, 2, 2, 2, 2 },
        spell_shockwave,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_shockwave,           FALSE,  SLOT(255),      170,     18,
        "shockwave",       "!Shockwave",
        ""
    },

    {
	"sleep",
	{ 20, 103, 103, 103, 21, 103, 15,
	  10, 95, 103, 103, 11, 103, 5 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_sleep,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_sleep,	TRUE,	SLOT(38),	15,	12,
	"",			"You feel less tired.",	""
    },

    {
        "slow",
	{ 45, 40, 103, 103, 60, 66, 51,
	  35, 30, 103, 103, 50, 56, 41 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
        spell_slow,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,		TRUE,	SLOT(515),      30,     12,
        "",                     "You feel yourself speed up.",	""
    },

    {
	"stone skin",
	{ 49, 70, 102, 102, 100, 90, 102,
	  39, 60, 102, 102, 90, 80, 102 },
	{ 1, 1, 2, 2, 2, 1, 2,
	  1, 1, 2, 2, 2, 1, 2 },
	spell_stone_skin,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,		FALSE,	SLOT(66),	12,	18,
	"",			"Your skin feels soft again.",	""
    },

    {
	"summon",
	{ 48, 23, 102, 102, 33, 74, 51,
	  38, 13, 102, 102, 23, 62, 41 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_summon,		TAR_IGNORE,		POS_STANDING,
	NULL,		FALSE,	SLOT(40),	50,	12,
	"",			"!Summon!",		""
    },

    {
	"teleport",
	{ 24, 44, 102, 102, 59, 102, 59,
	  14, 34, 102, 102, 49, 102, 49 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_teleport,		TAR_CHAR_SELF,		POS_FIGHTING,
	NULL,	 	TRUE,	SLOT( 2),	35,	12,
	"",			"!Teleport!",		""
    },

    {
	"transport",
	{ 25, 46, 102, 102, 45, 53, 51,
	  15, 36, 102, 102, 35, 43, 41 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_transport,	TAR_OBJ_TRAN,		POS_STANDING,
	NULL,		FALSE,	SLOT(524),	30,	12,
	"",			"!Transport!",		""
    },

    {
	"ventriloquate",
	{ 1, 102, 2, 102, 5, 102, 4,
	  1, 102, 1, 102, 1, 102, 3 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_ventriloquate,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(41),	 5,	12,
	"",			"!Ventriloquate!",	""
    },

    {
	"voodoo",
	{ 102, 80, 102, 102, 102, 102, 65,
	  102, 45, 102, 102, 102, 102, 30 },
	{ 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	spell_voodoo,		TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(286),	80,	12,
	"",			"!Voodoo!",		""
    },

    {
	"weaken",
	{ 21, 28, 103, 103, 40, 31, 25,
	  11, 18, 103, 103, 30, 21, 15 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_weaken,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_weaken,	TRUE,	SLOT(68),	20,	12,
	"spell",		"You feel stronger.",	""
    },

    {
	"word of recall",
	{ 63, 56, 103, 103, 47, 103, 73,
	  53, 46, 103, 103, 37, 103, 63 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_word_of_recall,	TAR_CHAR_SELF,		POS_RESTING,
	NULL,		TRUE,	SLOT(42),	 5,	12,
	"",			"!Word of Recall!",	""
    },

/*
 * Dragon breath
 */
    {
	"acid breath",
	{ 62, 103, 103, 103, 103, 103, 103,
	  52, 103, 103, 103, 103, 103, 103 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_acid_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(200),	100,	24,
	"blast of acid",	"!Acid Breath!",	""
    },

    {
	"fire breath",
	{ 80, 90, 103, 103, 103, 103, 103,
	  70, 80, 103, 103, 103, 103, 103 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_fire_breath,	TAR_IGNORE,	POS_FIGHTING,
	&gsn_fire_breath,	TRUE,	SLOT(201),	100, 24,
	"blast of flame",	"The smoke leaves your eyes.",	""
    },

    {
	"frost breath",
	{ 67, 103, 103, 103, 103, 103, 103,
	  57, 103, 103, 103, 103, 103, 103 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_frost_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(202),	125,	24,
	"blast of frost",	"!Frost Breath!",	""
    },

    {
	"gas breath",
	{ 77, 103, 103, 103, 103, 103, 103,
	  67, 103, 103, 103, 103, 103, 103 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_gas_breath,	TAR_IGNORE,		POS_FIGHTING,
	NULL,		TRUE,	SLOT(203),	150,	24,
	"blast of gas",		"!Gas Breath!",		""
    },

    {
	"lightning breath",
	{ 74, 103, 103, 103, 103, 103, 103,
	  64, 103, 103, 103, 103, 103, 103 },
	{ 1, 1, 2, 2, 2, 1, 1,
	  1, 1, 2, 2, 2, 1, 1 },
	spell_lightning_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,		TRUE,	SLOT(204),	150,	24,
	"blast of lightning",	"!Lightning Breath!",	""
    },

/*
 * Spells for mega1.are from Glop/Erkenbrand.
 */
    {
        "general purpose",
	{ 104, 104, 104, 104, 104, 104, 104,
	  104, 104, 104, 104, 104, 104, 104 },
	{ 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0 },
        spell_general_purpose,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,		TRUE,	SLOT(401),      0,      12,
        "general purpose ammo", "!General Purpose Ammo!",	""
    },
 
    {
        "high explosive",
	{ 104, 104, 104, 104, 104, 104, 104,
	  104, 104, 104, 104, 104, 104, 104 },
	{ 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0 },
        spell_high_explosive,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,		TRUE,	SLOT(402),      0,      12,
        "high explosive ammo",  "!High Explosive Ammo!",	""
    },


/* combat and weapons skills */


    {
	"axe",
	{ 1, 2, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	{ 6, 6, 5, 3, 4, 6, 5,
	  5, 5, 5, 2, 3, 5, 4 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_axe,	TRUE,	SLOT( 0),       0,      0,
        "",                     "!Axe!",		""
    },

    {
        "dagger",
	{ 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	{ 2, 3, 2, 1, 2, 3, 1,
	  2, 2, 1, 1, 1, 2, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_dagger,	TRUE,	SLOT( 0),       0,      0,
        "",                     "!Dagger!",		""
    },
 
    {
	"flail",
	{ 1, 2, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	{ 6, 3, 6, 3, 4, 3, 5,
	  5, 2, 5, 2, 3, 2, 4 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_flail,	TRUE,	SLOT( 0),       0,      0,
        "",                     "!Flail!",		""
    },

    {
	"mace",
	{ 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	{ 5, 1, 3, 2, 3, 2, 3,
	  4, 1, 3, 1, 2, 1, 2 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_mace,	TRUE,	SLOT( 0),       0,      0,
        "",                     "!Mace!",		""
    },

    {
	"polearm",
	{ 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	{ 6, 6, 6, 3, 3, 2, 5,
	  5, 5, 5, 2, 2, 1, 4 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_polearm,	TRUE,	SLOT( 0),       0,      0,
        "",                     "!Polearm!",		""
    },
    
    {
	"shield block",
	{ 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	{ 6, 4, 6, 1, 2, 4, 3,
	  5, 3, 5, 1, 1, 3, 2 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_shield_block,	TRUE,	SLOT(0),	0,	0,
	"",			"!Shield!",		""
    },
 
    {
	"spear",
	{ 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	{ 4, 4, 4, 2, 1, 3, 5,
	  3, 3, 3, 1, 1, 2, 4 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_spear,	TRUE,	SLOT( 0),       0,      0,
        "",                     "!Spear!",		""
    },

    {
	"sword",
	{ 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	{ 5, 6, 3, 1, 2, 4, 3,
	  4, 5, 2, 1, 1, 3, 2 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_sword,	TRUE,	SLOT( 0),       0,      0,
        "",                     "!sword!",		""
    },

    {
	"whip",
	{ 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	{ 6, 5, 5, 3, 3, 4, 5,
	  5, 4, 4, 3, 2, 3, 4 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_whip,	TRUE,	SLOT( 0),       0,      0,
        "",                     "!Whip!",	""
    },

    {
        "backstab",
	{ 103, 103, 4, 55, 7, 103, 8,
	  103, 103, 1, 45, 3, 103, 6 },
	{ 1, 1, 2, 4, 6, 1, 2,
	  1, 1, 2, 3, 5, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_backstab,	TRUE,	SLOT( 0),        0,     32,
        "backstab",             "!Backstab!",		""
    },

    {
	"bash",
	{ 103, 103, 103, 1, 103, 103, 103,
	  103, 103, 103, 1, 103, 103, 103 },
	{ 1, 1, 1, 2, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_bash,	TRUE,	SLOT( 0),       0,      24,
        "bash",                 "!Bash!",		""
    },

    {
	"berserk",
	{ 103, 103, 103, 27, 103, 103, 103,
	  103, 103, 103, 17, 103, 103, 103 },
	{ 1, 1, 1, 2, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_berserk,	TRUE,	SLOT( 0),       0,      24,
        "",                     "You feel your pulse slow down.",	""
    },

    {
        "circle",
	{ 103, 103, 40, 103, 103, 103, 78,
	  103, 103, 26, 103, 103, 103, 70 },
	{ 1, 1, 5, 1, 1, 1, 7,
	  1, 1, 4, 1, 1, 1, 6 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_circle,	TRUE,	SLOT( 0),        0,     24,
        "circle",             "!Circle!",		""
    },

    {
        "critical strike",
        { 83, 83, 70, 30, 35, 83, 75,
	  72, 72, 59, 20, 25, 72, 63 },
        { 6, 6, 3, 3, 4, 3, 5, 
	  4, 4, 2, 2, 2, 2, 4 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_critical,   TRUE,	SLOT( 0),       0,      0,
        "",                     "!Critical Strike!",   ""
    },

    {
	"dirt kicking",
	{ 103, 103, 4, 4, 1, 9, 3,
	  103, 103, 1, 1, 1, 8, 2 },
	{ 1, 1, 4, 4, 4, 6, 2, 
	  1, 1, 3, 3, 3, 5, 1 }, 
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_dirt,	TRUE,	SLOT( 0),	0,	24,
	"kicked dirt",		"You rub the dirt out of your eyes.",	""
    },

    {
        "disarm",
	{ 103, 103, 18, 18, 32, 103, 90,
	  103, 103, 8, 8, 22, 103, 80 },
	{ 1, 1, 6, 3, 5, 1, 3,
	  1, 1, 5, 2, 4, 1, 2 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_disarm,	TRUE,	SLOT( 0),        0,     24,
        "",                     "!Disarm!",		""
    },
 
    {
        "dodge",
	{ 103, 103, 1, 19, 10, 103, 10,
	  103, 103, 1, 9, 1, 103, 1 },
	{ 1, 1, 4, 6, 5, 1, 2,
	  1, 1, 3, 5, 4, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_dodge,	TRUE,	SLOT( 0),        0,     0,
        "",                     "!Dodge!",		""
    },

    {
        "phase",
	{ 70, 101, 103, 103, 103, 90, 103,
	  50,  95, 103, 103, 103, 80, 103 },
	{ 4, 5, 1, 1, 1, 5, 1,
	  3, 4, 1, 1, 1, 4, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_phase,	TRUE,	SLOT( 0),        0,     0,
        "",                     "!Phase!",		""
    },
 
    {
        "dual wield",
	{ 103, 103, 35, 3, 29, 103, 103,
	  103, 103, 8, 1, 15, 103, 103 },
	{ 1, 1, 3, 2, 3, 1, 1,
	  1, 1, 2, 1, 3, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_dual_wield,	TRUE,	SLOT( 0),        0,     0,
        "",			"!Dual Wield!",		""
    },

    {
        "enhanced damage",
	{ 103, 103, 25, 1, 30, 103, 33,
	  100, 103, 15, 1, 20, 103, 32 },
	{ 1, 1, 4, 2, 6, 1, 7,
	  1, 1, 3, 1, 5, 1, 6 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_enhanced_damage,   TRUE,	SLOT( 0),        0,     0,
        "",                     "!Enhanced Damage!",	""
    },

    {
	"envenom",
	{ 103, 103, 15, 90, 19, 103, 9,
	  103, 103, 5, 80, 9, 103, 1 },
	{ 1, 1, 4, 6, 1, 1, 3,
	  1, 1, 3, 5, 1, 1, 2 },
	spell_null,		TAR_IGNORE,	  	POS_RESTING,
	&gsn_envenom,	TRUE,	SLOT(0),	0,	36,
	"",			"!Envenom!",		""
    },

    {
        "feed",
	{ 103, 103, 103, 103, 103, 103, 29,
	  103, 103, 103, 103, 103, 103, 11 },
	{ 1, 1, 1, 1, 1, 1, 2,
	  1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_feed,	TRUE,	SLOT( 0),	0,	12,
        "feed",             "!Feed!",		""
    },

    {
	"gouge",
	{ 103, 103, 52, 103, 103, 103, 103,
	  103, 103, 24, 103, 103, 103, 103 },
	{ 1, 1, 2, 1, 1, 1, 1, 
	  1, 1, 1, 1, 1, 1, 1 }, 
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_gouge,	TRUE,	SLOT( 0),	0,	24,
	"gouge",		"Your vision clears.",	""
    },

    {
        "grip",
	{ 103, 103, 103, 25, 103, 103, 103,
	  103, 103, 103, 10, 103, 103, 103 },
	{ 1, 1, 1, 2, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_grip,	TRUE,	SLOT( 0),        0,     0,
        "",                     "!Grip!",		""
    },

    {
	"hand to hand",
	{ 103, 103, 22, 9, 22, 103, 12,
	  103, 103, 12, 8, 12, 103, 2 },
	{ 1, 1, 4, 2, 4, 1, 2,
	  1, 1, 3, 1, 3, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_hand_to_hand,	TRUE,	SLOT( 0),	0,	0,
	"",			"!Hand to Hand!",	""
    },

    {
        "kick",
	{ 103, 18, 21, 12, 20, 17, 15,
	  103, 8, 11, 2, 10, 7, 5 },
	{ 1, 4, 6, 3, 4, 6, 2,
	  1, 3, 5, 2, 3, 5, 1 },
        spell_null,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_kick,	TRUE,	SLOT( 0),        0,     12,
        "kick",                 "!Kick!",		""
    },

    {
        "parry",
	{ 103, 103, 19, 1, 15, 27, 17,
	  103, 103, 9, 1, 5, 17, 7 },
	{ 1, 1, 4, 4, 5, 5, 2,
	  1, 1, 3, 3, 4, 4, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_parry,	TRUE,	SLOT( 0),        0,     0,
        "",                     "!Parry!",		""
    },

    {
        "rescue",
	{ 103, 103, 103, 1, 13, 103, 103,
	  103, 103, 103, 1, 3, 103, 103 },
	{ 1, 1, 1, 4, 3, 1, 1,
	  1, 1, 1, 3, 2, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_rescue,	TRUE,	SLOT( 0),        0,     12,
        "",                     "!Rescue!",		""
    },

    {
	"trip",
	{ 103, 103, 2, 22, 25, 103, 5,
	  103, 103, 1, 12, 15, 103, 1 },
	{ 1, 1, 4, 8, 5, 1, 3,
	  1, 1, 3, 7, 4, 1, 2 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_trip,	TRUE,	SLOT( 0),	0,	24,
	"trip",			"!Trip!",		""
    },

    {
        "stun",
	{ 102, 102, 102, 45, 102, 102, 103,
	  102, 102, 102, 23, 102, 102, 103 },
	{ 1, 1, 1, 3, 1, 1, 1,
	  1, 1, 1, 2, 1, 1, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_stun,	TRUE,	SLOT( 0),        0,     0,
        "",			"!Stun!",		""
    },

    {
        "second attack",
	{ 45, 36, 18, 7, 1, 25, 20,
	  35, 26, 8, 6, 1, 15, 10 },
	{ 7, 8, 5, 3, 4, 8, 2,
	  6, 7, 4, 2, 3, 7, 1 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_second_attack,	TRUE,	SLOT( 0),        0,     0,
        "",                     "!Second Attack!",	""
    },

    {
        "third attack",
	{ 102, 102, 36, 18, 29, 102, 30,
	  102, 102, 26, 8, 19, 102, 20 },
	{ 1, 1, 9, 4, 7, 1, 3,
	  1, 1, 8, 3, 6, 1, 2 },
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_third_attack,	TRUE,	SLOT( 0),        0,     0,
        "",                     "!Third Attack!",	""
    },

    {
	"fourth attack",
	{ 102, 102, 90, 65, 80, 102, 102,
	  102, 102, 80, 55, 70, 102, 102 },
	{ 1, 1, 10, 4, 5, 1, 1,
	  1, 1, 9, 3, 4, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_fourth_attack,	TRUE,	SLOT(0),	0,	0,
	"",			"!Fourth Attack!",	""
    },

    {
	"fifth attack",
	{ 102, 102, 102, 80, 95, 102, 102,
	  102, 102, 102, 70, 85, 102, 102 },
	{ 1, 1, 1, 4, 5, 1, 1,
	  1, 1, 1, 3, 4, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_fifth_attack,	TRUE,	SLOT(0),	0,	0,
	"",			"!Fifth Attack!",	""
    },

/* non-combat skills */

    { 
	"fast healing",
	{ 102, 13, 102, 9, 55, 34, 2,
	  102, 12, 102, 1, 45, 24, 1 },
	{ 1, 5, 1, 4, 8, 5, 1,
	  1, 4, 1, 3, 7, 4, 1 },
	spell_null,		TAR_IGNORE,		POS_SLEEPING,
	&gsn_fast_healing,	TRUE,	SLOT( 0),	0,	0,
	"",			"!Fast Healing!",	""
    },

    {
	"haggle",
	{ 102, 102, 1, 102, 42, 102, 102,
	  102, 102, 1, 102, 32, 102, 102 },
	{ 1, 1, 3, 1, 5, 1, 1,
	  1, 1, 2, 1, 4, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_haggle,	TRUE,	SLOT( 0),	0,	0,
	"",			"!Haggle!",		""
    },

    {
	"hide",
	{ 102, 102, 1, 18, 5, 102, 1,
	  102, 102, 1, 8, 1, 102, 1 },
	{ 1, 1, 4, 6, 5, 1, 3,
	  1, 1, 3, 5, 4, 1, 2 },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_hide,	TRUE,	SLOT( 0),	 0,	12,
	"",			"!Hide!",		""
    },

    {
	"search",
	{ 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_search,	TRUE,	SLOT( 0),	 0,	24,
	"",			"!Search!",		""
    },

    {
	"lore",
	{ 15, 15, 9, 102, 5, 14, 102,
	  5, 5, 1, 102, 4, 4, 102 },
	{ 6, 6, 4, 1, 4, 5, 1,
	  5, 5, 3, 1, 3, 4, 1 },
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_lore,	TRUE,	SLOT( 0),	0,	36,
	"",			"!Lore!",		""
    },

    {
	"meditation",
	{ 9, 9, 102, 102, 15, 13, 102,
	  8, 1, 102, 102, 5, 3, 70 },
	{ 5, 5, 1, 1, 6, 6, 1,
	  4, 4, 1, 1, 5, 5, 6 },
	spell_null,		TAR_IGNORE,		POS_SLEEPING,
	&gsn_meditation,	TRUE,	SLOT( 0),	0,	0,
	"",			"Meditation",		""
    },

    {
	"peek",
	{ 102, 102, 1, 102, 102, 102, 4,
	  90, 102, 1, 102, 102, 102, 1 },
	{ 1, 1, 3, 1, 1, 1, 4,
	  6, 1, 2, 1, 1, 1, 3 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_peek,	TRUE,	SLOT( 0),	 0,	 0,
	"",			"!Peek!",		""
    },

    {
	"pick lock",
	{ 102, 102, 10, 102, 102, 102, 37,
	  102, 102, 6, 102, 102, 102, 27 },
	{ 1, 1, 4, 1, 1, 1, 8,
	  1, 1, 3, 1, 1, 1, 7 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_pick_lock,	TRUE,	SLOT( 0),	 0,	12,
	"",			"!Pick!",		""
    },

    {
	"sneak",
	{ 102, 102, 6, 15, 2, 102, 2,
	  102, 102, 5, 5, 1, 102, 1 },
	{ 1, 1, 4, 6, 3, 1, 1,
	  1, 1, 3, 5, 2, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_sneak,	TRUE,	SLOT( 0),	 0,	12,
	"",			"You no longer feel stealthy.",	""
    },

    {
	"steal",
	{ 102, 102, 7, 102, 102, 102, 102,
	  102, 102, 5, 102, 102, 102, 102 },
	{ 1, 1, 4, 1, 1, 1, 1,
	  1, 1, 3, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_steal,	TRUE,	SLOT( 0),	 0,	24,
	"",			"!Steal!",		""
    },

    {
	"scrolls",
	{ 2, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	{ 2, 3, 5, 8, 7, 3, 8,
	  1, 2, 4, 7, 6, 2, 7 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_scrolls,	TRUE,	SLOT( 0),	0,	24,
	"",			"!Scrolls!",		""
    },

    {
	"staves",
	{ 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	{ 2, 3, 5, 8, 7, 3, 8,
	  1, 2, 4, 7, 6, 2, 7 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_staves,	TRUE,	SLOT( 0),	0,	12,
	"",			"!Staves!",		""
    },
    
    {
	"track",
	{ 102, 102, 102, 102, 20, 102, 102,
	  102, 102, 102, 102, 10, 102, 102 },
	{ 1, 1, 1, 1, 2, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_track,	TRUE,	SLOT( 0),	 0,	12,
	"",			"!Track!",		""
    },

    {
	"wands",
	{ 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	{ 2, 3, 5, 8, 7, 3, 8,
	  1, 2, 5, 7, 6, 2, 7 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_wands,	TRUE,	SLOT( 0),	0,	12,
	"",			"!Wands!",		""
    },

    {
	"recall",
	{ 1, 1, 1, 1, 1, 1, 1,
	  1, 1, 1, 1, 1, 1, 1 },
	{ 2, 2, 2, 2, 2, 2, 2,
	  1, 1, 2, 1, 1, 1, 1 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_recall,	TRUE,	SLOT( 0),	0,	12,
	"",			"!Recall!",		""
    },

    {
	"furor",
	{ 102, 102, 102, 75, 70, 102, 102,
	  102, 102, 102, 65, 60, 102, 102 },
	{ 1, 1, 1, 8, 6, 1, 1,
	  1, 1, 1, 7, 5, 1, 1 },
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_furor,	TRUE,	SLOT( 0),	0,	12,
	"furious strike",	"!Furor!",		""
    },

    {
	"create cloud",
	{ 102, 102, 102, 102, 102, 102, 102,
	  102, 102, 102, 102, 102, 102, 102 },
	{ 1, 1, 2, 2, 1, 1, 2,
	  1, 1, 2, 2, 1, 1, 2 },
	spell_create_cloud,	TAR_IGNORE,		POS_STANDING,
	NULL,		TRUE,	SLOT(12),	 5,	12,
	"",			"!Create Cloud!",	""
    },

    {
	"vacillating aura",
	{ 102, 102, 102, 102, 102, 102, 102,
	  80,   99, 102, 102, 102, 102, 102 },
	{ 3, 3, 5, 5, 5, 3, 5,
	  3, 3, 5, 5, 5, 3, 5 },
	spell_vacillating_aura,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_vacillating_aura,	FALSE,	SLOT(412),	70, 12,
	"vacillating aura",
	"Your fluctuating shields warps and disappears.",
	""
    },

    {
	"fishing",
	{ 80, 80, 70, 60, 60, 80, 70,
	  70, 70, 60, 50, 50, 70, 60 },
	{ 3, 3, 3, 3, 3, 3, 3 ,
	  3, 3, 3, 3, 3, 3, 3 },
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_fishing,	TRUE,	SLOT( 0),	 0,	12,
	"",			"!Fishing!",		""
    }


};

const   struct  group_type      group_table     [MAX_GROUP]     =
{

    {
	"rom basics",
	{ 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0 },
	{ "scrolls", "staves", "wands", "recall" }
    },

    {
	"mage basics",
	{ 0, -1, -1, -1, -1, -1, -1,
	  -1, -1, -1, -1, -1, -1, -1 },
	{ "dagger" }
    },

    {
	"cleric basics",
	{ -1, 0, -1, -1, -1, -1, -1,
	  -1, -1, -1, -1, -1, -1, -1 },
	{ "mace" }
    },
   
    {
	"thief basics",
	{ -1, -1, 0, -1, -1, -1, -1,
	  -1, -1, -1, -1, -1, -1, -1 },
	{ "dagger", "steal", "pass door" }
    },

    {
	"warrior basics",
	{ -1, -1, -1, 0, -1, -1, -1,
	  -1, -1, -1, -1, -1, -1, -1 },
	{ "sword", "second attack", "dual wield" }
    },

    {
	"ranger basics",
	{ -1, -1, -1, -1, 0, -1, -1,
	  -1, -1, -1, -1, -1, -1, -1 },
	{ "spear", "second attack", "track" }
    },

    {
	"druid basics",
	{ -1, -1, -1, -1, -1, 0, -1,
	  -1, -1, -1, -1, -1, -1, -1 },
	{ "polearm", "invisibility" }
    },

    {
	"vampire basics",
	{ -1, -1, -1, -1, -1, -1, 0,
	  -1, -1, -1, -1, -1, -1, -1 },
	{ "dagger", "hide", "sneak" }
    },

    {
	"wizard basics",
	{ -1, -1, -1, -1, -1, -1, -1,
	  0, -1, -1, -1, -1, -1, -1 },
	{ "dagger" }
    },

    {
	"priest basics",
	{ -1, -1, -1, -1, -1, -1, -1,
	  -1, 0, -1, -1, -1, -1, -1 },
	{ "mace" }
    },
   
    {
	"mercenary basics",
	{ -1, -1, -1, -1, -1, -1, -1,
	  -1, -1, 0, -1, -1, -1, -1 },
	{ "dagger", "steal", "grip", "pass door" }
    },

    {
	"gladiator basics",
	{ -1, -1, -1, -1, -1, -1, -1,
	  -1, -1, -1, 0, -1, -1, -1 },
	{ "sword", "second attack", "dual wield" }
    },

    {
	"strider basics",
	{ -1, -1, -1, -1, -1, -1, -1,
	  -1, -1, -1, -1, 0, -1, -1 },
	{ "dagger", "second attack", "track" }
    },

    {
	"sage basics",
	{ -1, -1, -1, -1, -1, -1, -1,
	  -1, -1, -1, -1, -1, 0, -1 },
	{ "polearm", "invisibility" }
    },

    {
	"lich basics",
	{ -1, -1, -1, -1, -1, -1, -1,
	  -1, -1, -1, -1, -1, -1, 0 },
	{ "dagger", "hide", "sneak", "feed" }
    },

    {
	"mage default",
	{ 40, -1, -1, -1, -1, -1, -1,
	  -1, -1, -1, -1, -1, -1, -1 },
	{
	 "lore",
	 "beguiling",
	 "combat",
	 "detection",
	 "enhancement",
	 "illusion",
	 "maladictions",
	 "protective",
	 "shielding",
	 "transportation",
	 "weather"
	}
    },

    {
	"cleric default",
	{ -1, 40, -1, -1, -1, -1, -1,
	  -1, -1, -1, -1, -1, -1, -1 },
	{
	 "flail",
	 "attack",
	 "benedictions", 
	 "creation",
	 "curative",
	 "detection",
	 "healing",
	 "maladictions",
	 "protective",
	 "shield block", 
	 "transportation",
	 "weather"
	}
    },
 
    {
	"thief default",
	{ -1, -1, 40, -1, -1, -1, -1,
	  -1, -1, -1, -1, -1, -1, -1 },
	{
	 "backstab",
	 "circle",
	 "disarm",
	 "dodge",
	 "hide",
	 "mace",
	 "peek",
	 "pick lock",
	 "sneak",
	 "sword",
	 "trip",
	 "second attack",
	 "charm person"
	}
    },

    {
	"warrior default",
	{ -1, -1, -1, 40, -1, -1, -1,
	  -1, -1, -1, -1, -1, -1, -1 },
	{
	 "weaponsmaster",
	 "bash",
	 "disarm",
	 "enhanced damage", 
	 "grip",
	 "parry",
	 "rescue",
	 "shield block",
	 "third attack",
	 "fourth attack"
	}
    },

    {
	"ranger default",
	{ -1, -1, -1, -1, 40, -1, -1,
	  -1, -1, -1, -1, -1, -1, -1 },
	{
	 "weaponsmaster",
	 "dirt kick",
	 "enhanced damage",
	 "envenom",
	 "hand to hand",
	 "kick",
	 "parry",
	 "shield block",
	 "third attack",
	 "curative",
	 "healing",
	 "transportation",
	 "earthquake"
	}
    },

    {
	"druid default",
	{ -1, -1, -1, -1, -1, 40, -1,
	  -1, -1, -1, -1, -1, -1, -1 },
	{
	 "lore",
	 "shield block",
	 "second attack",
	 "attack",
	 "benedictions",
	 "combat",
	 "creation",
	 "curative",
	 "healing",
	 "protective",
	 "weather",
	 "harm"
	}
    },

    {
	"vampire default",
	{ -1, -1, -1, -1, -1, -1, 40,
	  -1, -1, -1, -1, -1, -1, -1 },
	{
	 "backstab",
	 "disarm",
	 "dodge",
	 "fast healing",
	 "feed",
	 "hand to hand",
	 "shield block",
	 "beguiling",
	 "detection",
	 "enhancement",
	 "illusion",
	 "maladictions",
	 "transportation"
	}
    },

    {
	"wizard default",
	{ -1, -1, -1, -1, -1, -1, -1,
	  40, -1, -1, -1, -1, -1, -1 },
	{
	 "lore",
	 "beguiling",
	 "combat",
	 "detection",
	 "enhancement",
	 "illusion",
	 "maladictions",
	 "protective",
	 "shielding",
	 "transportation",
	 "weather",
	 "cure blindness"
	}
    },

    {
	"priest default",
	{ -1, -1, -1, -1, -1, -1, -1,
	  -1, 40, -1, -1, -1, -1, -1 },
	{
	 "flail",
	 "shield block", 
	 "attack",
	 "benedictions", 
	 "creation",
	 "curative",
	 "detection",
	 "healing",
	 "maladictions",
	 "protective",
	 "transportation",
	 "weather"
	}
    },
 
    {
	"mercenary default",
	{ -1, -1, -1, -1, -1, -1, -1,
	  -1, -1, 40, -1, -1, -1, -1 },
	{
	 "backstab",
	 "circle",
	 "disarm",
	 "dodge",
	 "dual wield",
	 "hide",
	 "mace",
	 "peek",
	 "pick lock",
	 "sneak",
	 "sword",
	 "trip",
	 "second attack",
	 "charm person"
	}
    },

    {
	"gladiator default",
	{ -1, -1, -1, -1, -1, -1, -1,
	  -1, -1, -1, 40, -1, -1, -1 },
	{
	 "weaponsmaster",
	 "bash",
	 "disarm",
	 "enhanced damage", 
	 "parry",
	 "rescue",
	 "shield block",
	 "third attack",
	 "fourth attack"
	}
    },

    {
	"strider default",
	{ -1, -1, -1, -1, -1, -1, -1,
	  -1, -1, -1, -1, 40, -1, -1 },
	{
	 "weaponsmaster",
	 "dual wield",
	 "enhanced damage",
	 "envenom",
	 "hand to hand",
	 "kick",
	 "parry",
	 "third attack",
	 "beguiling",
	 "curative",
	 "healing",
	 "protective",
	 "transportation"
	}
    },

    {
	"sage default",
	{ -1, -1, -1, -1, -1, -1, -1,
	  -1, -1, -1, -1, -1, 40, -1 },
	{
	 "lore",
	 "shield block",
	 "second attack",
	 "attack",
	 "benedictions",
	 "combat",
	 "creation",
	 "curative",
	 "healing",
	 "protective",
	 "weather"
	}
    },

    {
	"lich default",
	{ -1, -1, -1, -1, -1, -1, -1,
	  -1, -1, -1, -1, -1, -1, 40 },
	{
	 "backstab",
	 "detection",
	 "disarm",
	 "dodge",
	 "fast healing",
	 "hand to hand",
	 "shield block",
	 "beguiling",
	 "enhancement",
	 "illusion",
	 "maladictions",
	 "transportation"
	}
    },

    {
	"weaponsmaster",
	{ 40, 40, 40, 20, 30, 40, 30,
	  40, 40, 40, 20, 30, 40, 30 },
	{ "axe", "dagger", "flail", "mace", "polearm", "spear", "sword",
	  "whip" }
    },

    {
	"attack",
	{ 6, 5, -1, -1, -1, 6, 5,
	  5, 4, -1, -1, -1, 5, 4 },
	{ "demonfire", "dispel evil", "dispel good", "earthquake", 
	  "flamestrike", "heat metal", "ray of truth", "shockwave", "divine wrath" }
    },

    {
	"beguiling",
	{ 5, 4, -1, -1, 5, -1, 6,
	  4, 3, -1, -1, 4, -1, 5 },
	{ "animate", "calm", "charm person", "resurrect", "sleep" }
    },

    {
	"benedictions",
	{ 2, 4, -1, -1, 5, 4, 3,
	  1, 3, -1, -1, 4, 3, 2 },
	{ "bless", "calm", "frenzy", "holy word", "remove curse" }
    },

    {
	"combat",
	{ 5, 5, -1, -1, 4, 6, 5,
	  4, 4, -1, -1, 3, 5, 4 },
	{ "acid blast", "burning hands", "chain lightning", "chill touch",
	  "colour spray", "fireball", "lightning bolt", "magic missile",
	  "shocking grasp" }
    },

    {
	"creation",
	{ 4, 4, -1, -1, 7, 4, -1,
	  3, 3, -1, -1, 6, 3, -1 },
	{ "continual light", "create food", "create spring", "create water",
	  "create rose", "conjure", "empower", "floating disc" }
    },

    {
	"curative",
	{ -1, 4, -1, -1, 7, 5, -1,
	  -1, 3, -1, -1, 6, 4, -1 },
	{ "cure blindness", "cure disease", "cure poison" }
    }, 

    {
	"detection",
	{ 4, 3, 6, -1, 3, 6, 4,
	  3, 2, 5, -1, 2, 5, 3 },
 	{ "detect evil", "detect good", "detect hidden", "detect invis", 
	  "detect magic", "detect poison", "farsight", "identify", 
	  "know alignment", "locate object" } 
    },

    {
	"draconian",
	{ 8, 4, -1, -1, -1, -1, -1,
	  7, 3, -1, -1, -1, -1, -1 },
	{ "acid breath", "fire breath", "frost breath", "gas breath",
	  "lightning breath"  }
    },

    {
	"enchantment",
	{ 4, 7, -1, -1, -1, 3, -1,
	  3, 6, -1, -1, 3, 3, -1 },
	{ "enchant armor", "enchant weapon", "fireproof", "recharge" }
    },

    { 
	"enhancement",
	{ 5, 6, -1, -1, 4, 4, 6,
	  4, 5, -1, -1, 3, 3, 5 },
	{ "giant strength", "haste", "infravision", "refresh" }
    },

    {
	"harmful",
	{ 3, 4, -1, -1, -1, -1, 6,
	  2, 3, -1, -1, -1, 4, 5 },
	{ "cause critical", "cause light", "cause serious", "harm" }
    },

    {   
	"healing",
	{ -1, 3, -1, -1, 5, 4, -1,
	  -1, 1, -1, -1, 5, 3, -1 },
 	{ "cure critical", "cure light", "cure serious", "heal", 
	  "mass healing" }
    },

    {
	"illusion",
	{ 4, 6, 7, -1, 7, -1, 4,
	  3, 5, 6, -1, 6, -1, 3 },
	{ "invis", "mass invis", "ventriloquate" }
    },
  
    {
	"maladictions",
	{ 5, 5, -1, -1, 6, 8, 5,
	  4, 4, -1, -1, 5, 7, 4 },
	{ "blindness", "change sex", "curse", "energy drain", "plague", 
	  "poison", "slow", "voodoo", "weaken" }
    },

    { 
	"protective",
	{ 4, 4, -1, -1, 7, 5, 7,
	  3, 3, -1, -1, 6, 4, 6 },
	{ "armor", "cancellation", "dispel magic", "fireproof",
	  "protection evil", "protection good", "sanctuary", "shield", 
	  "stone skin" }
    },

    {
	"shielding",
	{ 8, 8, -1, -1, -1, 8, -1,
	  6, 7, -1, -1, -1, 7, -1 },
	{ "iceshield", "fireshield", "shockshield", "vacillating aura", "manashield" }
    },

    {
	"transportation",
	{ 4, 4, -1, -1, 5, 7, 4,
	  3, 3, -1, -1, 4, 6, 3 },
	{ "fly", "gate", "nexus", "pass door", "portal", "summon", "teleport", 
	  "transport", "word of recall" }
    },
   
    {
	"weather",
	{ 4, 4, -1, -1, 5, 5, 7,
	  3, 3, -1, -1, 4, 4, 6 },
	{ "call lightning", "control weather", "faerie fire", "faerie fog",
	  "lightning bolt" }
    }

};


const	struct	lang_type	language_table	[MAX_LANGUAGE]	=
{
    {
	"common", "{Wcommon{x",
    },
    {
	"elvish", "{Welvish{x"
    },
    {
	"dwarvish", "{Wdwarvish{x"
    },
    {
	"orcish", "{Worcish{x"
    }
};
