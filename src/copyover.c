#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> /* execl */
#include "merc.h"
#include "recycle.h"

DECLARE_DO_FUN(do_asave	        );
DECLARE_DO_FUN(do_quit          );
DECLARE_DO_FUN(do_look          );
DECLARE_DO_FUN(do_return        );
DECLARE_DO_FUN(do_gedit        );

void terminate_python();

bool    write_to_descriptor     args( ( int desc, char *txt, int length ) );
bool    write_to_desc     (DESCRIPTOR_DATA *d, char *txt, int length ) ;

#define CH(descriptor)  ((descriptor)->original ? \
(descriptor)->original : (descriptor)->character)

/* This file holds the copyover data */
#define COPYOVER_FILE "copyover.data"

/* This is the executable file */
#define EXE_PATH	"../src/rot"
#define EXE_FILE 	"rot"

/*  Copyover - Original idea: Fusion of MUD
 *  Adapted to Diku by Erwin S. Andreasen, <erwin@pip.dknet.dk>
 *  http://pip.dknet.dk/~pip1773
 *  Changed into a ROM patch after seeing the 100th request for it :)
 */
void do_copyover (CHAR_DATA *ch, char * argument)
{
	extern int port;
	extern int control;

	int ret;

	int child_pid;

	FILE *fp;
	DESCRIPTOR_DATA *d, *d_next;
	char buf [100], buf2[100];
	extern int port,control; /* db.c */

//        fclose(fpReserve);	
	fp = fopen (COPYOVER_FILE, "w");
	
	if (!fp)
	{
		send_to_char ("Copyover file not writeable, aborted.\n\r",ch);
		log_string (LOG_ERR,"Could not write to copyover file: %s", COPYOVER_FILE);
		perror ("do_copyover:fopen");
		return;
	}
	
	/* Consider changing all saved areas here, if you use OLC */	
	do_asave (NULL, "changed"); /* autosave changed areas */
	do_asave (ch, "helps"); /* autosave changed helps */
	do_gedit (ch, "save"); /* autosave changed guilds */
	
	
	/* For each playing descriptor, save its state */
	for (d = descriptor_list; d ; d = d_next)
	{
		CHAR_DATA * och = CH (d);
		d_next = d->next; /* We delete from the list , so need to save this */
		
		if (!d->character || d->connected > CON_PLAYING) /* drop those logging on */
		{
			write_to_desc (d, "\n\rSorry, we are rebooting. Come back in a few minutes.\n\r", 0);
			close_socket (d); /* throw'em out */
		}
		else
		{
                        if ( d->original != NULL )
                             do_return( d->character, "" );
			fprintf (fp, "%d %s %s %d\n", d->descriptor, och->name, d->host,d->hostip);

			save_char_obj (och);

                        compressEnd(d);

			sprintf (buf, "\n\rCopyover Reboot by %s.  Please remain connected.\n\r",
			    can_see(d->character,ch) ? ch->name : "An Immortal");
			write_to_desc (d, buf, 0);
		}
	}
	
	fprintf (fp, "-1\n");
	fclose (fp);
	
	/* Close reserve and other always-open files and release other resources */
	
	fclose (fpReserve);
	
        /* Shut down python interpreter */
        terminate_python();

	/* exec - descriptors are inherited */
	
	sprintf (buf, "%d", port);
	sprintf (buf2, "%d", control);
	
	log_string(LOG_GAME, "Attempting copyover on port %s\n\r", buf);

	signal(SIGCLD, SIG_IGN);

	child_pid = fork();

	if( child_pid < 0 )
	{
		perror("fork failure");
		exit(1);
	}

	if( child_pid == 0 )
	{
	        ret = execl(EXE_PATH, EXE_FILE, buf, "copyover", buf2, NULL);

		/* Failed - sucessful exec will not return */

	        perror ("do_copyover: execl");
	        log_string (LOG_ERR,"Copyover FAILED! Error Code: %d\n\r",ret);

	        _exit(1);
  	}

	waitpid( child_pid, NULL, WNOHANG);

	exit(1);

	fpReserve = fopen (NULL_FILE, "r");

	return;
}

/* Recover from a copyover - load players */
void copyover_recover ()
{
	DESCRIPTOR_DATA *d;
	FILE *fp;
	char name [100];
	char host[MSL];
	int desc;
	bool fOld;
	unsigned int hostip = 0;

	log_string(LOG_GAME,"Copyover recovery initiated");

	fp = fopen (COPYOVER_FILE, "r");
	
	if (!fp) /* there are some descriptors open which will hang forever then ? */
	{
		perror ("copyover_recover:fopen");
		log_string (LOG_GAME,"Copyover file not found. Exiting.\n\r");
		exit (1);
	}

//	unlink (COPYOVER_FILE); /* In case something crashes - doesn't prevent reading	*/
	
	for (;;)
	{
		fscanf (fp, "%d %s %s %d\n", &desc, name, host, &hostip);
		if (desc == -1)
			break;

		/* Write something, and check if it goes error-free */		
		if (!write_to_descriptor (desc, "\n\rReloading descriptors!\n\r",0))
		{
			close (desc); /* nope */
			continue;
		}
		
		d = new_descriptor();
		d->descriptor = desc;
		
		d->host = str_dup (host);
		d->hostip = hostip;
		d->next = descriptor_list;
		descriptor_list = d;
		d->connected = CON_COPYOVER_RECOVER; /* -15, so close_socket frees the char */
		
	
		/* Now, find the pfile */
		
		fOld = load_char_obj (d, name);
		
		if (!fOld) /* Player file not found?! */
		{
			write_to_descriptor (desc, "\n\rSomehow, your character was lost in the copyover. Sorry.\n\r", 0);
			close_socket (d);			
		}
		else /* ok! */
		{
			write_to_descriptor (desc, "\n\rReboot complete.\n\r",0);
	

			/* Just In Case */
			if (!d->character->in_room)
				d->character->in_room = get_room_index (ROOM_VNUM_TEMPLE);

			d->character->pcdata->socket = str_dup (d->host);

			/* Insert in the char_list */
			d->character->next = char_list;
			char_list = d->character;
			d->character->plr_next = plr_list;
			plr_list = d->character;

			char_to_room (d->character, d->character->in_room);
			do_look (d->character, "auto");
			act ("$n materializes!", d->character, NULL, NULL, TO_ROOM);
			d->connected = CON_PLAYING;

			if (d->character->pet != NULL)
			{
			    char_to_room(d->character->pet,d->character->in_room);
			    act("$n materializes!.",d->character->pet,NULL,NULL,TO_ROOM);
			}
		}
		
	}
        fclose (fp);
	
}

void do_copyove( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to COPYOVER, spell it out.\n\r", ch );
    return;
}
