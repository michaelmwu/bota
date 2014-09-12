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
#include "quest.h"
#include "recycle.h"

DECLARE_DO_FUN( do_say );
DECLARE_DO_FUN( do_tell );

ROOM_INDEX_DATA *find_location args( ( CHAR_DATA *ch, char * arg ) );

#define MAX_QUEST_TOKEN		7

const int questlist [MAX_QUEST_TOKEN] =
{
   1204,
   1205,
   1206,
   1207,
   1233,
   1234,
   1235
};

/* Local functions */
void generate_quest	args(( CHAR_DATA *ch, CHAR_DATA *questmaster ));
void quest_update	args(( void ));
bool quest_level_diff   args(( int clevel, int mlevel));

QUEST_DATA *generate_escort_quest(CHAR_DATA *ch);

/* The main quest function */
void do_quest(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *questmaster;
    OBJ_DATA *obj=NULL, *obj_next;
    OBJ_INDEX_DATA *questinfoobj;
    MOB_INDEX_DATA *questinfo;
    char buf [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    int i;

/* Add your rewards here.  Works as follows:
"Obj name shown when quest list is typed", "keywords for buying",
"Amount of quest points",  Does it load an object?,  IF it loads an
object, then the vnum, otherwise a value to set in the next thing,  This
is last field is a part of the char_data to be modified */

const struct reward_type reward_table[]=
{
  { "Healing Mist", "healing mist", 1650, TRUE, 1213, 0 },
  { "\"Smasher\" Axe", "smasher axe", 1550, TRUE, 1216, 0 },
  { "\"Devil's Tongue\" Whip", "devil's tongue whip", 1700, TRUE, 1217, 0 },
  { "\"Rabbitbite\" Dagger", "rabbitbite dagger rabbit", 1550, TRUE, 1211, 0 },
  { "\"Deathstrike\" Long Sword", "deathstrike longsword long sword", 1550, TRUE, 1208, 0 },
  { "Eternal Rose Light", "eternal rose light", 1400, TRUE, 1212, 0 },
  { "Blasted Metal Ring", "blasted metal ring", 1400, TRUE, 1215, 0 },
  { "A Cubic Zirconium", "cubic zirconium", 2000, TRUE, 3386, 0 },
  { NULL, NULL, 0, FALSE, 0, 0  } /* Never remove this!!! */
};

/*
  { "Impenetrable Breast Plate", "breastplate breast plate", 1500, TRUE, 1210, 0 },
  { "Honor Shield", "honorshield honor shield", 1400, TRUE, 1209, 0 },
  { "Kyrrian Bracer", "kyrrian bracer", 1350, TRUE, 1214, 0 },
  { "Plate of Thoth", "plate Thoth", 5500, TRUE, 17160, 0 },
  { "Plate of Belan", "plate Belan", 5500, TRUE, 17161, 0 },
  { "Justice Bringer", "justice bringer", 6000, TRUE, 17162, 0 },
  { "Deceit: the Evil Blade", "deceit evil blade", 6000, TRUE, 17163, 0 },
  { "Pendant of Winter's Wrath", "pendant winter", 9800, TRUE, 17165, 0 },
  { "Necklace of Demonic Wrath", "necklace demonic wrath", 10000, TRUE, 17164, 0 },
  { "Slivers of Thor's Hammer", "sliver thor hammer", 10000, TRUE, 17166, 0 },
*/

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if(IS_NPC(ch))
	{ send_to_char("NPC's can't quest.\n\r",ch); return; }

    if (arg1[0] == '\0')
    {
        send_to_char("Quest commands: Info, Time, Request, Complete, Quit, List, Points  and Buy.\n\r",ch);
        send_to_char("For more information, type 'Help Quest'.\n\r",ch);
        send_to_char("(See Also: iquest)\n\r",ch);
	return;
    }

    if (!str_prefix(arg1, "now") && get_trust(ch) == MAX_LEVEL)
    {
        ch->pcdata->nextquest = 0;
        send_to_char("Go ahead and quest away!\n\r",ch);
        return;
    }


    if (!str_prefix(arg1, "info"))
    {
	if (IS_SET(ch->plt,PLT_QUESTOR))
	{
	    if (ch->pcdata->questmob < 0 && ch->pcdata->questgiver->short_descr) {
	        sprintf(buf,"Your quest is ALMOST complete!\n\rGet back to %s before your time runs out!\n\r",ch->pcdata->questgiver->short_descr);
		send_to_char(buf, ch);
	    }
	    else if (ch->pcdata->questobj > 0)
	    {
                questinfoobj = get_obj_index(ch->pcdata->questobj);
		if (questinfoobj != NULL) {
		    sprintf(buf,"You are on a quest to recover the fabled %s!\n\r",questinfoobj->name);
		    send_to_char(buf, ch);
		}
		else send_to_char("You aren't currently on a quest.\n\r",ch);
		return;
	    }
	    else if (ch->pcdata->questmob > 0)
	    {
                questinfo = get_mob_index(ch->pcdata->questmob);
		if (questinfo != NULL) {
		    sprintf(buf,"You are on a quest to slay the dreaded %s!\n\r",questinfo->short_descr);
		    send_to_char(buf, ch);
		}
		else send_to_char("You aren't currently on a quest.\n\r",ch);
		return;
	    }
	}
	else
	    send_to_char("You aren't currently on a quest.\n\r",ch);
	return;
    }
    else if (!str_prefix(arg1, "points"))
    {
        if ( ch->pcdata->questpoints <= 0 )
        {
            send_to_char("You don't have any questpoints yet.\n\r",ch);
            return;
        }

        sprintf(buf,"You have {G%d{x questpoints!\n\r", ch->pcdata->questpoints);
        send_to_char(buf,ch);
        return;

    }
    else if (!str_prefix(arg1, "time"))
    {
	if (!IS_SET(ch->plt,PLT_QUESTOR))
	{
	    send_to_char("You aren't currently on a quest.\n\r",ch);
	    if (ch->pcdata->nextquest > 1) {
	        sprintf(buf,"There are %d hours remaining until you can go on another quest.\n\r",ch->pcdata->nextquest);
		send_to_char(buf, ch);
	    }
	    else if (ch->pcdata->nextquest == 1) {
		sprintf(buf, "There is less than an hour remaining until you can go on another quest.\n\r");
		send_to_char(buf, ch);
	    }
	}
        else if (ch->pcdata->countdown > 0)
        {
	    sprintf(buf, "Time left for current quest: %d hours\n\r",ch->pcdata->countdown);
	    send_to_char(buf, ch);
	}
	return;
    }

    for ( questmaster = ch->in_room->people; questmaster != NULL; questmaster = questmaster->next_in_room )
    {
	if (!IS_NPC(questmaster)) continue;
        if (questmaster->spec_fun == spec_lookup( "spec_questmaster" )) break;
    }

    if (questmaster == NULL || questmaster->spec_fun != spec_lookup("spec_questmaster" ))
    {
        send_to_char("You can't do that here.\n\r",ch);
        return;
    }

    if ( questmaster->fighting != NULL)
    {
	send_to_char("Wait until the fighting stops.\n\r",ch);
        return;
    }


    if(!str_prefix(arg1, "nrequest") && get_trust(ch) == MAX_LEVEL)
    {
        QUEST_DATA *quest = generate_escort_quest(ch);
        if(!quest) return;

        quest->questgiver = questmaster;
        ch->pcdata->questdata = quest;

        sprintf(buf,"%s %s would like to go to %s.\n\r",
            ch->name,
            capitalize(quest->target.ch->short_descr),
            quest->target2.room->name);

        do_tell(ch, buf);

        return;
    }


    if(!str_prefix(arg1, "ncomplete") && get_trust(ch) == MAX_LEVEL)
    {
        act("$n informs $N $e has completed $S quest.", ch, NULL, questmaster, TO_ROOM);
	act("You inform $N you have completed $S quest.",ch, NULL, questmaster, TO_CHAR);

        if(!ch->pcdata->questdata)
        {
            do_say(questmaster, "What quest? Surely not a NewStyle Quest(tm)!");
            return;
        }

        if(ch->pcdata->questdata->status != QUEST_MUST_RETURN)
        {
            do_say(questmaster,"You're not done with your quest!");
            return;
        }

        do_say(questmaster, "Cool, but the NewStyle Quest(tm) rewards "
                         "code hasn't been written!");
        do_say(questmaster,"Good job, though! Give yourself a pat on the back.");



        free_quest(ch->pcdata->questdata);
    }



    if (!str_prefix(arg1, "list"))
    {
        act("$n asks $N for a list of quest items.", ch, NULL, questmaster, TO_ROOM);
	act ("You ask $N for a list of quest items.",ch, NULL, questmaster, TO_CHAR);
	send_to_char("Current Quest Items available for Purchase:\n\r", ch);
	if(reward_table[0].name == NULL)
	  send_to_char("  Nothing.\n\r",ch);
	else {
	send_to_char("  {B[{WCost{B]     [{WName{B]{x\n\r",ch);
	for(i=0;reward_table[i].name != NULL;i++) {
	      sprintf(buf,"   %-4d       %s\n\r"
			,reward_table[i].cost,reward_table[i].name);
	      send_to_char(buf, ch);
	}
	     }
	send_to_char("\n\rTo buy an item, type 'Quest buy <item>'.\n\r",ch);
	return;
    }

    else if (!str_prefix(arg1, "buy"))
    {
	bool found=FALSE;
	if (arg2[0] == '\0')
	{
	    send_to_char("To buy an item, type 'Quest buy <item>'.\n\r",ch);
	    return;
	}
	/* Use keywords rather than the name listed in qwest list */
	/* Do this to avoid problems with something like 'qwest buy the' */
	/* And people getting things they don't want... */
      for(i=0;reward_table[i].name != NULL;i++)
	if (is_name(arg2, reward_table[i].keyword))
	{ found = TRUE;
	    if (ch->pcdata->questpoints >= reward_table[i].cost)
	    {
		ch->pcdata->questpoints -= reward_table[i].cost;
		if(reward_table[i].object)
	            obj = create_object(get_obj_index(reward_table[i].value),ch->level);
		else
		   {
		   sprintf(buf,"In exchange for %d quest points, %s gives you %s.\n\r",
			reward_table[i].cost, questmaster->short_descr, reward_table[i].name );
		   send_to_char(buf, ch);
		   *(int *)reward_table[i].where += reward_table[i].value;
		   }
		break;
	    }
	    else
	    {
		sprintf(buf, "Sorry, %s, but you don't have enough quest points for that.",ch->name);
		do_say(questmaster,buf);
		return;
	    }
	}
	if(!found)
	{
	    sprintf(buf, "I don't have that item, %s.",ch->name);
	    do_say(questmaster, buf);
	}
	if (obj != NULL)
	{
	    sprintf(buf,"In exchange for %d quest points, %s gives you %s.\n\r",
		    reward_table[i].cost, questmaster->short_descr, obj->short_descr);
	    send_to_char(buf, ch);
	    obj_to_char(obj, ch);

            wiznet("$N buys $p from a questmaster.",
                ch,obj,WIZ_TRANSFERS,0,0);
	}
	return;
    }
    else if (!str_prefix(arg1, "request"))
    {
        act("$n asks $N for a quest.", ch, NULL, questmaster, TO_ROOM);
	act("You ask $N for a quest.",ch, NULL, questmaster, TO_CHAR);
	if (IS_SET(ch->plt,PLT_QUESTOR))
	{
	    sprintf(buf, "But you're already on a quest!");
	    do_say(questmaster, buf);
	    return;
	}
	if (ch->pcdata->nextquest > 0)
	{
	    sprintf(buf, "You're very brave, %s, but let someone else have a chance.",ch->name);
	    do_say(questmaster, buf);
	    sprintf(buf, "Come back later.");
	    do_say(questmaster, buf);
	    return;
	}

	sprintf(buf, "Thank you, brave %s!",ch->name);
/*	if (!IS_SET(ch->plt,PLT_QUESTOR))*/
	do_say(questmaster, buf);
        ch->pcdata->questmob = 0;
	ch->pcdata->questobj = 0;
	generate_quest(ch, questmaster);
        ch->pcdata->questgiver = questmaster->pIndexData;

        if (ch->pcdata->questmob > 0 || ch->pcdata->questobj > 0)
	{
            ch->pcdata->countdown = number_range(20,30);
	    SET_BIT(ch->plt,PLT_QUESTOR);
            act("$N discusses something with $n.", ch, NULL, questmaster, TO_ROOM);
	    sprintf(buf, "You have %d hours to complete this quest.",ch->pcdata->countdown);
	    do_say(questmaster, buf);
	    sprintf(buf, "May the gods go with you!");
	    do_say(questmaster, buf);
	}
	return;
    }

    else if (!str_prefix(arg1, "complete") || !str_prefix(arg1, "done"))
    {
        act("$n informs $N $e has completed $S quest.", ch, NULL, questmaster, TO_ROOM);
	act("You inform $N you have completed $S quest.",ch, NULL, questmaster, TO_CHAR);

	if (ch->pcdata->questgiver != questmaster->pIndexData)
	{
	    sprintf(buf, "I never sent you on a quest! Perhaps you're thinking of someone else.");
	    do_say(questmaster,buf);
	    return;
	}

	if (IS_SET(ch->plt,PLT_QUESTOR))
	{
	bool obj_found = FALSE;
	    if (ch->pcdata->questobj > 0 && ch->pcdata->countdown > 0)
	    {
    		for (obj = ch->carrying; obj != NULL; obj= obj_next)
    		{
        	    obj_next = obj->next_content;

		    if (obj != NULL && obj->pIndexData->vnum == ch->pcdata->questobj)
		    {
			obj_found = TRUE;
            	        break;
		    }
        	}
	    }

     if ((ch->pcdata->questmob < 0 || (ch->pcdata->questobj && obj_found))
	&& ch->pcdata->countdown > 0)
	{
                int reward, pointreward, pracreward = 0;
	    	reward = number_range(95,105);
	    	pointreward = number_range(25,45);

		if(number_range(0,10) == 5)
		{
		    pracreward = number_range(1,5);
		}

/*                if(ch->pcdata->questpoints > 3000)
                {
                    pointreward -= interpolate(3001, 6000, 5, 20,
                        number_fuzzy(ch->pcdata->questpoints) );
                    if(pointreward <= 20)
                        pointreward = 24 + number_range(1,8);
                } */


		sprintf(buf, "Congratulations on completing your quest!");
		do_say(questmaster,buf);

                if(ch->pcdata->questmob < 0)
                {
                    MOB_INDEX_DATA *victim = get_mob_index(abs(ch->pcdata->questmob));
                    if(victim)
		    {
	                    sprintf(buf, "You have slain %s and restored peace for the time being.",victim->short_descr);
				if(victim->level > ch->level)
					pointreward += (victim->level - ch->level) / 2;
		    }
                }
                else
		    sprintf(buf, "You have recovered the prized %s.",obj->short_descr);

		do_say(questmaster,buf);

if(pracreward <= 0)
		sprintf(buf,"As a reward, I am giving you %d quest points, and %d gold.",pointreward,reward);
else
		sprintf(buf,"As a reward, I am giving you %d quest points, %d practices, and %d gold.",pointreward,pracreward,reward);

		do_say(questmaster,buf);

	        REMOVE_BIT(ch->plt,PLT_QUESTOR);
	        ch->pcdata->questgiver = NULL;
	        ch->pcdata->countdown = 0;
	        ch->pcdata->questmob = 0;
		ch->pcdata->questobj = 0;
	        ch->pcdata->nextquest = 5;

		add_cost(ch,reward,VALUE_GOLD);

		ch->practice += pracreward;

		ch->pcdata->questpoints += pointreward;
		if(obj_found) extract_obj(obj);
	 return;
	}
     else if((ch->pcdata->questmob > 0 || ch->pcdata->questobj > 0)
          && ch->pcdata->countdown > 0)
	    {
		sprintf(buf, "You haven't completed the quest yet, but there is still time!");
		do_say(questmaster, buf);
		return;
	    }
     }
	if (ch->pcdata->nextquest > 0)
	{
	    sprintf(buf,"But you didn't complete your quest in time!");
	}
	else sprintf(buf, "You have to request a quest first, %s.",ch->name);
	do_say(questmaster, buf);
	return;
    }

    else if (!str_prefix(arg1, "convert"))
    {
        int convertamount = 0;

        if (arg2[0] == '\0')
        {
	    convertamount = 1;
        }
	else
	{
            if( !is_number(arg2) )
            {
                send_to_char("How many?\n\r",ch);
                return;
            }
	    convertamount = atoi(arg2);
	}

        if( convertamount < 1 || convertamount > 1000 )
        {
            send_to_char("You can't convert that.\n\r",ch);
            return;
        }

        if( convertamount > ch->qps )
        {
            send_to_char("You don't have that many iquest points.\n\r",ch);
            return;
        }

        sprintf(buf, "You convert {R%d{x iquest points into {R%d{x aquest points\n\r",
	             convertamount, convertamount * 50);

	send_to_char(buf,ch);

        ch->pcdata->questpoints += convertamount * 50;
        ch->qps -= convertamount;
        return;
    }

    else if (!str_prefix(arg1, "quit"))
    {
        act("$n informs $N $e wishes to quit $s quest.", ch, NULL,questmaster, TO_ROOM);
	act ("You inform $N you wish to quit $s quest.",ch, NULL, questmaster, TO_CHAR);
	if (ch->pcdata->questgiver != questmaster->pIndexData)
	{
	    sprintf(buf, "I never sent you on a quest! Perhaps you're thinking of someone else.");
	    do_say(questmaster,buf);
	    return;
	}

	if (IS_SET(ch->plt,PLT_QUESTOR))
	{
            REMOVE_BIT(ch->plt,PLT_QUESTOR);
            ch->pcdata->questgiver = NULL;
            ch->pcdata->countdown = 0;
            ch->pcdata->questmob = 0;
	    ch->pcdata->questobj = 0;
            ch->pcdata->nextquest = 10;

	    sprintf(buf, "Your quest is over, but for your cowardly behavior, you may not quest again for 10 hours.");
	    do_say(questmaster,buf);
		    return;
        }
	else
	{
	send_to_char("You aren't on a quest!",ch);
	return;
	}
    }


    send_to_char("Quest commands: Info, Time, Request, Complete, Quit, List, and Buy.\n\r",ch);
    send_to_char("For more information, type 'Help Quest'.\n\r",ch);
    return;
}

void generate_quest(CHAR_DATA *ch, CHAR_DATA *questmaster)
{
    CHAR_DATA *victim;
    MOB_INDEX_DATA *vsearch;
    ROOM_INDEX_DATA *room;
    OBJ_DATA *questitem;
    char buf [MAX_STRING_LENGTH];
    long mcounter;
    int mob_vnum;

    for (mcounter = 0; mcounter < 99999; mcounter ++)
    {
	mob_vnum = number_range(50, 32000);

	if ( (vsearch = get_mob_index(mob_vnum) ) != NULL )
	{
            if (quest_level_diff(ch->level, vsearch->level) == TRUE
                && !vsearch->pShop
    		&& !IS_SET(vsearch->area->area_flags, AREA_NOPLAYERS)
		&& !IS_SET(vsearch->act, ACT_NO_QUEST)
    		&& !IS_SET(vsearch->act, ACT_TRAIN)
    		&& !IS_SET(vsearch->imm_flags, IMM_WEAPON)
    		&& !IS_SET(vsearch->act, ACT_PRACTICE)
    		&& !IS_SET(vsearch->act, ACT_IS_HEALER)
		&& !IS_SET(vsearch->act, ACT_PET)
		&& !IS_SET(vsearch->affected_by, AFF_CHARM)
		&& number_percent() < 40)
            break;
		else vsearch = NULL;
	}
    }

    if ( vsearch == NULL || ( victim = get_char_vnum( vsearch->vnum ) ) == NULL ||
        !can_see_room(ch,victim->in_room ) )
    {
	sprintf(buf, "I'm sorry, but I don't have any quests for you at this time.");
	do_say(questmaster, buf);
	sprintf(buf, "Try again later.");
	do_say(questmaster, buf);
	ch->pcdata->nextquest = 2;
        return;
    }

	room = victim->in_room;

    /*  chance it will send the player on a 'recover item' quest. */

    if (number_percent() < 65)
    {

        questitem = create_object ( get_obj_index
		( questlist[number_range(0,MAX_QUEST_TOKEN -1)] ),
		ch->level);

        questitem->timer = 45;

	obj_to_room(questitem, room);
	ch->pcdata->questobj = questitem->pIndexData->vnum;

        switch(number_range(1,3))
        {
            case 1:
              sprintf(buf,
                "Vile pilferers have stolen {+%s{= from the royal treasury!",
                 questitem->short_descr);
              break;

            case 2:
              sprintf(buf,
                "It seems that {+%s{= was lost in transit not long ago!",
                 questitem->short_descr);
              break;

            case 3:
              sprintf(buf,
                "A stupid, lazy questor failed to recover {+%s{=!",
                 questitem->short_descr);
              break;


            default:
              sprintf(buf,
                "Vile pilferers have stolen {+%s{= from the royal treasury!",
                 questitem->short_descr);
              break;
        }

	act ("$N tells you '{U$t{x'",ch, buf, questmaster, TO_CHAR);

        sprintf(buf,"My court wizardess, with her magic mirror, has pinpointed its location.");
	act ("$N tells you '{U$t{x'",ch, buf, questmaster, TO_CHAR);

	sprintf(buf, "Look in the general area of %s for %s!",room->area->name, room->name);
	act ("$N tells you '{U$t{x'",ch, buf, questmaster, TO_CHAR);

	return;
    }

    /* Quest to kill a mob */

    else
    {

    /*
    switch(number_range(0,1))
    {
	case 0:
        sprintf(buf, "%s An enemy of mine, {=%s{+, is making vile threats against the crown.",ch->name, victim->short_descr);
        do_tell(questmaster, buf);
        sprintf(buf, "%s This threat must be eliminated!",ch->name);
        do_tell(questmaster, buf);
	break;

	case 1:
	sprintf(buf, "Rune's most heinous criminal, %s, has escaped from the dungeon!",victim->short_descr);
	do_say(questmaster, buf);
	sprintf(buf, "Since the escape, %s has murdered %d civillians!",victim->short_descr, number_range(2,20));
	do_say(questmaster, buf);
	do_say(questmaster,"The penalty for this crime is death, and you are to deliver the sentence!");

	break;
    }*/

    sprintf(buf,"An enemy of old, %s, has reappeared to cause harm.",victim->short_descr);
    act ("$N tells you '{U$t{x'",ch, buf, questmaster, TO_CHAR);
    sprintf(buf,"Please, you must destroy %s quickly.",
	(victim->sex == 0) ? "it" : ((victim->sex == 1) ? "him":"her"));
    act ("$N tells you '{U$t{x'",ch, buf, questmaster, TO_CHAR);


    if (room->name != NULL)
    {
        sprintf(buf, "Seek %s out somewhere in the vicinity of %s!",victim->short_descr,room->name);
        act ("$N tells you '{U$t{x'",ch, buf, questmaster, TO_CHAR);


	sprintf(buf, "That location is in the general area of %s.",room->area->name);
        act ("$N tells you '{U$t{x'",ch, buf, questmaster, TO_CHAR);

    }
    ch->pcdata->questmob = victim->pIndexData->vnum;
    }

    return;
}

bool quest_level_diff(int clevel, int mlevel)
{
    if (clevel < 9 && mlevel < clevel + 5) return TRUE;
    else if (clevel <= 9 && mlevel < clevel + 8
	  && mlevel > clevel - 10) return TRUE;
    else if (clevel <= 14 && mlevel < clevel + 8
	  && mlevel > clevel - 10) return TRUE;
    else if (clevel <= 21 && mlevel < clevel + 8
	  && mlevel > clevel - 10) return TRUE;
    else if (clevel <= 29 && mlevel < clevel + 8
	  && mlevel > clevel - 10) return TRUE;
    else if (clevel <= 37 && mlevel < clevel + 9
	  && mlevel > clevel - 10) return TRUE;
    else if (clevel <= 55 && mlevel < clevel + 11
	  && mlevel > clevel - 10) return TRUE;
    else if (clevel <= 70 && mlevel < clevel + 13
	  && mlevel > clevel - 10  ) return TRUE;
    else if (clevel <= 81 && mlevel < clevel + 15
	  && mlevel > clevel - 10 ) return TRUE;
    else if (clevel <= 95 && mlevel < clevel + 18
	  && mlevel > clevel - 10   ) return TRUE;
    else if (clevel <= 101 && mlevel < clevel + 40
	  && mlevel > clevel - 10   ) return TRUE;

    else if(clevel >= 102) return TRUE; /* Imms can get anything :) */
    else return FALSE;
}

/* Called from update_handler() by pulse_area */

void quest_update(void)
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *ch;
    char buf[MAX_STRING_LENGTH];

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if (d->character != NULL && d->connected == CON_PLAYING)
        {
        ch = d->character;
	if(IS_NPC(ch))
	continue;
        if (ch->pcdata->nextquest > 0)
        {
            ch->pcdata->nextquest--;
            if (ch->pcdata->nextquest == 0)
            {
                send_to_char("You may now quest again.\n\r",ch);
                return;
            }
        }
        else if (IS_SET(ch->plt,PLT_QUESTOR))
        {
            if (--ch->pcdata->countdown <= 0)
            {
                ch->pcdata->nextquest = 10;
                sprintf(buf,"You have run out of time for your quest!\n\rYou may quest again in %d hours.\n\r",ch->pcdata->nextquest);
		send_to_char(buf, ch);
                REMOVE_BIT(ch->plt,PLT_QUESTOR);
                ch->pcdata->questgiver = NULL;
                ch->pcdata->countdown = 0;
                ch->pcdata->questmob = 0;
                ch->pcdata->questobj = 0;
            }
            if (ch->pcdata->countdown > 0 && ch->pcdata->countdown < 6)
            {
                send_to_char("Better hurry, you're almost out of time for your quest!\n\r",ch);
                return;
            }
        }
        }
    }
    return;
}
