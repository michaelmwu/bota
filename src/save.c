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
/*#include <sys/malloc.h>*/
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "interp.h"
 
#if !defined(macintosh)
extern  int     _filbuf         args( (FILE *) );
#endif


int rename(const char *oldfname, const char *newfname);

char *print_flags(int flag)
{
    int count, pos = 0;
    static char buf[52];


    for (count = 0; count < 32;  count++)
    {
        if (IS_SET(flag,1<<count))
        {
            if (count < 26)
                buf[pos] = 'A' + count;
            else
                buf[pos] = 'a' + (count - 26);
            pos++;
        }
    }

    if (pos == 0)
    {
        buf[pos] = '0';
        pos++;
    }

    buf[pos] = '\0';

    return buf;
}


/*
 * Array of containers read for proper re-nesting of objects.
 */
#define MAX_NEST	100
static	OBJ_DATA *	rgObjNest	[MAX_NEST];



/*
 * Local functions.
 */
void	fwrite_char	args( ( CHAR_DATA *ch,  FILE *fp ) );
void	fwrite_room	args( ( ROOM_INDEX_DATA *room,  FILE *fp ) );
void	fwrite_obj	args( ( CHAR_DATA *ch,  OBJ_DATA  *obj,
			    FILE *fp, int iNest ) );
void	fwrite_pet	args( ( CHAR_DATA *pet, FILE *fp) );
void	fread_char	args( ( CHAR_DATA *ch,  FILE *fp ) );
void    fread_pet	args( ( CHAR_DATA *ch,  FILE *fp ) );
void	fread_obj	args( ( void *loadto,  FILE *fp, bool isroom ) );
void	save_area_room_obj	args( (AREA_DATA *area, FILE *fp) );

void	fread_bank_account	args( (CHAR_DATA *ch, FILE *fp) );
void	fwrite_bank_account	args( (BANK_ACCOUNT_DATA *account, FILE *fp) );
void	fwrite_pk_counter(PK_COUNT_DATA *pkc, FILE *fp);
PK_COUNT_DATA	*fread_pk_counter(FILE *fp);


/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
void save_char_obj( CHAR_DATA *ch )
{
    char strsave[MAX_INPUT_LENGTH];
    FILE *fp;

    if ( IS_NPC(ch) )
	return;

    if ( ch->in_arena )
        return;

    if ( ch->desc != NULL && ch->desc->original != NULL )
	ch = ch->desc->original;

    if (!IS_VALID(ch))
        bug_f("Attempting to save an invalidated char structure!");

    if ( !ch->pcdata->pwd
       || ch->pcdata->pwd[0] != ch->name[0]
       || ch->pcdata->pwd[1] != ch->name[1] )
    {
        send_to_char("{RWarning! Possible password corruption detected!\n\r"
                     "Please report immediately!\n\r{x",ch);
        bug_f("Invalid password crypt salt for %s!", ch->name);
        return;
    }

#if defined(unix)
    /* create god log */
    if (IS_IMMORTAL(ch) || ch->level >= LEVEL_IMMORTAL)
    {
	fclose(fpReserve);
	sprintf(strsave, "%s%s",GOD_DIR, capitalize(ch->name));
	if ((fp = fopen(strsave,"w")) == NULL)
	{
	    bug("Save_char_obj: fopen",0);
	    perror(strsave);
 	}
        else
        {
	    fprintf(fp,"Lev %2d Trust %2d  %s%s\n",
	        ch->level, get_trust(ch), ch->name, ch->pcdata->title);
            fclose( fp );
        }

	fpReserve = fopen( NULL_FILE, "r" );
    }
#endif

    fclose( fpReserve );
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
    if ( ( fp = fopen( TEMP_FILE, "w" ) ) == NULL )
    {
	bug( "Save_char_obj: fopen", 0 );
	perror( strsave );
    }
    else
    {
	fwrite_char( ch, fp );
	if ( ch->carrying != NULL )
	    fwrite_obj( ch, ch->carrying, fp, 0 );
	/* save the pets */
	if (ch->pet != NULL)
	    fwrite_pet(ch->pet,fp);
	if (!IS_NPC(ch) && ch->pcdata->bank_account )
	    fwrite_bank_account(ch->pcdata->bank_account,fp);
	if (!IS_NPC(ch) && ch->pcdata->pk_counter )
	    fwrite_pk_counter(ch->pcdata->pk_counter,fp);
	fprintf( fp, "#END\n" );
    }
    fclose( fp );
    rename(TEMP_FILE,strsave);
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}



/*
 * Write the char.
 */
void fwrite_char( CHAR_DATA *ch, FILE *fp )
{
    AFFECT_DATA *paf;
    GRANT_DATA *gran;
    int sn, gn, pos, lang, i;

    fprintf( fp, "#%s\n", IS_NPC(ch) ? "MOB" : "PLAYER"	);

    fprintf( fp, "Name %s~\n",	ch->name		);
    fprintf( fp, "Id   %ld\n", ch->id			);
    fprintf( fp, "LogO %ld\n",
            (ch->pseudo_login)?ch->llogoff:current_time);

    fprintf( fp, "Vers %d\n",   7			);
    if (ch->short_descr[0] != '\0')
      	fprintf( fp, "ShD  %s~\n",	ch->short_descr	);
    if( ch->long_descr[0] != '\0')
	fprintf( fp, "LnD  %s~\n",	ch->long_descr	);
    if (ch->description[0] != '\0')
    	fprintf( fp, "Desc %s~\n",	ch->description	);
    if (ch->prompt != NULL || !str_cmp(ch->prompt,"<%h/%Hhp %m/%Mm %v/%Vmv %Xtnl> "))
        fprintf( fp, "Prom %s~\n",      ch->prompt  	);
    fprintf( fp, "Race %s~\n", pc_race_table[ch->race].name );
    if (ch->clan)
    {
	    fprintf( fp,   "Clan %s~\n",clan_table[ch->clan].name);

	    fprintf( fp,   "Rank %d\n", ch->rank                );
    }
    if (ch->color != 0)
	fprintf( fp, "Color %d\n", ch->color);
    if (ch->color_auc != 0)
	fprintf( fp, "Coauc %d\n", ch->color_auc);
    if (ch->color_cgo != 0)
	fprintf( fp, "Cocgo %d\n", ch->color_cgo);
    if (ch->color_cla != 0)
	fprintf( fp, "Cocla %d\n", ch->color_cla);
    if (ch->color_con != 0)
	fprintf( fp, "Cocon %d\n", ch->color_con);
    if (ch->color_dis != 0)
	fprintf( fp, "Codis %d\n", ch->color_dis);
    if (ch->color_fig != 0)
	fprintf( fp, "Cofig %d\n", ch->color_fig);
    if (ch->color_gos != 0)
	fprintf( fp, "Cogos %d\n", ch->color_gos);
    if (ch->color_gra != 0)
	fprintf( fp, "Cogra %d\n", ch->color_gra);
    if (ch->color_gte != 0)
	fprintf( fp, "Cogte %d\n", ch->color_gte);
    if (ch->color_imm != 0)
	fprintf( fp, "Coimm %d\n", ch->color_imm);
    if (ch->color_mob != 0)
	fprintf( fp, "Comob %d\n", ch->color_mob);
    if (ch->color_mus != 0)
	fprintf( fp, "Comus %d\n", ch->color_mus);
    if (ch->color_opp != 0)
	fprintf( fp, "Coopp %d\n", ch->color_opp);
    if (ch->color_qgo != 0)
	fprintf( fp, "Coqgo %d\n", ch->color_qgo);
    if (ch->color_que != 0)
	fprintf( fp, "Coque %d\n", ch->color_que);
    if (ch->color_quo != 0)
	fprintf( fp, "Coquo %d\n", ch->color_quo);
    if (ch->color_roo != 0)
	fprintf( fp, "Coroo %d\n", ch->color_roo);
    if (ch->color_say != 0)
	fprintf( fp, "Cosay %d\n", ch->color_say);
    if (ch->color_sho != 0)
	fprintf( fp, "Cosho %d\n", ch->color_sho);
    if (ch->color_tel != 0)
	fprintf( fp, "Cotel %d\n", ch->color_tel);
    if (ch->color_wit != 0)
	fprintf( fp, "Cowit %d\n", ch->color_wit);
    if (ch->color_wiz != 0)
	fprintf( fp, "Cowiz %d\n", ch->color_wiz);
    fprintf( fp, "Sex  %d\n",	ch->sex			);
    fprintf( fp, "Clas %s~\n",  class_table[ch->class].name );
    fprintf( fp, "Levl %d\n",	ch->level		);
    if (ch->class < MAX_CLASS/2)
    if (ch->log_level != 0)
	fprintf( fp, "Logl %s~\n",
		(ch->log_level == 9) ? "always" : "simple"	);

    fprintf( fp, "Plyd %d\n",
	ch->played + (int) (current_time - ch->logon)	);

    fprintf( fp, "Scro %d\n", 	ch->lines		);
    fprintf( fp, "Room %d\n",
        (  ch->in_room == get_room_index( ROOM_VNUM_LIMBO )
        && ch->was_in_room != NULL )
            ? ch->was_in_room->vnum
            : ch->in_room == NULL ? 3001 : ch->in_room->vnum );

    fprintf( fp, "HMV  %d %d %d %d %d %d\n",
	ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move );
    if (ch->platinum > 0)
      fprintf( fp, "Plat %ld\n",	ch->platinum		);
    else
      fprintf( fp, "Plat %d\n", 0			); 
    if (ch->gold > 0)
      fprintf( fp, "Gold %ld\n",	ch->gold		);
    else
      fprintf( fp, "Gold %d\n", 0			); 
    if (ch->silver > 0)
	fprintf( fp, "Silv %ld\n",ch->silver		);
    else
	fprintf( fp, "Silv %d\n",0			);
    fprintf( fp, "Exp  %ld\n",	ch->exp			);
    if (ch->qps != 0)
	fprintf( fp, "Qps  %d\n", ch->qps		);

    if (ch->pcdata->questpoints != 0)
        fprintf( fp, "Qaps %d\n", ch->pcdata->questpoints );
    if (ch->pcdata->nextquest != 0)
        fprintf( fp, "Qnxt %d\n", ch->pcdata->nextquest );
    if (ch->pcdata->countdown != 0)
        fprintf( fp, "Qcdn %d\n", ch->pcdata->countdown );
    if (ch->pcdata->questmob != 0)
        fprintf( fp, "Qmob %d\n", ch->pcdata->questmob );
    if (ch->pcdata->questobj != 0)
        fprintf( fp, "Qobj %d\n", ch->pcdata->questobj );
    if (ch->pcdata->questgiver != NULL)
        fprintf( fp, "Qgvr %d\n", ch->pcdata->questgiver->vnum);
    if(!IS_NPC(ch) && ch->pcdata->security)
        fprintf( fp, "Sec  %d\n",    ch->pcdata->security	);

    if(ch->pcdata->arena_won != 0)
	fprintf( fp, "Awon %d\n", ch->pcdata->arena_won );
    if(ch->pcdata->arena_lost != 0)
        fprintf( fp, "Alost %d\n", ch->pcdata->arena_won );

    if (ch->act != 0)
	fprintf( fp, "Act  %s\n",   print_flags(ch->act));
    if (ch->plt != 0)
	fprintf( fp, "Plt  %s\n",   print_flags(ch->plt));
    if (ch->affected_by != 0)
	fprintf( fp, "AfBy %s\n",   print_flags(ch->affected_by));
    if (ch->shielded_by != 0)
	fprintf( fp, "ShBy %s\n",   print_flags(ch->shielded_by));
    fprintf( fp, "Comm %s\n",       print_flags(ch->comm));
    if (ch->comm2 != 0)
    fprintf( fp, "Com2 %s\n",       print_flags(ch->comm2));
    if (ch->wiznet)
    	fprintf( fp, "Wizn %s\n",   print_flags(ch->wiznet));
    if (get_trust(ch) > LEVEL_HERO)
	fprintf( fp, "Tru  %d\n",  get_trust(ch)                 );
    if (ch->invis_level)
	fprintf( fp, "Invi %d\n", 	ch->invis_level	);
    if (ch->incog_level)
	fprintf(fp,"Inco %d\n",ch->incog_level);
    if (ch->ghost_level)
	fprintf(fp,"Ghos %d\n",ch->ghost_level);
    fprintf( fp, "Pos  %d\n",	
	ch->position == POS_FIGHTING ? POS_STANDING : ch->position );
    if (ch->practice != 0)
    	fprintf( fp, "Prac %d\n",	ch->practice	);
    if (ch->train != 0)
	fprintf( fp, "Trai %d\n",	ch->train	);
    if (ch->saving_throw != 0)
	fprintf( fp, "Save  %d\n",	ch->saving_throw);
    fprintf( fp, "Alig  %d\n",	ch->alignment		);
    if (ch->hitroll != 0)
	fprintf( fp, "Hit   %d\n",	ch->hitroll	);
    if (ch->damroll != 0)
	fprintf( fp, "Dam   %d\n",	ch->damroll	);
    fprintf( fp, "ACs %d %d %d %d\n",	
	ch->armor[0],ch->armor[1],ch->armor[2],ch->armor[3]);
    if (ch->wimpy !=0 )
	fprintf( fp, "Wimp  %d\n",	ch->wimpy	);
    fprintf( fp, "Attr %d %d %d %d %d\n",
	ch->perm_stat[STAT_STR],
	ch->perm_stat[STAT_INT],
	ch->perm_stat[STAT_WIS],
	ch->perm_stat[STAT_DEX],
	ch->perm_stat[STAT_CON] );

    fprintf (fp, "AMod %d %d %d %d %d\n",
	ch->mod_stat[STAT_STR],
	ch->mod_stat[STAT_INT],
	ch->mod_stat[STAT_WIS],
	ch->mod_stat[STAT_DEX],
	ch->mod_stat[STAT_CON] );

    for ( lang = 0; lang < MAX_LANGUAGE; lang++ )
    {
        if (ch->language_known[lang])
        {
            fprintf( fp, "Lang %d '%s'\n",
                ch->language_known[lang],language_table[lang].name);
        }
    }

    fprintf(fp, "PrimLang '%s'\n",language_table[ch->primary_lang].name);


    if ( IS_NPC(ch) )
    {
	fprintf( fp, "Vnum %d\n",	ch->pIndexData->vnum	);
    }
    else
    {
	fprintf( fp, "Pass %s~\n",	ch->pcdata->pwd		);
	if (ch->pcdata->bamfin[0] != '\0')
	    fprintf( fp, "Bin  %s~\n",	ch->pcdata->bamfin);
	if (ch->pcdata->bamfout[0] != '\0')
		fprintf( fp, "Bout %s~\n",	ch->pcdata->bamfout);
	if (ch->pcdata->who_descr[0] != '\0')
		fprintf( fp, "Whod %s~\n",	ch->pcdata->who_descr);
	fprintf( fp, "Titl %s~\n",	ch->pcdata->title	);
    	fprintf( fp, "Pnts %d\n",   	ch->pcdata->points      );
	fprintf( fp, "TSex %d\n",	ch->pcdata->true_sex	);
        if(ch->pcdata->socket && ch->pcdata->socket[0] != '\0')
	    fprintf( fp, "LastIP %s~\n",ch->pcdata->socket	);
        else
	    fprintf( fp, "LastIP %s~\n",ch->pcdata->last_ip	);
	fprintf( fp, "LLev %d\n",	ch->pcdata->last_level	);
	fprintf( fp, "Reca %d\n",	ch->pcdata->recall	);
	fprintf( fp, "HMVP %d %d %d\n", ch->pcdata->perm_hit, 
						   ch->pcdata->perm_mana,
						   ch->pcdata->perm_move);
	fprintf( fp, "Cnd  %d %d %d %d %d %d\n",
	    ch->pcdata->condition[0],
	    ch->pcdata->condition[1],
	    ch->pcdata->condition[2],
	    ch->pcdata->condition[3],
	    ch->pcdata->condition[4],
	    ch->pcdata->condition[5] );

        fprintf( fp, "AutoAway %d\n", ch->pcdata->auto_away );

        for (gran=ch->pcdata->granted; gran != NULL; gran=gran->next)
            fprintf(fp,"Grant '%s' %d\n",gran->name, gran->duration);

	/* write forgets */
        for (pos = 0; pos < MAX_FORGET; pos++)
	{
	    if (ch->pcdata->forget[pos] == NULL)
		break;

	    fprintf(fp,"Forge %s~\n",ch->pcdata->forget[pos]);
	}

	/* write dupes */
        for (pos = 0; pos < MAX_DUPES; pos++)
	{
	    if (ch->pcdata->dupes[pos] == NULL)
		break;

	    fprintf(fp,"Dupes %s~\n",ch->pcdata->dupes[pos]);
	}

	/* write alias */
        for (pos = 0; pos < MAX_ALIAS; pos++)
	{
	    if (ch->pcdata->alias[pos] == NULL
	    ||  ch->pcdata->alias_sub[pos] == NULL)
		break;

	    fprintf(fp,"Alias %s %s~\n",ch->pcdata->alias[pos],
		    ch->pcdata->alias_sub[pos]);
	}

	/* Save note board status */
	/* Save number of boards in case that number changes */
	fprintf (fp, "Boards       %d ", MAX_BOARD);
	for (i = 0; i < MAX_BOARD; i++)
	    fprintf (fp, "%s %ld ", boards[i].short_name, ch->pcdata->last_note[i]);
	fprintf (fp, "\n");

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name != NULL && ch->pcdata->learned[sn] > 0 )
	    {
		fprintf( fp, "Sk %d '%s'\n",
		    ch->pcdata->learned[sn], skill_table[sn].name );
	    }
	}

	for ( gn = 0; gn < MAX_GROUP; gn++ )
        {
            if ( group_table[gn].name != NULL && ch->pcdata->group_known[gn])
            {
                fprintf( fp, "Gr '%s'\n",group_table[gn].name);
            }
        }


    }

    for ( paf = ch->affected; paf != NULL; paf = paf->next )
    {
	if (paf->type < 0 || paf->type>= MAX_SKILL)
	    continue;
	
	fprintf( fp, "Affc '%s' %3d %3d %3d %3d %3d %10d\n",
	    skill_table[paf->type].name,
	    paf->where,
	    paf->level,
	    paf->duration,
	    paf->modifier,
	    paf->location,
	    paf->bitvector
	    );
    }

    fprintf( fp, "End\n\n" );
    return;
}

/* write a pet */
void fwrite_pet( CHAR_DATA *pet, FILE *fp)
{
    AFFECT_DATA *paf;
    
    fprintf(fp,"#PET\n");
    
    fprintf(fp,"Vnum %d\n",pet->pIndexData->vnum);
    
    fprintf(fp,"Name %s~\n", pet->name);
    fprintf(fp,"LogO %ld\n", current_time);
    if (pet->short_descr != pet->pIndexData->short_descr)
    	fprintf(fp,"ShD  %s~\n", pet->short_descr);
    if (pet->long_descr != pet->pIndexData->long_descr)
    	fprintf(fp,"LnD  %s~\n", pet->long_descr);
    if (pet->description != pet->pIndexData->description)
    	fprintf(fp,"Desc %s~\n", pet->description);
    if (pet->race != pet->pIndexData->race)
    	fprintf(fp,"Race %s~\n", race_table[pet->race].name);
    if (pet->clan)
        fprintf( fp, "Clan %s~\n",clan_table[pet->clan].who_name);
    fprintf(fp,"Sex  %d\n", pet->sex);
    if (pet->level != pet->pIndexData->level)
    	fprintf(fp,"Levl %d\n", pet->level);
    fprintf(fp, "HMV  %d %d %d %d %d %d\n",
    	pet->hit, pet->max_hit, pet->mana, pet->max_mana, pet->move, pet->max_move);
    if (pet->platinum > 0)
    	fprintf(fp,"Plat %ld\n",pet->platinum);
    if (pet->gold > 0)
    	fprintf(fp,"Gold %ld\n",pet->gold);
    if (pet->silver > 0)
	fprintf(fp,"Silv %ld\n",pet->silver);
    if (pet->exp > 0)
    	fprintf(fp, "Exp  %ld\n", pet->exp);
    if (pet->act != pet->pIndexData->act)
    	fprintf(fp, "Act  %s\n", print_flags(pet->act));
    if (pet->affected_by != pet->pIndexData->affected_by)
    	fprintf(fp, "AfBy %s\n", print_flags(pet->affected_by));
    if (pet->shielded_by != pet->pIndexData->shielded_by)
    	fprintf(fp, "ShBy %s\n", print_flags(pet->shielded_by));
    if (pet->comm != 0)
    	fprintf(fp, "Comm %s\n", print_flags(pet->comm));
    fprintf(fp,"Pos  %d\n", pet->position = POS_FIGHTING ? POS_STANDING : pet->position);
    if (pet->saving_throw != 0)
    	fprintf(fp, "Save %d\n", pet->saving_throw);
    if (pet->alignment != pet->pIndexData->alignment)
    	fprintf(fp, "Alig %d\n", pet->alignment);
    if (pet->hitroll != pet->pIndexData->hitroll)
    	fprintf(fp, "Hit  %d\n", pet->hitroll);
    if (pet->damroll != pet->pIndexData->damage[DICE_BONUS])
    	fprintf(fp, "Dam  %d\n", pet->damroll);
    fprintf(fp, "ACs  %d %d %d %d\n",
    	pet->armor[0],pet->armor[1],pet->armor[2],pet->armor[3]);
    fprintf(fp, "Attr %d %d %d %d %d\n",
    	pet->perm_stat[STAT_STR], pet->perm_stat[STAT_INT],
    	pet->perm_stat[STAT_WIS], pet->perm_stat[STAT_DEX],
    	pet->perm_stat[STAT_CON]);
    fprintf(fp, "AMod %d %d %d %d %d\n",
    	pet->mod_stat[STAT_STR], pet->mod_stat[STAT_INT],
    	pet->mod_stat[STAT_WIS], pet->mod_stat[STAT_DEX],
    	pet->mod_stat[STAT_CON]);
    
    for ( paf = pet->affected; paf != NULL; paf = paf->next )
    {
    	if (paf->type < 0 || paf->type >= MAX_SKILL)
    	    continue;
    	    
    	fprintf(fp, "Affc '%s' %3d %3d %3d %3d %3d %10d\n",
    	    skill_table[paf->type].name,
    	    paf->where, paf->level, paf->duration, paf->modifier,paf->location,
    	    paf->bitvector);
    }
    
    fprintf(fp,"End\n");
    return;
}
    
/*
 * Write an object and its contents.
 */
void fwrite_obj( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest )
{
    EXTRA_DESCR_DATA *ed;
    AFFECT_DATA *paf;

    /*
     * Slick recursion to write lists backwards,
     *   so loading them will load in forwards order.
     */
    if ( obj->next_content != NULL )
	fwrite_obj( ch, obj->next_content, fp, iNest );

    /*
     * Castrate storage characters.
     */
    if ( ch && ((ch->level < obj->level - 4 && obj->level > 19
    && (obj->item_type != ITEM_CONTAINER
    &&  obj->item_type != ITEM_PIT))
    ||   obj->item_type == ITEM_KEY
    ||   (obj->item_type == ITEM_MAP && !obj->value[0])))
	return;

    if ( IS_SET(obj->extra_flags,ITEM_NOSAVE) )
        return; /* Room saving stuff */
/*
    fprintf( fp, "* object-v%d: %s\n", obj->pIndexData->vnum, obj->name );
*/
    fprintf( fp, "#O\n" );
    fprintf( fp, "Vnum %d\n",   obj->pIndexData->vnum        );
    if (!obj->pIndexData->new_format)
	fprintf( fp, "Oldstyle\n");
    if (obj->enchanted)
	fprintf( fp,"Enchanted\n");
    fprintf( fp, "Nest %d\n",	iNest	  	     );

    /* these data are only used if they do not match the defaults */


  if(obj->item_type != 0)
  {
    if ( obj->name != obj->pIndexData->name
        && obj->name[0] != '\0')
    	fprintf( fp, "Name %s~\n",	obj->name		     );
    if ( obj->short_descr != obj->pIndexData->short_descr
        && obj->short_descr[0] != '\0')
        fprintf( fp, "ShD  %s~\n",	obj->short_descr	     );
    if ( obj->description != obj->pIndexData->description)
        fprintf( fp, "Desc %s~\n",	obj->description	     );
    if ( obj->extra_flags != obj->pIndexData->extra_flags
        && obj->extra_flags)
        fprintf( fp, "ExtF %d\n",	obj->extra_flags	     );
    if ( obj->wear_flags != obj->pIndexData->wear_flags
        && obj->wear_flags)
        fprintf( fp, "WeaF %d\n",	obj->wear_flags		     );
    if ( obj->item_type != obj->pIndexData->item_type)
        fprintf( fp, "Ityp %d\n",	obj->item_type		     );
    if ( obj->weight != obj->pIndexData->weight)
        fprintf( fp, "Wt   %d\n",	obj->weight		     );
    if ( obj->condition != obj->pIndexData->condition)
	fprintf( fp, "Cond %d\n",	obj->condition		     );
  }

    /* variable data */

    fprintf( fp, "Wear %d\n",   obj->wear_loc                );
    if (obj->level != obj->pIndexData->level && obj->level > 0)
        fprintf( fp, "Lev  %d\n",	obj->level		     );
    if (obj->timer != 0)
        fprintf( fp, "Time %d\n",	obj->timer	     );
    fprintf( fp, "Cost %d\n",	obj->cost		     );

  if(ch->version != 42)
  {
    if (obj->value[0] != obj->pIndexData->value[0]
    ||  obj->value[1] != obj->pIndexData->value[1]
    ||  obj->value[2] != obj->pIndexData->value[2]
    ||  obj->value[3] != obj->pIndexData->value[3]
    ||  obj->value[4] != obj->pIndexData->value[4])
    	fprintf( fp, "Val  %d %d %d %d %d\n",
	    obj->value[0], obj->value[1], obj->value[2], obj->value[3],
	    obj->value[4]	     );
  }

    switch ( obj->item_type )
    {
    case ITEM_POTION:
    case ITEM_SCROLL:
	if ( obj->value[1] > 0 )
	{
	    fprintf( fp, "Spell 1 '%s'\n", 
		skill_table[obj->value[1]].name );
	}

	if ( obj->value[2] > 0 )
	{
	    fprintf( fp, "Spell 2 '%s'\n", 
		skill_table[obj->value[2]].name );
	}

	if ( obj->value[3] > 0 )
	{
	    fprintf( fp, "Spell 3 '%s'\n", 
		skill_table[obj->value[3]].name );
	}

	break;

    case ITEM_PILL:
    case ITEM_STAFF:
    case ITEM_WAND:
	if ( obj->value[3] > 0 )
	{
	    fprintf( fp, "Spell 3 '%s'\n", 
		skill_table[obj->value[3]].name );
	}

	break;
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	if (paf->type < 0 || paf->type >= MAX_SKILL)
	    continue;
        fprintf( fp, "Affc '%s' %3d %3d %3d %3d %3d %10d\n",
            skill_table[paf->type].name,
            paf->where,
            paf->level,
            paf->duration,
            paf->modifier,
            paf->location,
            paf->bitvector
            );
    }

    for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
    {
	fprintf( fp, "ExDe %s~ %s~\n",
	    ed->keyword, ed->description );
    }

    fprintf( fp, "End\n\n" );

    if ( obj->contains != NULL )
	fwrite_obj( ch, obj->contains, fp, iNest + 1 );

    return;
}

/*
 * Load a char and inventory into a new ch structure.
 */
bool load_char_obj( DESCRIPTOR_DATA *d, char *name )
{
    char strsave[MAX_INPUT_LENGTH];
    char buf[100];
    CHAR_DATA *ch;
    FILE *fp;
    bool found;
    int stat;

    ch = new_char();
    ch->pcdata = new_pcdata();

    d->character			= ch;
    ch->desc				= d;
    ch->name				= str_dup( name );
    ch->id				= get_pc_id();
    ch->race				= race_lookup("human");
    ch->act				= PLR_NOSUMMON | PLR_COLOUR
					| PLR_AUTOEXIT;
    ch->plt				= 0;
    ch->comm				= COMM_COMBINE 
					| COMM_PROMPT
					| COMM_STORE
					| COMM_LONG;
    ch->comm2				= 0;
    ch->prompt 				= str_dup("<%h/%Hhp %m/%Mm %v/%Vmv %Xtnl> ");
    ch->pcdata->confirm_delete		= 0;
    ch->pcdata->board                   = &boards[DEFAULT_BOARD];
    ch->pcdata->pwd			= str_dup( "" );
    ch->pcdata->bamfin			= str_dup( "" );
    ch->pcdata->bamfout			= str_dup( "" );
    ch->pcdata->who_descr		= str_dup( "" );
    ch->pcdata->title			= str_dup( "" );
    ch->pcdata->csmsg			= str_dup( "" );
    ch->pcdata->tier			= 0;
    ch->pcdata->nextquest		= 0;
    ch->pcdata->questpoints		= 0;
    ch->pcdata->questmob		= 0;
    ch->pcdata->questobj		= 0;
    ch->pcdata->questgiver		= NULL;
    for (stat =0; stat < MAX_STATS; stat++)
	ch->perm_stat[stat]		= 13;
    ch->pcdata->condition[COND_THIRST]	= 48; 
    ch->pcdata->condition[COND_FULL]	= 48;
    ch->pcdata->condition[COND_HUNGER]	= 48;
    ch->pcdata->condition[COND_FATIGUE]	= 48;
    ch->pcdata->condition[COND_PAIN]	= 48;
    ch->pcdata->security		= 0;	/* OLC */
    ch->pcdata->granted			= NULL;

    for (stat =0; stat < MAX_LANGUAGE; stat++)
	ch->language_known[stat]	= 0;
    ch->primary_lang			= 0;
    ch->in_arena = FALSE;

    found = FALSE;
    fclose( fpReserve );
    
    #if defined(unix)
    /* decompress if .gz file exists */
    sprintf( strsave, "%s%s%s", PLAYER_DIR, capitalize(name),".gz");
    if ( ( fp = fopen( strsave, "r" ) ) != NULL )
    {
	fclose(fp);
	sprintf(buf,"gzip -dfq %s",strsave);
	system(buf);
    }
    #endif

    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( name ) );
    if ( ( fp = fopen( strsave, "r" ) ) != NULL )
    {
	int iNest;

	for ( iNest = 0; iNest < MAX_NEST; iNest++ )
	    rgObjNest[iNest] = NULL;

	found = TRUE;
	for ( ; ; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_char_obj: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if      ( !str_cmp( word, "PLAYER" ) ) fread_char ( ch, fp );
	    else if ( !str_cmp( word, "OBJECT" ) ) fread_obj  ( ch, fp, FALSE );
	    else if ( !str_cmp( word, "O"      ) ) fread_obj  ( ch, fp, FALSE );
	    else if ( !str_cmp( word, "PET"    ) ) fread_pet  ( ch, fp );
	    else if ( !str_cmp( word, "BANK-ACCOUNT") ) fread_bank_account( ch, fp );
	    else if ( !str_cmp( word, "PK-COUNTER") ) ch->pcdata->pk_counter = fread_pk_counter( fp );
	    else if ( !str_cmp( word, "END"    ) ) break;
	    else
	    {
		bug( "Load_char_obj: bad section.", 0 );
		break;
	    }
	}
	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );


    /* initialize race */
    if (found)
    {
	int i;

	if (ch->race == 0)
	    ch->race = race_lookup("human");

	ch->size = pc_race_table[ch->race].size;
	ch->dam_type = 17; /*punch */

	for (i = 0; i < 5; i++)
	{
	    if (pc_race_table[ch->race].skills[i] == NULL)
		break;
	    group_add(ch,pc_race_table[ch->race].skills[i],FALSE);
	}
	ch->affected_by = ch->affected_by|race_table[ch->race].aff;
	ch->shielded_by = ch->shielded_by|race_table[ch->race].shd;
	ch->imm_flags	= ch->imm_flags | race_table[ch->race].imm;
	ch->res_flags	= ch->res_flags | race_table[ch->race].res;
	ch->vuln_flags	= ch->vuln_flags | race_table[ch->race].vuln;
	ch->form	= race_table[ch->race].form;
	ch->parts	= race_table[ch->race].parts;
    }

	
    /* RT initialize skills */

    if (found && ch->version < 2)  /* need to add the new skills */
    {
	group_add(ch,"rom basics",FALSE);
	group_add(ch,class_table[ch->class].base_group,FALSE);
	group_add(ch,class_table[ch->class].default_group,TRUE);
	ch->pcdata->learned[gsn_recall] = 50;
    }

    if (found && ch->version < 6)
    {
        ch->primary_lang = LANGUAGE_COMMON;
        ch->language_known[ch->primary_lang] = 100;
    }

    return found;
}


bool load_char_reroll( DESCRIPTOR_DATA *d, char *name )
{
    CHAR_DATA *ch;
    bool found;
    int stat;

    ch = new_char();
    ch->pcdata = new_pcdata();

    d->character			= ch;
    ch->desc				= d;
    ch->name				= str_dup( name );
    ch->id				= get_pc_id();
    ch->race				= race_lookup("human");
    ch->act				= PLR_NOSUMMON
					| PLR_AUTOEXIT
					| PLR_AUTOLOOT
					| PLR_AUTOGOLD;
    ch->plt				= 0;
    ch->comm				= COMM_COMBINE 
					| COMM_PROMPT
					| COMM_STORE;
    ch->comm2				= 0;
    ch->prompt 				= str_dup("<%h/%Hhp %m/%Mm %v/%Vmv %Xtnl> ");
    ch->pcdata->confirm_delete		= 0;
    ch->pcdata->board                   = &boards[DEFAULT_BOARD];
    ch->pcdata->pwd			= str_dup( "" );
    ch->pcdata->bamfin			= str_dup( "" );
    ch->pcdata->bamfout			= str_dup( "" );
    ch->pcdata->who_descr		= str_dup( "" );
    ch->pcdata->title			= str_dup( "" );
    ch->pcdata->tier			= 1;
    ch->pcdata->nextquest               = 0;
    ch->pcdata->questpoints             = 0;
    ch->pcdata->questmob                = 0;
    ch->pcdata->questobj                = 0;
    ch->pcdata->questgiver              = NULL;
    for (stat =0; stat < MAX_STATS; stat++)
	ch->perm_stat[stat]		= 13;
    ch->pcdata->condition[COND_THIRST]	= 48; 
    ch->pcdata->condition[COND_FULL]	= 48;
    ch->pcdata->condition[COND_HUNGER]	= 48;
    ch->pcdata->condition[COND_FATIGUE]	= 48;
    ch->pcdata->condition[COND_PAIN]	= 48;
    ch->pcdata->granted                 = NULL;

    for (stat =0; stat < MAX_LANGUAGE; stat++)
	ch->language_known[stat]	= 0;
    ch->primary_lang			= 0;
    ch->in_arena = FALSE;

    found = FALSE;

    return found;
}

/*
 * Read in a char.
 */

#if defined(KEY)
#undef KEY
#endif
#if defined(SKEY)
#undef SKEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

#define SKEY( string, field )                       \
                if ( !str_cmp( word, string ) )     \
                {                                   \
                    free_string( field );           \
                    field = fread_string( fp );     \
                    fMatch = TRUE;                  \
                    break;                          \
                                }

void fread_char( CHAR_DATA *ch, FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    int count = 0;
    int dcount = 0;
    int fcount = 0;
    int lastlogoff = current_time;
    int percent;

    sprintf(buf,"Loading %s.",ch->name);
    if (strcmp(ch->name, "") )
    {
	log_string(LOG_CONNECT,buf);
    }

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    KEY( "Act",		ch->act,		fread_flag( fp ) );
	    KEY( "AffectedBy",	ch->affected_by,	fread_flag( fp ) );
	    KEY( "AfBy",	ch->affected_by,	fread_flag( fp ) );
	    KEY( "Alignment",	ch->alignment,		fread_number( fp ) );
	    KEY( "AutoAway",	ch->pcdata->auto_away,	fread_number( fp ) );
	    KEY( "Alig",	ch->alignment,		fread_number( fp ) );
            KEY( "Awon",        ch->pcdata->arena_won,	fread_number( fp ) );
            KEY( "Alost",       ch->pcdata->arena_lost,	fread_number( fp ) );

	    if (!str_cmp( word, "Alia"))
	    {
		if (count >= MAX_ALIAS)
		{
		    fread_to_eol(fp);
		    fMatch = TRUE;
		    break;
		}

		ch->pcdata->alias[count] 	= str_dup(fread_word(fp));
		ch->pcdata->alias_sub[count]	= str_dup(fread_word(fp));
		count++;
		fMatch = TRUE;
		break;
	    }

            if (!str_cmp( word, "Alias"))
            {
                if (count >= MAX_ALIAS)
                {
                    fread_to_eol(fp);
                    fMatch = TRUE;
                    break;
                }
 
                ch->pcdata->alias[count]        = str_dup(fread_word(fp));
                ch->pcdata->alias_sub[count]    = fread_string(fp);
                count++;
                fMatch = TRUE;
                break;
            }

	    if (!str_cmp( word, "AC") || !str_cmp(word,"Armor"))
	    {
		fread_to_eol(fp);
		fMatch = TRUE;
		break;
	    }

	    if (!str_cmp(word,"ACs"))
	    {
		int i;

		for (i = 0; i < 4; i++)
		    ch->armor[i] = fread_number(fp);
		fMatch = TRUE;
		break;
	    }

	    if (!str_cmp(word, "AffD"))
	    {
		AFFECT_DATA *paf;
		int sn;

		paf = new_affect();

		sn = skill_lookup(fread_word(fp));
		if (sn < 0)
		    bug("Fread_char: unknown skill.",0);
		else
		    paf->type = sn;

		paf->level	= fread_number( fp );
		paf->duration	= fread_number( fp );
		paf->modifier	= fread_number( fp );
		paf->location	= fread_number( fp );
		paf->bitvector	= fread_number( fp );
		paf->next	= ch->affected;
		ch->affected	= paf;
		fMatch = TRUE;
		break;
	    }

            if (!str_cmp(word, "Affc"))
            {
                AFFECT_DATA *paf;
                int sn;
 
                paf = new_affect();
 
                sn = skill_lookup(fread_word(fp));
                if (sn < 0)
                    bug("Fread_char: unknown skill.",0);
                else
                    paf->type = sn;
 
                paf->where  = fread_number(fp);
                paf->level      = fread_number( fp );
                paf->duration   = fread_number( fp );
                paf->modifier   = fread_number( fp );
                paf->location   = fread_number( fp );
                paf->bitvector  = fread_number( fp );
                paf->next       = ch->affected;
                ch->affected    = paf;
                fMatch = TRUE;
                break;
            }

	    if ( !str_cmp( word, "AttrMod"  ) || !str_cmp(word,"AMod"))
	    {
		int stat;
		for (stat = 0; stat < MAX_STATS; stat ++)
		   ch->mod_stat[stat] = fread_number(fp);
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "AttrPerm" ) || !str_cmp(word,"Attr"))
	    {
		int stat;

		for (stat = 0; stat < MAX_STATS; stat++)
		    ch->perm_stat[stat] = fread_number(fp);
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'B':
	    KEY( "Bamfin",	ch->pcdata->bamfin,	fread_string( fp ) );
	    KEY( "Bamfout",	ch->pcdata->bamfout,	fread_string( fp ) );
	    KEY( "Bin",		ch->pcdata->bamfin,	fread_string( fp ) );
	    KEY( "Bout",	ch->pcdata->bamfout,	fread_string( fp ) );

	    /* Read in board status */
	    if (!str_cmp(word, "Boards" ))
	    {
		int i,num = fread_number (fp); /* number of boards saved */
                char *boardname;

                for (; num ; num-- ) /* for each of the board saved */
                {
		    boardname = fread_word (fp);
		    i = board_lookup (boardname); /* find board number */

		    if (i == BOARD_NOTFOUND) /* Does board still exist ? */
                    {
			sprintf (buf, "fread_char: %s had unknown board name: %s. Skipped.",
			    ch->name, boardname);
			log_string (LOG_ERR,buf);
			fread_number (fp); /* read last_note and skip info */
		    }
		    else /* Save it */
			ch->pcdata->last_note[i] = fread_number (fp);
		} /* for */

                fMatch = TRUE;
	      } /* Boards */
	    break;

	case 'C':
	    KEY( "Clas",        ch->class,	
				class_lookup(fread_string( fp )) );
	    KEY( "Class",	ch->class,		fread_number( fp ) );
	    KEY( "Cla",		ch->class,		fread_number( fp ) );
	    KEY( "Clan",	ch->clan,	clan_lookup(fread_string(fp)));
	    KEY( "Color",	ch->color,		fread_number( fp ) );
	    KEY( "Coauc",	ch->color_auc,		fread_number( fp ) );
	    KEY( "Cocgo",	ch->color_cgo,		fread_number( fp ) );
	    KEY( "Cocla",	ch->color_cla,		fread_number( fp ) );
	    KEY( "Cocon",	ch->color_con,		fread_number( fp ) );
	    KEY( "Codis",	ch->color_dis,		fread_number( fp ) );
	    KEY( "Cofig",	ch->color_fig,		fread_number( fp ) );
	    KEY( "Cogos",	ch->color_gos,		fread_number( fp ) );
	    KEY( "Cogra",	ch->color_gra,		fread_number( fp ) );
	    KEY( "Cogte",	ch->color_gte,		fread_number( fp ) );
	    KEY( "Coimm",	ch->color_imm,		fread_number( fp ) );
	    KEY( "Comob",	ch->color_mob,		fread_number( fp ) );
	    KEY( "Comus",	ch->color_mus,		fread_number( fp ) );
	    KEY( "Coopp",	ch->color_opp,		fread_number( fp ) );
	    KEY( "Coqgo",	ch->color_qgo,		fread_number( fp ) );
	    KEY( "Coque",	ch->color_que,		fread_number( fp ) );
	    KEY( "Coquo",	ch->color_quo,		fread_number( fp ) );
	    KEY( "Coroo",	ch->color_roo,		fread_number( fp ) );
	    KEY( "Cosay",	ch->color_say,		fread_number( fp ) );
	    KEY( "Cosho",	ch->color_sho,		fread_number( fp ) );
	    KEY( "Cotel",	ch->color_tel,		fread_number( fp ) );
	    KEY( "Cowit",	ch->color_wit,		fread_number( fp ) );
	    KEY( "Cowiz",	ch->color_wiz,		fread_number( fp ) );

	    if ( !str_cmp( word, "Condition" ) || !str_cmp(word,"Cond")
		|| !str_cmp( word, "Cnd") )
	    {
		ch->pcdata->condition[0] = fread_number( fp );
		ch->pcdata->condition[1] = fread_number( fp );
		ch->pcdata->condition[2] = fread_number( fp );
		ch->pcdata->condition[3] = fread_number( fp );
		if(ch->version >= 7)
		{
		    ch->pcdata->condition[4] = fread_number( fp );
		    ch->pcdata->condition[5] = fread_number( fp );
		}

		fMatch = TRUE;
		break;
	    }
	    KEY("Comm",		ch->comm,		fread_flag( fp ) );
	    KEY("Com2",		ch->comm2,		fread_flag( fp ) );
          
	    break;

	case 'D':
	    KEY( "Damroll",	ch->damroll,		fread_number( fp ) );
	    KEY( "Dam",		ch->damroll,		fread_number( fp ) );
	    KEY( "Description",	ch->description,	fread_string( fp ) );
	    KEY( "Desc",	ch->description,	fread_string( fp ) );
	    if (!str_cmp( word, "Dupes"))
            {
                if (dcount >= MAX_DUPES)
                {
                    fread_to_eol(fp);
                    fMatch = TRUE;
                    break;
                }
 
                ch->pcdata->dupes[dcount]        = fread_string(fp);
                dcount++;
                fMatch = TRUE;
            }
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
    		/* adjust hp mana move up  -- here for speed's sake */
    		percent = (current_time - lastlogoff) * 25 / ( 2 * 60 * 60);

		percent = UMIN(percent,100);
 
    		if (percent > 0 && !IS_AFFECTED(ch,AFF_POISON)
    		&&  !IS_AFFECTED(ch,AFF_PLAGUE))
    		{
        	    ch->hit	+= (ch->max_hit - ch->hit) * percent / 100;
        	    ch->mana    += (ch->max_mana - ch->mana) * percent / 100;
        	    ch->move    += (ch->max_move - ch->move)* percent / 100;
    		}
		return;
	    }
	    KEY( "Exp",		ch->exp,		fread_number( fp ) );
	    break;

	case 'F':
	    if (!str_cmp( word, "Forge"))
            {
                if (fcount >= MAX_FORGET)
                {
                    fread_to_eol(fp);
                    fMatch = TRUE;
                    break;
                }
 
                ch->pcdata->forget[fcount]        = fread_string(fp);
                fcount++;
                fMatch = TRUE;
            }
	    break;

	case 'G':
	    KEY( "Ghos",	ch->ghost_level,	fread_number( fp ) );
	    KEY( "Gold",	ch->gold,		fread_number( fp ) );
            if ( !str_cmp( word, "Grant" ) )
            {
              GRANT_DATA *gran;
              int cmd;

              gran = new_grant();
              gran->name = str_dup(fread_word(fp));
              gran->duration = fread_number(fp);
              gran->next = NULL;
              gran->do_fun = NULL;
              for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
                if ( gran->name[0] == cmd_table[cmd].name[0]
                   &&  is_exact_name( gran->name, cmd_table[cmd].name ) )
                {
                  gran->do_fun = cmd_table[cmd].do_fun;
                  gran->level = cmd_table[cmd].level;
                  break;
                }

              gran->next = ch->pcdata->granted;
              ch->pcdata->granted = gran;

              if (gran->do_fun == NULL)
              {
                sprintf(buf,"Grant: Command %s not found in pfile for %s",
                        gran->name,ch->name);
                log_string(LOG_ERR,buf);
              }

              fMatch = TRUE;
            }

            if ( !str_cmp( word, "Group" )  || !str_cmp(word,"Gr"))
            {
                int gn;
                char *temp;
 
                temp = fread_word( fp ) ;
                gn = group_lookup(temp);
                /* gn    = group_lookup( fread_word( fp ) ); */
                if ( gn < 0 )
                {
                    fprintf(stderr,"%s",temp);
                    bug( "Fread_char: unknown group. ", 0 );
                }
                else
		    gn_add(ch,gn);
                fMatch = TRUE;
            }
	    break;

	case 'H':
	    KEY( "Hitroll",	ch->hitroll,		fread_number( fp ) );
	    KEY( "Hit",		ch->hitroll,		fread_number( fp ) );

	    if ( !str_cmp( word, "HpManaMove" ) || !str_cmp(word,"HMV"))
	    {
		ch->hit		= fread_number( fp );
		ch->max_hit	= fread_number( fp );
		ch->mana	= fread_number( fp );
		ch->max_mana	= fread_number( fp );
		ch->move	= fread_number( fp );
		ch->max_move	= fread_number( fp );
		fMatch = TRUE;
		break;
	    }

            if ( !str_cmp( word, "HpManaMovePerm" ) || !str_cmp(word,"HMVP"))
            {
                ch->pcdata->perm_hit	= fread_number( fp );
                ch->pcdata->perm_mana   = fread_number( fp );
                ch->pcdata->perm_move   = fread_number( fp );
                fMatch = TRUE;
                break;
            }
      
	    break;

	case 'I':
	    KEY( "Id",		ch->id,			fread_number( fp ) );
	    KEY( "InvisLevel",	ch->invis_level,	fread_number( fp ) );
	    KEY( "Inco",	ch->incog_level,	fread_number( fp ) );
	    KEY( "Invi",	ch->invis_level,	fread_number( fp ) );
            KEY( "Invite",	ch->invite,		fread_number( fp ) );
	    break;

	case 'L':
	    if ( !str_cmp( word, "Lang" ) )
	    {
		int sn;
		int value;
		char *temp;

		value = fread_number( fp );
		temp = fread_word( fp ) ;
		sn = language_lookup(temp);
		if ( sn < 0 )
		{
		    fprintf(stderr,"%s",temp);
		    bug( "Fread_char: unknown language. ", 0 );
		}
		else
		    ch->language_known[sn] = value;
		fMatch = TRUE;
	    }

	    KEY( "LastIP",	ch->pcdata->last_ip, fread_string( fp ) );
	    KEY( "LastLevel",	ch->pcdata->last_level, fread_number( fp ) );
	    KEY( "LLev",	ch->pcdata->last_level, fread_number( fp ) );
	    KEY( "Level",	ch->level,		fread_number( fp ) );
	    KEY( "Lev",		ch->level,		fread_number( fp ) );
	    KEY( "Levl",	ch->level,		fread_number( fp ) );
            if (!str_cmp(word,"LogO"))
            {
		lastlogoff		= fread_number( fp );
		ch->llogoff		= (time_t) lastlogoff;
		fMatch = TRUE;
		break;
	    }
            if (!str_cmp(word,"Logl"))
            {
		char *temp		= fread_string( fp );
		if(!str_cmp(temp,"always"))
                    ch->log_level = 9;
		if(!str_cmp(temp,"simple"))
                    ch->log_level = 5;
		else
                    ch->log_level = 0;
                free_string(temp);

		fMatch = TRUE;
		break;
	    }
	    KEY( "LongDescr",	ch->long_descr,		fread_string( fp ) );
	    KEY( "LnD",		ch->long_descr,		fread_string( fp ) );
	    break;

	case 'N':
	    KEY( "Name",	ch->name,		fread_string( fp ) );
	    if (!str_cmp(word,"Not"))
	    {
		fread_number(fp);
		fread_number(fp);
		fread_number(fp);
		fread_number(fp);
		fread_number(fp);
		fMatch = TRUE;
		break;
	    }
	    if (!str_cmp(word,"Notb"))
	    {
		fread_number(fp);
		fread_number(fp);
		fread_number(fp);
		fread_number(fp);
		fread_number(fp);
		fread_number(fp);
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'P':
	    KEY( "Password",	ch->pcdata->pwd,	fread_string( fp ) );
	    KEY( "Pass",	ch->pcdata->pwd,	fread_string( fp ) );
	    KEY( "Plat",	ch->platinum,		fread_number( fp ) );
	    KEY( "Played",	ch->played,		fread_number( fp ) );
	    KEY( "Plyd",	ch->played,		fread_number( fp ) );
	    KEY( "Plt",		ch->plt,		fread_flag  ( fp ) );
	    KEY( "Points",	ch->pcdata->points,	fread_number( fp ) );
	    KEY( "Pnts",	ch->pcdata->points,	fread_number( fp ) );
	    KEY( "Position",	ch->position,		fread_number( fp ) );
	    KEY( "Pos",		ch->position,		fread_number( fp ) );
	    KEY( "Practice",	ch->practice,		fread_number( fp ) );
	    KEY( "Prac",	ch->practice,		fread_number( fp ) );
	    KEY( "PrimLang",	ch->primary_lang,
					language_lookup( fread_word( fp ) ));
            KEY( "Prompt",      ch->prompt,             fread_string( fp ) );
 	    KEY( "Prom",	ch->prompt,		fread_string( fp ) );
	    break;

	case 'Q':
	    KEY( "Qps",		ch->qps,		fread_number( fp ) );
	    KEY( "Qaps",        ch->pcdata->questpoints,fread_number( fp ) );
	    KEY( "Qnxt",	ch->pcdata->nextquest,	fread_number( fp ) );
	    KEY( "Qcdn",	ch->pcdata->countdown,	fread_number( fp ) );
            KEY( "Qmob",        ch->pcdata->questmob,	fread_number( fp ) );
            KEY( "Qobj",        ch->pcdata->questobj,	fread_number( fp ) );
	    KEY( "Qgvr",	ch->pcdata->questgiver,
					get_mob_index(fread_number(fp)) );
/*            if (!str_cmp(word, "Qgvr"))
	    {
	        ch->pcdata->questgiver = get_mob_index(fread_number(fp));
	        break;
	    }*/

	    break;

	case 'R':
	    KEY( "Race",        ch->race,	
				race_lookup(fread_string( fp )) );
           KEY( "Rank",        ch->rank, fread_number( fp ) );
	    KEY( "Reca",	ch->pcdata->recall, fread_number( fp ) );

	    if ( !str_cmp( word, "Room" ) )
	    {
		ch->in_room = get_room_index( fread_number( fp ) );
		if ( ch->in_room == NULL )
		    ch->in_room = get_room_index( ROOM_VNUM_LIMBO );
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'S':
	    KEY( "SavingThrow",	ch->saving_throw,	fread_number( fp ) );
	    KEY( "Save",	ch->saving_throw,	fread_number( fp ) );
	    KEY( "Scro",	ch->lines,		fread_number( fp ) );
	    KEY( "Sex",		ch->sex,		fread_number( fp ) );
	    KEY( "ShBy",	ch->shielded_by,	fread_flag( fp ) );
	    KEY( "ShieldedBy",	ch->shielded_by,	fread_flag( fp ) );
	    KEY( "ShortDescr",	ch->short_descr,	fread_string( fp ) );
	    KEY( "ShD",		ch->short_descr,	fread_string( fp ) );
	    KEY( "Sec",         ch->pcdata->security,	fread_number( fp ) );	/* OLC */
            KEY( "Silv",        ch->silver,             fread_number( fp ) );


	    if ( !str_cmp( word, "Skill" ) || !str_cmp(word,"Sk"))
	    {
		int sn;
		int value;
		char *temp;

		value = fread_number( fp );
		temp = fread_word( fp ) ;
		sn = skill_lookup(temp);
		/* sn    = skill_lookup( fread_word( fp ) ); */
		if ( sn < 0 )
		{
		    fprintf(stderr,"%s",temp);
		    bug( "Fread_char: unknown skill. ", 0 );
		}
		else
		    ch->pcdata->learned[sn] = value;
		fMatch = TRUE;
	    }

	    break;

	case 'T':
	    KEY( "Tier",	ch->pcdata->tier,	fread_number( fp ) );
	    KEY( "TrueSex",     ch->pcdata->true_sex,  	fread_number( fp ) );
	    KEY( "TSex",	ch->pcdata->true_sex,   fread_number( fp ) );
	    KEY( "Trai",	ch->train,		fread_number( fp ) );
	    KEY( "Trust",	ch->trust,		fread_number( fp ) );
	    KEY( "Tru",		ch->trust,		fread_number( fp ) );

	    if ( !str_cmp( word, "Title" )  || !str_cmp( word, "Titl"))
	    {
		ch->pcdata->title = fread_string( fp );
    		if (ch->pcdata->title[0] != '.' && ch->pcdata->title[0] != ',' 
		&&  ch->pcdata->title[0] != '!' && ch->pcdata->title[0] != '?')
		{
		    sprintf( buf, " %s", ch->pcdata->title );
		    free_string( ch->pcdata->title );
		    ch->pcdata->title = str_dup( buf );
		}
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'V':
	    KEY( "Version",     ch->version,		fread_number ( fp ) );
	    KEY( "Vers",	ch->version,		fread_number ( fp ) );
	    if ( !str_cmp( word, "Vnum" ) )
	    {
		ch->pIndexData = get_mob_index( fread_number( fp ) );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'W':
	    KEY( "Whod",	ch->pcdata->who_descr,	fread_string( fp ) );
	    KEY( "Wimpy",	ch->wimpy,		fread_number( fp ) );
	    KEY( "Wimp",	ch->wimpy,		fread_number( fp ) );
	    KEY( "Wizn",	ch->wiznet,		fread_flag( fp ) );
	    break;
	}

	if ( !fMatch )
	{
	    bug( "Fread_char: no match.", 0 );
	    log_string(LOG_ERR, word );
	    fread_to_eol( fp );
	}
    }
}

/* load a pet from the forgotten reaches */
void fread_pet( CHAR_DATA *ch, FILE *fp )
{
    char *word;
    CHAR_DATA *pet;
    bool fMatch;
    int lastlogoff = current_time;
    int percent;

    /* first entry had BETTER be the vnum or we barf */
    word = feof(fp) ? "END" : fread_word(fp);
    if (!str_cmp(word,"Vnum"))
    {
    	int vnum;
    	
    	vnum = fread_number(fp);
    	if (get_mob_index(vnum) == NULL)
	{
    	    bug("Fread_pet: bad vnum %d.",vnum);
	    pet = create_mobile(get_mob_index(MOB_VNUM_FIDO));
	}
    	else
    	    pet = create_mobile(get_mob_index(vnum));
    }
    else
    {
        bug("Fread_pet: no vnum in file.",0);
        pet = create_mobile(get_mob_index(MOB_VNUM_FIDO));
    }
    
    for ( ; ; )
    {
    	word 	= feof(fp) ? "END" : fread_word(fp);
    	fMatch = FALSE;
    	
    	switch (UPPER(word[0]))
    	{
    	case '*':
    	    fMatch = TRUE;
    	    fread_to_eol(fp);
    	    break;
    		
    	case 'A':
    	    KEY( "Act",		pet->act,		fread_flag(fp));
    	    KEY( "AfBy",	pet->affected_by,	fread_flag(fp));
    	    KEY( "Alig",	pet->alignment,		fread_number(fp));
    	    
    	    if (!str_cmp(word,"ACs"))
    	    {
    	    	int i;
    	    	
    	    	for (i = 0; i < 4; i++)
    	    	    pet->armor[i] = fread_number(fp);
    	    	fMatch = TRUE;
    	    	break;
    	    }
    	    
    	    if (!str_cmp(word,"AffD"))
    	    {
    	    	AFFECT_DATA *paf;
    	    	int sn;
    	    	
    	    	paf = new_affect();
    	    	
    	    	sn = skill_lookup(fread_word(fp));
    	     	if (sn < 0)
    	     	    bug("Fread_char: unknown skill.",0);
    	     	else
    	     	   paf->type = sn;
    	     	   
    	     	paf->level	= fread_number(fp);
    	     	paf->duration	= fread_number(fp);
    	     	paf->modifier	= fread_number(fp);
    	     	paf->location	= fread_number(fp);
    	     	paf->bitvector	= fread_number(fp);
    	     	paf->next	= pet->affected;
    	     	pet->affected	= paf;
    	     	fMatch		= TRUE;
    	     	break;
    	    }

            if (!str_cmp(word,"Affc"))
            {
                AFFECT_DATA *paf;
                int sn;
 
                paf = new_affect();
 
                sn = skill_lookup(fread_word(fp));
                if (sn < 0)
                    bug("Fread_char: unknown skill.",0);
                else
                   paf->type = sn;
 
		paf->where	= fread_number(fp);
                paf->level      = fread_number(fp);
                paf->duration   = fread_number(fp);
                paf->modifier   = fread_number(fp);
                paf->location   = fread_number(fp);
                paf->bitvector  = fread_number(fp);
                paf->next       = pet->affected;
                pet->affected   = paf;
                fMatch          = TRUE;
                break;
            }
    	     
    	    if (!str_cmp(word,"AMod"))
    	    {
    	     	int stat;
    	     	
    	     	for (stat = 0; stat < MAX_STATS; stat++)
    	     	    pet->mod_stat[stat] = fread_number(fp);
    	     	fMatch = TRUE;
    	     	break;
    	    }
    	     
    	    if (!str_cmp(word,"Attr"))
    	    {
    	         int stat;
    	         
    	         for (stat = 0; stat < MAX_STATS; stat++)
    	             pet->perm_stat[stat] = fread_number(fp);
    	         fMatch = TRUE;
    	         break;
    	    }
    	    break;
    	     
    	 case 'C':
             KEY( "Clan",       pet->clan,       clan_lookup(fread_string(fp)));
    	     KEY( "Comm",	pet->comm,		fread_flag(fp));
    	     break;
    	     
    	 case 'D':
    	     KEY( "Dam",	pet->damroll,		fread_number(fp));
    	     KEY( "Desc",	pet->description,	fread_string(fp));
    	     break;
    	     
    	 case 'E':
    	     if (!str_cmp(word,"End"))
	     {
		pet->leader = ch;
		pet->master = ch;
		ch->pet = pet;
    		/* adjust hp mana move up  -- here for speed's sake */
    		percent = (current_time - lastlogoff) * 25 / ( 2 * 60 * 60);
 
    		if (percent > 0 && !IS_AFFECTED(ch,AFF_POISON)
    		&&  !IS_AFFECTED(ch,AFF_PLAGUE))
    		{
		    percent = UMIN(percent,100);
    		    pet->hit	+= (pet->max_hit - pet->hit) * percent / 100;
        	    pet->mana   += (pet->max_mana - pet->mana) * percent / 100;
        	    pet->move   += (pet->max_move - pet->move)* percent / 100;
    		}
    	     	return;
	     }
    	     KEY( "Exp",	pet->exp,		fread_number(fp));
    	     break;
    	     
    	 case 'G':
    	     KEY( "Gold",	pet->gold,		fread_number(fp));
    	     break;
    	     
    	 case 'H':
    	     KEY( "Hit",	pet->hitroll,		fread_number(fp));
    	     
    	     if (!str_cmp(word,"HMV"))
    	     {
    	     	pet->hit	= fread_number(fp);
    	     	pet->max_hit	= fread_number(fp);
    	     	pet->mana	= fread_number(fp);
    	     	pet->max_mana	= fread_number(fp);
    	     	pet->move	= fread_number(fp);
    	     	pet->max_move	= fread_number(fp);
    	     	fMatch = TRUE;
    	     	break;
    	     }
    	     break;
    	     
     	case 'L':
    	     KEY( "Levl",	pet->level,		fread_number(fp));
    	     KEY( "LnD",	pet->long_descr,	fread_string(fp));
	     KEY( "LogO",	lastlogoff,		fread_number(fp));
    	     break;
    	     
    	case 'N':
    	     KEY( "Name",	pet->name,		fread_string(fp));
    	     break;
    	     
    	case 'P':
    	     KEY( "Plat",	pet->platinum,		fread_number(fp));
    	     KEY( "Pos",	pet->position,		fread_number(fp));
    	     break;
    	     
	case 'R':
    	    KEY( "Race",	pet->race, race_lookup(fread_string(fp)));
    	    break;
 	    
    	case 'S' :
    	    KEY( "Save",	pet->saving_throw,	fread_number(fp));
    	    KEY( "Sex",		pet->sex,		fread_number(fp));
    	    KEY( "ShBy",	pet->shielded_by,	fread_flag(fp));
            KEY( "Silv",        pet->silver,            fread_number( fp ) );
    	    break;
    	    
    	if ( !fMatch )
    	{
    	    bug("Fread_pet: no match.",0);
    	    fread_to_eol(fp);
    	}
    	
    	}
    }
}

extern	OBJ_DATA	*obj_free;

void fread_obj( void *loadto, FILE *fp, bool isroom )
{
    OBJ_DATA *obj;
    char *word;
    int iNest;
    bool fMatch;
    bool fNest;
    bool fVnum;
    bool first;
    bool new_format;  /* to prevent errors */
    bool make_new;    /* update object */
    ROOM_INDEX_DATA *room = NULL;
    CHAR_DATA *ch = NULL;
   
    fVnum = FALSE;
    obj = NULL;
    first = TRUE;  /* used to counter fp offset */
    new_format = FALSE;
    make_new = FALSE;

    if(isroom)
    {
        room = (ROOM_INDEX_DATA *) loadto;
    }
    else
    {
        ch = (CHAR_DATA *) loadto;
    }

    word   = feof( fp ) ? "End" : fread_word( fp );
    if (!str_cmp(word,"Vnum" ))
    {
        int vnum;
	first = FALSE;  /* fp will be in right place */
 
        vnum = fread_number( fp );
        if (  get_obj_index( vnum )  == NULL )
	{
            bug( "Fread_obj: bad vnum %d.", vnum );
	    obj = create_object(get_obj_index(OBJ_VNUM_BAG),-1);
	    new_format = TRUE;
	}
        else
	{
	    obj = create_object(get_obj_index(vnum),-1);
	    new_format = TRUE;
	}
	    
    }

    if (obj == NULL)  /* either not found or old style */
    {
    	obj = new_obj();
    	obj->name		= str_dup( "" );
    	obj->short_descr	= str_dup( "" );
    	obj->description	= str_dup( "" );
    }

    fNest		= FALSE;
    fVnum		= TRUE;
    iNest		= 0;

    for ( ; ; )
    {
	if (first)
	    first = FALSE;
	else
	    word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    if (!str_cmp(word,"AffD"))
	    {
		AFFECT_DATA *paf;
		int sn;

		paf = new_affect();

		sn = skill_lookup(fread_word(fp));
		if (sn < 0)
		    bug("Fread_obj: unknown skill.",0);
		else
		    paf->type = sn;

		paf->level	= fread_number( fp );
		paf->duration	= fread_number( fp );
		paf->modifier	= fread_number( fp );
		paf->location	= fread_number( fp );
		paf->bitvector	= fread_number( fp );
		paf->next	= obj->affected;
		obj->affected	= paf;
		fMatch		= TRUE;
		break;
	    }
            if (!str_cmp(word,"Affc"))
            {
                AFFECT_DATA *paf;
                int sn;
 
                paf = new_affect();
 
                sn = skill_lookup(fread_word(fp));
                if (sn < 0)
                    bug("Fread_obj: unknown skill.",0);
                else
                    paf->type = sn;
 
		paf->where	= fread_number( fp );
                paf->level      = fread_number( fp );
                paf->duration   = fread_number( fp );
                paf->modifier   = fread_number( fp );
                paf->location   = fread_number( fp );
                paf->bitvector  = fread_number( fp );
                paf->next       = obj->affected;
                obj->affected   = paf;
                fMatch          = TRUE;
                break;
            }
	    break;

	case 'C':
	    KEY( "Cond",	obj->condition,		fread_number( fp ) );
	    KEY( "Cost",	obj->cost,		fread_number( fp ) );
	    break;

	case 'D':
	    KEY( "Description",	obj->description,	fread_string( fp ) );
	    KEY( "Desc",	obj->description,	fread_string( fp ) );
	    break;

	case 'E':

	    if ( !str_cmp( word, "Enchanted"))
	    {
		obj->enchanted = TRUE;
	 	fMatch 	= TRUE;
		break;
	    }

	    KEY( "ExtraFlags",	obj->extra_flags,	fread_number( fp ) );
	    KEY( "ExtF",	obj->extra_flags,	fread_number( fp ) );

	    if ( !str_cmp( word, "ExtraDescr" ) || !str_cmp(word,"ExDe"))
	    {
		EXTRA_DESCR_DATA *ed;

		ed = new_extra_descr();

		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= obj->extra_descr;
		obj->extra_descr	= ed;
		fMatch = TRUE;
	    }

	    if ( !str_cmp( word, "End" ) )
	    {
		if ( !fNest || ( fVnum && obj->pIndexData == NULL ) )
		{
		    bug( "Fread_obj: incomplete object.", 0 );
		    free_obj(obj);
		    return;
		}
		else
	        {
		    if ( !fVnum )
		    {
			free_obj( obj );
			obj = create_object( get_obj_index( OBJ_VNUM_DUMMY ), 0 );
		    }

		    if (!new_format)
		    {
		    	obj->next	= object_list;
		    	object_list	= obj;
		    	obj->pIndexData->count++;
		    }

		    if (!obj->pIndexData->new_format 
		    && obj->item_type == ITEM_ARMOR
		    &&  obj->value[1] == 0)
		    {
			obj->value[1] = obj->value[0];
			obj->value[2] = obj->value[0];
		    }
		    if (make_new)
		    {
			int wear;
			
			wear = obj->wear_loc;
			extract_obj(obj);

			obj = create_object(obj->pIndexData,0);
			obj->wear_loc = wear;
		    }
		    if ( iNest == 0 || rgObjNest[iNest] == NULL )
		    {
			if(ch)
			    obj_to_char( obj, ch );
			else if(room)
			    obj_to_char( obj, ch );
		    }
		    else
			obj_to_obj( obj, rgObjNest[iNest-1] );
		    if (obj->pIndexData->vnum == OBJ_VNUM_QDIAMOND)
		    {
			if(ch)
			    ch->platinum++;
			else if(room)
			    obj_to_room( create_money( 1, 0, 0 ), room );
			extract_obj( obj );
		    }
		    return;
		}
	    }
	    break;

	case 'I':
	    KEY( "ItemType",	obj->item_type,		fread_number( fp ) );
	    KEY( "Ityp",	obj->item_type,		fread_number( fp ) );
	    break;

	case 'L':
	    KEY( "Level",	obj->level,		fread_number( fp ) );
	    KEY( "Lev",		obj->level,		fread_number( fp ) );
	    break;

	case 'N':
	    KEY( "Name",	obj->name,		fread_string( fp ) );

	    if ( !str_cmp( word, "Nest" ) )
	    {
		iNest = fread_number( fp );
		if ( iNest < 0 || iNest >= MAX_NEST )
		{
		    bug( "Fread_obj: bad nest %d.", iNest );
		}
		else
		{
		    rgObjNest[iNest] = obj;
		    fNest = TRUE;
		}
		fMatch = TRUE;
	    }
	    break;

   	case 'O':
	    if ( !str_cmp( word,"Oldstyle" ) )
	    {
		if (obj->pIndexData != NULL && obj->pIndexData->new_format)
		    make_new = TRUE;
		fMatch = TRUE;
	    }
	    break;
		    

	case 'S':
	    KEY( "ShortDescr",	obj->short_descr,	fread_string( fp ) );
	    KEY( "ShD",		obj->short_descr,	fread_string( fp ) );

	    if ( !str_cmp( word, "Spell" ) )
	    {
		int iValue;
		int sn;

		iValue = fread_number( fp );
		sn     = skill_lookup( fread_word( fp ) );
		if ( iValue < 0 || iValue > 3 )
		{
		    bug( "Fread_obj: bad iValue %d.", iValue );
		}
		else if ( sn < 0 )
		{
		    bug( "Fread_obj: unknown skill.", 0 );
		}
		else
		{
		    obj->value[iValue] = sn;
		}
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'T':
	    KEY( "Timer",	obj->timer,		fread_number( fp ) );
	    KEY( "Time",	obj->timer,		fread_number( fp ) );
	    break;

	case 'V':
	    if ( !str_cmp( word, "Values" ) || !str_cmp(word,"Vals"))
	    {
		obj->value[0]	= fread_number( fp );
		obj->value[1]	= fread_number( fp );
		obj->value[2]	= fread_number( fp );
		obj->value[3]	= fread_number( fp );
		if (obj->item_type == ITEM_WEAPON && obj->value[0] == 0)
		   obj->value[0] = obj->pIndexData->value[0];
		fMatch		= TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Val" ) )
	    {
		obj->value[0] 	= fread_number( fp );
	 	obj->value[1]	= fread_number( fp );
	 	obj->value[2] 	= fread_number( fp );
		obj->value[3]	= fread_number( fp );
		obj->value[4]	= fread_number( fp );
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Vnum" ) )
	    {
		int vnum;

		vnum = fread_number( fp );
		if ( ( obj->pIndexData = get_obj_index( vnum ) ) == NULL )
		    bug( "Fread_obj: bad vnum %d.", vnum );
		else
		    fVnum = TRUE;
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'W':
	    KEY( "WearFlags",	obj->wear_flags,	fread_number( fp ) );
	    KEY( "WeaF",	obj->wear_flags,	fread_number( fp ) );
	    KEY( "WearLoc",	obj->wear_loc,		fread_number( fp ) );
	    KEY( "Wear",	obj->wear_loc,		fread_number( fp ) );
	    KEY( "Weight",	obj->weight,		fread_number( fp ) );
	    KEY( "Wt",		obj->weight,		fread_number( fp ) );
	    break;

	}

	if ( !fMatch )
	{
	    bug( "Fread_obj: no match.", 0 );
	    fread_to_eol( fp );
	}
    }
    if (obj->pIndexData->vnum == OBJ_VNUM_QDIAMOND && room)
    {
	ch->platinum++;
	extract_obj( obj );
    }
}

void fwrite_room(ROOM_INDEX_DATA *room, FILE *fp)
{
    fprintf(fp,"#ROOM\n");
    fprintf(fp,"VNUM %d\n",room->vnum);
}

void save_room_obj(ROOM_INDEX_DATA *room)
{
    char strsave[MAX_INPUT_LENGTH];
    FILE *fp;

    fclose( fpReserve );
    sprintf( strsave, "%s%d.room", HOUSE_DIR, room->vnum );
    if ( ( fp = fopen( TEMP_FILE_HOUSE, "w" ) ) == NULL )
    {
        bug( "Save_room_obj: fopen", 0 );
        perror( strsave );
        abort();
    }

    fwrite_room(room, fp);
    if ( room->contents != NULL )
            fwrite_obj( NULL, room->contents, fp, 0 );

    fclose( fp );

    rename(TEMP_FILE_HOUSE,strsave);

    fpReserve = fopen( NULL_FILE, "r" );

}

void save_area_room_obj(AREA_DATA *area, FILE *fp)
{

    int vnum;
    ROOM_INDEX_DATA *room;

    for ( vnum = area->min_vnum; vnum <= area->max_vnum; vnum++ )
    {
        if ( ( room = get_room_index( vnum ) ) )
        {
		save_room_obj( room );
        }
    }
}

void do_rsave (CHAR_DATA *ch, char *argument)
{
    ROOM_INDEX_DATA *room;

    if(  (is_number(argument) &&
         (room = get_room_index(atoi(argument))) == NULL)
      || (room = ch->in_room) == NULL )
    {
        send_to_char("!!BUG!! do_rsave room not found!\n\r",ch);
        return;
    }

    save_room_obj(room);

    send_to_char("Saved.\n\r",ch);
    return;
}

void fwrite_bank_account(BANK_ACCOUNT_DATA *account,FILE *fp)
{
    fprintf(fp,"#BANK-ACCOUNT\n");
    fprintf(fp,"Balance %ld\n", account->balance);
    if(account->flags)
        fprintf(fp,"Flags %s\n", print_flags(account->flags));
    fprintf(fp,"End\n\n");

    return;
}

void fread_bank_account(CHAR_DATA *ch, FILE *fp)
{
    bool fMatch;
    char *word;
    BANK_ACCOUNT_DATA *account = new_bank_account();
    account->balance = 0;
    account->last_transaction = -1;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;
	case 'B':
	    KEY( "Balance",	account->balance,	fread_number( fp ) );
	    break;
	case 'F':
	    KEY( "Flags",	account->flags,		fread_flag( fp ) );
	    break;
	case 'L':
	    KEY( "LastTrans",	account->last_transaction, fread_number(fp) );
	    break;
	case 'R':
	    if(!str_cmp(word,"Record"))
	    {
                int tr = ++account->last_transaction;
                if(tr >= MAX_BANK_RECORDS - 1)
                    account->last_transaction = tr = 0;

                fread_word(fp); /* type */
		fread_number(fp); /* time */
		account->transactions[tr].who = fread_word(fp);
		account->transactions[tr].amount = fread_number(fp);
		account->transactions[tr].description = fread_string(fp);
	    }
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		ch->pcdata->bank_account = account;
		return;
	    }
	    break;

	} /* switch */

	if ( !fMatch )
	{
	    bug( "Fread_bank_account: no match.", 0 );
	    log_string(LOG_ERR, word );
	    fread_to_eol( fp );
	}
    }
}

void fwrite_pk_counter(PK_COUNT_DATA *pkc,FILE *fp)
{
    fprintf(fp,"#PK-COUNTER\n");

    for(; pkc; pkc = pkc->next)
    {
        fprintf(fp, "PK %s %d %ld %d %d %d\n",
               pkc->name,
               pkc->type,
               pkc->last_fight,
               pkc->wins, pkc->losses, pkc->ties);
    }

    fprintf(fp,"End\n\n");

    return;
}

PK_COUNT_DATA *fread_pk_counter(FILE *fp)
{
    bool fMatch;
    char *word;
    PK_COUNT_DATA *pkc = NULL;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		return pkc;
	    }
	    break;

        case 'P':
	    if ( !str_cmp( word, "PK" ) )
	    {
                PK_COUNT_DATA *npkc = new_pk_count();
                npkc->next = pkc;
                pkc = npkc;

                pkc->name = str_dup(fread_word(fp));
                pkc->type = fread_number(fp);
                pkc->last_fight = fread_number(fp);
                pkc->wins = fread_number(fp);
                pkc->losses = fread_number(fp);
                pkc->ties = fread_number(fp);

		fMatch = TRUE;
	    }            
            break;

	} /* switch */

	if ( !fMatch )
	{
	    bug( "Fread_pk_count: no match.", 0 );
	    log_string(LOG_ERR,word );
	    fread_to_eol( fp );
	}
    }

    return pkc;
}

/** Function: save_guilds
  * Descr   : Writes the clan_table to /data/guild.dat file.
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */
void save_guilds(CHAR_DATA *ch, char *argument)
{
  FILE *fp;
  int i;
  int value;
  char buf[MSL];
  
  sprintf(buf, "%sguild.dat", DATA_DIR);

  if ( !( fp = fopen(buf, "w+" ) ) )
  {
     bug( "Open_guild: fopen", 0 );
     return;
  }
  
  for (i=1; clan_lookup(clan_table[i].name) != 0; i++)
  {
    if ( !IS_SET(clan_table[i].flags, GUILD_DELETED) && clan_lookup(clan_table[i].name) != 0)
    {
      fprintf(fp, "\nGuild\t%s~\n", clan_table[i].name);

      fprintf(fp, "Who\t%s~\n", clan_table[i].who_name);

      fprintf(fp, "Description\t%s", clan_table[i].description);

      fprintf(fp, "Pit\t%d~\n", clan_table[i].pit);

      fprintf(fp, "Rooms\t%d\t%d\t%d\n", 
                   clan_table[i].room[0],  
                   clan_table[i].room[1],
                   clan_table[i].room[2]);

      for (value=0; value < MAX_RANK; value++)
      {  
        if (clan_table[i].rank[value].rankname == NULL ||
            clan_table[i].rank[value].rankname[0] == '\0')
           clan_table[i].rank[value].rankname = str_dup("Unassigned");
            
          fprintf(fp, "Rank\t%d\t%s~\n", 
                       value+1, 
                       clan_table[i].rank[value].rankname);
      }
        
      for (value=0; value < MAX_RANK; value++)
        if (clan_table[i].rank[value].skillname != NULL &&
            clan_table[i].rank[value].skillname[0] != '\0')
          fprintf(fp, "Skill\t%d\t%s~\n", 
                       value+1, 
                       clan_table[i].rank[value].skillname);
  
      fprintf(fp, "ML\t%d  %d  %d  %d\n", 
                   clan_table[i].ml[0],
                   clan_table[i].ml[1],
                   clan_table[i].ml[2],
                   clan_table[i].ml[3]);
      
      /* Remove "changed" bit before writing flags to file */
      if (IS_SET(clan_table[i].flags, GUILD_CHANGED))
        REMOVE_BIT(clan_table[i].flags, GUILD_CHANGED);
        
      fprintf( fp, "Flags %s \n",  fwrite_flag( clan_table[i].flags,  buf ) );

      fprintf(fp, "\n");
    }
  }  
  
  fprintf(fp, "\nEnd\n");
  fclose(fp);
  send_to_char("Guild data file (guild.dat) written.\n\r", ch);
  return;        
}

/** Function: load_guilds
  * Descr   : Loads the information specific to each guild/clan from
  *           the ../data/guild.dat file.
  * Returns : (n/a)
  * Syntax  : (n/a)
  * Written : v1.0 1/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

void load_guilds(void)
{
    FILE *fp;
    char buf[MAX_STRING_LENGTH];
    char *word;
    int count = 0;
    int i;
    bool fMatch = FALSE;

    for (i=0; i < MAX_CLAN; i++)
    {
      clan_table[i].name = "";
      clan_table[i].who_name = "";
	clan_table[i].pit = 0;
      clan_table[i].room[0]= 0;
      clan_table[i].room[1]= 0;
      clan_table[i].room[2]= 0;
      clan_table[i].rank[0].rankname = "";
      clan_table[i].rank[0].skillname = "";
      clan_table[i].ml[0] = 0;
      clan_table[i].ml[1] = 0;
      clan_table[i].ml[2] = 0;
      clan_table[i].ml[3] = 0;
      clan_table[i].flags = 0;
    }
    
    sprintf(buf, "%sguild.dat", DATA_DIR);

    if ((fp = fopen(buf, "r")) == NULL)
    {
	log_string(LOG_ERR,"Error: guild.dat file not found!");
	exit(1);
    }
    for (;;)
    {
	word = feof(fp) ? "End" : fread_word(fp);

	if (!str_cmp(word, "End"))
	    break;

	switch (UPPER(word[0]))
	  {
	   case 'D':
		clan_table[count].description = fread_string(fp);
		break;
          case 'F':
              clan_table[count].flags  = fread_flag( fp );
              fMatch = TRUE;
              break;
              
	  case 'G':
	      count++;
	      clan_table[count].name = fread_string(fp);
	      fMatch = TRUE;
	      break;

	  case 'P':
	      clan_table[count].pit = fread_number(fp);
	      fMatch = TRUE;
	      break;

	  case 'R':
	      if (!str_cmp(word, "Rooms"))
	      {
		  clan_table[count].room[0] = fread_number(fp);	/* hall   */
		  clan_table[count].room[1] = fread_number(fp);	/* morgue */
		  clan_table[count].room[2] = fread_number(fp);	/* temple */
		  fMatch = TRUE;
	      } else if (!str_cmp(word, "Rank"))
	      {
		  i = fread_number(fp);
		  clan_table[count].rank[i - 1].rankname = fread_string(fp);
		  fMatch = TRUE;
	      }
	      break;

	  case 'S':
	      i = fread_number(fp);
	      clan_table[count].rank[i - 1].skillname = fread_string(fp);
	      fMatch = TRUE;
	      break;

	  case 'M':
	      clan_table[count].ml[0] = fread_number(fp);
	      clan_table[count].ml[1] = fread_number(fp);
	      clan_table[count].ml[2] = fread_number(fp);
	      clan_table[count].ml[3] = fread_number(fp);
	      fMatch = TRUE;
	      break;

	  case 'W':
	      clan_table[count].who_name = fread_string(fp);
	      fMatch = TRUE;
	      break;

	  }			/* end of switch */

    }				/* end of while (!feof) */

    if (!fMatch)
    {
	bug("Fread_guilds: no match.", 0);
	fread_to_eol(fp);
    }
    fclose(fp);
    return;
} /* end: load_guilds */

