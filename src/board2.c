
void handle_con_note_to (DESCRIPTOR_DATA *d, char * argument)
{
	char buf [MAX_INPUT_LENGTH];
	CHAR_DATA *ch = d->character;

	if (!ch->pcdata->in_progress)
	{
		d->connected = CON_PLAYING;
		bug ("nanny: In CON_NOTE_TO, but no note in progress",0);
		return;
	}

	strcpy (buf, argument);
	smash_tilde (buf); /* change ~ to - as we save this field as a string later */

	switch (ch->pcdata->board->force_type)
	{
		case DEF_NORMAL: /* default field */
			if (!buf[0]) /* empty string? */
			{
				ch->pcdata->in_progress->to_list = str_dup (ch->pcdata->board->names);
				sprintf (buf, "Assumed default recipient: " BOLD "%s" NO_COLOR "\n\r", ch->pcdata->board->names);
				write_to_buffer (d, buf, 0);
			}
			else
				ch->pcdata->in_progress->to_list = str_dup (buf);
				
			break;
		
		case DEF_INCLUDE: /* forced default */
			if (!is_full_name (ch->pcdata->board->names, buf))
			{
				strcat (buf, " ");
				strcat (buf, ch->pcdata->board->names);
				ch->pcdata->in_progress->to_list = str_dup(buf);

				sprintf (buf, "\n\rYou did not specify %s as recipient, so it was automatically added.\n\r"
				         BOLD YELLOW "New To" NO_COLOR " :  %s\n\r",
						 ch->pcdata->board->names, ch->pcdata->in_progress->to_list);
				write_to_buffer (d, buf, 0);
			}
			else
				ch->pcdata->in_progress->to_list = str_dup (buf);
			break;
		
		case DEF_EXCLUDE: /* forced exclude */
			if (!buf[0])
			{
				write_to_buffer (d, "You must specify a recipient.\n\r"
									BOLD YELLOW "To" NO_COLOR ":      ", 0);
				return;
			}
			
			if (is_full_name (ch->pcdata->board->names, buf))
			{
				sprintf (buf, "You are not allowed to send notes to %s on this board. Try again.\n\r"
				         BOLD YELLOW "To" NO_COLOR ":      ", ch->pcdata->board->names);
				write_to_buffer (d, buf, 0);
				return; /* return from nanny, not changing to the next state! */
			}
			else
				ch->pcdata->in_progress->to_list = str_dup (buf);
			break;
		
	}		

	write_to_buffer (d, BOLD YELLOW "\n\rSubject" NO_COLOR ": ", 0);
	d->connected = CON_NOTE_SUBJECT;
}

void handle_con_note_subject (DESCRIPTOR_DATA *d, char * argument)
{
	char buf [MAX_INPUT_LENGTH];
	CHAR_DATA *ch = d->character;

	if (!ch->pcdata->in_progress)
	{
		d->connected = CON_PLAYING;
		bug ("nanny: In CON_NOTE_SUBJECT, but no note in progress",0);
		return;
	}

	strcpy (buf, argument);
	smash_tilde (buf); /* change ~ to - as we save this field as a string later */
	
	/* Do not allow empty subjects */
	
	if (!buf[0])		
	{
		write_to_buffer (d, "Please find a meaningful subject!\n\r",0);
		write_to_buffer (d, BOLD YELLOW "Subject" NO_COLOR ": ", 0);
	}
	else  if (strlen(buf)>60)
	{
		write_to_buffer (d, "No, no. This is just the Subject. You're note writing the note yet. Twit.\n\r",0);
	}
	else
	/* advance to next stage */
	{
		ch->pcdata->in_progress->subject = str_dup(buf);
		if (IS_IMMORTAL(ch)) /* immortals get to choose number of expire days */
		{
			sprintf (buf,"\n\rHow many days do you want this note to expire in?\n\r"
			             "Press Enter for default value for this board, " BOLD "%d" NO_COLOR " days.\n\r"
           				 BOLD YELLOW "Expire" NO_COLOR ":  ",
		                 ch->pcdata->board->purge_days);
			write_to_buffer (d, buf, 0);				               
			d->connected = CON_NOTE_EXPIRE;
		}
		else
		{
			ch->pcdata->in_progress->expire = 
				current_time + ch->pcdata->board->purge_days * 24L * 3600L;				
			sprintf (buf, "This note will expire %s\r",ctime(&ch->pcdata->in_progress->expire));
			write_to_buffer (d,buf,0);
			write_to_buffer (d, "\n\rEnter text. Type " BOLD "~" NO_COLOR " or " BOLD "END" NO_COLOR " on an empty line to end note.\n\r"
			                    "=======================================================\n\r",0);
			d->connected = CON_NOTE_TEXT;
		}
	}
}

void handle_con_note_expire(DESCRIPTOR_DATA *d, char * argument)
{
	CHAR_DATA *ch = d->character;
	char buf[MAX_STRING_LENGTH];
	time_t expire;
	int days;

	if (!ch->pcdata->in_progress)
	{
		d->connected = CON_PLAYING;
		bug ("nanny: In CON_NOTE_EXPIRE, but no note in progress",0);
		return;
	}
	
	/* Numeric argument. no tilde smashing */
	strcpy (buf, argument);
	if (!buf[0]) /* assume default expire */
		days = 	ch->pcdata->board->purge_days;
	else /* use this expire */
		if (!is_number(buf))
		{
			write_to_buffer (d,"Write the number of days!\n\r",0);
			write_to_buffer (d,BOLD YELLOW "Expire" NO_COLOR ":  ",0);
			return;
		}
		else
		{
			days = atoi (buf);
			if (days <= 0)
			{
				write_to_buffer (d, "This is a positive MUD. Use positive numbers only! :)\n\r",0);
				write_to_buffer (d, BOLD YELLOW "Expire" NO_COLOR ":  ",0);
				return;
			}
		}
			
	expire = current_time + (days*24L*3600L); /* 24 hours, 3600 seconds */

	ch->pcdata->in_progress->expire = expire;
	
	/* note that ctime returns XXX\n so we only need to add an \r */

	write_to_buffer (d, "\n\rEnter text. Type " BOLD "~" NO_COLOR " or " BOLD "END" NO_COLOR " on an empty line to end note.\n\r"
	                    "=======================================================\n\r",0);

	d->connected = CON_NOTE_TEXT;
}



void handle_con_note_text (DESCRIPTOR_DATA *d, char * argument)
{
	CHAR_DATA *ch = d->character;
	char buf[MAX_STRING_LENGTH];
	char letter[4*MAX_STRING_LENGTH];
	
	if (!ch->pcdata->in_progress)
	{
		d->connected = CON_PLAYING;
		bug ("nanny: In CON_NOTE_TEXT, but no note in progress",0);
		return;
	}

	/* First, check for EndOfNote marker */

	strcpy (buf, argument);
	if ((!str_cmp(buf, "~")) || (!str_cmp(buf, "END")))
	{
		write_to_buffer (d, "\n\r\n\r",0);
		write_to_buffer (d, szFinishPrompt, 0);
		write_to_buffer (d, "\n\r", 0);
		d->connected = CON_NOTE_FINISH;
		return;
	}
	
	smash_tilde (buf); /* smash it now */

	/* Check for too long lines. Do not allow lines longer than 80 chars */
	
	if (strlen (buf) > MAX_LINE_LENGTH)
	{
		write_to_buffer (d, "Too long line rejected. Do NOT go over 80 characters!\n\r",0);
		return;
	}
	
	/* Not end of note. Copy current text into temp buffer, add new line, and copy back */

	/* How would the system react to strcpy( , NULL) ? */		
	if (ch->pcdata->in_progress->text)
	{
		strcpy (letter, ch->pcdata->in_progress->text);
		free_string (ch->pcdata->in_progress->text);
		ch->pcdata->in_progress->text = NULL; /* be sure we don't free it twice */
	}
	else
		strcpy (letter, "");
		
	/* Check for overflow */
	
	if ((strlen(letter) + strlen (buf)) > MAX_NOTE_TEXT)
	{ /* Note too long, take appropriate steps */
		write_to_buffer (d, "Note too long!\n\r", 0);
		free_note (ch->pcdata->in_progress);
		ch->pcdata->in_progress = NULL;			/* important */
		d->connected = CON_PLAYING;
		return;			
	}
	
	/* Add new line to the buffer */
	
	strcat (letter, buf);
	strcat (letter, "\r\n"); /* new line. \r first to make note files better readable */

	/* allocate dynamically */		
	ch->pcdata->in_progress->text = str_dup (letter);
}

void handle_con_note_finish (DESCRIPTOR_DATA *d, char * argument)
{

	CHAR_DATA *ch = d->character;
	
		if (!ch->pcdata->in_progress)
		{
			d->connected = CON_PLAYING;
			bug ("nanny: In CON_NOTE_FINISH, but no note in progress",0);
			return;
		}
		
		switch (tolower(argument[0]))
		{
			case 'c': /* keep writing */
				write_to_buffer (d,"Continuing note...\n\r",0);
				d->connected = CON_NOTE_TEXT;
				break;

			case 'v': /* view note so far */
				if (ch->pcdata->in_progress->text)
				{
					write_to_buffer (d,GREEN "Text of your note so far:" NO_COLOR "\n\r",0);
					write_to_buffer (d, ch->pcdata->in_progress->text, 0);
				}
				else
					write_to_buffer (d,"You haven't written a thing!\n\r\n\r",0);
				write_to_buffer (d, szFinishPrompt, 0);
				write_to_buffer (d, "\n\r",0);
				break;

			case 'p': /* post note */
				finish_note (ch->pcdata->board, ch->pcdata->in_progress);
				write_to_buffer (d, "Note posted.\n\r",0);
				d->connected = CON_PLAYING;
				/* remove AFK status */
				ch->pcdata->in_progress = NULL;
				act (BOLD GREEN "$n finishes $s note." NO_COLOR , ch, NULL, NULL, TO_ROOM);
				break;
				
			case 'f':
				write_to_buffer (d, "Note cancelled!\n\r",0);
				free_note (ch->pcdata->in_progress);
				ch->pcdata->in_progress = NULL;
				d->connected = CON_PLAYING;
				/* remove afk status */
				break;
			
			default: /* invalid response */
				write_to_buffer (d, "Huh? Valid answers are:\n\r\n\r",0);
				write_to_buffer (d, szFinishPrompt, 0);
				write_to_buffer (d, "\n\r",0);
				
		}
}

