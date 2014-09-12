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
#include "recycle.h"
#include "tables.h"

/* Thanks to Zallik for modifications to the code */

void do_omni( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    BUFFER *output;
    DESCRIPTOR_DATA *d;
    int immmatch;
    int mortmatch;
    int hptemp;  
 /*
 * Initalize Variables.
 */

    immmatch = 0;
       mortmatch = 0;
    buf[0] = '\0';
    output = new_buf();

 /*          
 * Count and output */
      sprintf(buf,"\n\r{B---{WI{wm{Wm{wo{Wr{wt{Wa{wl{Ws{B----{x\n\r");
        add_buf(output,buf);
        sprintf( buf, "{WN{wa{Wm{we           {WL{we{Wv{we{Wl   {WW{wi{Wz      {WI{wn{Wc{wo{Wg    {WG{wh{Wo{ws{Wt    {W[{wV{Wn{wu{Wm{W]{x\n\r");      
        add_buf(output,buf);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;

        if ( d->connected != CON_PLAYING || !can_see( ch, d->character ) )    
            continue;

        wch   = ( d->original != NULL ) ? d->original : d->character;

        if (!can_see(ch,wch)  || wch->level < 102)
            continue;

        immmatch++;

        sprintf( buf, "{R%-14s %-3d     %-3d      %-3d      %-3d      [%-3d]{x\n\r",
                        wch->name,
                        wch->level,
                        wch->invis_level,
                        wch->incog_level,
			   wch->ghost_level,
                        wch->in_room->vnum);
                        add_buf(output,buf);
    }


 /*
 * Count and output the Morts.
 */
        sprintf( buf, " \n\r {B----{YM{yo{Yr{yt{Ya{yl{Ys{B----{x\n\r");
        add_buf(output,buf);
        sprintf( buf, "{WN{wa{Wm{we           {WG{wo{Wd      {WR{wa{Wc{we{W/{wC{Wl{wa{Ws{ws  {WP{wo{Ws{wi{Wt{wi{Wo{wn      {WL{we{Wv   {w%%{Wh{wp{Ws    {w[{WV{wn{Wu{wm{W]{x\n\r");   

        add_buf(output,buf);
        hptemp = 0;

   for ( d = descriptor_list; d != NULL; d = d->next )    
    {
        CHAR_DATA *wch;
        char const *class;

        if ( d->connected != CON_PLAYING || !can_see( ch, d->character ) )
            continue;

        wch   = ( d->original != NULL ) ? d->original : d->character;

        if (!can_see(ch,wch) || wch->level > ch->level || wch->level > 101)   
            continue;

        mortmatch++;
        if ((wch->max_hit != wch->hit) && (wch->hit > 0))
                hptemp = (wch->hit*100)/wch->max_hit;
        else if (wch->max_hit == wch->hit)
                hptemp = 100;
        else if (wch->hit < 0)
                hptemp = 0;    

        class = class_table[wch->class].who_name;

        sprintf( buf, "{G%-14s %-7s  %6.6s/%3.3s  %-10s    %-3d   %-3d%%    [%-3d]{x\n\r",
                wch->name,
                IS_GOOD(wch)? "Good" : IS_EVIL(wch) ? "Evil" : "Neutral",
                wch->race < MAX_PC_RACE ? pc_race_table[wch->race].who_name : " ",
                class,
                capitalize( position_table[wch->position].name) ,
                wch->level,
                hptemp,
                wch->in_room->vnum);
        add_buf(output,buf);
    }

/*
 * Tally the counts and send the whole list out.
 */
   sprintf( buf2, "\n\r{CIMMs found: {w%d{x\n\r", immmatch );
    add_buf(output,buf2);
    sprintf( buf2, "{CMorts found: {y%d{x\n\r", mortmatch );    
    add_buf(output,buf2);
    page_to_char( buf_string(output), ch );
    free_buf(output);
    return;
}                          
