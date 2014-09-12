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
#include "recycle.h"
#include "tables.h"
#include "olc.h"

AREA_DATA *get_area_data        args( ( int vnum ) );

void do_vmfind(CHAR_DATA *ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    char name[MAX_INPUT_LENGTH] = "";
    AREA_DATA *pArea = NULL;
    MOB_INDEX_DATA *pMobIndex;
    int nMatch = 0;
    int vnum;
    sh_int level_min = 0;
    sh_int level_max = 0;
    long act = 0, imm = 0, off = 0, res = 0, aff = 0, vuln = 0;
#if 1
    long shd = 0;
#endif

    if(argument[0] == '\0')
    {
        send_to_char("Syntax: vmfind <field> <value(s)> [...]\n\r",ch);
        send_to_char("Valid fields: name level\n\r",ch);
        send_to_char("Valid fields: act imm off res aff vuln shd\n\r",ch);
        return;
    }
    output = new_buf();

    while(argument[0] != '\0')
    {
        argument = one_argument(argument, arg1);
        argument = one_argument(argument, arg2);
        if(!str_cmp(arg1, "name"))
            strcpy(name,arg2);
        if(!str_cmp(arg1, "area"))
            pArea = get_area_data(atoi(arg2));
        else if(!str_cmp(arg1, "level"))
            level_min = level_max = atoi(arg2);
        else if(!str_cmp(arg1, "area"))
        {
        }
        else if(!str_cmp(arg1, "act"))
        {
            if((act = flag_value(act_flags,arg2)) == NO_FLAG)
            {
                send_to_char("No flags found.\n\r",ch);
                return;
            }
        }
        else if(!str_prefix(arg1, "affect"))
        {
            if((aff = flag_value(affect_flags,arg2)) == NO_FLAG)
            {
                send_to_char("No flags found.\n\r",ch);
                return;
            }
        }
#if 1
        else if(!str_prefix(arg1, "shield"))
        {
            if((shd = flag_value(shield_flags,arg2)) == NO_FLAG)
            {
                send_to_char("No flags found.\n\r",ch);
                return;
            }
        }
#endif
        else if(!str_prefix(arg1, "offensive"))
        {
            if((off = flag_value(off_flags,arg2)) == NO_FLAG)
            {
                send_to_char("No flags found.\n\r",ch);
                return;
            }
        }
        else if(!str_prefix(arg1, "immune"))
        {
            if((imm = flag_value(imm_flags,arg2)) == NO_FLAG)
            {
                send_to_char("No flags found.\n\r",ch);
                return;
            }
        }
        else if(!str_prefix(arg1, "resistant"))
        {
            if((res = flag_value(res_flags,arg2)) == NO_FLAG)
            {
                send_to_char("No flags found.\n\r",ch);
                return;
            }
        }
        else if(!str_prefix(arg1, "vulnerable"))
        {
            if((vuln = flag_value(vuln_flags,arg2)) == NO_FLAG)
            {
                send_to_char("No flags found.\n\r",ch);
                return;
            }
        }
        else
        {
            send_to_char("Bad flag",ch);
            return;
        }

    }


    for ( vnum = 0; nMatch < top_mob_index; vnum++ )
    {
        if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
        {
            nMatch++;
            if ( (name[0] == '\0'
                 || is_name( argument, pMobIndex->player_name ))
                 && (!act || IS_SET(pMobIndex->act, act) )
                 && (!off || IS_SET(pMobIndex->off_flags, off) )
                 && (!imm || IS_SET(pMobIndex->imm_flags, imm) )
                 && (!vuln|| IS_SET(pMobIndex->vuln_flags, vuln) ) 
                 && (!res || IS_SET(pMobIndex->res_flags, res) )
                 && (!shd || IS_SET(pMobIndex->shielded_by, shd) )
               )
            {
                sprintf( buf, "[%5d] %s\n\r",
                    pMobIndex->vnum, pMobIndex->short_descr );
                add_buf(output,buf);
            }
        }
    }

    page_to_char( buf_string(output), ch );

    free_buf(output);
    return;

}
