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
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"

/* command procedures needed */
DECLARE_DO_FUN(	do_exits	);
DECLARE_DO_FUN( do_look		);
DECLARE_DO_FUN( do_help		);
DECLARE_DO_FUN( do_affects	);
DECLARE_DO_FUN( do_play		);
DECLARE_DO_FUN(	do_inventory	);
DECLARE_DO_FUN(	do_titlelock	);

void show_obj_description(OBJ_DATA *obj, CHAR_DATA *ch);
int total_unread_notes(CHAR_DATA *ch);

#define WHO_TITLE_WIDTH 43

bool buf_who_list args((CHAR_DATA *ch, char *argument, BUFFER *buffer,
                  bool lswivel, bool immso, int *counter));

char *	const	where_name	[] =
{
    "{G<{Bused as light{G>{x     ",
    "{G<{Bworn on finger{G>{x    ",
    "{G<{Bworn on finger{G>{x    ",
    "{G<{Bworn around neck{G>{x  ",
    "{G<{Bworn around neck{G>{x  ",
    "{G<{Bworn on torso{G>{x     ",
    "{G<{Bworn on head{G>{x      ",
    "{G<{Bworn on legs{G>{x      ",
    "{G<{Bworn on feet{G>{x      ",
    "{G<{Bworn on hands{G>{x     ",
    "{G<{Bworn on arms{G>{x      ",
    "{G<{Bworn as shield{G>{x    ",
    "{G<{Bworn about body{G>{x   ",
    "{G<{Bworn about waist{G>{x  ",
    "{G<{Bworn around wrist{G>{x ",
    "{G<{Bworn around wrist{G>{x ",
    "{G<{Bprimary wield{G>{x     ",
    "{G<{Bheld{G>{x              ",
    "{G<{Bfloating nearby{G>{x   ",
    "{G<{Bsecondary wield{G>{x   ",
    "{G<{Bworn on face{G>{x      "
};

sh_int const  where_order	[] =
{
	 1,  2,  3,  4,  5,
	 6, 20,  7,  8,  9,
	10, 11, 12, 13, 14,
	15, 16, 19, 17, 18,
	0
};

/*
char *	const	where_name	[] =
{
    "{G<{Cworn on head{G>{x      ",
    "{G<{Cworn around neck{G>{x  ",
    "{G<{Cworn around neck{G>{x  ",
    "{G<{Cworn on face{G>{x      "
    "{G<{Cworn on arms{G>{x      ",
    "{G<{Cworn around wrist{G>{x ",
    "{G<{Cworn around wrist{G>{x ",
    "{G<{Cworn on hands{G>{x     ",
    "{G<{Cworn on finger{G>{x    ",
    "{G<{Cworn on finger{G>{x    ",
    "{G<{Cworn as shield{G>{x    ",
    "{G<{Cprimary wield{G>{x     ",
    "{G<{Csecondary wield{G>{x   ",
    "{G<{Cheld{G>{x              ",
    "{G<{Cworn on torso{G>{x     ",
    "{G<{Cworn about body{G>{x   ",
    "{G<{Cworn about waist{G>{x  ",
    "{G<{Cworn on legs{G>{x      ",
    "{G<{Cworn on feet{G>{x      ",
    "{G<{Cfloating nearby{G>{x   ",
    "{G<{Cused as light{G>{x     ",
};

sh_int const  where_order	[] =
{
	 1,  2,  3,  4,  5,
	 6, 20,  7,  8,  9,
	10, 11, 12, 13, 14,
	15, 16, 19, 17, 18,
	0
};
*/

/* for do_count */
int max_on = 0;
int max_on_bootup = 0;
bool is_pm = FALSE;


/*
 * Local functions.
 */
char *	format_obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch,
				    bool fShort ) );
BUFFER * show_list_to_char	args( ( OBJ_DATA *list, CHAR_DATA *ch,
				    bool fShort, bool fShowNothing ) );
void	show_char_to_char_0	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_spells_to_char	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char_1	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char	args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
bool	check_blind		args( ( CHAR_DATA *ch ) );



char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if ((fShort && (obj->short_descr == NULL || obj->short_descr[0] == '\0'))
    ||  (obj->description == NULL || obj->description[0] == '\0'))
	return buf;

    if (!IS_SET(ch->comm, COMM_LONG) )
    {
	strcat( buf, "{x[{y.{R.{B.{M.{Y.{W.{G.{x]");
	if ( IS_OBJ_STAT(obj, ITEM_INVIS)	)   buf[5] = 'V';
	if ( IS_AFFECTED(ch, AFF_DETECT_EVIL)
	&& IS_OBJ_STAT(obj, ITEM_EVIL)		)   buf[8] = 'E';
	if (IS_AFFECTED(ch, AFF_DETECT_GOOD)
	&&  IS_OBJ_STAT(obj,ITEM_BLESS)		)   buf[11] = 'B';
	if ( IS_AFFECTED(ch, AFF_DETECT_MAGIC)
	&& IS_OBJ_STAT(obj, ITEM_MAGIC)		)   buf[14] = 'M';
	if ( IS_OBJ_STAT(obj, ITEM_GLOW)	)   buf[17] = 'G';
	if ( IS_OBJ_STAT(obj, ITEM_HUM)		)   buf[20] = 'H';
	if ( IS_OBJ_STAT(obj, ITEM_QUEST)	)   buf[23] = 'Q';
	if (!strcmp(buf, "{x[{y.{R.{B.{M.{Y.{W.{G.{x]") )
	    buf[0] = '\0';
    }
    else
    {
	if ( IS_OBJ_STAT(obj, ITEM_INVIS)	)   strcat(buf, "({yInvis{x)");
	if ( IS_OBJ_STAT(obj, ITEM_DARK)	)   strcat(buf, "({DHidden{x)");
	if ( IS_AFFECTED(ch, AFF_DETECT_EVIL)
	&& IS_OBJ_STAT(obj, ITEM_EVIL)		)   strcat(buf, "({RRed Aura{x)");
	if (IS_AFFECTED(ch, AFF_DETECT_GOOD)
	&&  IS_OBJ_STAT(obj,ITEM_BLESS)		)   strcat(buf,"({BBlue Aura{x)");
	if ( IS_AFFECTED(ch, AFF_DETECT_MAGIC)
	&& IS_OBJ_STAT(obj, ITEM_MAGIC)		)   strcat(buf, "({yMagical{x)");
	if ( IS_OBJ_STAT(obj, ITEM_GLOW)	)   strcat(buf, "({YGlowing{x)");
	if ( IS_OBJ_STAT(obj, ITEM_HUM)		)   strcat(buf, "({yHumming{x)");
	if ( IS_OBJ_STAT(obj, ITEM_QUEST)	)   strcat(buf, "({GQuest{x)");
    }

    if (buf[0] != '\0')
    {
	strcat(buf, " ");
    }

    if ( fShort )
    {
	if ( obj->short_descr != NULL )
	    strcat( buf, obj->short_descr );
    }
    else
    {
	if ( obj->description != NULL)
	    strcat( buf, obj->description );
    }
    if (strlen(buf)<=0)
	strcat(buf,"This object has no description. Please inform the IMP.");

    return buf;
}



/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
BUFFER *show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    char **prgpstrShow;
    int *prgnShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count;
    bool fCombine;

    /*
     * Alloc space for output lines.
     */
    output = new_buf();
    count = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
	count++;
    prgpstrShow	= alloc_mem( count * sizeof(char *) );
    prgnShow    = alloc_mem( count * sizeof(int)    );
    nShow	= 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj != NULL; obj = obj->next_content )
    { 
	if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj )) 
	{
	    pstrShow = format_obj_to_char( obj, ch, fShort );

	    fCombine = FALSE;

	    if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
	    {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		{
		    if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
		    {
			prgnShow[iShow]++;
			fCombine = TRUE;
			break;
		    }
		}
	    }

	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if ( !fCombine )
	    {
		prgpstrShow [nShow] = str_dup( pstrShow );
		prgnShow    [nShow] = 1;
		nShow++;
	    }
	}
    }

    /*
     * Output the formatted list.
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
	if (prgpstrShow[iShow][0] == '\0')
	{
	    free_string(prgpstrShow[iShow]);
	    continue;
	}

	if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
	{
	    if ( prgnShow[iShow] != 1 )
	    {
		sprintf( buf, "(%2d) ", prgnShow[iShow] );
		add_buf(output,buf);
	    }
	    else
	    {
		add_buf(output,"     ");
	    }
	}
	add_buf(output,prgpstrShow[iShow]);
	add_buf(output,"\n\r");
	free_string( prgpstrShow[iShow] );
    }

    if ( fShowNothing && nShow == 0 )
    {
	if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
	    send_to_char( "     ", ch );
	send_to_char( "Nothing.\n\r", ch );
    }
    /*
     * Clean up.
     */
    free_mem( prgpstrShow, count * sizeof(char *) );
    free_mem( prgnShow,    count * sizeof(int)    );

    return output;
}



void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH],message[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if (!IS_SET(ch->comm, COMM_LONG) )
    {
	strcat( buf, "{x[{y.{D.{c.{b.{w.{C.{r.{B.{W.{R.{Y.{W.{B.{G.{x]");
	if ( IS_SHIELDED(victim, SHD_INVISIBLE)   ) buf[5] = 'V';
	if ( IS_AFFECTED(victim, AFF_HIDE)        ) buf[8] = 'H';
	if ( IS_AFFECTED(victim, AFF_CHARM)       ) buf[11] = 'C';
	if ( IS_AFFECTED(victim, AFF_PASS_DOOR)   ) buf[14] = 'T';
	if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) ) buf[17] = 'P';
	if ( IS_SHIELDED(victim, SHD_ICE)  	      ) buf[20] = 'I';
	if ( IS_SHIELDED(victim, SHD_FIRE)        ) buf[23] = 'F';
	if ( IS_SHIELDED(victim, SHD_SHOCK)       ) buf[26] = 'L';
        if ( IS_SHIELDED(victim, SHD_MANA)       ) buf[29] = 'M';
	if ( IS_EVIL(victim)
	&& IS_AFFECTED(ch, AFF_DETECT_EVIL)     ) buf[32] = 'E';
	if ( IS_GOOD(victim)
	&&   IS_AFFECTED(ch, AFF_DETECT_GOOD)     ) buf[35] = 'G';
	if ( IS_SHIELDED(victim, SHD_SANCTUARY)   ) buf[38] = 'S';
        if ( IS_SHIELDED(victim, SHD_VACILLATING) ) buf[41] = 'V';
	if ( victim->on_quest)			    buf[44] = 'Q';
	if (!strcmp(buf, "{x[{y.{D.{c.{b.{w.{C.{r.{B.{W.{R.{Y.{W.{B.{G.{x]") )
	    buf[0] = '\0';
	if ( IS_SET(victim->comm,COMM_AFK  )      ) strcat( buf, "[{yAFK{x]");
	if ( !IS_NPC(victim) && !victim->desc ) strcat( buf, "[{yLinkless{x]");
	if ( victim->invis_level >= LEVEL_HERO    ) strcat( buf, "({WWizi{x)");
    }
    else
    {
	if ( IS_SET(victim->comm,COMM_AFK  )      ) strcat( buf, "[{yAFK{x]");
	if ( IS_SHIELDED(victim, SHD_INVISIBLE)   ) strcat( buf, "({yInvis{x)");
	if ( victim->invis_level >= LEVEL_HERO    ) strcat( buf, "({WWizi{x)");
	if ( IS_AFFECTED(victim, AFF_HIDE)        ) strcat( buf, "({DHide{x)");
	if ( !IS_NPC(victim) && !victim->desc ) strcat( buf, "[{yLinkless{x]");

/*
	if ( IS_AFFECTED(victim, AFF_CHARM)       ) strcat( buf, "({cCharmed{x)");
	if ( IS_AFFECTED(victim, AFF_PASS_DOOR)   ) strcat( buf, "({bTranslucent{x)");
	if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) ) strcat( buf, "({wPink Aura{x)");
	if ( IS_SHIELDED(victim, SHD_ICE)         ) strcat( buf, "({DGrey Aura{x)");
	if ( IS_SHIELDED(victim, SHD_FIRE)        ) strcat( buf, "({rOrange Aura{x)");
	if ( IS_SHIELDED(victim, SHD_SHOCK)       ) strcat( buf, "({BBlue Aura{x)");
	if ( IS_EVIL(victim)
	&&   IS_AFFECTED(ch, AFF_DETECT_EVIL)     ) strcat( buf, "({RRed Aura{x)");
	if ( IS_GOOD(victim)
	&&   IS_AFFECTED(ch, AFF_DETECT_GOOD)     ) strcat( buf, "({YGolden Aura{x)");
	if ( IS_SHIELDED(victim, SHD_SANCTUARY)   ) strcat( buf, "({WWhite Aura{x)");
*/
	if ( victim->on_quest)			    strcat( buf, "({GQuest{x)");
    }
    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_TWIT ) )
						strcat( buf, "({rTWIT{x)");

    if ( IS_NPC(victim) && !IS_NPC(ch) && victim->pIndexData->vnum ==
         ch->pcdata->questmob )
	strcat (buf, "{B[{RT{WA{RR{WG{RET{B]{x");

    if ( victim->in_arena )
    {
        strcat(buf,"[");
        strcat(buf,arena.teams[victim->arena_team].name);
        strcat(buf,"]");
    }

    if (buf[0] != '\0')
    {
	strcat( buf, " " );
    }
    if ( victim->position == victim->start_pos && victim->long_descr[0] != '\0' )
    {
	strcat( buf, victim->long_descr );
	send_to_char( buf, ch );
        show_spells_to_char(victim, ch);
	return;
    }

    strcat( buf, PERS( victim, ch ) );
    if ( !IS_NPC(victim) && !IS_SET(ch->comm, COMM_BRIEF) 
    &&   victim->position == POS_STANDING && ch->on == NULL )
	strcat( buf, victim->pcdata->title );

	    if(is_clan(victim))
	    {
		sprintf(message,", %s of %s,",
		    clan_table[victim->clan].rank[victim->rank].rankname,clan_table[victim->clan].who_name);
		strcat(buf,message);
	    }

    switch ( victim->position )
    {
    case POS_DEAD:     strcat( buf, " is DEAD!!" );              break;
    case POS_MORTAL:   strcat( buf, " is mortally wounded." );   break;
    case POS_INCAP:    strcat( buf, " is incapacitated." );      break;
    case POS_STUNNED:  strcat( buf, " is lying here stunned." ); break;
    case POS_SLEEPING: 
	if (victim->on != NULL)
	{
	    if (IS_SET(victim->on->value[2],SLEEP_AT))
  	    {
		sprintf(message," is sleeping at %s.",
		    victim->on->short_descr);
		strcat(buf,message);
	    }
	    else if (IS_SET(victim->on->value[2],SLEEP_ON))
	    {
		sprintf(message," is sleeping on %s.",
		    victim->on->short_descr); 
		strcat(buf,message);
	    }
	    else
	    {
		sprintf(message, " is sleeping in %s.",
		    victim->on->short_descr);
		strcat(buf,message);
	    }
	}
	else 
	    strcat(buf," is sleeping here.");
	break;
    case POS_RESTING:  
        if (victim->on != NULL)
	{
            if (IS_SET(victim->on->value[2],REST_AT))
            {
                sprintf(message," is resting at %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else if (IS_SET(victim->on->value[2],REST_ON))
            {
                sprintf(message," is resting on %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else 
            {
                sprintf(message, " is resting in %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
	}
        else
	    strcat( buf, " is resting here." );       
	break;
    case POS_SITTING:  
        if (victim->on != NULL)
        {
            if (IS_SET(victim->on->value[2],SIT_AT))
            {
                sprintf(message," is sitting at %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else if (IS_SET(victim->on->value[2],SIT_ON))
            {
                sprintf(message," is sitting on %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else
            {
                sprintf(message, " is sitting in %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
        }
        else
	    strcat(buf, " is sitting here.");
	break;
    case POS_STANDING: 
	if (victim->on != NULL)
	{
	    if (IS_SET(victim->on->value[2],STAND_AT))
	    {
		sprintf(message," is standing at %s.",
		    victim->on->short_descr);
		strcat(buf,message);
	    }
	    else if (IS_SET(victim->on->value[2],STAND_ON))
	    {
		sprintf(message," is standing on %s.",
		   victim->on->short_descr);
		strcat(buf,message);
	    }
	    else
	    {
		sprintf(message," is standing in %s.",
		    victim->on->short_descr);
		strcat(buf,message);
	    }
	}
	else
	    strcat( buf, " is here." );               
	break;
    case POS_FIGHTING:
	strcat( buf, " is here, fighting " );
	if ( victim->fighting == NULL )
	    strcat( buf, "thin air??" );
	else if ( victim->fighting == ch )
	    strcat( buf, "YOU!" );
	else if ( victim->in_room == victim->fighting->in_room )
	{
	    strcat( buf, PERS( victim->fighting, ch ) );
	    strcat( buf, "." );
	}
	else
	    strcat( buf, "someone who left??" );
	break;
    }

    strcat( buf, "\n\r" );
    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );

    show_spells_to_char(victim, ch);

    return;
}



void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    BUFFER *output;
    BUFFER *outlist;
    int iWear;
    int oWear;
    int percent;
    bool found;

    if ( can_see( victim, ch )
    && get_trust(victim) >= ch->ghost_level)
    {
	if (ch == victim)
	    act_new_2( "$n looks at $mself.",ch,NULL,NULL,TO_ROOM, POS_RESTING, 0, ACTION_MUST_SEE);
	else
	{
	    act_new_2( "$n looks at you.", ch, NULL, victim, TO_VICT, POS_RESTING, 0, ACTION_MUST_SEE );
	    act_new_2( "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT, POS_RESTING, 0, ACTION_MUST_SEE );
	}
    }

    output = new_buf();
    if ( victim->description[0] != '\0' )
    {
	sprintf( buf, "{C%s{x", victim->description);
    }
    else
    {
	sprintf( buf, "{CYou see nothing special about %s{x\n\r", victim->name);
    }

    add_buf(output,buf);

	if( is_clan(victim) )
	{
		sprintf( buf, "%s is the %s of %s\n\r", victim->name,clan_table[victim->clan].rank[victim->rank].rankname,clan_table[victim->clan].who_name );
    add_buf(output,buf);
	}

    if ( victim->max_hit > 0 )
	percent = ( 100 * victim->hit ) / victim->max_hit;
    else
	percent = -1;

    buf[0] = '\0';
    strcpy( buf, PERS(victim, ch) );

    if(IS_NPC(victim) && IS_BUILDER(ch, victim->in_room->area))
    {
        char buf1[MAX_STRING_LENGTH];
        sprintf(buf1, " [Mob %d]", victim->pIndexData->vnum);
        strcat(buf, buf1);
    }

    if (percent >= 100) 
	strcat( buf, " {fis in excellent condition.{x\n\r");
    else if (percent >= 90) 
	strcat( buf, " {fhas a few scratches.{x\n\r");
    else if (percent >= 75) 
	strcat( buf," {fhas some small wounds and bruises.{x\n\r");
    else if (percent >=  50) 
	strcat( buf, " {fhas quite a few wounds.{x\n\r");
    else if (percent >= 30)
	strcat( buf, " {fhas some big nasty wounds and scratches.{x\n\r");
    else if (percent >= 15)
	strcat ( buf, " {flooks pretty hurt.{x\n\r");
    else if (percent >= 0 )
	strcat (buf, " {fis in awful condition.{x\n\r");
    else
	strcat(buf, " {fis bleeding to death.{x\n\r");

    buf[0] = UPPER(buf[0]);
    add_buf(output,buf);

    if ( IS_SHIELDED(victim, SHD_ICE))
    {
	sprintf( buf, "%s is surrounded by an {Cicy{x shield.\n\r", PERS(victim, ch));
	buf[0] = UPPER(buf[0]);
	add_buf(output,buf);
    }
    if ( IS_SHIELDED(victim, SHD_FIRE))
    {
	sprintf( buf, "%s is surrounded by a {Rfiery{x shield.\n\r", PERS(victim, ch));
	buf[0] = UPPER(buf[0]);
	add_buf(output,buf);
    }
    if ( IS_SHIELDED(victim, SHD_SHOCK))
    {
	sprintf( buf, "%s is surrounded by a {Bcrackling{x shield.\n\r", PERS(victim, ch));
	buf[0] = UPPER(buf[0]);
	add_buf(output,buf);
    }
    if ( IS_SHIELDED(victim, SHD_VACILLATING))
    {
        sprintf( buf, "%s is surrounded by a {Bfluctuating{x aura.\n\r", PERS(victim, ch));
        buf[0] = UPPER(buf[0]);
        add_buf(output,buf);
    }
    if ( IS_SHIELDED(victim, SHD_MANA))
    {
        sprintf( buf, "%s is surrounded by a shield of {Wmana{x.\n\r", PERS(victim, ch));
        buf[0] = UPPER(buf[0]);
        add_buf(output,buf);
    }

    found = FALSE;
    for ( oWear = 0; oWear < MAX_WEAR; oWear++ )
    {
	iWear = where_order[oWear];
	if ( ( obj = get_eq_char( victim, iWear ) ) != NULL
	&&   can_see_obj( ch, obj ) )
	{
	    if ( !found )
	    {
		sprintf( buf, "\n\r");
		add_buf(output,buf);
		sprintf( buf, "{G%s is using:{x\n\r", victim->name );
		add_buf(output,buf);
		found = TRUE;
	    }
	    sprintf( buf, "%s%s\n\r", where_name[iWear], format_obj_to_char( obj, ch, TRUE ));
	    add_buf(output,buf);
	}
    }

    if ( victim != ch
    &&   !IS_NPC(ch)
    &&	 !IS_IMMORTAL(victim)
    &&   number_percent( ) < get_skill(ch,gsn_peek)
    &&   IS_SET(ch->act,PLR_AUTOPEEK))
    {
	sprintf( buf, "\n\r{GYou peek at the inventory:{x\n\r");
	add_buf(output,buf);
	check_improve(ch,gsn_peek,TRUE,4);
	outlist = show_list_to_char( victim->carrying, ch, TRUE, TRUE );
	add_buf(output,buf_string(outlist));
	free_buf(outlist);
    }
    page_to_char( buf_string(output), ch );
    free_buf(output);
    return;
}



void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;

    for ( rch = list; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch == ch )
	    continue;

	if ( get_trust(ch) < rch->invis_level)
	    continue;

	if ( get_trust(ch) < rch->ghost_level)
	    continue;

	if ( can_see( ch, rch ) )
	{
	    show_char_to_char_0( rch, ch );
	}
	else if ( room_is_dark( ch->in_room )
	&&        IS_AFFECTED(rch, AFF_INFRARED ) )
	{
	    send_to_char( "You see {Rglowing red{x eyes watching YOU!\n\r", ch );
	}
    }

    return;
} 

void do_peek( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    BUFFER *outlist;
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if (IS_NPC(ch))
	return;

    if (arg[0] == '\0')
    {
	send_to_char("Peek at who?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }

    if (victim == ch)
    {
	do_inventory(ch,"");
	return;
    }

    if ( can_see( victim, ch )
    && get_trust(victim) >= ch->ghost_level)
    {
	act_new_2( "$n peers intently at you.", ch, NULL, victim, TO_VICT, POS_RESTING, 0, ACTION_MUST_SEE ); 
	act_new_2( "$n peers intently at $N.",  ch, NULL, victim, TO_NOTVICT, POS_RESTING, 0, ACTION_MUST_SEE ); 

    }

    output = new_buf();

    if (number_percent( ) < get_skill(ch,gsn_peek))
    {
        sprintf( buf, "\n\r{GYou peek at the inventory:{x\n\r");
        add_buf(output,buf);
        check_improve(ch,gsn_peek,TRUE,4);
        outlist = show_list_to_char( victim->carrying, ch, TRUE, TRUE );
        add_buf(output,buf_string(outlist));
        free_buf(outlist);
    } else {
	sprintf( buf, "{RYou fail to see anything.{x\n\r");
	add_buf(output,buf);
	check_improve(ch,gsn_peek,FALSE,2);
    }
    page_to_char( buf_string(output), ch );
    free_buf(output);
    return;
}

bool check_blind( CHAR_DATA *ch )
{

    if (!IS_NPC(ch) && IS_SET(ch->act,PLR_HOLYLIGHT))
	return TRUE;

    if ( IS_AFFECTED(ch, AFF_BLIND) )
    { 
	send_to_char( "You can't see a thing!\n\r", ch ); 
	return FALSE; 
    }

    return TRUE;
}

/* changes your scroll */
void do_scroll(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[100];
    int lines;

    one_argument(argument,arg);
    
    if (arg[0] == '\0')
    {
	if (ch->lines == 0)
	    send_to_char("You do not page long messages.\n\r",ch);
	else
	{
	    sprintf(buf,"You currently display %d lines per page.\n\r",
		    ch->lines + 2);
	    send_to_char(buf,ch);
	}
	return;
    }

    if (!is_number(arg))
    {
	send_to_char("You must provide a number.\n\r",ch);
	return;
    }

    lines = atoi(arg);

    if (lines == 0)
    {
        send_to_char("Paging disabled.\n\r",ch);
        ch->lines = 0;
        return;
    }

    if (lines < 10 || lines > 100)
    {
	send_to_char("You must provide a reasonable number.\n\r",ch);
	return;
    }

    sprintf(buf,"Scroll set to %d lines.\n\r",lines);
    send_to_char(buf,ch);
    ch->lines = lines - 2;
}

/* RT does socials */
void do_socials(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col;
     
    col = 0;
   
    for (iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++)
    {
	sprintf(buf,"%-12s",social_table[iSocial].name);
	send_to_char(buf,ch);
	if (++col % 6 == 0)
	    send_to_char("\n\r",ch);
    }

    if ( col % 6 != 0)
	send_to_char("\n\r",ch);
    return;
}


 
/* RT Commands to replace news, motd, imotd, etc from ROM */

void do_motd(CHAR_DATA *ch, char *argument)
{
    do_help(ch,"motd");
}

void do_imotd(CHAR_DATA *ch, char *argument)
{  
    do_help(ch,"imotd");
}

void do_rules(CHAR_DATA *ch, char *argument)
{
    do_help(ch,"rules");
}

void do_story(CHAR_DATA *ch, char *argument)
{
    do_help(ch,"story");
}

/* RT this following section holds all the auto commands from ROM, as well as
   replacements for config */

void do_autolist(CHAR_DATA *ch, char *argument)
{
    /* lists most player flags */
    if (IS_NPC(ch))
      return;

    send_to_char("   {Caction     {Cstatus{x\n\r",ch);
    send_to_char("{c---------------------{x\n\r",ch);
 
    send_to_char("{Cautoassist{x     ",ch);
    if (IS_SET(ch->act,PLR_AUTOASSIST))
        send_to_char("{GON{x\n\r",ch);
    else
        send_to_char("{ROFF{x\n\r",ch); 

    send_to_char("{Cautoexit{x       ",ch);
    if (IS_SET(ch->act,PLR_AUTOEXIT))
        send_to_char("{GON{x\n\r",ch);
    else
        send_to_char("{ROFF{x\n\r",ch);

    send_to_char("{Cautogold{x       ",ch);
    if (IS_SET(ch->act,PLR_AUTOGOLD))
        send_to_char("{GON{x\n\r",ch);
    else
        send_to_char("{ROFF{x\n\r",ch);

    send_to_char("{Cautoloot{x       ",ch);
    if (IS_SET(ch->act,PLR_AUTOLOOT))
        send_to_char("{GON{x\n\r",ch);
    else
        send_to_char("{ROFF{x\n\r",ch);

    send_to_char("{Cautosac{x        ",ch);
    if (IS_SET(ch->act,PLR_AUTOSAC))
        send_to_char("{GON{x\n\r",ch);
    else
        send_to_char("{ROFF{x\n\r",ch);

    send_to_char("{Cautosplit{x      ",ch);
    if (IS_SET(ch->act,PLR_AUTOSPLIT))
        send_to_char("{GON{x\n\r",ch);
    else
        send_to_char("{ROFF{x\n\r",ch);

    send_to_char("{Cautopeek{x       ",ch);
    if (IS_SET(ch->act,PLR_AUTOPEEK))
	send_to_char("{GON{x\n\r",ch);
    else
	send_to_char("{ROFF{x\n\r",ch);

    send_to_char("{Cautostore{x      ",ch);
    if (IS_SET(ch->comm,COMM_STORE))
	send_to_char("{GON{x\n\r",ch);
    else
	send_to_char("{ROFF{x\n\r",ch);

    send_to_char("{Ccompact mode{x   ",ch);
    if (IS_SET(ch->comm,COMM_COMPACT))
        send_to_char("{GON{x\n\r",ch);
    else
        send_to_char("{ROFF{x\n\r",ch);

    send_to_char("{Cprompt{x         ",ch);
    if (IS_SET(ch->comm,COMM_PROMPT))
	send_to_char("{GON{x\n\r",ch);
    else
	send_to_char("{ROFF{x\n\r",ch);

    send_to_char("{Ccombine items{x  ",ch);
    if (IS_SET(ch->comm,COMM_COMBINE))
	send_to_char("{GON{x\n\r",ch);
    else
	send_to_char("{ROFF{x\n\r",ch);

    send_to_char("{Clong flags{x     ",ch);
    if (IS_SET(ch->comm,COMM_LONG))
	send_to_char("{GON{x\n\r",ch);
    else
	send_to_char("{ROFF{x\n\r",ch);

    if (!IS_SET(ch->act,PLR_CANLOOT))
	send_to_char("{GYour corpse is safe from thieves.{x\n\r",ch);
    else 
        send_to_char("{RYour corpse may be looted.{x\n\r",ch);

    if (IS_SET(ch->act,PLR_NOSUMMON))
	send_to_char("{RYou cannot be summoned{x.\n\r",ch);
    else
	send_to_char("{GYou can be summoned{x.\n\r",ch);
   
    if (IS_SET(ch->act,PLR_NOFOLLOW))
	send_to_char("{RYou do not welcome followers.{x\n\r",ch);
    else
	send_to_char("{GYou accept followers.{x\n\r",ch);
}

void do_autoassist(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
    
    if (IS_SET(ch->act,PLR_AUTOASSIST))
    {
      send_to_char("Autoassist removed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOASSIST);
    }
    else
    {
      send_to_char("You will now assist when needed.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOASSIST);
    }
}

void do_autoexit(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOEXIT))
    {
      send_to_char("Exits will no longer be displayed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOEXIT);
    }
    else
    {
      send_to_char("Exits will now be displayed.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOEXIT);
    }
}

void do_autogold(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOGOLD))
    {
      send_to_char("Autogold removed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOGOLD);
    }
    else
    {
      send_to_char("Automatic gold looting set.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOGOLD);
    }
}

void do_autoloot(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOLOOT))
    {
      send_to_char("Autolooting removed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOLOOT);
    }
    else
    {
      send_to_char("Automatic corpse looting set.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOLOOT);
    }
}

void do_autosac(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOSAC))
    {
      send_to_char("Autosacrificing removed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOSAC);
    }
    else
    {
      send_to_char("Automatic corpse sacrificing set.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOSAC);
    }
}

void do_autosplit(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOSPLIT))
    {
      send_to_char("Autosplitting removed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOSPLIT);
    }
    else
    {
      send_to_char("Automatic gold splitting set.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOSPLIT);
    }
}

void do_autopeek(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_AUTOPEEK))
    {
      send_to_char("Autopeek removed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOPEEK);
    }
    else
    {
      send_to_char("Automatic peek set.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOPEEK);
    }
}

void do_brief(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_BRIEF))
    {
      send_to_char("Full descriptions activated.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_BRIEF);
    }
    else
    {
      send_to_char("Short descriptions activated.\n\r",ch);
      SET_BIT(ch->comm,COMM_BRIEF);
    }
}

void do_compact(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_COMPACT))
    {
      send_to_char("Compact mode removed.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_COMPACT);
    }
    else
    {
      send_to_char("Compact mode set.\n\r",ch);
      SET_BIT(ch->comm,COMM_COMPACT);
    }
}

void do_long(CHAR_DATA *ch, char *argument)
{
    if (!IS_SET(ch->comm,COMM_LONG))
    {
      send_to_char("Long flags activated.\n\r",ch);
      SET_BIT(ch->comm,COMM_LONG);
    }
    else
    {
      send_to_char("Short flags activated.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_LONG);
    }
}

void do_show(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_SHOW_AFFECTS))
    {
      send_to_char("Affects will no longer be shown in score.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_SHOW_AFFECTS);
    }
    else
    {
      send_to_char("Affects will now be shown in score.\n\r",ch);
      SET_BIT(ch->comm,COMM_SHOW_AFFECTS);
    }
}

void do_prompt(CHAR_DATA *ch, char *argument)
{
   char buf[MAX_STRING_LENGTH];
 
   if ( argument[0] == '\0' )
   {
	if (IS_SET(ch->comm,COMM_PROMPT))
   	{
      	    send_to_char("You will no longer see prompts.\n\r",ch);
      	    REMOVE_BIT(ch->comm,COMM_PROMPT);
    	}
    	else
    	{
      	    send_to_char("You will now see prompts.\n\r",ch);
      	    SET_BIT(ch->comm,COMM_PROMPT);
    	}
       return;
   }
 
   if( !strcmp( argument, "all" ) )
      strcpy( buf, "<%hhp %mm %vmv> ");
   else
   {
      if ( strlen(argument) > 65 )
         argument[65] = '\0';
      strcpy( buf, argument );
      smash_tilde( buf );
      if (str_suffix("%c",buf))
	strcat(buf,"{x ");
	
   }
 
   free_string( ch->prompt );
   ch->prompt = str_dup( buf );
   sprintf(buf,"Prompt set to %s\n\r",ch->prompt );
   send_to_char(buf,ch);
   return;
}

void do_combine(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_COMBINE))
    {
      send_to_char("Long inventory selected.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_COMBINE);
    }
    else
    {
      send_to_char("Combined inventory selected.\n\r",ch);
      SET_BIT(ch->comm,COMM_COMBINE);
    }
}

void do_noloot(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_CANLOOT))
    {
      send_to_char("Your corpse is now safe from thieves.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_CANLOOT);
    }
    else
    {
      send_to_char("Your corpse may now be looted.\n\r",ch);
      SET_BIT(ch->act,PLR_CANLOOT);
    }
}

void do_nofollow(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;
 
    if (IS_SET(ch->act,PLR_NOFOLLOW))
    {
      send_to_char("You now accept followers.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_NOFOLLOW);
    }
    else
    {
      send_to_char("You no longer accept followers.\n\r",ch);
      SET_BIT(ch->act,PLR_NOFOLLOW);
      die_follower( ch );
    }
}

void do_nosummon(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
    {
      if (IS_SET(ch->imm_flags,IMM_SUMMON))
      {
	send_to_char("You are no longer immune to summon.\n\r",ch);
	REMOVE_BIT(ch->imm_flags,IMM_SUMMON);
      }
      else
      {
	send_to_char("You are now immune to summoning.\n\r",ch);
	SET_BIT(ch->imm_flags,IMM_SUMMON);
      }
    }
    else
    {
      if (IS_SET(ch->act,PLR_NOSUMMON))
      {
        send_to_char("You are no longer immune to summon.\n\r",ch);
        REMOVE_BIT(ch->act,PLR_NOSUMMON);
      }
      else
      {
        send_to_char("You are now immune to summoning.\n\r",ch);
        SET_BIT(ch->act,PLR_NOSUMMON);
      }
    }
}

void do_notran(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
    {
	return;
    }
    else
    {
      if (IS_SET(ch->act,PLR_NOTRAN))
      {
        send_to_char("You are no longer immune to transport.\n\r",ch);
        REMOVE_BIT(ch->act,PLR_NOTRAN);
      }
      else
      {
        send_to_char("You are now immune to transport.\n\r",ch);
        SET_BIT(ch->act,PLR_NOTRAN);
      }
    }
}

void do_look( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    BUFFER *outlist;
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char *pdesc;
    int door;
    int number,count;

    if ( ch->desc == NULL )
	return;

    if ( ch->position < POS_SLEEPING )
    {
	send_to_char( "You can't see anything but stars!\n\r", ch );
	return;
    }

    if ( ch->position == POS_SLEEPING )
    {
	send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    if ( !IS_NPC(ch)
    &&   !IS_SET(ch->act, PLR_HOLYLIGHT)
    &&   room_is_dark( ch->in_room ) )
    {
	send_to_char( "It is pitch black ... \n\r", ch );
	show_char_to_char( ch->in_room->people, ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    number = number_argument(arg1,arg3);
    count = 0;

    if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
    {
	/* 'look' or 'look auto' */
	send_to_char( "{e", ch);
	send_to_char( ch->in_room->name, ch );
	send_to_char( "{x", ch);

	if ( (IS_IMMORTAL(ch) && (IS_NPC(ch) || IS_SET(ch->act,PLR_HOLYLIGHT)))
	||   IS_BUILDER(ch, ch->in_room->area) )
	{
	    sprintf(buf," [Room %d]",ch->in_room->vnum);
	    send_to_char(buf,ch);
	}

	send_to_char( "\n\r", ch );

	if ( arg1[0] == '\0'
	|| ( !IS_NPC(ch) && !IS_SET(ch->comm, COMM_BRIEF) ) )
	{
	    send_to_char( "  ",ch);
	    send_to_char( ch->in_room->description, ch );
	    if (ch->in_room->vnum == chain)
	    {
		send_to_char("A huge black iron chain as thick as a tree trunk is drifting above the ground\n\r",ch);
		send_to_char("here.\n\r",ch);
	    }
	}

        if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT) )
	{
	    send_to_char("{x\n\r",ch);
            do_exits( ch, "auto" );
	}

	outlist = show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
	page_to_char( buf_string(outlist), ch );
	free_buf(outlist);
	show_char_to_char( ch->in_room->people,   ch );
	send_to_char("{x",ch);
	return;
    }

    if ( !str_cmp( arg1, "i" ) || !str_cmp(arg1, "in")  || !str_cmp(arg1,"on"))
    {
	ROOM_INDEX_DATA *location;

	/* 'look in' */
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Look in what?\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not see that here.\n\r", ch );
	    return;
	}

	switch ( obj->item_type )
	{
	default:
	    send_to_char( "That is not a container.\n\r", ch );
	    break;

	case ITEM_DRINK_CON:
	    if ( obj->value[1] <= 0 )
	    {
		send_to_char( "It is empty.\n\r", ch );
		break;
	    }

	    sprintf( buf, "It's %sfilled with  a %s liquid.\n\r",
		obj->value[1] <     obj->value[0] / 4
		    ? "less than half-" :
		obj->value[1] < 3 * obj->value[0] / 4
		    ? "about half-"     : "more than half-",
		liq_table[obj->value[2]].liq_color
		);

	    send_to_char( buf, ch );
	    break;

        case ITEM_PORTAL:
            act("$n peers intently into $p.",ch,obj,NULL,TO_ROOM);
            act("You peer into $p.",ch,obj,NULL,TO_CHAR);

            /* Normal exit shows the proper exit */
            if( !IS_SET(obj->value[1], EX_CLOSED) )
               location = get_room_index(obj->value[3]);

            /*  Closed or other exit flags */
            else
            {
               send_to_char("The portal shimmers out of existance, stopping you from seeing through it.\n\r",ch);
               return;
            }
            send_to_char("Through the shimmering portal you see:\n\r",ch);
            /* show room desc without the room name or room vnum (IMM) */ 
            sprintf(buf,"{w%s{x",location->description);
	     send_to_char(buf,ch);

           /* show exit location. show objects and people in the room */	       
	    show_list_to_char( location->contents, ch, FALSE, FALSE );
	    show_char_to_char( location->people,   ch );
	    break;

	case ITEM_CONTAINER:
	case ITEM_PIT:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    {
		send_to_char( "It is closed.\n\r", ch );
		break;
	    }

	    act( "$p holds:", ch, obj, NULL, TO_CHAR );
	    outlist = show_list_to_char( obj->contains, ch, TRUE, TRUE );
	    page_to_char( buf_string(outlist), ch );
	    free_buf(outlist);
	    break;
	}
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) != NULL )
    {
	show_char_to_char_1( victim, ch );
	return;
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{  /* player can see object */
	    pdesc = get_extra_descr( arg3, obj->extra_descr );
	    if ( pdesc != NULL )
            {
	    	if (++count == number)
	    	{
		    send_to_char( pdesc, ch );
		    return;
	    	}
	    	else continue;
            }

 	    pdesc = get_extra_descr( arg3, obj->pIndexData->extra_descr );
 	    if ( pdesc != NULL )
            {
 	    	if (++count == number)
 	    	{	
		    send_to_char( pdesc, ch );
		    return;
	     	}
		else continue;
            }

	    if ( is_name( arg3, obj->name ) )
            {
	    	if (++count == number)
	    	{
		    if(obj->pyobject)
		    {
		        show_obj_description(obj,ch);
		        return;
		    }
	    	    send_to_char( obj->description, ch );
	    	    send_to_char( "\n\r",ch);
		    return;
		}
            }
	  }
    }

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    pdesc = get_extra_descr( arg3, obj->extra_descr );
	    if ( pdesc != NULL )
	    	if (++count == number)
	    	{
		    send_to_char( pdesc, ch );
		    return;
	    	}

	    pdesc = get_extra_descr( arg3, obj->pIndexData->extra_descr );
	    if ( pdesc != NULL )
	    	if (++count == number)
	    	{
		    send_to_char( pdesc, ch );
		    return;
	    	}
	if ( is_name( arg3, obj->name ) )
	    if (++count == number)
	    {
                if(obj->pyobject)
                {
                    show_obj_description(obj,ch);
                    return;
                }

	    	send_to_char( obj->description, ch );
	    	send_to_char("\n\r",ch);
	    	return;
	    }
	}
    }

    pdesc = get_extra_descr(arg3,ch->in_room->extra_descr);
    if (pdesc != NULL)
    {
	if (++count == number)
	{
	    send_to_char(pdesc,ch);
	    return;
	}
    }
    
    if (count > 0 && count != number)
    {
    	if (count == 1)
    	    sprintf(buf,"You only see one %s here.\n\r",arg3);
    	else
    	    sprintf(buf,"You only see %d of those here.\n\r",count);
    	
    	send_to_char(buf,ch);
    	return;
    }

         if ( !str_cmp( arg1, "n" ) || !str_cmp( arg1, "north" ) ) door = 0;
    else if ( !str_cmp( arg1, "e" ) || !str_cmp( arg1, "east"  ) ) door = 1;
    else if ( !str_cmp( arg1, "s" ) || !str_cmp( arg1, "south" ) ) door = 2;
    else if ( !str_cmp( arg1, "w" ) || !str_cmp( arg1, "west"  ) ) door = 3;
    else if ( !str_cmp( arg1, "u" ) || !str_cmp( arg1, "up"    ) ) door = 4;
    else if ( !str_cmp( arg1, "d" ) || !str_cmp( arg1, "down"  ) ) door = 5;
    else
    {
	send_to_char( "You do not see that here.\n\r", ch );
	return;
    }

    /* 'look direction' */
    if ( ( ch->alignment < 0 )
    &&   ( pexit = ch->in_room->exit[door+6] ) != NULL )
	door += 6;
    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	send_to_char( "Nothing special there.\n\r", ch );
	return;
    }

    if ( pexit->description != NULL && pexit->description[0] != '\0' )
	send_to_char( pexit->description, ch );
    else
	send_to_char( "Nothing special there.\n\r", ch );

    if ( pexit->keyword    != NULL
    &&   pexit->keyword[0] != '\0'
    &&   pexit->keyword[0] != ' ' )
    {
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	{
	    act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	}
	else if ( IS_SET(pexit->exit_info, EX_ISDOOR) )
	{
	    act( "The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR );
	}
    }

    return;
}

/* RT added back for the hell of it */
void do_read (CHAR_DATA *ch, char *argument )
{
    do_look(ch,argument);
}

void do_examine( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    ROOM_INDEX_DATA *location;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Examine what?\n\r", ch );
	return;
    }

    do_look( ch, arg );

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	switch ( obj->item_type )
	{
	default:
	    break;
	
	case ITEM_JUKEBOX:
	    do_play(ch,"list");
	    break;

	case ITEM_MONEY:
	    if (obj->value[0] == 0)
	    {
	        if (obj->value[1] == 0)
		    sprintf(buf,"Odd...there's no coins in the pile.\n\r");
		else if (obj->value[1] == 1)
		    sprintf(buf,"Wow. One gold coin.\n\r");
		else
		    sprintf(buf,"There are %d gold coins in the pile.\n\r",
			obj->value[1]);
	    }
	    else if (obj->value[1] == 0)
	    {
		if (obj->value[0] == 1)
		    sprintf(buf,"Wow. One silver coin.\n\r");
		else
		    sprintf(buf,"There are %d silver coins in the pile.\n\r",
			obj->value[0]);
	    }
	    else
		sprintf(buf,
		    "There are %d gold and %d silver coins in the pile.\n\r",
		    obj->value[1],obj->value[0]);
	    send_to_char(buf,ch);
	    break;
        case ITEM_PORTAL:
            act("$n peers intently into $p.",ch,obj,NULL,TO_ROOM);
            act("You peer into $p.",ch,obj,NULL,TO_CHAR);

            /* Normal exit shows the proper exit */
            if ( !IS_SET(obj->value[1], EX_CLOSED) )
               location = get_room_index(obj->value[3]);
            else
            {
               send_to_char("The portal shimmers out of existance, stopping you from seeing through it.\n\r",ch);
               return;
            }
            send_to_char("Through the shimmering portal you see:\n\r",ch);
            /* show room desc without the room name or room vnum (IMM) */ 
            sprintf(buf,"{w%s{x",location->description);
	     send_to_char(buf,ch);

           /* show exit location. show objects and people in the room */	       
	    show_list_to_char( location->contents, ch, FALSE, FALSE );
	    show_char_to_char( location->people,   ch );
	    break;

	case ITEM_DRINK_CON:
	case ITEM_CONTAINER:
	case ITEM_PIT:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    sprintf(buf,"in %s",argument);
	    do_look( ch, buf );
	}
    }

    return;
}



/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits( CHAR_DATA *ch, char *argument )
{
    extern char * const dir_name[];
    char buf[MAX_STRING_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool round;
    bool fAuto;
    int door;
    int outlet;

    fAuto  = !str_cmp( argument, "auto" );

    if ( !check_blind( ch ) )
	return;

    if (fAuto)
	sprintf(buf,"[{gExits:{x");
    else if (IS_IMMORTAL(ch))
	sprintf(buf,"Obvious exits from room %d:\n\r",ch->in_room->vnum);
    else
	sprintf(buf,"Obvious exits:\n\r");

    found = FALSE;
    for ( door = 0; door < 6; door++ )
    {
	round = FALSE;
	outlet = door;
	if ( ( ch->alignment < 0 )
	&&   ( pexit = ch->in_room->exit[door+6] ) != NULL)
	    outlet += 6;
	if ( ( pexit = ch->in_room->exit[outlet] ) != NULL
	&&   pexit->u1.to_room != NULL
        &&   (!IS_SET(pexit->exit_info, EX_NOSHOW)
             || IS_SET(ch->act,PLR_HOLYLIGHT))
	&&   can_see_room(ch,pexit->u1.to_room) )
	{
	    found = TRUE;
	    round = TRUE;
	    if ( fAuto )
	    {
		strcat( buf, " " );
		if(IS_SET(pexit->exit_info,EX_CLOSED))
                    strcat( buf, "{W={x");
		strcat( buf, dir_name[outlet] );
		if(IS_SET(pexit->exit_info,EX_CLOSED))
                    strcat( buf, "{W={x");
	    }
	    else
	    {
		sprintf( buf + strlen(buf), "%-5s - %s",
		    capitalize( dir_name[outlet] ),
		    (room_is_dark( pexit->u1.to_room )
		     && !IS_IMMORTAL(ch))
			?  "Too dark to tell"
			: pexit->u1.to_room->name
		    );
		if (IS_IMMORTAL(ch))
		    sprintf(buf + strlen(buf), 
			" (room %d)\n\r",pexit->u1.to_room->vnum);
		else
		    sprintf(buf + strlen(buf), "\n\r");
	    }
	}
	if (!round)
	{
	    OBJ_DATA *portal;
	    ROOM_INDEX_DATA *to_room;

	    portal = get_obj_exit( dir_name[door], ch->in_room->contents );
	    if (portal != NULL)
	    {
		found = TRUE;
		round = TRUE;
		if ( fAuto )
		{
		    strcat( buf, " " );
		    strcat( buf, dir_name[door] );
		}
		else
		{
		    to_room = get_room_index(portal->value[0]);
		    sprintf( buf + strlen(buf), "%-5s - %s",
			capitalize( dir_name[door] ),
			room_is_dark( to_room )
			    ?  "Too dark to tell"
			    : to_room->name
			);
		    if (IS_IMMORTAL(ch))
			sprintf(buf + strlen(buf), 
			    " (room %d)\n\r",to_room->vnum);
		    else
			sprintf(buf + strlen(buf), "\n\r");
		}
	    }
	}
    }

    if ( !found )
	strcat( buf, fAuto ? " none" : "None.\n\r" );

    if ( fAuto )
	strcat( buf, "]\n\r" );

    send_to_char( buf, ch );
    return;
}

void do_worth( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
    {
	sprintf(buf,"You have %ld platinum, %ld gold and %ld silver.\n\r",
	    ch->platinum,ch->gold,ch->silver);
	send_to_char(buf,ch);
	return;
    }

    sprintf(buf, 
    "You have %ld platinum, %ld gold, %ld silver,\n\rand %ld experience (%ld exp to level).\n\r",
	ch->platinum, ch->gold, ch->silver, ch->exp,
	(ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp);

    send_to_char(buf,ch);

    return;
}


void do_score( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    int i;

    output = new_buf();
    sprintf( buf,
	"{xYou are {G%s{x%s{x\n\r",
	ch->name,
	IS_NPC(ch) ? ", the mobile." : ch->pcdata->title);
    add_buf(output,buf);

    sprintf( buf,
	"{xLevel {B%d{x,  {B%d{x years old.\n\r",
	ch->level, get_age(ch) );
    add_buf(output,buf);

    if ( is_clan(ch) )
    {
      sprintf( buf, "You are a %s of the %s\n\r",
               player_rank(ch), player_clan(ch) );
      send_to_char(buf, ch);
    }

    if ( get_trust( ch ) != ch->level )
    {
	sprintf( buf, "{xYou are trusted at level {B%d{x.\n\r",
	    get_trust( ch ) );
	add_buf(output,buf);
    }

    sprintf(buf, "{xRace: {M%s{x  Sex: {M%s{x  Class:  {M%s{x\n\r",
	race_table[ch->race].name,
	ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female",
 	IS_NPC(ch) ? "mobile" : class_table[ch->class].name);
    add_buf(output,buf);
	

    sprintf( buf,
	"{xYou have {G%d{x/{B%d{x hit, {G%d{x/{B%d{x mana, {G%d{x/{B%d{x movement.\n\r",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move);
    add_buf(output,buf);

    sprintf( buf,
	"{xYou have {B%d{x practices and {B%d{x training sessions.\n\r",
	ch->practice, ch->train);
    add_buf(output,buf);

    sprintf( buf,
	"{xYou are carrying {G%d{x/{B%d{x items with weight {G%ld{x/{B%d{x pounds.\n\r",
	ch->carry_number, can_carry_n(ch),
	get_carry_weight(ch) / 10, can_carry_w(ch) /10 );
    add_buf(output,buf);

    sprintf( buf,
	"{xStr: {R%d{x({r%d{x)  Int: {R%d{x({r%d{x)  Wis: {R%d{x({r%d{x)  Dex: {R%d{x({r%d{x)  Con: {R%d{x({r%d{x)\n\r",
	ch->perm_stat[STAT_STR],
	get_curr_stat(ch,STAT_STR),
	ch->perm_stat[STAT_INT],
	get_curr_stat(ch,STAT_INT),
	ch->perm_stat[STAT_WIS],
	get_curr_stat(ch,STAT_WIS),
	ch->perm_stat[STAT_DEX],
	get_curr_stat(ch,STAT_DEX),
	ch->perm_stat[STAT_CON],
	get_curr_stat(ch,STAT_CON) );
    add_buf(output,buf);

    sprintf( buf,
	"{xYou have {Y%ld{x platinum, {Y%ld{x gold and {Y%ld{x silver coins.\n\r",
	ch->platinum, ch->gold, ch->silver);
    add_buf(output,buf);

    if (!IS_NPC(ch) && ch->level == LEVEL_HERO)
    {
	sprintf( buf,"{xYou have scored {C%ld exp{x.\n\r",ch->exp);
	add_buf(output,buf);
    } else if (!IS_NPC(ch) && ch->level < LEVEL_HERO) {
	sprintf( buf,"{xYou have scored {C%ld exp{x. You need {C%ld exp{x to level.\n\r",
	    ch->exp, ((ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp));
	add_buf(output,buf);
    }

    if (ch->wimpy)
    {
	sprintf( buf, "Wimpy set to %d hit points.\n\r", ch->wimpy );
	add_buf(output,buf);
    }

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]   > 10 )
    {
	sprintf(buf, "{yYou are drunk.{x\n\r");
	add_buf(output,buf);
    }
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] ==  0 )
    {
	sprintf(buf, "{yYou are thirsty.{x\n\r");
	add_buf(output,buf);
    }
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER]   ==  0 )
    {
	sprintf(buf, "{yYou are hungry.{x\n\r");
	add_buf(output,buf);
    }

    switch ( ch->position )
    {
    case POS_DEAD:     
	sprintf(buf, "{RYou are DEAD!!{x\n\r");
	add_buf(output,buf);
	break;
    case POS_MORTAL:
	sprintf(buf, "{RYou are mortally wounded.{x\n\r");
	add_buf(output,buf);
	break;
    case POS_INCAP:
	sprintf(buf, "{RYou are incapacitated.{x\n\r");
	add_buf(output,buf);
	break;
    case POS_STUNNED:
	sprintf(buf, "{RYou are stunned.{x\n\r");
	add_buf(output,buf);
	break;
    case POS_SLEEPING:
	sprintf(buf, "{BYou are sleeping.{x\n\r");
	add_buf(output,buf);
	break;
    case POS_RESTING:
	sprintf(buf, "{BYou are resting.{x\n\r");
	add_buf(output,buf);
	break;
    case POS_STANDING:
	sprintf(buf, "{BYou are standing.{x\n\r");
	add_buf(output,buf);
	break;
    case POS_FIGHTING:
	sprintf(buf, "{RYou are fighting.{x\n\r");
	add_buf(output,buf);
	break;
    }


    /* print AC values */
    if (ch->level >= 25)
    {	
	sprintf( buf,"{xArmor: pierce: {G%d{x  bash: {G%d{x  slash: {G%d{x  magic: {G%d{x\n\r",
		 GET_AC(ch,AC_PIERCE),
		 GET_AC(ch,AC_BASH),
		 GET_AC(ch,AC_SLASH),
		 GET_AC(ch,AC_EXOTIC));
	add_buf(output,buf);
    }

    for (i = 0; i < 4; i++)
    {
	char * temp;

	switch(i)
	{
	    case(AC_PIERCE):	temp = "piercing";	break;
	    case(AC_BASH):	temp = "bashing";	break;
	    case(AC_SLASH):	temp = "slashing";	break;
	    case(AC_EXOTIC):	temp = "magic";		break;
	    default:		temp = "error";		break;
	}
	
	sprintf(buf,"{xYou are ");
	add_buf(output,buf);

	if      (GET_AC(ch,i) >=  101 ) 
	    sprintf(buf,"{Rhopelessly vulnerable{x to %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 80) 
	    sprintf(buf,"{Rdefenseless{x against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 60)
	    sprintf(buf,"{Rbarely protected{x from %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 40)
	    sprintf(buf,"{yslightly armored{x against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 20)
	    sprintf(buf,"{ysomewhat armored{x against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= 0)
	    sprintf(buf,"{yarmored{x against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -20)
	    sprintf(buf,"{ywell-armored{x against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -40)
	    sprintf(buf,"{yvery well-armored{x against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -60)
	    sprintf(buf,"{Bheavily armored{x against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -80)
	    sprintf(buf,"{Bsuperbly armored{x against %s.\n\r",temp);
	else if (GET_AC(ch,i) >= -100)
	    sprintf(buf,"{Balmost invulnerable{x to %s.\n\r",temp);
	else
	    sprintf(buf,"{Wdivinely armored{x against %s.\n\r",temp);

	add_buf(output,buf);
    }


    /* RT wizinvis and holy light */
    if ( IS_IMMORTAL(ch))
    {
      sprintf(buf,"Holy Light: ");
      add_buf(output,buf);
      if (IS_SET(ch->act,PLR_HOLYLIGHT))
        sprintf(buf,"on");
      else
        sprintf(buf,"off");

      add_buf(output,buf);
 
      if (ch->invis_level)
      {
        sprintf( buf, "  Invisible: level %d",ch->invis_level);
	add_buf(output,buf);
      }

      if (ch->incog_level)
      {
	sprintf(buf,"  Incognito: level %d",ch->incog_level);
	add_buf(output,buf);
      }
      sprintf(buf,"\n\r");
      add_buf(output,buf);
    }

    if ( ch->level >= 15 )
    {
	sprintf( buf, "{xHitroll: {G%d{x  Damroll: {G%d{x.\n\r",
	    GET_HITROLL(ch), GET_DAMROLL(ch) );
	add_buf(output,buf);
    }
    
    if ( ch->level >= 10 )
    {
	sprintf( buf, "{xAlignment: {B%d{x.  ", ch->alignment );
	add_buf(output,buf);
    }

    sprintf(buf, "{xYou are ");
    add_buf(output,buf);
         if ( ch->alignment >  900 ) sprintf(buf, "{Wangelic{x.\n\r");
    else if ( ch->alignment >  700 ) sprintf(buf, "{Wsaintly{x.\n\r");
    else if ( ch->alignment >  350 ) sprintf(buf, "{wgood{x.\n\r");
    else if ( ch->alignment >  100 ) sprintf(buf, "kind.\n\r");
    else if ( ch->alignment > -100 ) sprintf(buf, "neutral.\n\r");
    else if ( ch->alignment > -350 ) sprintf(buf, "mean.\n\r");
    else if ( ch->alignment > -700 ) sprintf(buf, "{revil{x.\n\r");
    else if ( ch->alignment > -900 ) sprintf(buf, "{Rdemonic{x.\n\r");
    else                             sprintf(buf, "{Rsatanic{x.\n\r");

    add_buf(output,buf);

    if (ch->qps)
    {
	if (ch->qps == 1)
	    sprintf( buf, "{xYou have {M%d{x aquest point.\n\r", ch->qps );
	else
	    sprintf( buf, "{xYou have {M%d{x aquest points.\n\r", ch->qps );
	add_buf(output,buf);
    }
    page_to_char( buf_string(output), ch ); 
    free_buf(output); 

    if (IS_SET(ch->comm,COMM_SHOW_AFFECTS))
	do_affects(ch,"");
}

void do_affects(CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA *paf, *paf_last = NULL;
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    
    output = new_buf();
    if ( ch->affected != NULL )
    {
	sprintf(buf, "You are affected by the following spells:\n\r");
	add_buf(output,buf);
	for ( paf = ch->affected; paf != NULL; paf = paf->next )
	{
	    if (paf_last != NULL && paf->type == paf_last->type)
		if (ch->level >= 20)
		    sprintf( buf, "                      ");
		else
		    continue;
	    else
		sprintf( buf, "Spell: %-15s", skill_table[paf->type].name );

	    add_buf(output,buf);

	    if ( ch->level >= 20 )
	    {
		sprintf( buf,
		    ": modifies %s by %d ",
		    affect_loc_name( paf->location ),
		    paf->modifier);
		add_buf(output,buf);
		if ( paf->duration == -1 )
		    sprintf( buf, "permanently" );
		else
		    sprintf( buf, "for %d hours", paf->duration );
		add_buf(output,buf);
	    }

	    sprintf(buf, "\n\r");
	    add_buf(output,buf);
	    paf_last = paf;
	}
    }
    else 
    {
	sprintf(buf,"You are not affected by any spells.\n\r");
	add_buf(output,buf);
    }
    page_to_char( buf_string(output), ch );
    free_buf(output);
    return;
}



char *	const	day_name	[] =
{
    "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
    "the Great Gods", "the Sun"
};

char *	const	month_name	[] =
{
    "Winter", "the Winter Wolf", "the Frost Giant", "the Old Forces",
    "the Grand Struggle", "the Spring", "Nature", "Futility", "the Dragon",
    "the Sun", "the Heat", "the Battle", "the Dark Shades", "the Shadows",
    "the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void do_time( CHAR_DATA *ch, char *argument )
{
    extern char str_boot_time[];
    char buf[MAX_STRING_LENGTH];
    char *suf;
    int day;

    day     = time_info.day + 1;

         if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";

    if(IS_IMMORTAL(ch))
    {
        extern long gamelag;

        sprintf(buf,"Last compiled act_info.c: %s %s\n\r\n\r",
	__TIME__, __DATE__
	);
        send_to_char( buf, ch );

        sprintf(buf,"Present game load: %ld\n\r", gamelag);
        send_to_char( buf, ch );
    }

    sprintf( buf,
	"It is %d o'clock %s, Day of %s, %d%s the Month of %s.\n\r",
	(time_info.hour % 12 == 0) ? 12 : time_info.hour %12,
	time_info.hour >= 12 ? "pm" : "am",
	day_name[day % 7],
	day, suf,
	month_name[time_info.month]);
    send_to_char(buf,ch);
    sprintf(buf,"ROT started up at %s\n\rThe system time is %s\n\r",
	str_boot_time,
	(char *) ctime( &current_time )
	);
    send_to_char( buf, ch );

    return;
}



void do_weather( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    static char * const sky_look[4] =
    {
	"cloudless",
	"cloudy",
	"rainy",
	"lit by flashes of lightning"
    };

    if ( !IS_OUTSIDE(ch) )
    {
	send_to_char( "You can't see the weather indoors.\n\r", ch );
	return;
    }

    sprintf( buf, "The sky is %s and %s.\n\r",
	sky_look[weather_info.sky],
	weather_info.change >= 0
	? "a warm southerly breeze blows"
	: "a cold northern gust blows"
	);
    send_to_char( buf, ch );
    return;
}

void do_help( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    HELP_DATA *pHelp;
    BUFFER *output;
    BUFFER *output2;
    char argall[MAX_INPUT_LENGTH],argone[MAX_INPUT_LENGTH];
    int level;
    int count = 0;

    output = new_buf();
    output2 = new_buf();

    if ( argument[0] == '\0' )
	argument = "summary";

    /* this parts handles help a b so that it returns help 'a b' */
    argall[0] = '\0';
    while (argument[0] != '\0' )
    {
	argument = one_argument(argument,argone);
	if (argall[0] != '\0')
	    strcat(argall," ");
	strcat(argall,argone);
    }

	add_buf(output2,"\n\r");
	add_buf(output2,"More than one help file found for what you were looking for\n\r");
	add_buf(output2,"Here is the list of them all\n\r");
	add_buf(output2,"{r=========={R=========={b=========={B=========={g=========={G=========={x\n\r\n\r");

	for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
	{
		level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;

		if (level > get_trust( ch ) )
			continue;

		if( is_name( argall, pHelp->keyword ) )
		{
			count++;
			sprintf( buf,"{B<{G%-2d{R:{x ", count);
			add_buf( output2,buf);
	
			add_buf( output2,pHelp->keyword);
			add_buf( output2," {B>{x\n\r");

			clear_buf(output);

			/* 
			 * Strip leading '.' to allow initial blanks. 
			 */ 
			if ( strlen( argall ) == 1 ) 
				; // DJR HACK - display only keywords 
			else if ( pHelp->text[0] == '.' ) 
				add_buf(output,pHelp->text+1); 
			else 
				add_buf(output,pHelp->text); 

			if( is_exact_name(argall, pHelp->keyword) )
			{
				page_to_char(buf_string(output),ch);
				free_buf(output);
				free_buf(output2);
				return;
			}
			/* small hack :) */
			if (ch->desc != NULL && ch->desc->connected != CON_PLAYING 
			&&  		    ch->desc->connected != CON_GEN_GROUPS)
				break;
		}
	}

	sprintf(buf,"\n\rThere was {R%d{x keywords found for what you were looking for", count);
	add_buf(output2,buf);


    if (count == 0)
    {
    	send_to_char( "We are always working on new help files\n\r", ch );
	send_to_char( "So always check back for the help file you are looking for", ch );
	sprintf( log_buf, "NO HELP: %s could not find a help file for %s", ch->name, argone );
        log_string( LOG_ERR, log_buf );
    }
    else if (count == 1)
	page_to_char(buf_string(output),ch);
    else
	page_to_char(buf_string(output2),ch);
    free_buf(output);
	free_buf(output2);
}

/* whois command */
void do_whois (CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    BUFFER *output;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    bool found = FALSE;

    one_argument(argument,arg);
  
    if (arg[0] == '\0')
    {
	send_to_char("You must provide a name.\n\r",ch);
	return;
    }

    output = new_buf();

    for (d = descriptor_list; d != NULL; d = d->next)
    {
	CHAR_DATA *wch;
	char class[MAX_STRING_LENGTH];

 	if (d->connected != CON_PLAYING || !can_see(ch,d->character))
	    continue;
	
	wch = ( d->original != NULL ) ? d->original : d->character;

 	if (!can_see(ch,wch))
	    continue;

	if (!str_prefix(arg,wch->name))
	{
	    found = TRUE;
	    
	    /* work out the printing */
	    sprintf(buf2, "%3d", wch->level);
	    switch(wch->level)
	    {
		case MAX_LEVEL - 0 :
		    sprintf(buf2, "{GIMP{x");
		    break;
		case MAX_LEVEL - 1 :
		    sprintf(buf2, "{GCRE{x");
		    break;
		case MAX_LEVEL - 2 :
		    sprintf(buf2, "{GSUP{x");
		    break;
		case MAX_LEVEL - 3 :
		    sprintf(buf2, "{GDEI{x");
		    break;
		case MAX_LEVEL - 4 :
		    sprintf(buf2, "{GGOD{x");
		    break;
		case MAX_LEVEL - 5 :
		    sprintf(buf2, "{GIMM{x");
		    break;
		case MAX_LEVEL - 6 :
		    sprintf(buf2, "{GDEM{x");
		    break;
		case MAX_LEVEL - 7 :
		    sprintf(buf2, "{CKNI{x");
		    break;
		case MAX_LEVEL - 8 :
		    sprintf(buf2, "{CSQU{x");
		    break;
		case MAX_LEVEL - 9 :
		    sprintf(buf2, "{BHRO{x");
		    break;
	    }

	    if (wch->class < MAX_CLASS/2)
	    {
		sprintf(class, "{R%c{r%c%c{x",
		    class_table[wch->class].who_name[0],
		    class_table[wch->class].who_name[1],
		    class_table[wch->class].who_name[2]);
	    } else {
		sprintf(class, "{B%c{b%c%c{x",
		    class_table[wch->class].who_name[0],
		    class_table[wch->class].who_name[1],
		    class_table[wch->class].who_name[2]);
	    }

	    /* a little formatting */
	    sprintf(buf, "[%s %-6s %s] %s%s%s%s%s%s%s%s\n\r",
		buf2,
		wch->race < MAX_PC_RACE ? pc_race_table[wch->race].who_name
					: "     ",
		class,
	     ((wch->ghost_level >= LEVEL_HERO)&&(ch->level >= wch->level)) ? "(Ghost) ": "",
	     wch->incog_level >= LEVEL_HERO ? "(Incog) ": "",
 	     wch->invis_level >= LEVEL_HERO ? "(Wizi) " : "",
	        (is_clan(wch) || is_independent(wch)) ?
    "         " : clan_table[wch->clan].who_name,
	     IS_SET(wch->comm, COMM_AFK) ? "[{yAFK{x] " : "",
             IS_SET(wch->act,PLR_TWIT) ? "({RTWIT{x) " : "",
		wch->name, IS_NPC(wch) ? "" : wch->pcdata->title);
	    add_buf(output,buf);
	}
    }

    if (!found)
    {
	send_to_char("No one of that name is playing.\n\r",ch);
	return;
    }

    page_to_char(buf_string(output),ch);
    free_buf(output);
}


/*
 * New 'who' command originally by Alander of Rivers of Mud.
 */
void do_who( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char questdat[MAX_STRING_LENGTH];
    BUFFER *output;
    BUFFER *outputimm;
    DESCRIPTOR_DATA *d;
    int iClass;
    int iRace;
    int iLevelLower;
    int iLevelUpper;
    int nNumber;
    int nMatch;
    bool rgfClass[MAX_CLASS];
    bool rgfRace[MAX_PC_RACE];
    bool fClassRestrict = FALSE;
    bool fRaceRestrict = FALSE;
    bool fImmortalOnly = FALSE;
    int count;
    int countimm;
    int hour;

    /*
     * Set default arguments.
     */
    iLevelLower    = 0;
    iLevelUpper    = MAX_LEVEL;
    for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
        rgfClass[iClass] = FALSE;
    for ( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
        rgfRace[iRace] = FALSE;
 
    /*
     * Parse arguments.
     */
    count = 0;
    countimm = 0;
    nNumber = 0;
    for ( ;; )
    {
        char arg[MAX_STRING_LENGTH];
 
        argument = one_argument( argument, arg );
        if ( arg[0] == '\0' )
            break;
 
        if ( is_number( arg ) )
        {
            switch ( ++nNumber )
            {
            case 1: iLevelLower = atoi( arg ); break;
            case 2: iLevelUpper = atoi( arg ); break;
            default:
                send_to_char( "Only two level numbers allowed.\n\r", ch );
                return;
            }
        }
        else
        {
 
            /*
             * Look for classes to turn on.
             */
            if (!str_prefix(arg,"immortals"))
            {
                fImmortalOnly = TRUE;
            }
            else
            {
                iClass = class_lookup(arg);
                if (iClass == -1)
                {
                    iRace = race_lookup(arg);
 
                    if (iRace == 0 || iRace >= MAX_PC_RACE)
		    {
                        send_to_char(
                          "That's not a valid race or class.\n\r",
			   ch);
                          return;
		    }
                    else
                    {
                        fRaceRestrict = TRUE;
                        rgfRace[iRace] = TRUE;
                    }
                }
                else
                {
                    fClassRestrict = TRUE;
                    rgfClass[iClass] = TRUE;
                }
            }
        }
    }
 
    /*
     * Now show matching chars.
     */
    nMatch = 0;
    buf[0] = '\0';
    output = new_buf();
    outputimm = new_buf();
    add_buf(outputimm,"{xVisible {GImmortals{x:\n\r");
    add_buf(output,"{xVisible {GMortals{x:\n\r");
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;
	char class[MAX_STRING_LENGTH];
 
        /*
         * Check for match against restrictions.
         * Don't use trust as that exposes trusted mortals.
         */
        if ( d->connected != CON_PLAYING || !can_see( ch, d->character ) )
            continue;
 
        wch   = ( d->original != NULL ) ? d->original : d->character;

	if (!can_see(ch,wch) && IS_SHIELDED(wch,SHD_INVISIBLE))
	    continue;

        if (!can_see(ch,wch) && wch->level >= SQUIRE
	&& ch->level < wch->level )
	    continue;

	if (!IS_IMMORTAL(wch) || wch->level <= LEVEL_HERO)
	    continue;

        if ( wch->level < iLevelLower
        ||   wch->level > iLevelUpper
        || ( fImmortalOnly  && wch->level < LEVEL_IMMORTAL )
        || ( fClassRestrict && !rgfClass[wch->class] )
        || ( fRaceRestrict && !rgfRace[wch->race]))
            continue;
 
	countimm++;

        nMatch++;
 
        /*
         * Figure out what to print for class.
	 */
	sprintf(buf2, "%3d", wch->level);
	switch ( wch->level )
	{
	default: break;
            {
		case MAX_LEVEL - 0 :
		    sprintf(buf2, "{GIMP{x");
		    break;
		case MAX_LEVEL - 1 :
		    sprintf(buf2, "{GCRE{x");
		    break;
		case MAX_LEVEL - 2 :
		    sprintf(buf2, "{GSUP{x");
		    break;
		case MAX_LEVEL - 3 :
		    sprintf(buf2, "{GDEI{x");
		    break;
		case MAX_LEVEL - 4 :
		    sprintf(buf2, "{GGOD{x");
		    break;
		case MAX_LEVEL - 5 :
		    sprintf(buf2, "{GIMM{x");
		    break;
		case MAX_LEVEL - 6 :
		    sprintf(buf2, "{GDEM{x");
		    break;
		case MAX_LEVEL - 7 :
		    sprintf(buf2, "{CKNI{x");
		    break;
		case MAX_LEVEL - 8 :
		    sprintf(buf2, "{CSQU{x");
		    break;
            }
	}

	if (wch->class < MAX_CLASS/2)
	{
	    sprintf(class, "{R%c{r%c%c{x",
		class_table[wch->class].who_name[0],
		class_table[wch->class].who_name[1],
		class_table[wch->class].who_name[2]);

	} else {
	    sprintf(class, "{B%c{b%c%c{x",
		class_table[wch->class].who_name[0],
		class_table[wch->class].who_name[1],
		class_table[wch->class].who_name[2]);
	}

	/*
	 * Format it up.
	 */
        if (!wch->on_quest)
        {
            questdat[0]='\0';
        } else {
            sprintf(questdat, "[{GQ{x] ");
        }
	if (wch->pcdata->who_descr[0] != '\0')
	{
	    sprintf( buf, "[%s %s] %s%s%s%s%s%s%s%s%s\n\r",
		buf2,
		wch->pcdata->who_descr,
		questdat,
		   (is_clan(wch) || is_independent(wch)) ?
    "         " : clan_table[wch->clan].who_name,
		((wch->ghost_level >= LEVEL_HERO)&&(ch->level >= wch->level)) ? "(Ghost) ": "",
		wch->incog_level >= LEVEL_HERO ? "(Incog) " : "",
		wch->invis_level >= LEVEL_HERO ? "(Wizi) " : "",
		IS_SET(wch->comm, COMM_AFK) ? "[{yAFK{x] " : "",
		IS_SET(wch->act, PLR_TWIT) ? "({RTWIT{x) " : "",
		wch->name,
		IS_NPC(wch) ? "" : wch->pcdata->title );
	} else
	{
	    sprintf( buf, "[%s %6.6s %s] %s%s%s%s%s%s%s%s%s\n\r",
		buf2,
		wch->race < MAX_PC_RACE ? pc_race_table[wch->race].who_name 
				    : "      ",
		class,
		questdat,
		   (is_clan(wch) || is_independent(wch)) ?
    "         " : clan_table[wch->clan].who_name,
		((wch->ghost_level >= LEVEL_HERO)&&(ch->level >= wch->level)) ? "(Ghost) ": "",
		wch->incog_level >= LEVEL_HERO ? "(Incog) " : "",
		wch->invis_level >= LEVEL_HERO ? "(Wizi) " : "",
		IS_SET(wch->comm, COMM_AFK) ? "[{yAFK{x] " : "",
		IS_SET(wch->act, PLR_TWIT) ? "({RTWIT{x) " : "",
		wch->name,
		IS_NPC(wch) ? "" : wch->pcdata->title );
	}
	add_buf(outputimm,buf);
    }
    add_buf(outputimm,"\n\r");
    buf[0] = '\0';
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;
	char class[MAX_STRING_LENGTH];
 
        /*
         * Check for match against restrictions.
         * Don't use trust as that exposes trusted mortals.
         */
        if ( d->connected != CON_PLAYING || !can_see( ch, d->character ) )
            continue;
 
        wch   = ( d->original != NULL ) ? d->original : d->character;

	if (!can_see(ch,wch) && IS_SHIELDED(wch,SHD_INVISIBLE))
	    continue;

        if (!can_see(ch,wch) && wch->level >= SQUIRE
	&& ch->level < wch->level )
	    continue;

	if (IS_IMMORTAL(wch) && wch->level > LEVEL_HERO)
	    continue;

	count++;

	if ((wch->in_room->vnum == ROOM_VNUM_CORNER)
	&& (!IS_IMMORTAL(ch)))
	    continue;

        if ( wch->level < iLevelLower
        ||   wch->level > iLevelUpper
        || ( fImmortalOnly  && wch->level < LEVEL_IMMORTAL )
        || ( fClassRestrict && !rgfClass[wch->class] )
        || ( fRaceRestrict && !rgfRace[wch->race]))
            continue;
 
        nMatch++;
 
        /*
         * Figure out what to print for class.
	 */
	sprintf(buf2, "%3d", wch->level);
	switch ( wch->level )
	{
	default: break;
            {
		case MAX_LEVEL - 9 :
		    sprintf(buf2, "{BHRO{x");
		    break;
            }
	}

	if (wch->class < MAX_CLASS/2)
	{
	    sprintf(class, "{R%c{r%c%c{x",
		class_table[wch->class].who_name[0],
		class_table[wch->class].who_name[1],
		class_table[wch->class].who_name[2]);
	} else {
	    sprintf(class, "{B%c{b%c%c{x",
		class_table[wch->class].who_name[0],
		class_table[wch->class].who_name[1],
		class_table[wch->class].who_name[2]);
	}

	/*
	 * Format it up.
	 */
	if (!wch->on_quest)
	{
	    questdat[0]='\0';
        } else {
	    sprintf(questdat, "[{GQ{x] ");
	}
	if (wch->pcdata->who_descr[0] != '\0')
	{
	    sprintf( buf, "[%s %s] %s%s%s%s%s%s%s%s%s\n\r",
		buf2,
		wch->pcdata->who_descr,
		questdat,
		   (is_clan(wch) || is_independent(wch)) ?
    "         " : clan_table[wch->clan].who_name,
		((wch->ghost_level >= LEVEL_HERO)&&(ch->level >= wch->level)) ? "(Ghost) ": "",
		wch->incog_level >= LEVEL_HERO ? "(Incog) " : "",
		wch->invis_level >= LEVEL_HERO ? "(Wizi) " : "",
		IS_SET(wch->comm, COMM_AFK) ? "[{yAFK{x] " : "",
		IS_SET(wch->act, PLR_TWIT) ? "({RTWIT{x) " : "",
		wch->name,
		IS_NPC(wch) ? "" : wch->pcdata->title );
	} else
	{
	    sprintf( buf, "[%s %6.6s %s] %s%s%s%s%s%s%s%s%s\n\r",
		buf2,
		wch->race < MAX_PC_RACE ? pc_race_table[wch->race].who_name 
				    : "      ",
		class,
		questdat,
		   (is_clan(wch) || is_independent(wch)) ?
    "         " : clan_table[wch->clan].who_name,
		((wch->ghost_level >= LEVEL_HERO)&&(ch->level >= wch->level)) ? "(Ghost) ": "",
		wch->incog_level >= LEVEL_HERO ? "(Incog) " : "",
		wch->invis_level >= LEVEL_HERO ? "(Wizi) " : "",
		IS_SET(wch->comm, COMM_AFK) ? "[{yAFK{x] " : "",
		IS_SET(wch->act, PLR_TWIT) ? "({RTWIT{x) " : "",
		wch->name,
		IS_NPC(wch) ? "" : wch->pcdata->title );
	}
	add_buf(output,buf);
    }

    count += countimm;
    hour = (int) (struct tm *)localtime(&current_time)->tm_hour;
    if (hour < 12)
    {
	if (is_pm)
	{
	    is_pm = FALSE;
	    max_on = 0;
	}
    } else
    {
	is_pm = TRUE;
    }
    max_on = UMAX(count,max_on);
    max_on_bootup = UMAX(count,max_on_bootup);
    if (global_quest)
    {
	sprintf( buf2, "\n\r{GThe global quest flag is on.{x");
	add_buf(output,buf2);
    }
    if (nMatch != count)
    {
	sprintf( buf2, "\n\r{BMatches found: {W%d{x\n\r", nMatch );
	add_buf(output,buf2);
    } else {
	sprintf( buf2,
            "\n\r{BPlayers found: {W%d   {BMost on today: {W%d{x\n\r"
            "{BMost on at any time: {W%d{x\n\r",
            count, max_on, max_on_bootup );
	add_buf(output,buf2);
    }
    if (countimm >= 1)
    {
	add_buf(outputimm, buf_string(output));
	page_to_char( buf_string(outputimm), ch );
    } else
    {
	page_to_char( buf_string(output), ch );
    }
    free_buf(output);
    free_buf(outputimm);
    return;
}

void do_count ( CHAR_DATA *ch, char *argument )
{
    int count;
    int hour;
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];

    send_to_char("This command is obsolete.\n\r",ch);
    return;

    count = 0;
    hour = (int) (struct tm *)localtime(&current_time)->tm_hour;
    if (hour < 12)
    {
	if (is_pm)
	{
	    is_pm = FALSE;
	    max_on = 0;
	}
    } else
    {
	is_pm = TRUE;
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
        if ( d->connected == CON_PLAYING && can_see( ch, d->character ) )
	    count++;

    max_on = UMAX(count,max_on);

    if (max_on == count)
        sprintf(buf,"{BThere are {W%d {Bcharacters on, the most so far today.{x\n\r",
	    count);
    else
	sprintf(buf,"{BThere are {W%d {Bcharacters on, the most on today was {W%d{x.\n\r",
	    count,max_on);

    send_to_char(buf,ch);
}

void do_inventory( CHAR_DATA *ch, char *argument )
{
    BUFFER *outlist;

    send_to_char( "You are carrying:\n\r", ch );
    outlist = show_list_to_char( ch->carrying, ch, TRUE, TRUE );
    page_to_char( buf_string(outlist), ch);
    free_buf(outlist);
    return;
}



/*
void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int iWear;
    int oWear;
    bool found;

    send_to_char( "You are using:\n\r", ch );
    found = FALSE;
    for ( oWear = 0; oWear < MAX_WEAR; oWear++ )
    {
	iWear = where_order[oWear];
	if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
	{
	    continue;
	}

	send_to_char( where_name[iWear], ch );
	if ( can_see_obj( ch, obj ) )
	{
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	}
	else
	{
	    send_to_char( "something.\n\r", ch );
	}
	found = TRUE;
    }

    if ( !found )
	send_to_char( "Nothing.\n\r", ch );

    return;
}
*/

void do_equipment (CHAR_DATA * ch, char *argument) {
    OBJ_DATA *obj;
    int iWear;
    bool found;
    send_to_char ("{wYou are using:{x\n\r", ch);
    found = FALSE;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
        if ((obj = get_eq_char (ch, iWear)) == NULL) {
                send_to_char("{w", ch);
                send_to_char(where_name[iWear], ch);
                send_to_char("     ---\r\n", ch);
                continue;
        }
        send_to_char("{Y", ch);
        send_to_char (where_name[iWear], ch);
        send_to_char("{W", ch);
        if (can_see_obj (ch, obj)) {
            send_to_char("{W", ch);
            send_to_char (format_obj_to_char (obj, ch, TRUE), ch);
            send_to_char ("{x\n\r", ch);
        }
        else    {
            send_to_char ("{Wsomething.\n\r", ch);
        }
        found = TRUE;
    }

    send_to_char("{x", ch);

    return;
}



void do_compare( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    char *msg;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Compare what to what?\n\r", ch );
	return;
    }

    if ( ( obj1 = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if (arg2[0] == '\0')
    {
	for (obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content)
	{
	    if (obj2->wear_loc != WEAR_NONE
	    &&  can_see_obj(ch,obj2)
	    &&  obj1->item_type == obj2->item_type
	    &&  (obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
		break;
	}

	if (obj2 == NULL)
	{
	    send_to_char("You aren't wearing anything comparable.\n\r",ch);
	    return;
	}
    } 

    else if ( (obj2 = get_obj_carry(ch,arg2) ) == NULL )
    {
	send_to_char("You do not have that item.\n\r",ch);
	return;
    }

    msg		= NULL;
    value1	= 0;
    value2	= 0;

    if ( obj1 == obj2 )
    {
	msg = "You compare $p to itself.  It looks about the same.";
    }
    else if ( obj1->item_type != obj2->item_type )
    {
	msg = "You can't compare $p and $P.";
    }
    else
    {
	switch ( obj1->item_type )
	{
	default:
	    msg = "You can't compare $p and $P.";
	    break;

	case ITEM_ARMOR:
	    if (obj1->clan)
	    {
		value1 = apply_ac(ch->level, obj1, WEAR_HOLD, 0);
		value1 += apply_ac(ch->level, obj1, WEAR_HOLD, 1);
		value1 += apply_ac(ch->level, obj1, WEAR_HOLD, 2);
	    } else
	    {
		value1 = obj1->value[0] + obj1->value[1] + obj1->value[2];
	    }
	    if (obj2->clan)
	    {
		value2 = apply_ac(ch->level, obj2, WEAR_HOLD, 0);
		value2 += apply_ac(ch->level, obj2, WEAR_HOLD, 1);
		value2 += apply_ac(ch->level, obj2, WEAR_HOLD, 2);
	    } else
	    {
		value2 = obj2->value[0] + obj2->value[1] + obj2->value[2];
	    }
	    break;

	case ITEM_WEAPON:
	    if (obj1->clan)
	    {
		value1 = 4 * (ch->level/3);
	    } else
	    {
		if (obj1->pIndexData->new_format)
		    value1 = (1 + obj1->value[2]) * obj1->value[1];
		else
	    	    value1 = obj1->value[1] + obj1->value[2];
	    }
	    if (obj2->clan)
	    {
		value2 = 4 * (ch->level/3);
	    } else
	    {
		if (obj2->pIndexData->new_format)
		    value2 = (1 + obj2->value[2]) * obj2->value[1];
		else
	    	    value2 = obj2->value[1] + obj2->value[2];
	    }
	    break;
	}
    }

    if ( msg == NULL )
    {
	     if ( value1 == value2 ) msg = "$p and $P look about the same.";
	else if ( value1  > value2 ) msg = "$p looks better than $P.";
	else                         msg = "$p looks worse than $P.";
    }

    act( msg, ch, obj1, obj2, TO_CHAR );
    return;
}



void do_credits( CHAR_DATA *ch, char *argument )
{
    do_help( ch, "diku" );
    return;
}



void do_where( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    bool found;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Players near you:\n\r", ch );
	found = FALSE;
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    && ( victim = d->character ) != NULL
	    &&   !IS_NPC(victim)
	    &&   victim->in_room != NULL
	    &&   !IS_SET(victim->in_room->room_flags,ROOM_NOWHERE)
 	    &&   (is_room_owner(ch,victim->in_room) 
	    ||    !room_is_private(ch,victim->in_room))
	    &&   victim->in_room->area == ch->in_room->area
	    &&   (!IS_SET(victim->in_room->area->area_flags, AREA_NOWHERE)
	    ||   IS_SET(ch->in_room->service_flags,SERVICE_VANTAGE_POINT) )
	    &&   get_trust(ch) >= victim->ghost_level
	    &&   can_see( ch, victim ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
		    victim->name, victim->in_room->name );
		send_to_char( buf, ch );
	    }
	}
	if ( !found )
	    send_to_char( "None\n\r", ch );
    }
    else
    {
	found = FALSE;
	for ( victim = char_list; victim != NULL; victim = victim->next )
	{
	    if ( victim->in_room != NULL
	    &&   victim->in_room->area == ch->in_room->area
	    &&   !IS_AFFECTED(victim, AFF_HIDE)
	    &&   !IS_AFFECTED(victim, AFF_SNEAK)
	    &&   get_trust(ch) >= victim->ghost_level
	    &&   can_see( ch, victim )
	    &&   is_name( arg, victim->name ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
		    PERS(victim, ch), victim->in_room->name );
		send_to_char( buf, ch );
		break;
	    }
	}
	if ( !found )
	    act( "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
    }

    return;
}

void do_track( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *in_room;
    int track_vnum;
    bool follow = FALSE;
    int door, move, chance, track;

    argument = one_argument( argument, arg );

    if ((chance = get_skill(ch,gsn_track)) == 0)
    {
	send_to_char( "You don't know how to track.\n\r", ch );
	return;
    }

    if ( !str_cmp(arg, "follow") )
    {
        follow = TRUE;
        strcpy(arg,argument);
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Track whom?\n\r", ch );
	return;
    }

    in_room = ch->in_room;
    track_vnum = in_room->vnum;
    move = movement_loss[UMIN(SECT_MAX-1, in_room->sector_type)];
    if ( ch->move < move )
    {
	send_to_char( "You are too exhausted.\n\r", ch );
	return;
    }

    if (number_percent() < (100-chance))
    {
	sprintf(buf, "You can find no recent tracks for %s.\n\r", arg);
	send_to_char(buf, ch);
	check_improve(ch,gsn_track,FALSE,1);
	WAIT_STATE( ch, 1 );
	ch->move -= move/2;
	return;
    }

    for ( victim = char_list; victim != NULL; victim = victim->next )
    {
	if ( victim->in_room != NULL
	&&   !IS_NPC(victim)
	&&   can_see( ch, victim )
	&&   is_name( arg, victim->name ) )
	{
	    if (victim->in_room->vnum == track_vnum)
	    {
		act( "The tracks end right under $S feet.", ch, NULL, victim, TO_CHAR);
		return;
	    }
	    for (track = 0; track < MAX_TRACK; track++)
	    {
		if (victim->track_from[track] == track_vnum)
		{
		    for (door = 0; door < 12; door++)
		    {
			if ( ( pexit = in_room->exit[door] ) != NULL)
			{
			    if (pexit->u1.to_room->vnum == victim->track_to[track])
			    {
				sprintf(buf, "Some tracks lead off to the %s.\n\r", dir_name[door]);
				send_to_char(buf, ch);
				check_improve(ch,gsn_track,TRUE,1);
				WAIT_STATE( ch, 1 );
				ch->move -= move;

                                if(follow)
                                {
                                    send_to_char("You follow the tracks.\n\r",ch);
                                    move_char(ch,door, FALSE, FALSE);
                                }

				return;
			    }
			}
		    }
		    act("$N seems to have vanished here.", ch, NULL, victim, TO_CHAR);
		    check_improve(ch,gsn_track,TRUE,1);
		    WAIT_STATE( ch, 1 );
		    ch->move -= move;
		    return;
		}
	    }
	    act("You can find no recent tracks for $N.", ch, NULL, victim, TO_CHAR);
	    check_improve(ch,gsn_track,FALSE,1);
	    WAIT_STATE( ch, 1 );
	    ch->move -= move/2;
	    return;
	}
    }

    for ( victim = char_list; victim != NULL; victim = victim->next )
    {
	if ( (victim->in_room != NULL)
	&&   IS_NPC(victim)
	&&   can_see( ch, victim )
	&&   (victim->in_room->area == ch->in_room->area)
	&&   is_name( arg, victim->name ) )
	{
	    if (victim->in_room->vnum == track_vnum)
	    {
		act( "The tracks end right under $S feet.", ch, NULL, victim, TO_CHAR);
		return;
	    }
	    for (track = 0; track < MAX_TRACK; track++)
	    {
		if (victim->track_from[track] == track_vnum)
		{
		    for (door = 0; door < 12; door++)
		    {
			if ( ( pexit = in_room->exit[door] ) != NULL)
			{
			    if (pexit->u1.to_room->vnum == victim->track_to[track])
			    {
				sprintf(buf, "Some tracks lead off to the %s.\n\r", dir_name[door]);
				send_to_char(buf, ch);
				check_improve(ch,gsn_track,TRUE,1);
				WAIT_STATE( ch, 1 );
				ch->move -= move;

                                if(follow)
                                {
                                    send_to_char("You follow the tracks.\n\r",ch);
                                    move_char(ch,door, FALSE, FALSE);
                                }

				return;
			    }
			}
		    }
		    act("$N seems to have vanished here.", ch, NULL, victim, TO_CHAR);
		    check_improve(ch,gsn_track,TRUE,1);
		    WAIT_STATE( ch, 1 );
		    ch->move -= move;
		    return;
		}
	    }
	}
    }
    sprintf(buf, "You can find no recent tracks for %s.\n\r", arg);
    send_to_char(buf, ch);
    check_improve(ch,gsn_track,FALSE,1);
    WAIT_STATE( ch, 1 );
    ch->move -= move/2;
    return;
}



void do_consider( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char *msg;
    int diff, vac, cac;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Consider killing whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    if (is_safe(ch,victim))
    {
	send_to_char("Don't even think about it.\n\r",ch);
	return;
    }

    diff = ((victim->hit / 50) - (ch->hit / 50));
    vac = -(GET_AC(victim,AC_PIERCE)+GET_AC(victim,AC_BASH)+GET_AC(victim,AC_SLASH)+GET_AC(victim,AC_EXOTIC));
    cac = -(GET_AC(ch,AC_PIERCE)+GET_AC(ch,AC_BASH)+GET_AC(ch,AC_SLASH)+GET_AC(ch,AC_EXOTIC));
    diff += (vac - cac);
    diff += (GET_DAMROLL(victim) - GET_DAMROLL(ch));
    diff += (GET_HITROLL(victim) - GET_HITROLL(ch));
    diff += (get_curr_stat(victim,STAT_STR) - get_curr_stat(ch,STAT_STR));

         if ( diff <=  -110 ) msg = "You can kill $N naked and weaponless.";
    else if ( diff <=  -70 )  msg = "$N is no match for you.";
    else if ( diff <=  -20 )  msg = "$N looks like an easy kill.";
    else if ( diff <=  20 )   msg = "The perfect match!";
    else if ( diff <=  70 )   msg = "$N says '{aDo you feel lucky, punk?{x'.";
    else if ( diff <=  110 )  msg = "$N laughs at you mercilessly.";
    else                      msg = "Death will thank you for your gift.";

    act( msg, ch, NULL, victim, TO_CHAR );
    return;
}

void set_title( CHAR_DATA *ch, char *title )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }

    if ( title[0] != '.' && title[0] != ',' && title[0] != '!' && title[0] != '?' && title[0] != ':' )
    {
	buf[0] = ' ';
	strcpy( buf+1, title );
    }
    else
    {
	strcpy( buf, title );
    }

    free_string( ch->pcdata->title );
    ch->pcdata->title = str_dup( buf );
    return;
}



void do_title( CHAR_DATA *ch, char *argument )
{
    int value;

    if ( IS_NPC(ch) )
	return;

    if ( IS_SET(ch->act, PLR_NOTITLE) )
	return;

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER)
    && (!IS_IMMORTAL(ch)))
    {
	send_to_char("Just keep your nose in the corner like a good little player.\n\r",ch);
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Change your title to what?\n\r", ch );
	return;
    }

    if ( !str_cmp(argument, "lock") )
    {
        do_titlelock(ch,"");
        return;
    }

    if ( strlen(argument) > 45 )
    {
	argument[45] = '{';
	argument[46] = 'x';
	argument[47] = '\0';
    }
    else
    {
	value = strlen(argument);
	argument[value] = '{';
	argument[value+1] = 'x';
	argument[value+2] = '\0';
    }
    smash_tilde( argument );
    set_title( ch, argument );
    send_to_char( "Ok.\n\r", ch );
}



void do_description( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( argument[0] != '\0' )
    {
	buf[0] = '\0';
	smash_tilde( argument );

    	if (argument[0] == '-')
    	{
            int len;
            bool found = FALSE;
 
            if (ch->description == NULL || ch->description[0] == '\0')
            {
                send_to_char("No lines left to remove.\n\r",ch);
                return;
            }
	
  	    strcpy(buf,ch->description);
 
            for (len = strlen(buf); len > 0; len--)
            {
                if (buf[len] == '\r')
                {
                    if (!found)  /* back it up */
                    {
                        if (len > 0)
                            len--;
                        found = TRUE;
                    }
                    else /* found the second one */
                    {
                        buf[len + 1] = '\0';
			free_string(ch->description);
			ch->description = str_dup(buf);
			send_to_char( "Your description is:\n\r", ch );
			send_to_char( ch->description ? ch->description : 
			    "(None).\n\r", ch );
                        return;
                    }
                }
            }
            buf[0] = '\0';
	    free_string(ch->description);
	    ch->description = str_dup(buf);
	    send_to_char("Description cleared.\n\r",ch);
	    return;
        }
	if ( argument[0] == '+' )
	{
	    if ( ch->description != NULL )
		strcat( buf, ch->description );
	    argument++;
	    while ( isspace(*argument) )
		argument++;
	}

	if ( strlen(buf) + strlen(argument) >= (MAX_STRING_LENGTH/3) - 4 )
	{
	    send_to_char( "Description too long.\n\r", ch );
	    return;
	}

	strcat( buf, argument );
	strcat( buf, "\n\r" );
	free_string( ch->description );
	ch->description = str_dup( buf );
    }

    send_to_char( "Your description is:\n\r", ch );
    send_to_char( ch->description ? ch->description : "(None).\n\r", ch );
    return;
}



void do_report( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];

    sprintf( buf,
	"You say '{SI have %d/%d hp %d/%d mana %d/%d mv %ld xp.{x'\n\r",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->exp   );

    send_to_char( buf, ch );

    sprintf( buf, "$n says '{SI have %d/%d hp %d/%d mana %d/%d mv %ld xp.{x'",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->move, ch->max_move,
	ch->exp   );

    act( buf, ch, NULL, NULL, TO_ROOM );

    return;
}



void do_practice( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int sn;

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	int col;
        CHAR_DATA *mob;

	col    = 0;

        for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
        {
            if ( IS_NPC(mob) && IS_SET(mob->act, ACT_PRACTICE) )
                break;
        }

        if(mob) send_to_char("This person can teach you:\n\r", ch);

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name == NULL )
		break;
	    if ( (ch->level < skill_table[sn].skill_level[ch->class] && !IS_IMMORTAL(ch))
	      || ch->pcdata->learned[sn] < 1 /* skill is not known */ )
		continue;

	    sprintf( buf, "%-18s %3d%%  ",
		skill_table[sn].name, ch->pcdata->learned[sn] );
	    send_to_char( buf, ch );
	    if ( ++col % 3 == 0 )
		send_to_char( "\n\r", ch );
	}

	if ( col % 3 != 0 )
	    send_to_char( "\n\r", ch );

	sprintf( buf, "You have %d practice sessions left.\n\r",
	    ch->practice );
	send_to_char( buf, ch );
    }
    else
    {
	CHAR_DATA *mob;
	int adept;

	if ( !IS_AWAKE(ch) )
	{
	    send_to_char( "In your dreams, or what?\n\r", ch );
	    return;
	}

	for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
	{
	    if ( IS_NPC(mob) && IS_SET(mob->act, ACT_PRACTICE) )
		break;
	}

	if ( mob == NULL )
	{
	    send_to_char( "You can't do that here.\n\r", ch );
	    return;
	}

	if ( ch->practice <= 0 )
	{
	    send_to_char( "You have no practice sessions left.\n\r", ch );
	    return;
	}

	if ( ( sn = find_skill( ch,argument ) ) < 0
	|| ( !IS_NPC(ch)
	&&   ((ch->level < skill_table[sn].skill_level[ch->class] && !IS_IMMORTAL(ch))
 	||    ch->pcdata->learned[sn] < 1 /* skill is not known */
	||    skill_table[sn].rating[ch->class] == 0)))
	{
	    send_to_char( "You can't practice that.\n\r", ch );
	    return;
	}

	adept = IS_NPC(ch) ? 100 : class_table[ch->class].skill_adept;

	if ( ch->pcdata->learned[sn] >= adept )
	{
	    sprintf( buf, "You are already learned at %s.\n\r",
		skill_table[sn].name );
	    send_to_char( buf, ch );
            return;
	}
	else
	{
	    ch->practice--;
	    ch->pcdata->learned[sn] += 
		int_app[get_curr_stat(ch,STAT_INT)].learn / 
	        skill_table[sn].rating[ch->class];
	    if ( ch->pcdata->learned[sn] < adept )
	    {
		act( "You practice $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR );
		act( "$n practices $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM );
	    }
	    else
	    {
		ch->pcdata->learned[sn] = adept;
		act( "You are now learned at $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR );
		act( "$n is now learned at $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM );
	    }
	}
    }
    return;
}



/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int wimpy;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	wimpy = ch->max_hit / 5;
    else
	wimpy = atoi( arg );

    if ( wimpy < 0 )
    {
	send_to_char( "Your courage exceeds your wisdom.\n\r", ch );
	return;
    }

    if ( wimpy > ch->max_hit/2 )
    {
	send_to_char( "Such cowardice ill becomes you.\n\r", ch );
	return;
    }

    ch->wimpy	= wimpy;
    sprintf( buf, "Wimpy set to %d hit points.\n\r", wimpy );
    send_to_char( buf, ch );
    return;
}



void do_password( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;

    if ( IS_NPC(ch) )
	return;

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
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
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
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
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: password <old> <new>.\n\r", ch );
	return;
    }

    if ( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
	WAIT_STATE( ch, 40 );
	send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
	return;
    }

    if ( strlen(arg2) < 5 )
    {
	send_to_char(
	    "New password must be at least five characters long.\n\r", ch );
	return;
    }

    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = crypt( arg2, ch->name );
    for ( p = pwdnew; *p != '\0'; p++ )
    {
	if ( *p == '~' )
	{
	    send_to_char(
		"New password not acceptable, try again.\n\r", ch );
	    return;
	}
    }

    free_string( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwdnew );
    save_char_obj( ch );
    send_to_char( "Ok.\n\r", ch );
    return;
}

void do_languages( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_STRING_LENGTH];
    int lang;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {

	send_to_char("Language     Skill\n\r",ch);
        for(lang = 0; lang < MAX_LANGUAGE; lang++)
        {
            if(ch->language_known[lang])
            {
		sprintf(buf,"%-10s    %3d%%\n\r",
                    language_table[lang].name,ch->language_known[lang]);
		send_to_char(buf,ch);
            }
        }

	send_to_char("\n\rOther otions:\n\r",ch);
	send_to_char("language primary <language>\n\r",ch);
	send_to_char("language learn <language>\n\r",ch);

        return;
    }

    if(!str_prefix(arg,"primary"))
    {
        if(argument[0] == '\0')
        {
            send_to_char("Syntax: language primary <language>\n\r",ch);
            return;
        }
        if( (lang = language_lookup(argument)) < 0 )
        {
            send_to_char("There's no such language.\n\r",ch);
            send_to_char("Languages:\n\r",ch);
            for(lang = 0; lang < MAX_LANGUAGE; lang++)
            {
                sprintf(buf," - %s\n\r",language_table[lang].lname);
                send_to_char(buf,ch);
            }
            return;
        }

        ch->primary_lang = lang;

        sprintf(buf,"You will now speak in %s.\n\r",
                      language_table[lang].lname);
        send_to_char(buf,ch);
        return;

    }

    if(!str_prefix(arg,"learn"))
    {
        if(argument[0] == '\0')
        {
            send_to_char("Syntax: language learn <language>\n\r",ch);
            return;
        }
        if( (lang = language_lookup(argument)) < 0 )
        {
            send_to_char("There's no such language.\n\r",ch);
            send_to_char("Languages:\n\r",ch);
            for(lang = 0; lang < MAX_LANGUAGE; lang++)
            {
                sprintf(buf," - %s",language_table[lang].lname);
                send_to_char(buf,ch);
            }
            return;
        }

        send_to_char("Learning is not implemented at this time.\n\r",ch);
        return;
    }

    send_to_char("Syntax:\n\r",ch);
    send_to_char("language primary <language>\n\r",ch);
    send_to_char("language learn <language>\n\r",ch);
    return;

}

void do_lore(CHAR_DATA *ch, char *argument)
{

    OBJ_DATA *obj;
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;


    if ( argument[0] == '\0' )
    {
        send_to_char( "What object do you wish to read lore from?\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_carry( ch, argument ) ) == NULL )
    {
        send_to_char( "You are not carrying that.\n\r", ch );
        return;
    }

    if ( obj->level > ch->level ||
	get_skill(ch,gsn_lore) -
	UMAX(ch->level - obj->level, 0) < number_percent() )
    {
        send_to_char("You fail to decipher the nature of the object.\n\r",ch);
        return;
    }

    sprintf( buf,
	"Object '%s' is type %s, extra flags %s.\n\rWeight is %d, value is %d, level is %d.\n\r",

	obj->name,
	item_type_name( obj ),
	extra_bit_name( obj->extra_flags ),
	obj->weight / 10,
	obj->cost,
	obj->level
	);
    send_to_char( buf, ch );

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
	sprintf( buf, "Has %d charges of level %d",
	    obj->value[2], obj->value[0] );
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
		
    case ITEM_WEAPON:
 	send_to_char("Weapon type is ",ch);
	switch (obj->value[0])
	{
	    case(WEAPON_EXOTIC) : send_to_char("exotic.\n\r",ch);	break;
	    case(WEAPON_SWORD)  : send_to_char("sword.\n\r",ch);	break;	
	    case(WEAPON_DAGGER) : send_to_char("dagger.\n\r",ch);	break;
	    case(WEAPON_SPEAR)	: send_to_char("spear/staff.\n\r",ch);	break;
	    case(WEAPON_MACE) 	: send_to_char("mace/club.\n\r",ch);	break;
	    case(WEAPON_AXE)	: send_to_char("axe.\n\r",ch);		break;
	    case(WEAPON_FLAIL)	: send_to_char("flail.\n\r",ch);	break;
	    case(WEAPON_WHIP)	: send_to_char("whip.\n\r",ch);		break;
	    case(WEAPON_POLEARM): send_to_char("polearm.\n\r",ch);	break;
	    default		: send_to_char("unknown.\n\r",ch);	break;
 	}
	if (obj->clan)
	{
	    sprintf( buf, "Damage is variable.\n\r");
	} else
	{
	    if (obj->pIndexData->new_format)
		sprintf(buf,"Damage is %dd%d (average %d).\n\r",
		obj->value[1],obj->value[2],
		(1 + obj->value[2]) * obj->value[1] / 2);
	    else
		sprintf( buf, "Damage is %d to %d (average %d).\n\r",
	    	obj->value[1], obj->value[2],
	    	( obj->value[1] + obj->value[2] ) / 2 );
	}
	send_to_char( buf, ch );
        if (obj->value[4])  /* weapon flags */
        {
            sprintf(buf,"Weapons flags: %s\n\r",weapon_bit_name(obj->value[4]));
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
	    "Armor class is %d pierce, %d bash, %d slash, and %d vs. magic.\n\r", 
	    obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
	}
	send_to_char( buf, ch );
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
    if (!obj->enchanted)
    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	    sprintf( buf, "Affects %s by %d.\n\r",
		affect_loc_name( paf->location ), paf->modifier );
	    send_to_char(buf,ch);
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
	        send_to_char( buf, ch );
	    }
	}
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	    sprintf( buf, "Affects %s by %d",
	    	affect_loc_name( paf->location ), paf->modifier );
	    send_to_char( buf, ch );
            if ( paf->duration > -1)
                sprintf(buf,", %d hours.\n\r",paf->duration);
            else
                sprintf(buf,".\n\r");
	    send_to_char(buf,ch);
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
		    case TO_WEAPON:
			sprintf(buf,"Adds %s weapon flags.\n",
			    weapon_bit_name(paf->bitvector));
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
    }

}

void do_nwho ( CHAR_DATA *ch, char *argument )
{

    bool lswivel = FALSE;
    BUFFER *buffer = new_buf();
    char buf[MAX_STRING_LENGTH];
    char mess1[MAX_STRING_LENGTH] = "", mess2[MAX_STRING_LENGTH] = "";
    int icount = 0, pcount = 0, notecount;
    extern char *whomessage;

    send_to_char( "\n\r" "{g                    _ _" "\n\r",ch);

    lswivel = buf_who_list(ch, argument, buffer, FALSE, TRUE, &icount);

    if(icount)
    {
        send_to_char(
        "{c.----- {DImmortals {c---" "{y\\{g+{y/{c"
        "---------------------------------------------" "{y*{c"
        "---------.\n\r",ch);
    }
    else
        send_to_char(
        "{c.-----  {DMortals  {c---" "{y\\{g+{y/{c"
        "---------------------------------------------" "{y*{c"
        "--------.\n\r",ch);

    page_to_char(buf_string(buffer),ch);
    clear_buf(buffer);

    buf_who_list(ch, argument, NULL, lswivel, FALSE, &pcount);

    if(icount && pcount)
    {
        sprintf(buf,"{c -=-=-  {DMortals{c  -=-%s"
        "{c-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-{y/{c-=-=-=-=-"
        "\n\r", lswivel ? "{g({y+{c=" : "{c={y+{g)");
        send_to_char(buf,ch);
	lswivel = !(lswivel);
    }

    pcount = 0;

    buf_who_list(ch, argument, buffer, lswivel, FALSE, &pcount);
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);

    send_to_char(
        "{c`-------------------"
        "'{g^{c`"
        "---------------------------------------------"
        "{y*{c"
        "---------'{x"
    "\n\r", ch);

    if(pcount > max_on)
    {
        max_on = pcount;
    }

    notecount = total_unread_notes(ch);

    if(notecount > 0)
    {
        sprintf(mess2, "      {gYou have {R%d{g unread note%s.{x",
                notecount, (notecount > 1) ? "s" : "");
    }
    if(global_quest)
    {
        sprintf(mess1, "   {YAn Immortal quest is in progress.{x");
        sprintf(mess2, "       {YType {yiquest{Y to join in.{x");
    }

    if(arena.status == ARENA_STATUS_GAME)
    {
        sprintf(mess1, "           {RRed Team{x:   {R%d{x",
                arena.teams[0].score);
        sprintf(mess2, "           {GGreen Team{x: {G%d{x",
                arena.teams[1].score);
    }

    sprintf(buf,"\n\r{BPlayers: {M%2d   {BImmortals: {M%d{x"
            "         %s\n\r"
            "{BMost players on at one time: {M%2d{x"
            "    %s\n\r",

            pcount, icount,
            mess1,
            max_on,
            mess2);
    send_to_char(buf,ch);


    if(whomessage && whomessage[0] != '\0')
    {
        sprintf(buf,"\n\r{m*{GANNOUNCEMENT{m*{D:{x\n\r%s",whomessage);
        send_to_char(buf,ch);
    }
}

bool buf_who_list(CHAR_DATA *ch, char *argument, BUFFER *buffer,
                  bool lswivel, bool immso, int *counter)
{
    DESCRIPTOR_DATA *d;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;
        char buf[MAX_STRING_LENGTH]		= "";
        char wstring[MAX_STRING_LENGTH]		= "";
        char nstring[MAX_STRING_LENGTH]		= "";
        char nflags[MAX_STRING_LENGTH]		= "";
        char nfstring[MAX_STRING_LENGTH]	= "";
        char clstring[MAX_STRING_LENGTH]	= "";
        char wscstring[MAX_STRING_LENGTH]	= "";
        char wsstring[MAX_STRING_LENGTH]	= "";

        if ( !d->character || (d->connected != CON_PLAYING
	     && d->connected != CON_NOTE_TO
	     && d->connected != CON_NOTE_TEXT
	     && d->connected != CON_NOTE_EXPIRE
	     && d->connected != CON_NOTE_SUBJECT)
             || !can_see_global( ch, d->character ) )
            continue;

        wch = ( d->original != NULL ) ? d->original : d->character;

        if ( IS_NPC(wch) ) continue;
        if ( (immso && wch->level <= LEVEL_HERO)
           || (!immso && wch->level > LEVEL_HERO) ) continue;

        if(counter) (*counter)++;
	if(!buffer) continue;

        if(wch->clan)
            sprintf(clstring, "%s", clan_table[wch->clan].who_name);

        if(wch->in_arena)
            sprintf(clstring, arena.teams[wch->arena_team].name);

        if(wch->pcdata->who_descr[0] == '\0')
        {
           sprintf(wstring, "%6.6s %-7.7s",
                pc_race_table[wch->race].who_name,
                class_table[wch->class].who_name);
        } else sprintf(wstring, "%s{x", wch->pcdata->who_descr);

        if(IS_SET(wch->comm, COMM_AFK)) strcat(nflags, "AFK ");
        if ( d->character && d->connected &&
	     ( d->connected == CON_NOTE_TO
	     || d->connected == CON_NOTE_TEXT
	     || d->connected == CON_NOTE_EXPIRE
	     || d->connected == CON_NOTE_SUBJECT))
            strcat(nflags, "WrNote ");

        if(wch->in_arena) strcat(nflags, "{RArena ");
        if(IS_SET(wch->act, PLR_TWIT)) strcat(nflags, "{RTwit ");
        if(wch->on_quest && global_quest) strcat(nflags, "{GQ ");

        if(IS_IMMORTAL(ch))
        {
            if(IS_SET(wch->comm, COMM_NOCHANNELS)) strcat(nflags, "{BNoChan ");
            if(IS_SET(wch->act, PLR_FREEZE)) strcat(nflags, "{BFrozen ");
        }

        if(IS_IMMORTAL(ch))
        {
            if(wch->invis_level >= LEVEL_HERO
               && get_trust(ch) >= get_trust(wch))
                strcat(nflags, "{YW");

            if(wch->incog_level >= LEVEL_HERO
               && get_trust(ch) >= get_trust(wch))
                strcat(nflags, "{YI");

            if(wch->ghost_level >= LEVEL_HERO
               && get_trust(ch) >= get_trust(wch))
                strcat(nflags, "{YG");
        }

        if( nflags[0] != '\0'
            && nflags[strlen(nflags) - 1] == ' ')
            nflags[strlen(nflags) - 1] = '\0';

        if(nflags[0] != '\0')
            sprintf(nfstring, " {m[{M%s{m]", nflags);


        sprintf(nstring,"%s{x%s%s", wch->name,
            ( strlen(wch->name) + nc_strlen(nfstring) +
                nc_strlen(wch->pcdata->title) <= WHO_TITLE_WIDTH)
            ? wch->pcdata->title : "",
            nfstring );

        if(WHO_TITLE_WIDTH - nc_strlen(nstring) > 0)
        {
            sprintf(wscstring, "%%%ds", WHO_TITLE_WIDTH - nc_strlen(nstring) );
            sprintf(wsstring, wscstring, "" );
        }

        sprintf(buf," {C%3d {R%s %s {W%s%s {y/%s{G%s{x\n\r",
            wch->level,
            wstring,
            (lswivel) ? "{g({y| " : " {y|{g)",
            nstring,
            wsstring,
	    (is_leader(wch) && !wch->in_arena) ? "{m*": " ",
            clstring
           );

        add_buf( buffer, buf );

        lswivel = !(lswivel);


    }

    return lswivel;
}


void do_nscore( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char hourbuf[MAX_STRING_LENGTH];
   char titlebuf[MAX_STRING_LENGTH]; 
   int x, mmtest;

    if ( argument[0] != '\0' && is_number(argument) )
      mmtest = abs( atoi(argument) );
    else
      mmtest = 10;

    sprintf( buf, "\n\r      /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/~~\\\n\r");
    send_to_char( buf, ch);
    sprintf(titlebuf,"%s%s",
                IS_NPC(ch) ? ch->short_descr : ch->name,
		IS_NPC(ch) ? "the NPC" : ch->pcdata->title);


    /* Was: 45/47 */

    if ( strlen(titlebuf) >= 34 )
      titlebuf[34] = '\0';
    else
      for ( x = 36 - strlen(titlebuf); x > 0; x-- )
        strcat( titlebuf, " ");


    if ( mmtest < 9999 )
      sprintf(hourbuf, "%3d years old (%d hours)", get_age(ch), mmtest);
    else if ( mmtest == 1 )
      sprintf(hourbuf, "%3d years old (1 hour)", get_age(ch));
    else
      sprintf(hourbuf, "%3d years old (9999+ hour)", get_age(ch));

    sprintf( buf, 
"     |   %s %26s |____|\n\r",
		titlebuf,hourbuf);
    send_to_char( buf, ch);
    sprintf( buf, "     |+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+|\n\r");
    send_to_char( buf, ch);
    sprintf( buf, 
"     | Level:  %3d          |  Str:  %2d(%2d)  | Religion  :  %-10s |\n\r",
		ch->level,ch->perm_stat[STAT_STR],get_curr_stat(ch,STAT_STR),
		"none");
    send_to_char( buf, ch); 
    sprintf( buf, 
"     | Race :  %-11s  |  Int:  %2d(%2d)  | Practice  :  %-4d       |\n\r",
		race_table[ch->race].name,
		ch->perm_stat[STAT_INT], get_curr_stat(ch,STAT_INT),
		ch->practice);
    send_to_char(buf, ch);

    sprintf( buf, 
"     | Sex  :  %-11s  |  Wis:  %2d(%2d)  | Train     :  %-4d       |\n\r",
	ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female",
	ch->perm_stat[STAT_WIS], get_curr_stat(ch,STAT_WIS),ch->train);
    send_to_char(buf, ch);

    sprintf( buf, 
"     | Class:  %-12s |  Dex:  %2d(%2d)  | Quest Pnts:  %-4d       |\n\r",
		IS_NPC(ch) ? "mobile" : class_table[ch->class].name,
		ch->perm_stat[STAT_DEX], get_curr_stat(ch,STAT_DEX),
		IS_NPC(ch) ? 0 : ch->pcdata->questpoints );
    send_to_char(buf, ch);

    return;
}


void do_message( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) ) return;

    if ( argument[0] != '\0' )
    {
        DESCRIPTOR_DATA *d;
	buf[0] = '\0';

	smash_tilde( argument );

        if (!str_cmp(argument,"whomessage"))
        {
            extern char *whomessage;
            free_string(whomessage);
            whomessage = str_dup(ch->pcdata->csmsg);
            send_to_char("Ok.\n\r",ch);
            return;
        }

        if (!str_cmp(argument,"global"))
        {

            for ( d = descriptor_list; d; d = d->next )
            {
                if ( d->connected != CON_PLAYING ) continue;

                if (d->character->level >= ch->level)
                    send_to_char( "global>\n\r",d->character);

                send_to_char( ch->pcdata->csmsg, d->character );
                send_to_char( "\n\r",   d->character );
            }

            return;
        }

        if (!str_cmp(argument,"room"))
        {
            CHAR_DATA *rch;

            for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
            {
                if (rch->level >= ch->level)
                    send_to_char( "local>\n\r",rch);
                send_to_char(ch->pcdata->csmsg,rch);
            }

            return;
        }

    	if (argument[0] == '-')
    	{
            int len;
            bool found = FALSE;
 
            if (ch->pcdata->csmsg == NULL || ch->pcdata->csmsg[0] == '\0')
            {
                send_to_char("No lines left to remove.\n\r",ch);
                return;
            }
	
  	    strcpy(buf,ch->pcdata->csmsg);
 
            for (len = strlen(buf); len > 0; len--)
            {
                if (buf[len] == '\r')
                {
                    if (!found)  /* back it up */
                    {
                        if (len > 0)
                            len--;
                        found = TRUE;
                    }
                    else /* found the second one */
                    {
                        buf[len + 1] = '\0';
			free_string(ch->pcdata->csmsg);
			ch->pcdata->csmsg = str_dup(buf);
			send_to_char( "Your message is:\n\r", ch );
			send_to_char( ch->pcdata->csmsg ?
                            ch->pcdata->csmsg : 
			    "(None).\n\r", ch );
                        return;
                    }
                }
            }
            buf[0] = '\0';
	    free_string(ch->pcdata->csmsg);
	    ch->pcdata->csmsg = str_dup(buf);
	    send_to_char("Message cleared.\n\r",ch);
	    return;
        }
	if ( argument[0] == '+' )
	{
	    if ( ch->pcdata->csmsg != NULL )
		strcat( buf, ch->pcdata->csmsg );
	    argument++;
	    while ( isspace(*argument) )
		argument++;
	}

	if ( strlen(buf) + strlen(argument) >= MAX_STRING_LENGTH - 4 )
	{
	    send_to_char( "Message too long.\n\r", ch );
	    return;
	}

	strcat( buf, argument );
	strcat( buf, "\n\r" );
	free_string( ch->pcdata->csmsg );
	ch->pcdata->csmsg = str_dup( buf );
    }

    send_to_char( "Your message is:\n\r", ch );
    send_to_char( ch->pcdata->csmsg ? ch->pcdata->csmsg
                      : "(None).\n\r", ch );
    return;
}


/* show_spells by Kyndig */
void show_spells_to_char( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char name[MAX_STRING_LENGTH];
    char nperson[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH]; 
    char buf3[MAX_STRING_LENGTH];
    int shcount = 0;
    int count = 0;

    buf[0] = '\0';
    buf2[0] = '\0';

/* Use he/she/it as shown after rather than name

    if ( IS_NPC( victim ) )
      strcpy( name, victim->short_descr );
    else
      strcpy( name, victim->name);
    name[0] = toupper(name[0]);
*/


/* Quick hack */
#define LA_SPACE "   "

    strcpy( name, (victim->sex == 0 ? "It" : (victim->sex == 1 ? "He" : "She")));
    strcpy( nperson, (victim->sex == 0 ? "it" : (victim->sex == 1 ? "him" : "her")));

    if(!IS_NPC(ch) && IS_SET(ch->comm, COMM_LONG))
    {

       if ( IS_SHIELDED(victim, SHD_SANCTUARY) )
       {
            if ( IS_GOOD(victim) )
               sprintf(buf,"%s glows with an aura of divine radiance.\n\r", name );
            else if ( IS_EVIL(victim) )
               sprintf(buf, "%s shimmers beneath an aura of dark energy.\n\r", name );
            else
               sprintf( buf, "%s is shrouded in flowing shadow and light.\n\r", name );

          strcat(buf2, LA_SPACE);
          strcat(buf2, buf);
       }

       if ( IS_AFFECTED(victim, AFF_FLYING) )
       {
          sprintf(buf, "%s floats upon thin currents of wind.\n\r", name);
          strcat(buf2, LA_SPACE);
          strcat(buf2, buf);
       }
 
       if ( IS_AFFECTED(victim, AFF_CHARM)
            && (!IS_NPC(victim) || !IS_SET(victim->act, ACT_PET)))
       {
          sprintf(buf, "%s wanders in a dazed, zombie-like state.\n\r", name );
          strcat(buf2, LA_SPACE);
          strcat(buf2, buf);
       }

       /* Shield shows by Thyrr */

       if ( IS_SHIELDED(victim, SHD_ICE)    )    shcount++;
       if ( IS_SHIELDED(victim, SHD_FIRE)   )    shcount++;
       if ( IS_SHIELDED(victim, SHD_VACILLATING) )    shcount++;
       if ( IS_SHIELDED(victim, SHD_SHOCK)  )    shcount++;
       if ( IS_SHIELDED(victim, SHD_MANA)  )    shcount++;

       if(shcount > 0)
       {
          count = 0;

          sprintf(buf,"%s has",name);

          if ( IS_SHIELDED(victim, SHD_ICE) )
          {
             strcat(buf, " {Cice{x");
             count++;
             strcat(buf, count == shcount ? " " : (count == (shcount-1) ? ( shcount != 2 ? ", and" : " and") : ","));

          }

          if ( IS_SHIELDED(victim, SHD_FIRE) )
          {
             strcat(buf, " {Rfire{x");
             count++;
             strcat(buf, count == shcount ? " " : (count == (shcount-1) ? ( shcount != 2 ? ", and" : " and") : ","));

          }

          if ( IS_SHIELDED(victim, SHD_VACILLATING) )
          {
             strcat(buf, " {Menergy{x");
             count++;
             strcat(buf, count == shcount ? " " : (count == (shcount-1) ? ( shcount != 2 ? ", and" : " and"): ","));

          }

          if ( IS_SHIELDED(victim, SHD_SHOCK) )
          {
             strcat(buf,  " {Bstatic{x");
             count++;
             strcat(buf, count == shcount ? " " : (count == (shcount-1) ? ( shcount != 2 ? ", and" : " and") : ","));
          }

          if ( IS_SHIELDED(victim, SHD_MANA) )
          {
             strcat(buf,  " {Wmana{x");
             count++;
             strcat(buf, count == shcount ? " " : (count == (shcount-1) ? ( shcount != 2 ? ", and" : " and") : ","));
          }

          sprintf(buf3, "pulsating around %s.\n\r", nperson);

          strcat(buf, buf3);
          strcat(buf2, LA_SPACE);
          strcat(buf2, buf);


       }

    }


    if(buf2[0] != '\0')
    {

/*       send_to_char("   ",ch);*/
       send_to_char(buf2,ch);

    }


  return;
}

void do_glance(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH] = "";
    int percent;
    CHAR_DATA *victim;

    if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }

    if ( victim->max_hit > 0 )
        percent = ( 100 * victim->hit ) / victim->max_hit;
    else 
        percent = -1;

    strcpy( buf, PERS(victim, ch) );

    if (percent >= 100) 
	strcat(buf, " {fis in excellent condition.{x\n\r");
    else if (percent >= 90) 
	strcat( buf, " {fhas a few scratches.{x\n\r");
    else if (percent >= 75) 
	strcat( buf," {fhas some small wounds and bruises.{x\n\r");
    else if (percent >=  50) 
	strcat( buf, " {fhas quite a few wounds.{x\n\r");
    else if (percent >= 30)
	strcat( buf, " {fhas some big nasty wounds and scratches.{x\n\r");
    else if (percent >= 15)
	strcat ( buf, " {flooks pretty hurt.{x\n\r");
    else if (percent >= 0 )
	strcat (buf, " {fis in awful condition.{x\n\r");
    else
	strcat(buf, " {fis bleeding to death.{x\n\r");
    
    send_to_char(buf,ch);

}

void do_autoaway(CHAR_DATA *ch, char *argument)
{
    int num;
    char buf [MAX_STRING_LENGTH];

    if(IS_NPC(ch)) return;

    if(argument[0] == '\0')
    {
        if(ch->pcdata->auto_away > 0)
        {
            char buf[MAX_STRING_LENGTH];

            sprintf(buf, "You will be marked AFK after %d minutes of inactivity.\n\r",
                    ch->pcdata->auto_away);
            send_to_char(buf,ch);
        }
        else
        {
            send_to_char("AutoAFK is currently off.\n\r",ch);
            send_to_char("To enable it, type autoafk <minutes idle>\n\r",ch);
        }
        return;
    }

    num = atoi(argument);

    if(num < 0 || num > 10)
    {
        send_to_char("Time must be between 0 (off) and 10 minutes.\n\r",ch);
        return;
    }

    ch->pcdata->auto_away = num;

    sprintf(buf,"Auto AFK set to %d minutes.\n\r", num);
    send_to_char(buf,ch);

    return;
}

void do_pkcount(CHAR_DATA *ch, char *argument)
{
    PK_COUNT_DATA *pkc, *total = NULL;
    char buf[MAX_STRING_LENGTH];
    char *stime;

    if(IS_NPC(ch)) return;

    pkc = ch->pcdata->pk_counter;

    if(!pkc)
    {
        send_to_char("You haven't PKed anybody (on record)\n\r",ch);
        return;
    }

    send_to_char("{BPlayer/Clan      Killed  |  Deaths By  |  Ties"
                      "  |  Last Scored Fight{x\n\r",ch);

    for(; pkc; pkc = pkc->next)
    {
        stime = ctime(&pkc->last_fight);
        stime[strlen(stime)-1] = '\0';

        if(pkc->type == PK_TOTAL)
        {
            total = pkc;
            continue;
        }

        sprintf(buf,"%-16s %6d  {B|{x  %9d  {B|{x  %3d   {B|{x  %s\n\r",
            pkc->name, pkc->wins, pkc->losses, pkc->ties,
            stime);
        send_to_char(buf,ch);
    }

    if(total)
    {
        stime = ctime(&total->last_fight);
        stime[strlen(stime)-1] = '\0';

       send_to_char("                         {B|"
                    "             |        |{x",ch);
       sprintf(buf,"\n\r%-16s %6d  {B|{x  %9d  {B|{x  %3d   {B|{x  %s\n\r",
        "Total", total->wins, total->losses, total->ties,
        stime);

        send_to_char(buf,ch);
    }

}

void do_hometown(CHAR_DATA *ch, char *argument)
{
    if(argument[0] == '\0')
    {
        send_to_char("Syntax: hometown <city>\n\r\n\r",ch);
        send_to_char("Valid choices: Midgaard, New Thalos\n\r",ch);
        return;
    }

    send_to_char("\n\r",ch);
    return;
}

void do_delay( CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if (IS_NPC(ch))
        return;
  
    if (arg[0] == '\0')
    {
	if (IS_SET(ch->act, PLR_WAIT))
	{
	   send_to_char("Delay display is now OFF.\n\r", ch);
	   REMOVE_BIT(ch->act, PLR_WAIT);
        }
	else
	{
	   send_to_char("Delay display is now ON.\n\r", ch);
	   SET_BIT(ch->act, PLR_WAIT);
        }
    }
	

    if ( !str_cmp( arg, "on" ))
    {
	send_to_char("Delay display is now ON.\n\r", ch);
	SET_BIT(ch->act, PLR_WAIT);
	return;
    }
    else if ( !str_cmp( arg, "off" ))
    {
	send_to_char("Delay display is now OFF.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_WAIT);
	return;
    }
    else if ( !str_cmp( arg, "status" ))
    {
	send_to_char("Your delay status is currently", ch);
	if (IS_SET(ch->act, PLR_WAIT))
	    send_to_char(" ON.\n\r", ch);
	else
	    send_to_char(" OFF.\n\r", ch);
	   
	return;
    }
}
