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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"

/*
 * Bank code written entirely by Thyrr of BOTA
 * Do not distribute.
 *
 */

bool	check_parse_name	args( ( char *name ) );
DECLARE_DO_FUN(do_quit);

void do_account(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    BANK_ACCOUNT_DATA *account;
    sh_int mtype = VALUE_GOLD;
    long amount;

/*
    send_to_char("You de\231%sgoss 'Lo%+<H1$n$h{R/player/%s%s(\n\r\n\r's backstab|",ch);
    return;
*/

    if(IS_NPC(ch))
    {
        send_to_char("NPCs cannot manage bank accounts.\n\r",ch);
        return;
    }

    if(!IS_SET(ch->in_room->service_flags, SERVICE_BANK))
    {
        send_to_char("This is not a bank.\n\r",ch);
        return;
    }
    
    argument = one_argument(argument,arg);

    if(arg[0] == '\0')
    {
        send_to_char("Syntax:\n\r",ch);
        if(ch->pcdata->bank_account)
        {
            send_to_char(
                "account deposit <amount> [silver/plat]\n\r"
                "account withdraw <amount> [silver/plat]\n\r"
                "account transfer <amount> [silver/plat] <recipient>\n\r"
                "account report\n\r"
                "\n\r ( account transfer in testing: use at your own risk )\n\r"
                "\n\rTransfers to clans: use # in front of the "
                "clan name\n\r",ch);
        }
        else
        {
            send_to_char("account create\n\r",ch);
            send_to_char("\n\r"
                "To apply for a bank account, type: account create\n\r",ch);
            send_to_char(
               "There is a 10 gold application fee and a minimum initial "
               "deposit of 30 gold.\n\r",ch);
            send_to_char(
                "You must be at least level 35 to create a bank account.\n\r",ch);
        }

        return;
    }

    if(!str_prefix(arg,"create"))
    {
        if(ch->pcdata->bank_account)
        {
            send_to_char("You already have a bank account.\n\r",ch);
            return;
        }

        if(get_trust(ch) < 35)
        {
            send_to_char("You must be at least level 35 to open a bank account.\n\r",ch);
            return;
        }

        if(TOTAL_MONEY_SILVER(ch) < 40 * GOLD_PER_SILVER)
        {
            send_to_char(
                "Need at least 40 gold to open an account.\n\r",ch);
            return;
        }

        ch->pcdata->bank_account = new_bank_account();
        deduct_cost(ch,40,VALUE_GOLD);
	BALANCE_MONEY(ch);
        ch->pcdata->bank_account->balance = 30 * GOLD_PER_SILVER;

        send_to_char("You open an account.\n\r",ch);

        return;
    }

    if(!ch->pcdata->bank_account)
    {
        send_to_char("You don't have a bank account!\n\r",ch);
        if(ch->level >= 35)
        {
            send_to_char(
                "To apply for a bank account, type: account create\n\r"
                "There is a 10 gold application fee and a minimum initial"
                " deposit of 30 gold.\n\r",ch);
        }
        else
            send_to_char(
                "You must be at least level 35 to create a bank account.\n\r",ch);
        return;
    }

    account = ch->pcdata->bank_account;

    if(IS_SET(account->flags, BANK_ACCOUNT_FROZEN))
    {
        send_to_char("This bank account is FROZEN.\n\r",ch);
        return;
    }

    if(!strcmp(arg,"deposit"))
    {
        argument = one_argument(argument,arg2);
        argument = one_argument(argument,arg3);

        if(IS_SET(account->flags, BANK_ACCOUNT_NODEPOSIT))
        {
            send_to_char("You are not allowed to deposit money"
                         "to this account.\n\r",ch);
            return;
        }

        if(arg2[0] == '\0')
        {
            send_to_char("Account deposit how much?\n\r",ch);
            return;
        }

        if(!is_number(arg2))
        {
            send_to_char("You can't deposit that.\n\r",ch);
            return;
        }

        if( (amount = atol(arg2)) < 1 )
        {
            send_to_char("Try withdraw instead.\n\r",ch);
            return;
        }

        if(amount > 30000)
        {
            send_to_char("You can't deposit that much of one currency at once.\n\r",ch);
            return;
        }

        if(arg3[0] == '\0' || !str_prefix(arg3,"gold"))
            mtype = VALUE_GOLD;
        else if(!str_prefix(arg3,"platinum"))
            mtype = VALUE_PLATINUM;
        else if(!str_prefix(arg3,"silver"))
            mtype = VALUE_SILVER;
        else
        {
            send_to_char("What kind of currency is THAT?\n\r",ch);
            return;
        }

        if(amount * MONEY_VALUE_SILVER(mtype) > TOTAL_MONEY_SILVER(ch))
        {
            send_to_char("You don't have that much money.\n\r",ch);
            return;
        }

        if(amount * MONEY_VALUE_SILVER(mtype) + account->balance >
	    200000 * PLATINUM_PER_SILVER)
        {
            send_to_char("Your bank account can't hold that much.\n\r",ch);
            return;
        }

        deduct_cost(ch,amount,mtype);
	BALANCE_MONEY(ch);

        account->balance +=
	    amount * MONEY_VALUE_SILVER(mtype);

        sprintf(buf,"You deposit %ld %s into your account.\n\r",
            amount, MONEY_VALUE_NAME(mtype));
        send_to_char(buf,ch);

        return;
    }

    if(!strcmp(arg,"withdraw"))
    {
        argument = one_argument(argument,arg2);
        argument = one_argument(argument,arg3);

        if(IS_SET(account->flags, BANK_ACCOUNT_NOWITHDRAW))
        {
            send_to_char("You are not allowed to withdraw money"
                         " from this account.\n\r",ch);
            return;
        }


        if(arg2[0] == '\0')
        {
            send_to_char("Account withdraw how much?\n\r",ch);
            return;
        }

        if(!is_number(arg2))
        {
            send_to_char("You can't withdraw that.\n\r",ch);
            return;
        }

        if( (amount = atol(arg2)) < 1 )
        {
            send_to_char("Try deposit instead.\n\r",ch);
            return;
        }

        if(amount > 30000)
        {
            send_to_char("You can't withdraw that much of one currency at once.\n\r",ch);
            return;
        }

        if(arg3[0] == '\0' || !str_prefix(arg3,"gold"))
            mtype = VALUE_GOLD;
        else if(!str_prefix(arg3,"platinum"))
            mtype = VALUE_PLATINUM;
        else if(!str_prefix(arg3,"silver"))
            mtype = VALUE_SILVER;
        else
        {
            send_to_char("What kind of currency is THAT?\n\r",ch);
            return;
        }

        if(amount * MONEY_VALUE_SILVER(mtype)
            > account->balance)
        {
            send_to_char("Your account doesn't have that much.\n\r",ch);
            return;
        }

        add_cost(ch,amount,mtype);
	BALANCE_MONEY(ch);

        account->balance -= MONEY_VALUE_SILVER(mtype) * amount;

        sprintf(buf,"You withdraw %ld %s from your account.\n\r",
            amount, MONEY_VALUE_NAME(mtype));
        send_to_char(buf,ch);

        return;
    }

    if(!strcmp(arg,"report"))
    {
        long total, numplat;
        int numgold, numsilver;

        total = account->balance;

        numplat = (total / PLATINUM_PER_SILVER);
        numgold = (total % PLATINUM_PER_SILVER ) / GOLD_PER_SILVER;
        numsilver = ((total % PLATINUM_PER_SILVER ) % GOLD_PER_SILVER);

        sprintf(buf,"Bank Account Statement for %s\n\r"
                    "Balance: %ld plat, %d gold, %d silver\n\r",
                    ch->name, numplat, numgold, numsilver);
        send_to_char(buf,ch);
        return;
    }

    if(!strcmp(arg,"transfer"))
    {
        char argrec[MAX_STRING_LENGTH];
        CHAR_DATA *vch = NULL, *wch;
        DESCRIPTOR_DATA *d = NULL;
        bool loaded = FALSE;

        argument = one_argument(argument,arg2);
        argument = one_argument(argument,arg3);

        if(IS_SET(account->flags, BANK_ACCOUNT_NODEPOSIT)
        || IS_SET(account->flags, BANK_ACCOUNT_NOTRANSFER))
        {
            send_to_char("You are not allowed to withdraw or transfer "
                         "money from this account.\n\r",ch);
            return;
        }

        if(arg2[0] == '\0' || arg3[0] == '\0')
        {
             send_to_char("Syntax: account transfer <gold> [plat/silver] <recipient>\n\r",ch);
             return;
        }

        if(!is_number(arg2))
        {
            send_to_char("You can't transfer that.\n\r",ch);
            return;
        }

        if( (amount = atol(arg2)) < 1 )
        {
            send_to_char("That only works the other way.\n\r",ch);
            return;
        }

        if(!str_prefix(arg3,"gold"))
        {
            mtype = VALUE_GOLD;
            argument = one_argument(argument,argrec);
        }
        else if(!str_prefix(arg3,"platinum"))
        {
            mtype = VALUE_PLATINUM;
            argument = one_argument(argument,argrec);
        }
        else if(!str_prefix(arg3,"silver"))
        {
            mtype = VALUE_SILVER;
            argument = one_argument(argument,argrec);
        }
        else
        {
            strcpy(argrec,arg3);
        }

        if(amount * MONEY_VALUE_SILVER(mtype)
            > account->balance)
        {
             send_to_char("You don't have that much money!",ch);
             return;
        }

        for ( wch = char_list; wch != NULL ; wch = wch->next )
        {
            if (!IS_NPC(wch) && !str_cmp(argrec, wch->name))
            {
                vch = wch;
                break;
            }
        }

        if(vch == ch)
        {
            send_to_char("That would be pointless.\n\r",ch);
            return;
        }

        if((!vch || vch->in_arena) && check_parse_name(argrec))
        {
            d = new_descriptor();
            if(load_char_obj(d, argrec))
            {
                vch = d->character;
                vch->desc = NULL;
                free_descriptor(d);
                vch->pseudo_login = TRUE;
                loaded = TRUE;
            }
            else vch = NULL;
        }
        

        if(!vch)
        {
            send_to_char("Could not find that person.\n\r",ch);
            return;
        }

        if(vch->pcdata->bank_account)
        {

            if(IS_SET(vch->pcdata->bank_account->flags, BANK_ACCOUNT_NODEPOSIT)
            || IS_SET(vch->pcdata->bank_account->flags, BANK_ACCOUNT_FROZEN))
            {
                send_to_char("You are not allowed to transfer money"
                             " to that person's account.\n\r",ch);
                return;
            }

            if(amount * MONEY_VALUE_SILVER(mtype) > 200000)
            {
                sprintf(buf, "%s transfers %ld to %s", ch->name,
                    amount * MONEY_VALUE_SILVER(mtype), vch->name);
                log_f("[itrack] %s", buf);

                wiznet(buf, ch, NULL, WIZ_TRANSFERS,0,0);
            }

            account->balance -=
				amount * MONEY_VALUE_SILVER(mtype);

            vch->pcdata->bank_account->balance +=
				amount * MONEY_VALUE_SILVER(mtype);

            sprintf(buf,"You transfer %ld %s to %s's account.\n\r",
		amount,	MONEY_VALUE_NAME(mtype), vch->name);

            send_to_char(buf,ch);

            save_char_obj(vch);
            save_char_obj(ch);

        }
        else
        {
            send_to_char("That character does not have a bank account.\n\r",ch);
        }

        if(loaded)
            free_char(vch);

        return;
    }

    return;
}
