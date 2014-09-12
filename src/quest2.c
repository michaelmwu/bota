/* Automated quest code originall written by Vassago (Who i'm
giving
credit to despite the fact he gives no credit to the people who's code he
copies (aka the rom consortium).  
Revamped by Kharas (mud@fading.tcimet.net)
Converted to ROM/STowers Format by Defiant

This was originally modified for my smaug mud, but I think i've changed
everything so it will work with rom.  Go ahead and write me if ya have
problems (mud@fading.tcimet.net)
*/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "merc.h"
#include "tables.h"
#include "recycle.h"
#include "quest.h"

DECLARE_DO_FUN( do_say );
DECLARE_DO_FUN( do_tell );

ROOM_INDEX_DATA *find_location args( ( CHAR_DATA *ch, char * arg ) );

/*5625 aarakocan town square
5021 sands of sorrow fungus temple
*/

int VIP_list[] =
{ 30502 };

QUEST_DATA* generate_escort_quest(CHAR_DATA *ch)
{
    QUEST_DATA *quest;
    ROOM_INDEX_DATA *startroom, *destination;
    CHAR_DATA *vc;

    if( (startroom = get_room_index(3382)) == NULL )
    {
        bug_f("Escort quest starting room NOT FOUND!");
        return NULL;
    }

    if( (destination = get_room_index(5625)) == NULL )
        bug_f("Escort quest destination room NOT FOUND!");

    for(vc = startroom->people; vc != NULL; vc = vc->next_in_room)
    {
        if(IS_NPC(vc) && vc->pIndexData->vnum == 30502)
            break;
    }

    quest = new_quest();

    quest->type = QUEST_ESCORT;

    quest->target.ch = vc; /* VIP */
    quest->target2.room = destination; /* destination */

    quest->timeleft = 60;
    quest->status = QUEST_IN_PROGRESS;

    /*add_follower(vc, ch);*/
    vc->master = ch;
    vc->leader = ch;

    return quest;
}
