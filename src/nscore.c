
void do_nscore( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char hourbuf[MAX_STRING_LENGTH];
   char titlebuf[MAX_STRING_LENGTH]; 
   int x, mmtest;

    if ( argument[0] != '\0' )
      mmtest = abs( atoi(argument) );
    else
      mmtest = 10;

    sprintf( buf, "\n\r
      /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/~~\\\n\r");
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
      sprintf(hourbuf, "%3d years old (9999+ hours)", get_age(ch));

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
