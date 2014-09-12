#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "event.h"
#include "signal.h"

EVENT *event_hash[MAX_EVENT_HASH];
EVENT *system_event_list;

int map_room args( (ROOM_INDEX_DATA *room, int cx, int cy, int ttl) );

int     xp_compute      args( ( CHAR_DATA *gch, CHAR_DATA *victim,
                            int total_levels ) );

bool microheal_test = TRUE;

#define MAX_AMAP_WIDTH 70
#define MAX_AMAP_LENGTH 22
#define AMAP_PATH_VERT -3
#define AMAP_PATH_HORZ -2
#define AMAP_PATH_VERT_CLSD -7
#define AMAP_PATH_HORZ_CLSD -6
#define AMAP_PATH_VERT_DOOR -5
#define AMAP_PATH_HORZ_DOOR -4
#define AMAP_NOTHING 0

int amap[MAX_AMAP_WIDTH][MAX_AMAP_LENGTH];
int amap_paths, amap_overlap_paths;

extern int micro_hit_gain(CHAR_DATA *ch);
extern int micro_mana_gain(CHAR_DATA *ch);
extern int micro_move_gain(CHAR_DATA *ch);

char *iotdm;

void event_update( )
{
    EVENT *e = NULL;

    while ((e = event_hash[(current_pulse) % MAX_EVENT_HASH]) != NULL)
    {

        if(e->when != current_pulse)
            break;

        if(e->next_global == e)
        {
            bug("event_update: Event points to self.",0);
            abort();
        }

        /* Remove from global */
        event_hash[(current_pulse) % MAX_EVENT_HASH] = e->next_global;

        if (IS_SET(e->flags, EVENT_DELETED))
        {            
            free_event(e);
            continue;
        }

        e->executions++;

        (e->call) (e); /* Run action */

        if(IS_SET(e->flags,EVENT_DELETED) || !IS_SET(e->flags,EVENT_REPEAT))
        {
            /* Remove from local */
            switch(e->actor_type)
            {
                case EVENT_ACTOR_CHAR:
	            event_remove_local(&e->actor.ch->event_list, e);
                    break;
                case EVENT_ACTOR_SYSTEM:
                    event_remove_local(&system_event_list, e);
                    break;
            }

            free_event(e); /* Assume no reuse */
        }

    }
}
/* event_add:
 * Add to the global list, assuming e already has necessary data.
 */

void event_add (EVENT *e)
{
    EVENT **bucket; /* Pointer to a pointer */

    if(!e)
    {
        bug("add_event: null event",0);
        abort(); /* Yucks! */
    }

    bucket = &event_hash[e->when % MAX_EVENT_HASH];

    /* Nothing there, or the first event there goes off after ours */
    if (*bucket == NULL || (*bucket)->when > e->when)
    {
        e->next_global = *bucket;
	*bucket = e;
    }
    else /* go through that list, and find the event to go off before ours */
    {
        EVENT *prev;
        for (prev = *bucket;
             prev->next_global && prev->next_global->when <= e->when;
             prev = prev->next_global) ; /* Merely skip to that point */

        e->next_global = prev->next_global;
        prev->next_global = e;
    }
}

void event_add_char(EVENT *e, CHAR_DATA *ch)
{
    e->next_local = ch->event_list;
    ch->event_list = e;
    event_add(e);
}

void event_add_system(EVENT *e)
{
    e->next_local = system_event_list;
    system_event_list = e;
    event_add(e);
}

EVENT *find_event_type(EVENT *list, EVENT_FUN *func)
{
    for(; list; list = list->next_local)
    {
        if(list->call == func)
            return list;
    }
    return NULL;
}

void interrupt_char(CHAR_DATA *ch, long flags)
{
    EVENT *e, *e_last = NULL;

    for(e = ch->event_list; e != NULL; e = e->next_local)
    {
        if(IS_SET(e->flags, flags) && !IS_SET(e->flags, EVENT_DELETED))
        {
            if(e->interrupt_func)
            {
                if(e->interrupt_func)
                   (e->interrupt_func)(e);
            }

            if(e_last)
                e_last->next_local = e->next_local;
            else
                ch->event_list = e->next_local;
            
            SET_BIT(e->flags, EVENT_DELETED);
        }

        e_last = e;
    }    
    return;
}

/* Find and remove this event from the global list */
void event_remove (EVENT *e)
{
    if(e->when > current_pulse + 600 * PPS) /* over ten minutes */
    { /* Easier to just let it sit */
        SET_BIT(e->flags, EVENT_DELETED);
        return;
    }

    if (event_hash[e->when % MAX_EVENT_HASH] == e)
        event_hash[e->when % MAX_EVENT_HASH] = e->next_global;
    else
    {
        EVENT *prev;

        for (prev = event_hash[e->when % MAX_EVENT_HASH]; prev; prev = prev->next_global)
        {
            if (prev->next_global == e)
                break;
        }
        if (prev)
            prev->next_global = e->next_global;
        else
            bug_f("event_remove: no previous!");
    }


}

/* Remove event 'e' from event list event_list
 * This will remove the event from a local (e.g. on an object, mob, room)
 * linked list (threaded via next_local)
 */
void event_remove_local(EVENT **event_list, EVENT *e)
{
    if (*event_list == e)
        *event_list = e->next_local;
    else
    {
        EVENT *prev;

        for (prev = *event_list; prev; prev = prev->next_local)
        {
            if (prev->next_local == e)
                break;
        }
        if (prev)
            prev->next_local = e->next_local;
        else /* Ouch, this shouldn't happen! */
            bug_f("event_remove_local: no previous!");
    }
}

void do_etest ( CHAR_DATA *ch, char *argument )
{
    EVENT *e;
    char buf[MAX_STRING_LENGTH * 2] = "";
/*    char arg1[MAX_STRING_LENGTH * 2] = "";*/
    char arg[MAX_STRING_LENGTH * 2] = "";

    if(argument[0] == '\0')
    {
        send_to_char("syntax: etest delay\n\r",ch);
        send_to_char("        etest repeat\n\r",ch);
        send_to_char("        etest abort\n\r",ch);
        return;
    }

    argument = one_argument(argument,arg);

    if(!str_cmp(arg,"coredump"))
    {
        log_string(LOG_ERR,"%s is safe-coredumping",ch->name);
        safe_coredump();
        send_to_char("Ok.\n\r",ch);
        return;
    }    

    if(!str_cmp(arg,"beep"))
    {
        char arg2[MSL];
        CHAR_DATA *victim;

        argument = one_argument(arg2,argument);

        if( (victim = get_char_world( ch, arg2 )) ==  NULL)
        {
            send_to_char("They aren't here.\n\r", ch);
            return;
        }

        send_to_char("\aBeep!\n\r",victim);
        return;
    }

    if(!str_cmp(arg,"dnslock"))
    {
        extern bool dnslock;

        if(!dnslock)
        {
            dnslock = TRUE;
            send_to_char("DNSlock on.\n\r",ch);
        }
        else
        {
            dnslock = FALSE;
            send_to_char("DNSlock off.\n\r",ch);
        }
        return;
    }

    if(!str_cmp(arg,"microheal"))
    {

        if(!microheal_test)
        {
            microheal_test = TRUE;
            send_to_char("MicroHealing on.\n\r",ch);
        }
        else
        {
            microheal_test = FALSE;
            send_to_char("Microhealing off.\n\r",ch);
        }
        return;
    }

    if(!str_cmp(arg,"analyze"))
    {
        CHAR_DATA *victim;
        MOB_INDEX_DATA *pIndex;
        int clevel;

        if( (victim = get_char_world( ch, argument)) ==  NULL)
        {
            send_to_char("They aren't here.\n\r", ch);
            return;
        }

send_to_char("Analyzing...\n\r",ch);

        pIndex = victim->pIndexData;


    if(victim->level > 20)
    {
        clevel = ch->level;

        for(ch->level = UMAX(0,pIndex->level - 10);
            ch->level < pIndex->level + 10;
            ch->level++)
       {
        sprintf(buf, "Level %d: %d xp\n\r",
          ch->level,
          xp_compute(ch, victim, ch->level));
        send_to_char(buf,ch);
       }
        ch->level = clevel;
    }
return;
    }

    if(!str_prefix(argument,"map "))
    {


    }


    if(!str_cmp(arg,"numberbar"))
    {
        send_to_char("[",ch);
        numberbar(buf,20,number_range(0,10),10);
        send_to_char(buf,ch);
        send_to_char("{x]\n\r",ch);
        return;
    }


    if(!str_cmp(arg,"delay"))
    {
        send_to_char("Event test: 5 seconds\n\r",ch);

        e = new_event();
        e->when = current_pulse + 5 * PPS;
        e->actor.ch = ch;
        e->actor_type = EVENT_ACTOR_CHAR;
        e->call = event_test;

        event_add_char(e,ch);
        return;
    }

    if(!str_cmp(arg,"repeat"))
    {
        send_to_char("Event test: EVERY ? seconds\n\r",ch);

        e = new_event();
        e->when = current_pulse + 5 * PPS;
        e->actor.ch = ch;
        e->actor_type = EVENT_ACTOR_CHAR;
        SET_BIT(e->flags,EVENT_REPEAT);
        e->call = event_test;

        event_add_char(e,ch);
        return;
    }

    if(!str_cmp(arg,"abort"))
    {
        EVENT *e;
        e = event_function_remove_local(&ch->event_list, event_test);
        event_remove(e);
        
        send_to_char("Etest aborted.\n\r",ch);
        return;
    }

    return;
}

/* Removes all the events from a list both locally and globally */
void event_list_kill ( EVENT *list )
{
    EVENT *e, *next_e;

    for( e = list; e != NULL; e = next_e)
    {
        event_remove(e);
        next_e = e->next_local;
        free_event(e);
     }
}

void event_test ( EVENT *e )
{

    return;

}

EVENT* event_function_remove_local ( EVENT **event_list, EVENT_FUN *func )
{
/*  Turns out this isn't worth the trouble


    EVENT *prev, *e;

    for (e = *event_list, prev = NULL; e; e = prev->next_local)
    {
        if(e->call == func);
        {
            if(e == *event_list)
                *event_list = NULL;
            else
                prev->next_local = e->next_local;

            return e;
        }
    }
*/

    EVENT *e;

    for (e = *event_list; e; e = e->next_local)
    {
        if(e->call == func);
        {
            SET_BIT(e->flags,EVENT_DELETED);
            return e;
        }
    }

    return NULL;
}

void event_hit_gain ( EVENT *e )
{
    int gain;
    int number;
  
    int timeadded = 10 * PPS + number_range(PPS/8, PPS/4);

    if(!microheal_test || e->actor.ch->hit >= e->actor.ch->max_hit)
    {
        e->actor.ch->hit = e->actor.ch->max_hit;
        SET_BIT(e->flags, EVENT_DELETED);
        return;
    }

   gain = micro_hit_gain(e->actor.ch);
   e->actor.ch->hit += gain;


    if(e->actor.ch->hit >= e->actor.ch->max_hit)
    {
        SET_BIT(e->flags, EVENT_DELETED);
        return;
    }

    number = number_percent() + 20;
    if (number < get_skill(e->actor.ch,gsn_fast_healing))
    {
       timeadded *= 0.7;
    }

    if(timeadded < 2 * PPS)
       timeadded = 2 * PPS;

    e->when = current_pulse + timeadded;
    event_add(e);

    return;
}


int map_room(ROOM_INDEX_DATA *room, int cx, int cy, int ttl)
{
    int newx, newy, door, error, pathtype, pathx, pathy;
    EXIT_DATA *exit;

    if(cx >= MAX_AMAP_WIDTH || cx < 0
        || cy >= MAX_AMAP_LENGTH || cy < 0)
        return 2; /* Should not happen */

    if(amap[cx][cy]) return 0; /* Already marked - die */
    amap[cx][cy] = room->vnum;
    if(ttl <= 0) return 0; /* Time to live expired - die */
    for(door = 0; door < 4; door++)
    {
        pathtype = 0;

        if(!(exit = room->exit[door])) continue;
        if(!(exit->u1.to_room)) continue;
        if(exit->u1.to_room == room) return 1; /* Loopback - error */
        if(room->area != exit->u1.to_room->area) continue;

        newx = pathx = cx; /* Copy location to an offsetable variable */
        newy = pathy = cy;

        if(door == 0) newy -= 2;
        if(door == 1) newx += 2;
        if(door == 2) newy += 2;
        if(door == 3) newx -= 2;

        if(door == 0){ pathy -= 1; pathtype = AMAP_PATH_VERT; }
        if(door == 1){ pathx += 1; pathtype = AMAP_PATH_HORZ; }
        if(door == 2){ pathy += 1; pathtype = AMAP_PATH_VERT; }
        if(door == 3){ pathx -= 1; pathtype = AMAP_PATH_HORZ; }

        if(IS_SET(exit->exit_info,EX_CLOSED))
            pathtype -= 4;
        else if(IS_SET(exit->exit_info,EX_ISDOOR))
            pathtype -= 2;


        if(!(newx >= MAX_AMAP_WIDTH || newx < 0
            || newy >= MAX_AMAP_LENGTH || newy < 0))
        {
            amap[pathx][pathy] = pathtype;
            amap_paths++;
        }

        if(newx >= MAX_AMAP_WIDTH || newx < 0
            || newy >= MAX_AMAP_LENGTH || newy < 0)
            continue; /* Hits boundary - skip it */

        if((error =
            map_room(exit->u1.to_room,newx,newy,ttl - 1)
                 ) > 0)
            return error;

    }

    return 0;

}

void do_map(CHAR_DATA *ch, char *argument)
{
    int x, y, error, startx, starty, cn;
    char buf[MAX_STRING_LENGTH * 2];
    char lbuf[MAX_STRING_LENGTH];

    amap_paths = 0;

    startx = MAX_AMAP_WIDTH/2;
    starty = MAX_AMAP_LENGTH/2;

    if(argument[0] == '\0' || !str_cmp(argument,"center"))
    {
        startx = MAX_AMAP_WIDTH/2;
        starty = MAX_AMAP_LENGTH/2;
    }

    if(!str_cmp(argument,"south"))
    { 
        startx = MAX_AMAP_WIDTH/2;
        starty = 1;
    }
    if(!str_cmp(argument,"north"))
    {
        startx = MAX_AMAP_WIDTH/2;
        starty = MAX_AMAP_LENGTH - 1;
    }
    if(!str_cmp(argument,"west"))
    {
        startx = MAX_AMAP_WIDTH - 1;
        starty = MAX_AMAP_LENGTH/2;
    }
    if(!str_cmp(argument,"east"))
    {
        startx = 1;
        starty = MAX_AMAP_LENGTH/2;
    }

    for(x = 0; x < MAX_AMAP_WIDTH; x++)
        for(y = 0; y < MAX_AMAP_LENGTH; y++)
            amap[x][y] = AMAP_NOTHING;

    error = map_room(ch->in_room,startx,starty,70);

    if(error)
    {
        send_to_char("Error auto-mapping.\n\r",ch);
        return;
    }

    send_to_char("\n\r",ch);

    strcpy(buf,"");

    for(y = 0; y < MAX_AMAP_LENGTH; y++)
    {
        cn = 0;
        strcpy(lbuf,"");
        for(x = 0; x < MAX_AMAP_WIDTH; x++)
            switch(amap[x][y])
            {
                case   AMAP_NOTHING: strcat(lbuf," "); break;
                case AMAP_PATH_HORZ: strcat(lbuf,"-"); cn++; break;
                case AMAP_PATH_VERT: strcat(lbuf,"|"); cn++; break;
                case AMAP_PATH_HORZ_CLSD: strcat(lbuf,"{b-{x"); cn++; break;
                case AMAP_PATH_VERT_CLSD: strcat(lbuf,"{b|{x"); cn++; break;
                case AMAP_PATH_HORZ_DOOR: strcat(lbuf,"{r-{x"); cn++; break;
                case AMAP_PATH_VERT_DOOR: strcat(lbuf,"{r|{x"); cn++; break;
                    default:
                        if(amap[x][y] == ch->in_room->vnum)
                            strcat(lbuf,"{Co{x");
                        else strcat(lbuf,"{co{x");
                        cn++; break;
            }
        strcat(lbuf,"\n\r");
        if(!cn) continue;
        strcat(buf,lbuf);
    }

    send_to_char(buf,ch);


    return;
}



void event_mana_move_gain ( EVENT *e )
{
    int gain;
    int number;
  
    int timeadded = 9 * PPS + number_range(PPS/8, PPS/4);

    if(!microheal_test
        && e->actor.ch->mana >= e->actor.ch->max_mana
        && e->actor.ch->move >= e->actor.ch->max_move)
    {
        e->actor.ch->mana = e->actor.ch->max_mana;
        e->actor.ch->move = e->actor.ch->max_move;
        SET_BIT(e->flags, EVENT_DELETED);
        return;
    }

    gain = micro_mana_gain(e->actor.ch);

    e->actor.ch->mana += gain;

    gain = micro_move_gain(e->actor.ch);
    e->actor.ch->move += gain;

    if(e->actor.ch->mana >= e->actor.ch->max_mana
       && e->actor.ch->move >= e->actor.ch->max_move)
    {
        SET_BIT(e->flags, EVENT_DELETED);
        return;
    }

    number = number_percent() + 20;
    if (number < get_skill(e->actor.ch,gsn_meditation))
    {
       timeadded *= 0.7;
    }

    if(timeadded < 5 * PPS)
       timeadded = 5 * PPS;

    e->when = current_pulse + timeadded;
    event_add(e);

    return;
}

