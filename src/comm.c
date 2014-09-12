/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku vMud improvments copyright (C) 1992, 1993 by Michael         *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
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

/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

/*
 MCCP Support by
Oliver Jowett <icecube@ihug.co.nz>, 990310
Dominic Eidson <dominic@the-infinite.org>, 990310
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>

#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "event.h"
#include "olc.h"

/* command procedures needed */
DECLARE_DO_FUN(do_help		);
DECLARE_DO_FUN(do_save		);
DECLARE_DO_FUN(do_look		);
DECLARE_DO_FUN(do_skills	);
DECLARE_DO_FUN(do_outfit	);
DECLARE_DO_FUN(do_asave		);
DECLARE_DO_FUN(do_dump		);


char colour_buffer = 'x';
char last_colour = 'x';


/*
 * Malloc debugging stuff.
 */
#if defined(sun)
#undef MALLOC_DEBUG
#endif

#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern	int	malloc_debug	args( ( int  ) );
extern	int	malloc_verify	args( ( void ) );
#endif



/*
 * Signal handling.
 * Apollo has a problem with __attribute(atomic) in signal.h,
 *   I dance around it.
 */
#if defined(apollo)
#define __attribute(x)
#endif

#if defined(unix)
#include <signal.h>
#endif

#if defined(apollo)
#undef __attribute
#endif



/*
 * Socket and TCP/IP stuff.
 */

#if	defined(unix)
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "telnet.h"
const	char	echo_off_str	[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const	char	echo_on_str	[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const	char 	go_ahead_str	[] = { IAC, GA, '\0' };
const	char 	ask_client_type	[] = { IAC, DO, TELOPT_TTYPE, '\0' };
const	char 	ask_client_next	[] = { IAC, SB, TELOPT_TTYPE,
                                       TELQUAL_SEND, IAC, SE, '\0' };

/* mccp: compression negotiation strings */
const   char    compress_dont   [] = { IAC, DONT, TELOPT_COMPRESS, '\0' };
const   char    compress2_will   [] = { IAC, WILL, TELOPT_COMPRESS2, '\0' };
const   char    compress2_dont   [] = { IAC, DONT, TELOPT_COMPRESS2, '\0' };
 
#endif



/*
 * OS-dependent declarations.
 */
#if	defined(_AIX)
#include <sys/select.h>
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if	defined(apollo)
#include <unistd.h>
void	bzero		args( ( char *b, int length ) );
#endif

#if	defined(__hpux)
int	accept		args( ( int s, void *addr, int *addrlen ) );
int	bind		args( ( int s, const void *addr, int addrlen ) );
void	bzero		args( ( char *b, int length ) );
int	getpeername	args( ( int s, void *addr, int *addrlen ) );
int	getsockname	args( ( int s, void *name, int *addrlen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	setsockopt	args( ( int s, int level, int optname,
 				const void *optval, int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if	defined(interactive)
#include <net/errno.h>
#include <sys/fnctl.h>
#endif

#if	defined(linux)
/*int	accept		args( ( int s, struct sockaddr *addr, int *addrlen
) );*/
/*int	bind		args( ( int s, struct sockaddr *name, int namelen
) );*/
int	close		args( ( int fd ) );
/*
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
*/
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
/*int	listen		args( ( int s, int backlog ) );*/
/*int	read		args( ( int fd, char *buf, int nbyte ) );*/
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	socket		args( ( int domain, int type, int protocol ) );
/*int	write		args( ( int fd, char *buf, int nbyte ) );*/
#endif

#if	defined(macintosh)
#include <console.h>
#include <fcntl.h>
#include <unix.h>
struct	timeval
{
	time_t	tv_sec;
	time_t	tv_usec;
};
#if	!defined(isascii)
#define	isascii(c)		( (c) < 0200 )
#endif
static	long			theKeys	[4];

int	gettimeofday		args( ( struct timeval *tp, void *tzp ) );
#endif

#if	defined(MIPS_OS)
extern	int		errno;
#endif

#if	defined(MSDOS)
int	gettimeofday	args( ( struct timeval *tp, void *tzp ) );
int	kbhit		args( ( void ) );
#endif

#if	defined(NeXT)
int	close		args( ( int fd ) );
int	fcntl		args( ( int fd, int cmd, int arg ) );
#if	!defined(htons)
u_short	htons		args( ( u_short hostshort ) );
#endif
#if	!defined(ntohl)
u_long	ntohl		args( ( u_long hostlong ) );
#endif
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if	defined(sequent)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
int	close		args( ( int fd ) );
int	fcntl		args( ( int fd, int cmd, int arg ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
#if	!defined(htons)
u_short	htons		args( ( u_short hostshort ) );
#endif
int	listen		args( ( int s, int backlog ) );
#if	!defined(ntohl)
u_long	ntohl		args( ( u_long hostlong ) );
#endif
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	setsockopt	args( ( int s, int level, int optname, caddr_t optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

/* This includes Solaris Sys V as well */
#if defined(sun)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
#if defined(SYSV)
int setsockopt		args( ( int s, int level, int optname,
			    const char *optval, int optlen ) );
#else
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
#endif
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if defined(ultrix)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );

#endif



/*
 * Global variables.
 */
DESCRIPTOR_DATA *   descriptor_list;	/* All open descriptors		*/
DESCRIPTOR_DATA *   d_next;		/* Next descriptor in loop	*/
FILE *		    fpReserve;		/* Reserved file handle		*/
bool		    god;		/* All new chars are gods!	*/
bool		    merc_down;		/* Shutdown			*/
bool		    wizlock;		/* Game is wizlocked		*/
bool		    newlock;		/* Game is newlocked		*/
bool		    dnslock = FALSE;	/* Game is dnslocked		*/
bool		    savecrash = TRUE;	/* Save on crash		*/
char *              whomessage = NULL;	/* Announcement on who display  */
long		    gamelag = 0;
char		    str_boot_time[MAX_INPUT_LENGTH];
time_t		    current_time;	/* time of this pulse */
time_t              start_time;	
char		    clcode[ MAX_INPUT_LENGTH ];
bool                MOBtrigger = TRUE;  /* act() switch                 */
unsigned long       current_pulse = 0;

/*
 * OS-dependent local functions.
 */
#if defined(macintosh) || defined(MSDOS)
void	game_loop_mac_msdos	args( ( void ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );
#endif

#if defined(unix)
void	game_loop_unix		args( ( int control ) );
int	init_socket		args( ( int port ) );
void	init_descriptor		args( ( int control ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );
#endif

void	bad_password args((DESCRIPTOR_DATA *d, char *argument));
#define MAX_LOGIN_FAILURE 50

struct
{
    int ip;
    int attempts;
    long lastattempt;
    long lockedout;
} login_failures[MAX_LOGIN_FAILURE];

void terminate_python();

/*
 * Other local functions (OS-independent).
 */
bool	check_parse_name	args( ( char *name ) );
bool	check_valid_name	args( ( char *name ) );
bool	check_reconnect		args( ( DESCRIPTOR_DATA *d, char *name,
				    bool fConn ) );
bool	check_playing		args( ( DESCRIPTOR_DATA *d, char *name ) );
int	main			args( ( int argc, char **argv ) );
void	nanny			args( ( DESCRIPTOR_DATA *d, char *argument ) );
bool	process_output		args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
void	read_from_buffer	args( ( DESCRIPTOR_DATA *d ) );
void	stop_idling		args( ( CHAR_DATA *ch ) );
void    bust_a_prompt           args( ( CHAR_DATA *ch ) );
void    sig_handler             args( ( int sig ) );
void    auto_ban_ip             args( ( int site ) );
void	init_signals		();

char last_command[MAX_STRING_LENGTH];
char last_from_buffer[MAX_STRING_LENGTH];
/*int  last_hostip;*/

int port, control;

int main( int argc, char **argv )
{
    struct timeval now_time;
    bool fCopyOver = FALSE;
    struct rlimit rli, rlid;

    FILE* log_file;

    log_file = NULL;

            log_file = fopen( "../log/args.log", "a" );
            fprintf( log_file, "BOTA : %d : %d : %s : %d\n", atoi( argv[0] ), atoi( argv[1] ), argv[2], atoi( argv[3] ) );
            fflush(log_file);
            fclose( log_file );


    /*
     * Memory debugging if needed.
     */
#if defined(MALLOC_DEBUG)
    malloc_debug( 2 );
#endif

    /*
     * Init time.
     */
    gettimeofday( &now_time, NULL );
    current_time 	= (time_t) now_time.tv_sec;
    start_time		= current_time;
    strcpy( str_boot_time, ctime( &current_time ) );

    /*
     * Macintosh console initialization.
     */
#if defined(macintosh)
    console_options.nrows = 31;
    cshow( stdout );
    csetmode( C_RAW, stdin );
    cecho2file( "log file", 1, stderr );
#endif

    /*
     * Reserve one channel for our use.
     */
    if ( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
    {
	perror( NULL_FILE );
	exit( 1 );
    }

    /*
     * Get the port number.
     */
    port = 8501;

    if ( argc > 1 )
    {
	if ( !is_number( argv[1] ) )
	{
	    fprintf( stderr, "Usage: %s [port #] [wwwport #]\n", argv[0] );
	    exit( 1 );
	}
	else if ( ( port = atoi( argv[1] ) ) <= 1024 )
	{
	    fprintf( stderr, "Port numbers must be above 1024.\n" );
	    exit( 1 );
	}

        /* Are we recovering from a copyover? */
 	if (argv[2] && argv[2][0])
	{
                fCopyOver = TRUE;
                control = atoi(argv[3]);

		log_string(LOG_GAME, "Copyover on port %d, control %d\n\r", port, control);
        }
        else
                fCopyOver = FALSE;
    }

	log_string(LOG_GAME,"Trying to run game on port %d\n\r",port);

    /*
     * Run the game.
     */
#if defined(macintosh) || defined(MSDOS)
    boot_db( fCopyOver );
    log_string(LOG_GAME, "Merc is ready to rock." );
    game_loop_mac_msdos( );
#endif

#if defined(unix)

    if (!fCopyOver)
    {
       control = init_socket( port );
    }

    boot_db( fCopyOver );

    log_string(LOG_GAME, "Initializing Signal Handlers and Resource Limits" );

    rli.rlim_max = RLIM_INFINITY; /* second of CPU time */
    rlid.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_CPU,&rli);
    setrlimit(RLIMIT_CORE,&rlid);

    init_signals();

    sprintf( log_buf, "BOTA is ready to *ROCK* on port %d!", port );
    log_string(LOG_GAME, log_buf );

    game_loop_unix( control );

    close (control);
#endif

    /*
     * That's all, folks.
     */
    log_string(LOG_GAME, "Normal termination of game." );

    signal( SIGABRT, SIG_DFL );
    signal( SIGTERM, SIG_DFL );
    signal( SIGSEGV, SIG_DFL );
    signal( SIGBUS, SIG_DFL );
    signal( SIGXCPU, SIG_DFL );

    terminate_python();

    exit( 0 );
    return 0;
}



#if defined(unix)
int init_socket( int port )
{
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa;
    int x = 1;
    int fd;

    if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
	perror( "Init_socket: socket" );
	exit( 1 );
    }

    if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
    (char *) &x, sizeof(x) ) < 0 )
    {
	perror( "Init_socket: SO_REUSEADDR" );
	close(fd);
	exit( 1 );
    }

#if defined(SO_DONTLINGER) && !defined(SYSV)
    {
	struct	linger	ld;

	ld.l_onoff  = 1;
	ld.l_linger = 1000;

	if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
	(char *) &ld, sizeof(ld) ) < 0 )
	{
	    perror( "Init_socket: SO_DONTLINGER" );
	    close(fd);
	    exit( 1 );
	}
    }
#endif

    sa		    = sa_zero;
    sa.sin_family   = AF_INET;
    sa.sin_port	    = htons( port );

    if ( bind( fd, (struct sockaddr *) &sa, sizeof(sa) ) < 0 )
    {
	perror("Init socket: bind" );
	close(fd);
	exit(1);
    }


    if ( listen( fd, 3 ) < 0 )
    {
	perror("Init socket: listen");
	close(fd);
	exit(1);
    }

    return fd;
}
#endif


#if defined(unix)
void game_loop_unix( int control )
{
    static struct timeval null_time;
    struct timeval last_time;
    extern char last_from_buffer[MAX_STRING_LENGTH];

    signal( SIGPIPE, SIG_IGN );
    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /* Main loop */
    while ( !merc_down )
    {
	fd_set in_set;
	fd_set out_set;
	fd_set exc_set;
	DESCRIPTOR_DATA *d;
	int maxdesc;
/*
#if defined(MALLOC_DEBUG)
	if ( malloc_verify( ) != 1 )
	    abort( );
#endif
*/

	/*
	 * Poll all active descriptors.
	 */
	FD_ZERO( &in_set  );
	FD_ZERO( &out_set );
	FD_ZERO( &exc_set );
	FD_SET( control, &in_set );
	maxdesc	= control;
	for ( d = descriptor_list; d; d = d->next )
	{
	    maxdesc = UMAX( maxdesc, d->descriptor );
	    FD_SET( d->descriptor, &in_set  );
	    FD_SET( d->descriptor, &out_set );
	    FD_SET( d->descriptor, &exc_set );
	}

	if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
	{
	    perror( "Game_loop: select: poll" );
	    exit( 1 );
	}

	/*
	 * New connection?
	 */
	if ( FD_ISSET( control, &in_set ) )
	    init_descriptor( control );

	/*
	 * Kick out the freaky folks.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;   
	    if ( FD_ISSET( d->descriptor, &exc_set ) )
	    {
		FD_CLR( d->descriptor, &in_set  );
		FD_CLR( d->descriptor, &out_set );
		if ( d->character && d->character->level > 1)
		    save_char_obj( d->character );
		d->outtop	= 0;
		close_socket( d );
	    }
	}

	/*
	 * Process input.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next	= d->next;
	    d->fcommand	= FALSE;

	    if ( FD_ISSET( d->descriptor, &in_set ) )
	    {
		if ( d->character != NULL )
		    d->character->timer = 0;
		if ( !read_from_descriptor( d ) )
		{
		    FD_CLR( d->descriptor, &out_set );
		    if ( d->character != NULL && d->character->level > 1)
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		    continue;
		}
	    }

	    if (d->character != NULL && d->character->daze > 0)
		--d->character->daze;

	    if ( d->character != NULL && d->character->wait > 0 )
	    {
		    if (d->character->position != POS_FIGHTING 
		    && IS_SET(d->character->act, PLR_WAIT))
		    {
   	               if (d->character->wait > 1)
			 {
	  	    	        --d->character->wait;
				 send_to_char(".", d->character);
	   	               if (d->character->wait == 1)
				 send_to_char("\n\r", d->character);
			        continue;
			 }
			 else
				 send_to_char("\r", d->character);

  	    	        --d->character->wait;
		    }
		    else
		    {
	  	    	--d->character->wait;
		        continue;
		    }
	    }

	    read_from_buffer( d );
            sprintf(last_from_buffer,"Desc %d: \"%s\"", d->descriptor, d->incomm);
	    if ( d->incomm[0] != '\0' )
	    {
		d->fcommand	= TRUE;
		stop_idling( d->character );

	/* OLC */
	if ( d->showstr_point )
	    show_string( d, d->incomm );
	else
	if ( d->pString )
	    string_add( d->character, d->incomm );
	else
	    switch ( d->connected )
	    {
	        case CON_PLAYING:
		    if ( !run_olc_editor( d ) )
    		        substitute_alias( d, d->incomm );
		    break;
	        default:
		    nanny( d, d->incomm );
		    break;
	    }

		d->incomm[0]	= '\0';
	    }
	}



	/*
	 * Autonomous game motion.
	 */
        event_update( );
	update_handler( );



	/*
	 * Output.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;

	    if ( ( d->fcommand || d->outtop > 0 || d->out_compress )
	    &&   FD_ISSET(d->descriptor, &out_set) )
	    {
                bool ok = TRUE;

                if( d->fcommand || d->outtop > 0 )
                    ok = process_output( d, TRUE );
                if(ok && d->out_compress)
                    ok = processCompressed(d);

                if(!ok)
		{
		    if ( d->character != NULL && d->character->level > 1)
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		}
	    }
	}



	/*
	 * Synchronize to a clock.
	 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
	 * Careful here of signed versus unsigned arithmetic.
	 */
	{
            extern long gamelag;
	    struct timeval now_time;
	    long secDelta;
	    long usecDelta;

	    gettimeofday( &now_time, NULL );
            gamelag     = (int) now_time.tv_usec - (int) last_time.tv_usec;
	    usecDelta	= ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
			+ 1000000 / PULSE_PER_SECOND;
	    secDelta	= ((int) last_time.tv_sec ) - ((int) now_time.tv_sec );
	    while ( usecDelta < 0 )
	    {
		usecDelta += 1000000;
		secDelta  -= 1;
	    }

	    while ( usecDelta >= 1000000 )
	    {
		usecDelta -= 1000000;
		secDelta  += 1;
	    }

	    if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
	    {
		struct timeval stall_time;

		stall_time.tv_usec = usecDelta;
		stall_time.tv_sec  = secDelta;
		if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 )
		{
		    if(errno != EINTR)
		    {
		        perror( "Game_loop: select: stall" );
		        exit( 1 );
		    }
		}
	    }
	}

	gettimeofday( &last_time, NULL );
	current_time = (time_t) last_time.tv_sec;
        current_pulse++;
    }

    return;
}
#endif



#if defined(unix)
void init_descriptor( int control )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *dnew;
    struct sockaddr_in sock;
    struct hostent *from;
    int desc;
    int size;

    size = sizeof(sock);
    getsockname( control, (struct sockaddr *) &sock, &size );
    if ( ( desc = accept( control, (struct sockaddr *) &sock, &size) ) < 0 )
    {
	perror( "New_descriptor: accept" );
	return;
    }

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

    if ( fcntl( desc, F_SETFL, FNDELAY ) == -1 )
    {
	perror( "New_descriptor: fcntl: FNDELAY" );
	return;
    }

    /*
     * Cons a new descriptor.
     */
    dnew = new_descriptor();

    dnew->descriptor    = desc;

    size = sizeof(sock);
    if ( getpeername( desc, (struct sockaddr *) &sock, &size ) < 0 )
    {
	perror( "New_descriptor: getpeername" );
	dnew->host = str_dup( "(unknown)" );
    }
    else
    {
	/*
	 * Would be nice to use inet_ntoa here but it takes a struct arg,
	 * which ain't very compatible between gcc and system libraries.
	 */
	int addr;

	addr = ntohl( sock.sin_addr.s_addr );
	sprintf( buf, "%d.%d.%d.%d",
	    ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
	    ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF
	    );
        log_f("Socket %d at %p connected from %s:%d", desc, &sock, buf, sock.sin_port);
        if(!dnslock)
            from = gethostbyaddr( (char *) &sock.sin_addr,
	         sizeof(sock.sin_addr), AF_INET );
        else
            from = NULL;
	dnew->host = str_dup( from ? from->h_name : buf );
	sprintf( log_buf, "Sock.sinaddr: %s (%s)", buf, dnew->host );
	log_string(LOG_GAME, log_buf );
        dnew->hostip = addr;

    }
	
    /*
     * Swiftest: I added the following to ban sites.  I don't
     * endorse banning of sites, but Copper has few descriptors now
     * and some people from certain sites keep abusing access by
     * using automated 'autodialers' and leaving connections hanging.
     *
     * Furey: added suffix check by request of Nickel of HiddenWorlds.
     */

    if( desc >= 230 && str_cmp(dnew->host,"poding"))
    {
        write_to_descriptor( desc,
            "The MUD is busy presently.\n\r", 0);
        close(desc);
        free_descriptor(dnew);
        return;
    }

    if ( check_ban(dnew->host,BAN_ALL) )
    {
	write_to_descriptor( desc,
	    "Your site has been banned from this mud.\n\r", 0 );
	close( desc );
	free_descriptor(dnew);
	return;
    }

    sprintf( buf, "%d.%d.%d.%d",
        ( dnew->hostip >> 24 ) & 0xFF, ( dnew->hostip >> 16 ) & 0xFF,
        ( dnew->hostip >>  8 ) & 0xFF, ( dnew->hostip       ) & 0xFF
        );

    if ( check_ban(buf,BAN_ALL) )
    {
	write_to_descriptor( desc,
	    "Your site has been auto-banned from this mud.\n\r", 0 );
	write_to_descriptor( desc,
	    "If this is a mistake, contact thyrr@bb3.betterbox.net\n\r", 0 );
	close( desc );
	free_descriptor(dnew);
	return;
    }

    /*
     * Init descriptor data.
     */
    dnew->next			= descriptor_list;
    descriptor_list		= dnew;

    /*
     * Send the greeting.
     */
    {
	extern char * help_greetinga;
	extern char * help_greetingb;
	extern char * help_greetingc;
	extern char * help_greetingd;
	extern char * help_greetinge;
	extern char * help_authors;
	extern char * help_login;
	switch (number_range(0,4))
	{
	    default:
		if ( help_greetinga[0] == '.' )
		    write_to_buffer( dnew, help_greetinga+1, 0 );
		else
		    write_to_buffer( dnew, help_greetinga  , 0 );
		break;
	    case 0:
		if ( help_greetinga[0] == '.' )
		    write_to_buffer( dnew, help_greetinga+1, 0 );
		else
		    write_to_buffer( dnew, help_greetinga  , 0 );
		break;
	    case 1:
		if ( help_greetingb[0] == '.' )
		    write_to_buffer( dnew, help_greetingb+1, 0 );
		else
		    write_to_buffer( dnew, help_greetingb  , 0 );
		break;
	    case 2:
		if ( help_greetingc[0] == '.' )
		    write_to_buffer( dnew, help_greetingc+1, 0 );
		else
		    write_to_buffer( dnew, help_greetingc  , 0 );
		break;
	    case 3:
		if ( help_greetingd[0] == '.' )
		    write_to_buffer( dnew, help_greetingd+1, 0 );
		else
		    write_to_buffer( dnew, help_greetingd  , 0 );
		break;
	    case 4:
		if ( help_greetinge[0] == '.' )
		    write_to_buffer( dnew, help_greetinge+1, 0 );
		else
		    write_to_buffer( dnew, help_greetinge  , 0 );
		break;
	}
	if ( help_authors[0] == '.' )
	    write_to_buffer( dnew, help_authors+1, 0 );
	else
	    write_to_buffer( dnew, help_authors  , 0 );
	if ( help_login[0] == '.' )
	    write_to_buffer( dnew, help_login+1, 0 );
	else
	    write_to_buffer( dnew, help_login  , 0 );
    }
    return;
}
#endif

void close_socket( DESCRIPTOR_DATA *dclose )
{
    CHAR_DATA *ch;

    if ( dclose->outtop > 0 )
	process_output( dclose, FALSE );

    if ( dclose->snoop_by != NULL )
    {
	write_to_buffer( dclose->snoop_by,
	    "Your victim has left the game.\n\r", 0 );
    }

    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->snoop_by == dclose )
		d->snoop_by = NULL;
	}
    }

    if ( ( ch = dclose->character ) != NULL )
    {
	sprintf( log_buf, "Closing link to %s.", ch->name );
	log_string(LOG_CONNECT, log_buf );
	if ( (dclose->connected == CON_PLAYING)
           || ((dclose->connected >= CON_NOTE_TO)
           && (dclose->connected <= CON_NOTE_FINISH)))
	{
            save_char_obj(dclose->character);
	    act_new_2( "$n has lost $s link.", ch, NULL, NULL, TO_ROOM, POS_RESTING, 0, ACTION_MUST_SEE );
	    wiznet("Net death has claimed $N.",ch,NULL,WIZ_LINKS,0,get_trust(ch));
	    ch->desc = NULL;
	}
	else
	{
	    free_char( dclose->original ? dclose->original : dclose->character );
	}
    }

    if ( d_next == dclose )
	d_next = d_next->next;   

    if ( dclose == descriptor_list )
    {
	descriptor_list = descriptor_list->next;
    }
    else
    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d && d->next != dclose; d = d->next )
	    ;
	if ( d != NULL )
	    d->next = dclose->next;
	else
	    bug( "Close_socket: dclose not found.", 0 );
    }

    if (dclose->out_compress)
    {
        deflateEnd(dclose->out_compress);
        free_mem(dclose->out_compress_buf, COMPRESS_BUF_SIZE);
        free_mem(dclose->out_compress, sizeof(z_stream));
    }

    close( dclose->descriptor );
    free_descriptor(dclose);
#if defined(MSDOS) || defined(macintosh)
    exit(1);
#endif
    return;
}

bool write_to_desc(DESCRIPTOR_DATA *d, char *txt, int length)
{
    if(d->out_compress)
        return writeCompressed(d,txt,length);
    else
        return write_to_descriptor(d->descriptor, txt, length);
}

bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
    int iStart;

    /* Hold horses if pending command already. */
    if ( d->incomm[0] != '\0' )
	return TRUE;

    /* Check for overflow. */
    iStart = strlen(d->inbuf);
    if ( iStart >= sizeof(d->inbuf) - 10 )
    {
	sprintf( log_buf, "%s input overflow!", d->host );
	log_string(LOG_ERR, log_buf );
	write_to_desc( d,
	    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
	return FALSE;
    }

    /* Snarf input. */

#if defined(MSDOS) || defined(unix)
    for ( ; ; )
    {
	int nRead;

	nRead = read( d->descriptor, d->inbuf + iStart,
	    sizeof(d->inbuf) - 10 - iStart );
	if ( nRead > 0 )
	{
	    iStart += nRead;
            d->insize += nRead;
	    if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' )
		break;
	}
	else if ( nRead == 0 )
	{
	    log_string(LOG_ERR, "EOF encountered on read." );
	    return FALSE;
	}
	else if ( errno == EWOULDBLOCK )
	    break;
	else
	{
	    perror( "Read_from_descriptor" );
	    return FALSE;
	}
    }
#endif

    d->inbuf[iStart] = '\0';
    return TRUE;
}


/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer( DESCRIPTOR_DATA *d )
{
    int i, j, k;

    /*
     * Hold horses if pending command already.
     */
    if ( d->incomm[0] != '\0' )
	return;

    /*
     * Look for at least one new line, or deal with telnet options
     */
    for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
        if ( d->inbuf[i] == IAC )
        {
            bool seq_complete;
            unsigned char type = 0;
            int first_i;
            first_i = i;

            i++;
            if(i >= d->insize) return;
            type = d->inbuf[i];

            switch(type)
            {
                case WILL:
                case WONT:
                case DO:
                case DONT:
                    i++;
                    if(i >= d->insize) return;
                    else
                    {
                        if(d->inbuf[i] == TELOPT_COMPRESS2)
                        {
                            if(type == DO)
                            {
                                SET_BIT(d->flags, DESCRIPTOR_COMPRESS2);
                                compressStart(d);
                            }
                            else if(type == DONT)
                            {
                                REMOVE_BIT(d->flags, DESCRIPTOR_COMPRESS2);
                                compressEnd(d);
                            }
                            else if(type == WONT)
                            {
                                REMOVE_BIT(d->flags, DESCRIPTOR_COMPRESS2);
                            }
                        }
                        seq_complete = TRUE;
                        break;
                    }
                case SB:
                    i++;
                    if(i >= d->insize) return;

                    if(d->inbuf[i] == TELOPT_TTYPE)
                    {
                        char clientbuf[21];
                        int v = 0;

                        i++;
                        if(i >= d->insize) return;

                        /* umm, we'll assume this is IS */

                        if(i + 1 >= d->insize) return;

                        while(++i < d->insize && v < 20)
                        {
                            if(d->inbuf[i] == IAC)
                                ;
                            else if(d->inbuf[i] == SE)
                            {
                                seq_complete = TRUE;
                                break;
                            }
                            else
                                clientbuf[v++] = d->inbuf[i];
                        }
                        clientbuf[v] = '\0';
                        log_f("Ttype is: '%s'", clientbuf);

                    }

                    break;
            }
            if(seq_complete)
            { /* overwrite all that telnet gunk */
                for(; first_i <= i; first_i++)
                {
                    d->inbuf[first_i] = '\a';
                }
            }
        }

	if ( d->inbuf[i] == '\0' )
	    return;
    }

    /*
     * Canonical input processing.
     */
    for ( i = 0, k = 0;
          d->inbuf[i] != '\n' && d->inbuf[i] != '\r' && d->inbuf[i] != '\0';
          i++ )
    {
	if ( k >= MAX_INPUT_LENGTH - 2 )
	{
	    write_to_desc( d, "Line too long.\n\r", 0 );

	    /* skip the rest of the line */
	    for ( ; d->inbuf[i] != '\0'; i++ )
	    {
		if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
		    break;
	    }
	    d->inbuf[i]   = '\n';
	    d->inbuf[i+1] = '\0';
	    break;
	}

	if ( d->inbuf[i] == '\b' && k > 0 )
	    --k;
	else if ( isascii(d->inbuf[i]) && isprint(d->inbuf[i]) )
	    d->incomm[k++] = d->inbuf[i];
        else if ( d->inbuf[i] == '\0' ) return;
    }

    /*
     * Finish off the line.
     */
    if ( k == 0 )
	d->incomm[k++] = ' ';
    d->incomm[k] = '\0';

    /*
     * Deal with bozos with #repeat 1000 ...
     */

    if ( k > 1 || d->incomm[0] == '!' )
    {
    	if ( d->incomm[0] != '!' && strcmp( d->incomm, d->inlast ) )
	{
	    d->repeat = 0;
	}
	else
	{
            if (++d->repeat >= 15 && d->repeat % 10 && d->character
            &&  d->connected == CON_PLAYING)
	    {
/*		sprintf( log_buf, "%s input spamming!", d->host );
		log_string(LOG_GAME, log_buf );*/
		wiznet("--- $N spam! ---",
		       d->character,NULL,WIZ_SPAM,0,get_trust(d->character));
/*
		if (d->incomm[0] == '!')
		    wiznet(d->inlast,d->character,NULL,WIZ_SPAM,0,
			get_trust(d->character));
		else
		    wiznet(d->incomm,d->character,NULL,WIZ_SPAM,0,
			get_trust(d->character));
*/
            } else if ( d->repeat == 25 )
	    {
		write_to_desc( d,
		    "\n\n\n***** Do not spam! *****\n\r\n\r", 0 );

		wiznet("--- $N spam spam! ---",
		       d->character,NULL,WIZ_SPAM,0,get_trust(d->character));

            } else if ( d->repeat >= 35 )
	    {

		d->repeat = 0;
		write_to_desc( d,
		    "\n\n*** Hey, STOP SPAMMING! ***\n\r", 0 );
		strcpy( d->incomm, "quit" );
	    }
	}

    }


    /*
     * Do '!' substitution.
     */
    if ( d->incomm[0] == '!' )
	strcpy( d->incomm, d->inlast );
    else
	strcpy( d->inlast, d->incomm );

    /*
     * Shift the input buffer.
     */
    while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
	i++;
    for ( j = 0; ( d->inbuf[j] = d->inbuf[i+j] ) != '\0'; j++ )
	;

    d->insize -= j;

    return;
}



/*
 * Low level output function.
 */
bool process_output( DESCRIPTOR_DATA *d, bool fPrompt )
{
    extern bool merc_down;

    /*
     * Bust a prompt.
     */
    if ( !merc_down )
    {
	if ( d->showstr_point )
	    write_to_buffer( d, "[Hit Return to continue]\n\r", 0 );
	else if ( fPrompt && d->pString && d->connected == CON_PLAYING )
	    write_to_buffer( d, "> ", 2 );
	else if ( fPrompt && d->connected == CON_PLAYING )
	{
	    CHAR_DATA *ch;
	CHAR_DATA *victim;

	ch = d->character;

        /* battle prompt */
        if ((victim = ch->fighting) != NULL && can_see(ch,victim))
        {
            int percent;
            char wound[100];
	    char buf[MAX_STRING_LENGTH];
 
            if (victim->max_hit > 0)
                percent = victim->hit * 100 / victim->max_hit;
            else
                percent = -1;
 
            if (percent >= 100)
                sprintf(wound,"is in excellent condition.");
            else if (percent >= 90)
                sprintf(wound,"has a few scratches.");
            else if (percent >= 75)
                sprintf(wound,"has some small wounds and bruises.");
            else if (percent >= 50)
                sprintf(wound,"has quite a few wounds.");
            else if (percent >= 30)
                sprintf(wound,"has some big nasty wounds and scratches.");
            else if (percent >= 15)
                sprintf(wound,"looks pretty hurt.");
            else if (percent >= 0)
                sprintf(wound,"is in awful condition.");
            else
                sprintf(wound,"is bleeding to death.");
 
            sprintf(buf,"{f%s %s{x\n\r", 
	            IS_NPC(victim) ? victim->short_descr : victim->name,wound);
	    buf[0] = UPPER(buf[0]);
	    send_to_char(buf, ch);
	    if (victim->stunned)
	    {
		sprintf(buf,"{f%s is stunned.{x\n\r", 
	            IS_NPC(victim) ? victim->short_descr : victim->name);
		send_to_char(buf, ch);
	    }

/*            write_to_buffer( d, buf, 0); */
        }

	ch = d->original ? d->original : d->character;
	if (!IS_SET(ch->comm, COMM_COMPACT) )
	{   
	    if (ch->wait <= 0 || ch->fighting != NULL || !IS_SET(ch->act, PLR_WAIT))
	    write_to_buffer( d, "\n\r", 2 );
	}

       if ( IS_SET(ch->comm, COMM_PROMPT) )
            bust_a_prompt( d->character );

	if (IS_SET(ch->comm,COMM_TELNET_GA))
	    write_to_buffer(d,go_ahead_str,0);
      }
    }

    /*
     * Short-circuit if nothing to write.
     */
    if ( d->outtop == 0 )
	return TRUE;

    /*
     * Snoop-o-rama.
     */
    if ( d->snoop_by != NULL )
    {
	if (d->character != NULL)
	    write_to_buffer( d->snoop_by, d->character->name,0);
	write_to_buffer( d->snoop_by, "> ", 2 );
	write_to_buffer( d->snoop_by, d->outbuf, d->outtop );
    }

    /*
     * OS-dependent output.
     */
    if ( !write_to_desc( d, d->outbuf, d->outtop ) )
    {
	d->outtop = 0;
	return FALSE;
    }
    else
    {
	d->outtop = 0;
	return TRUE;
    }
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 * Completely overhauled for color by RW
 */
void bust_a_prompt( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char doors[MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool round;
    const char *dir_name[] = {"N","E","S","W","U","D"};
    char *door_name[] = {"north","east","south","west","up","down"};
    int door, outlet;

    sprintf(buf2, "%s", ch->prompt);

    if (ch->wait > 0 && ch->fighting == NULL && IS_SET(ch->act, PLR_WAIT))
        return;

    if (buf2 == NULL || buf2[0] == '\0')
    {
        sprintf( buf, "<%dhp %dm %dmv> %s",
	    ch->hit,ch->mana,ch->move,ch->prefix);
	send_to_char(buf,ch);
	return;
    }

   if (IS_SET(ch->comm,COMM_AFK))
   {
        if(ch->tells)
        {
            sprintf(buf, "<AFK | {R%d{x tells> " , ch->tells);
            send_to_char(buf,ch);
        }
	else send_to_char("<AFK> ",ch);
	return;
   }

    found = FALSE;
    doors[0] = '\0';
    for (door = 0; door < 6; door++)
    {
	round = FALSE;
	outlet = door;
	if ( ( ch->alignment < 0 )
	&&   ( pexit = ch->in_room->exit[door+6] ) != NULL)
	    outlet += 6;
	if ((pexit = ch->in_room->exit[outlet]) != NULL
	&&  pexit ->u1.to_room != NULL
	&&  (can_see_room(ch,pexit->u1.to_room)
	||   (IS_AFFECTED(ch,AFF_INFRARED) 
	&&    !IS_AFFECTED(ch,AFF_BLIND)))
	&&  !IS_SET(pexit->exit_info,EX_CLOSED)
        &&  !IS_SET(pexit->exit_info,EX_NOSHOW))
	{
	    found = TRUE;
	    round = TRUE;
	    strcat(doors,dir_name[door]);
	}
	if (!round)
	{
	    OBJ_DATA *portal;

            portal = get_obj_exit( door_name[door], ch->in_room->contents);
            if ((portal != NULL)
	    && !IS_AFFECTED(ch,AFF_BLIND))
            {
                found = TRUE;
                round = TRUE;
		strcat(doors,dir_name[door]);
	    }
	}
    }

    if((IS_AFFECTED(ch,AFF_BLIND) && !IS_SET(ch->act,PLR_HOLYLIGHT))
        || ch->position < POS_RESTING )
    {
        strcpy(doors,"???");
    }
    else if (!found)
    {
	sprintf(buf,"none");
    } else {
	sprintf(buf,"%s",doors);
    }
    str_replace_c(buf2, "%e", buf);
    str_replace_c(buf2, "%c", "\n\r");
    sprintf(buf,"%d",ch->hit);
    str_replace_c(buf2, "%h", buf);
    sprintf(buf,"%d",ch->max_hit);
    str_replace_c(buf2, "%H", buf);
    sprintf(buf,"%d",ch->mana);
    str_replace_c(buf2, "%m", buf);
    sprintf(buf,"%d",ch->max_mana);
    str_replace_c(buf2, "%M", buf);
    sprintf(buf,"%d",ch->move);
    str_replace_c(buf2, "%v", buf);
    sprintf(buf,"%d",ch->max_move);
    str_replace_c(buf2, "%V", buf);
    sprintf(buf,"%ld",ch->exp);
    str_replace_c(buf2, "%x", buf);
    if (!IS_NPC(ch) )
	sprintf(buf,"%ld",(ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp);
    else
	sprintf(buf,"none");
    str_replace_c(buf2, "%X", buf);
    sprintf(buf,"%ld",ch->platinum);
    str_replace_c(buf2, "%p", buf);
    sprintf(buf,"%ld",ch->gold);
    str_replace_c(buf2, "%g", buf);
    sprintf(buf,"%ld",ch->silver);
    str_replace_c(buf2, "%s", buf);
    if( ch->level > 9 )
	sprintf(buf,"%d",ch->alignment);
    else
	sprintf( buf, "%s", IS_GOOD(ch) ? "good" : IS_EVIL(ch) ?
	"evil" : "neutral" );
    str_replace_c(buf2, "%a", buf);
    if( ch->in_room != NULL )
	sprintf( buf, "%s", 
	    ((!IS_NPC(ch) && IS_SET(ch->act,PLR_HOLYLIGHT)) ||
	    (!IS_AFFECTED(ch,AFF_BLIND) && !room_is_dark( ch->in_room )))
	    ? ch->in_room->name : "darkness");
    else
	sprintf( buf, " " );
    str_replace_c(buf2, "%r", buf);
    if( IS_IMMORTAL( ch ) && ch->in_room != NULL )
	sprintf( buf, "%d", ch->in_room->vnum );
    else
	sprintf( buf, " " );
    str_replace_c(buf2, "%R", buf);
    if( IS_IMMORTAL( ch ) && ch->in_room != NULL )
	sprintf( buf, "%s", ch->in_room->area->name );
    else
	sprintf( buf, " " );
    str_replace_c(buf2, "%z", buf);

    if( IS_IMMORTAL( ch ) && ch->desc && ch->desc->editor
        && ch->desc->pEdit )
        sprintf(buf,"%s", olc_ed_vnum(ch));
    else
	strcpy( buf, " " );
    str_replace_c(buf2, "%O", buf);

    if( IS_IMMORTAL( ch ) && ch->desc && ch->desc->editor
        && ch->desc->pEdit )
        sprintf(buf,"%s", olc_ed_name(ch));
    else
	strcpy( buf, " " );
    str_replace_c(buf2, "%o", buf);

   send_to_char( buf2, ch );

   if (ch->prefix[0] != '\0')
        write_to_buffer(ch->desc,ch->prefix,0);
   return;
}



/*
 * Append onto an output buffer.
 */
void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
    /*
     * Find length in case caller didn't.
     */
    if ( length <= 0 )
	length = strlen(txt);

    if ( d->outtop == 0 && !d->fcommand )
    {
	if (d->character != NULL 
	&&  (d->character->wait <= 0 || d->character->fighting != NULL))
	{	
	  d->outbuf[0]	= '\n';
	  d->outbuf[1]	= '\r';
	  d->outtop	= 2;
	}
    }

    /*
     * Expand the buffer as needed.
     */
    while ( d->outtop + length >= d->outsize )
    {
	char *outbuf;

        if (d->outsize >= 32000)
	{
	    bug("Buffer overflow. Closing.\n\r",0);
	    close_socket(d);
	    return;
 	}
	outbuf      = alloc_mem( 2 * d->outsize );
	strncpy( outbuf, d->outbuf, d->outtop );
	free_mem( d->outbuf, d->outsize );
	d->outbuf   = outbuf;
	d->outsize *= 2;
    }

    /*
     * Copy.
     */
    strncpy( d->outbuf + d->outtop, txt, length );
    d->outtop += length;
    return;
}



/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor( int desc, char *txt, int length )
{
    int iStart;
    int nWrite;
    int nBlock;

#if defined(macintosh) || defined(MSDOS)
    if ( desc == 0 )
	desc = 1;
#endif

    if (desc < 0)
        bug_f("write_to_descriptor: negative descriptor");

    if ( length <= 0 )
	length = strlen(txt);

    for ( iStart = 0; iStart < length; iStart += nWrite )
    {
	nBlock = UMIN( length - iStart, 2048 );
	if ( ( nWrite = write( desc, txt + iStart, nBlock ) ) < 0 )
	    { perror( "Write_to_descriptor" ); return FALSE; }
    } 

    return TRUE;
}



/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
    DESCRIPTOR_DATA *d_old, *d_next;
    char buf[MAX_STRING_LENGTH];
    char newbuf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char strsave[MAX_INPUT_LENGTH];
    CHAR_DATA *ch;
    CHAR_DATA *victim;
    char *pwdnew;
    char *p;
    int iClass,race,i,weapon;
    int pos;
    bool fOld;
    int racec;

    /* Delete leading spaces UNLESS character is writing a note */
    if (d->connected != CON_NOTE_TEXT)
    {
        while ( isspace(*argument) )
            argument++;
    }

    ch = d->character;

    switch ( d->connected )
    {

    default:
	bug( "Nanny: bad d->connected %d.", d->connected );
	close_socket( d );
	return;

    case CON_GET_NAME:
	if ( argument[0] == '\0' )
	{
	    close_socket( d );
	    return;
	}

	argument[0] = UPPER(argument[0]);
	if ( !check_parse_name( argument ) )
	{
	    write_to_buffer( d, "Illegal name, try another.\n\r", 0 );
	    write_to_buffer( d, "(If you've used this name here before, and are no\n\r", 0 );
	    write_to_buffer( d, " longer able to, it may be because we've added a\n\r", 0 );
	    write_to_buffer( d, " new mobile that uses the same name. Log in with\n\r", 0 );
	    write_to_buffer( d, " a new name, and let an IMM know, and we will fix it.)\n\r", 0 );
	    write_to_buffer( d, "\n\rName: ", 0 );
	    return;
	}

	fOld = load_char_obj( d, argument );
	ch   = d->character;


	if (IS_SET(ch->act, PLR_DENY))
	{
	    sprintf( log_buf, "Denying access to %s@%s.", argument, d->host );
	    log_string(LOG_CONNECT, log_buf );
	    write_to_buffer( d, "You are denied access.\n\r", 0 );
	    close_socket( d );
	    return;
	}

	if (check_ban(d->host,BAN_PERMIT) && !IS_SET(ch->act,PLR_PERMIT))
	{
	    write_to_buffer(d,"Your site has been banned from this mud.\n\r",0);
	    close_socket(d);
	    return;
	}


/*
	if (check_adr(d->host,BAN_PERMIT) && (ch->level > 101) )
	{
	    write_to_buffer(d,"Immortals are not allowed to connect from your site.\n\r",0);
	    close_socket(d);
	    return;
	}
*/


	if (IS_SET(ch->comm, COMM_WIPED ) )
	{
	    close_socket( d );
	    return;
	}

	if ( check_reconnect( d, argument, FALSE ) )
	{
	    fOld = TRUE;
	}
	else
	{
	    if ( wizlock && !IS_IMMORTAL(ch)) 
	    {
		write_to_buffer( d, "The game is wizlocked.\n\r", 0 );
		close_socket( d );
		return;
	    }
	}

	if ( fOld )
	{
            int lfcount;

            /* fixme? move to below old character? */

            for(lfcount = 0; lfcount < MAX_LOGIN_FAILURE; lfcount++)
            {
                if(d->hostip == login_failures[lfcount].ip)
                {
                    if(login_failures[lfcount].lockedout &&
                        (login_failures[lfcount].lockedout > current_time))
                    {

	                write_to_buffer( d, "\n\r\n\r", 0 );
	                write_to_buffer( d, "Too many failed logins", 0);
	                write_to_buffer( d, " from your IP.\n\r", 0 );
	                write_to_buffer( d, "Return later.\n\r", 0);
                        close_socket(d);
                        break;
                    }
                }
            }

	    /* Old player */
	    write_to_buffer( d, "Password: ", 0 );
	    write_to_buffer( d, echo_off_str, 0 );
	    d->connected = CON_GET_OLD_PASSWORD;
	    return;
	}
	else
	{
	    /* New player */
 	    if (newlock)
	    {
                write_to_buffer( d, "The game is newlocked.\n\r", 0 );
                close_socket( d );
                return;
            }

	    if (check_ban(d->host,BAN_NEWBIES))
	    {
		write_to_buffer(d,
		    "New players are not allowed from your site.\n\r",0);
		close_socket(d);
		return;
	    }
	
	    sprintf( buf, "So your name is %s, right (Y/N)? ", argument );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_CONFIRM_NEW_NAME;
	    return;
	}
	break;

    case CON_GET_OLD_PASSWORD:
#if defined(unix)
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd))
	{
	    write_to_buffer( d, "Wrong password.\n\r", 0 );
            log_f("%s (%d) attempted wrong password",ch->name,d->descriptor);
            bad_password(d, argument);
	    close_socket( d );
	    return;
	}
 
	write_to_buffer( d, echo_on_str, 0 );

	if (check_playing(d,ch->name))
	    return;

	ch->pcdata->socket = str_dup( d->host );
	if ( check_reconnect( d, ch->name, TRUE ) )
	    return;

	sprintf( log_buf, "%s@%s (desc %d) has connected.", ch->name, 
                 d->host, d->descriptor );
	log_string(LOG_CONNECT, log_buf );
        wiznet(log_buf,NULL,NULL,WIZ_SITES,0,UMAX(get_trust(ch),MAX_LEVEL-2));
	sprintf( log_buf, "%s has connected.", ch->name );
	wiznet_new(log_buf,NULL,NULL,WIZ_SITES,0,get_trust(ch), LEVEL_HERO);
	ch->pcdata->socket = str_dup( d->host );

	if (IS_SET(ch->act, PLR_REROLL ) )
	{
            BANK_ACCOUNT_DATA *old_bank_account;
    	    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
	    sprintf(newbuf, "%s", str_dup( ch->pcdata->pwd ));
	    sprintf( argument, "%s", capitalize( ch->name ) );

            old_bank_account = ch->pcdata->bank_account;

	    free_char( d->character );
	    d->character = NULL;
	    fOld = load_char_reroll( d, argument );
	    ch   = d->character;


	    free_string( ch->pcdata->pwd );
	    ch->pcdata->pwd	= str_dup( newbuf );
	    newbuf[0] = '\0';
	    ch->pcdata->tier = 1;

            ch->pcdata->bank_account = old_bank_account;


	    ch->pcdata->socket = str_dup( d->host );
	    write_to_buffer( d, echo_on_str, 0 );
	    write_to_buffer(d,"The following races are available:\n\r\n\r",0);
	    pos = 0;
	    racec = 0;
	    for ( race = 1; race_table[race].name != NULL; race++ )
	    {
		if (!race_table[race].pc_race)
		    break;
		racec++;
		sprintf(newbuf, "%2d) %6s%-24s", racec, " ", race_table[race].name);
		write_to_buffer(d,newbuf,0);
		pos++;
		if (pos >= 2) {
		    write_to_buffer(d,"\n\r",0);
		    pos = 0;
		}
	    }
	    newbuf[0] = '\0';
	    write_to_buffer(d,"\n\r\n\r",0);
	    write_to_buffer(d,"What is your race (help <race> for more information)? ",0);
	    d->connected = CON_GET_NEW_RACE;
	    break;
	}

	if ( IS_IMMORTAL(ch) )
	{
	    do_help( ch, "imotd" );
	    d->connected = CON_READ_IMOTD;
 	}
	else
	{
	    do_help( ch, "motd" );
	    d->connected = CON_READ_MOTD;
	}

	write_to_buffer(d,"\n\rAttempting to determine client options ... \n\r",0);
        
	write_to_buffer(d,ask_client_type,0);
	write_to_buffer(d,ask_client_next,0);

	write_to_buffer(d,compress2_will,0);

        if(check_ban(d->host,BAN_MULTI))
        {
            DESCRIPTOR_DATA *vd, *vd_next;

	    for ( vd = descriptor_list; vd != NULL; vd = vd_next )
	    {
	        vd_next = vd->next;

                if(!str_cmp(d->host, vd->host) && d != vd)
                {
                    write_to_desc(vd,
                        "Another player from your IP logged on.\n\r", 0);

                    if(vd->character && !IS_IMMORTAL(vd->character))
                        force_quit(vd->character, "");
                }
            }
        }

	for (pos = 0; pos < MAX_DUPES; pos++)
	{
	    if (ch->pcdata->dupes[pos] == NULL)
		break;

/*	    if ( ( victim = get_char_mortal( NULL, str_dup(ch->pcdata->dupes[pos]) ) ) != NULL )
		force_quit(victim, "");*/
	    if ( ( victim = get_char_mortal( NULL, ch->pcdata->dupes[pos] ) ) != NULL )
		force_quit(victim, "");
	}

	break;

/* RT code for breaking link */
 
    case CON_BREAK_CONNECT:
	switch( *argument )
	{
	case 'y' : case 'Y':
            for ( d_old = descriptor_list; d_old != NULL; d_old = d_next )
	    {
		d_next = d_old->next;
		if (d_old == d || d_old->character == NULL)
		    continue;

		if (str_cmp(ch->name,d_old->original ?
		    d_old->original->name : d_old->character->name))
		    continue;

		close_socket(d_old);
	    }
	    ch->pcdata->socket = str_dup( d->host );
	    if (check_reconnect(d,ch->name,TRUE))
	    	return;
	    write_to_buffer(d,"Reconnect attempt failed.\n\rName: ",0);
            if ( d->character != NULL )
            {
                free_char( d->character );
                d->character = NULL;
            }
	    d->connected = CON_GET_NAME;
	    break;

	case 'n' : case 'N':
	    write_to_buffer(d,"Name: ",0);
            if ( d->character != NULL )
            {
                free_char( d->character );
                d->character = NULL;
            }
	    d->connected = CON_GET_NAME;
	    break;

	default:
	    write_to_buffer(d,"Please type Y or N? ",0);
	    break;
	}
	break;

    case CON_CONFIRM_NEW_NAME:
	switch ( *argument )
	{
	case 'y': case 'Y':
	    sprintf( buf, "New character.\n\rGive me a password for %s: %s",
		ch->name, echo_off_str );
	    ch->pcdata->socket = str_dup( d->host );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_GET_NEW_PASSWORD;
	    break;

	case 'n': case 'N':
	    write_to_buffer( d, "Ok, what IS it, then? ", 0 );
	    free_char( d->character );
	    d->character = NULL;
	    d->connected = CON_GET_NAME;
	    break;

	default:
	    write_to_buffer( d, "Please type Yes or No? ", 0 );
	    break;
	}
	break;

    case CON_GET_NEW_PASSWORD:
#if defined(unix)
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strlen(argument) < 5 )
	{
	    write_to_buffer( d,
		"Password must be at least five characters long.\n\rPassword: ",
		0 );
	    return;
	}

	pwdnew = crypt( argument, ch->name );
	for ( p = pwdnew; *p != '\0'; p++ )
	{
	    if ( *p == '~' )
	    {
		write_to_buffer( d,
		    "New password not acceptable, try again.\n\rPassword: ",
		    0 );
		return;
	    }
	}

	free_string( ch->pcdata->pwd );
	ch->pcdata->pwd	= str_dup( pwdnew );
	write_to_buffer( d, "Please retype password: ", 0 );
	d->connected = CON_CONFIRM_NEW_PASSWORD;
	break;

    case CON_CONFIRM_NEW_PASSWORD:
#if defined(unix)
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
	    write_to_buffer( d, "Passwords don't match.\n\rRetype password: ",
		0 );
	    d->connected = CON_GET_NEW_PASSWORD;
	    return;
	}

	ch->pcdata->socket = str_dup( d->host );
	write_to_buffer( d, echo_on_str, 0 );
	write_to_buffer(d,"The following races are available:\n\r\n\r",0);
	pos = 0;
	racec = 0;
	for ( race = 1; race_table[race].name != NULL; race++ )
	{
	    if (!race_table[race].pc_race)
		break;
            racec++;
	    sprintf(newbuf, "%2d) %6s%-24s", racec, " ", race_table[race].name);
	    write_to_buffer(d,newbuf,0);
	    pos++; 
	    if (pos >= 2) { 
		write_to_buffer(d,"\n\r",0);
		pos = 0;
	    } 
	}
	newbuf[0] = '\0';
	write_to_buffer(d,"\n\r\n\r",0);
	write_to_buffer(d,"What is your race (help for more information)? ",0);
	d->connected = CON_GET_NEW_RACE;
	break;

    case CON_GET_NEW_RACE:
	one_argument(argument,arg);


	if (!strcmp(arg,"help"))
	{
	    argument = one_argument(argument,arg);
	    if (argument[0] == '\0')
		do_help(ch,"race help");
	    else
		do_help(ch,argument);
            write_to_buffer(d,
		"What is your race (help <race> for more information)? ",0);
	    break;
  	}

        if( is_number(argument) )
	    race = race_pc_lookup(atoi(argument));
        else
	    race = race_lookup(argument);

	if (race == 0 || !race_table[race].pc_race)
	{
	    write_to_buffer(d,"That is not a valid race.\n\r",0);
            write_to_buffer(d,"The following races are available:\n\r",0);
	    pos = 0;
	    racec = 0;
            for ( race = 1; race_table[race].name != NULL; race++ )
            {
            	if (!race_table[race].pc_race)
                    break;
		sprintf(newbuf, "%2d) %6s%-24s", race, " ", race_table[race].name);
		write_to_buffer(d,newbuf,0);
		pos++;
		if (pos >= 2)
		{
		    write_to_buffer(d,"\n\r",0);
		    pos = 0;
		}
	    }
            write_to_buffer(d,"\n\r",0);
            write_to_buffer(d,
		"What is your race? (help <race> for more information) ",0);
	    break;
	}

        ch->race = race;
	/* initialize stats */
	for (i = 0; i < MAX_STATS; i++)
	    ch->perm_stat[i] = pc_race_table[race].stats[i];
	ch->affected_by = ch->affected_by|race_table[race].aff;
	ch->shielded_by = ch->shielded_by|race_table[race].shd;
	ch->imm_flags	= ch->imm_flags|race_table[race].imm;
	ch->res_flags	= ch->res_flags|race_table[race].res;
	ch->vuln_flags	= ch->vuln_flags|race_table[race].vuln;
	ch->form	= race_table[race].form;
	ch->parts	= race_table[race].parts;

	/* add skills */
	for (i = 0; i < 5; i++)
	{
	    if (pc_race_table[race].skills[i] == NULL)
	 	break;
	    group_add(ch,pc_race_table[race].skills[i],FALSE);
	}

	for (i = 0; i < MAX_LANGUAGE; i++)
	{
	    if (pc_race_table[race].lang[i] < 0)
	 	break;

            if (IS_SET(pc_race_table[race].lang[i][1],KSKILL_INITIAL))
            {
                ch->language_known[(pc_race_table[race].lang[i][0])] =
			pc_race_table[race].lang[i][2];
            }
	}

	/* add cost */
	ch->pcdata->points = pc_race_table[race].points;
	ch->size = pc_race_table[race].size;

        write_to_buffer( d, "What is your sex (M/F/N)? ", 0 );
        d->connected = CON_GET_NEW_SEX;
        break;
        

    case CON_GET_NEW_SEX:
	switch ( argument[0] )
	{
	case 'm': case 'M': ch->sex = SEX_MALE;    
			    ch->pcdata->true_sex = SEX_MALE;
			    break;
	case 'f': case 'F': ch->sex = SEX_FEMALE; 
			    ch->pcdata->true_sex = SEX_FEMALE;
			    break;
	case 'n': case 'N': ch->sex = SEX_NEUTRAL;
			    ch->pcdata->true_sex = SEX_NEUTRAL;
			    break;
	default:
	    write_to_buffer( d, "That's not a sex.\n\rWhat IS your sex? ", 0 );
	    return;
	}

	write_to_buffer( d, echo_on_str, 0 );
	write_to_buffer(d,"The following classes are available:\n\r\n\r",0);
	if (ch->pcdata->tier != 1) {
	    ch->pcdata->tier = 0;
	    for ( iClass = 0; iClass < MAX_CLASS/2; iClass++ )
	    {
		write_to_buffer(d,"      ",0);
		write_to_buffer(d,class_table[iClass].name,0);
		write_to_buffer(d,"\n\r",1);
	    }
	} else {
	    for ( iClass = MAX_CLASS/2; iClass < MAX_CLASS; iClass++ )
	    {
		write_to_buffer(d,"      ",0);
		write_to_buffer(d,class_table[iClass].name,0);
		write_to_buffer(d,"\n\r",1);
	    }
	}
	write_to_buffer(d,"\n\r\n\r",0);
	write_to_buffer(d,"What is your class ? ",0);
	d->connected = CON_GET_NEW_CLASS;
	break;

    case CON_GET_NEW_CLASS:
	iClass = class_lookup(argument);

	if ( iClass == -1 )
	{
	    write_to_buffer( d,
		"That's not a class.\n\rWhat IS your class? ", 0 );
	    return;
	}
	if (ch->pcdata->tier != 1) {
	    if (iClass >= (MAX_CLASS/2)) {
		write_to_buffer( d,
		    "That's not a first tier class.\n\rWhat IS your class? ", 0 );
		return;
	    }
	} else {
	    if (iClass < (MAX_CLASS/2)) {
		write_to_buffer( d,
		    "That's not a second tier class.\n\rWhat IS your class? ", 0 );
		return;
	    }
	}
        ch->class = iClass;

	sprintf( log_buf, "%s@%s (desc %d) new player.", ch->name, d->host,
                                                    d->descriptor );
	log_string(LOG_CONNECT, log_buf );
	wiznet("Newbie alert!  $N sighted.",ch,NULL,WIZ_NEWBIE,0,0);
	wiznet(log_buf,NULL,NULL,WIZ_SITES,0,UMAX(get_trust(ch),MAX_LEVEL));

	write_to_buffer( d, "\n\r", 2 );
	write_to_buffer( d, "You may be good, neutral, or evil.\n\r",0);
	write_to_buffer( d, "Which alignment (G/N/E)? ",0);
	d->connected = CON_GET_ALIGNMENT;
	break;

case CON_GET_ALIGNMENT:
	switch( argument[0])
	{
	    case 'g' : case 'G' : ch->alignment = 750;  break;
	    case 'n' : case 'N' : ch->alignment = 0;	break;
	    case 'e' : case 'E' : ch->alignment = -750; break;
	    default:
		write_to_buffer(d,"That's not a valid alignment.\n\r",0);
		write_to_buffer(d,"Which alignment (G/N/E)? ",0);
		return;
	}

	write_to_buffer(d,"\n\r",0);

        group_add(ch,"rom basics",FALSE);
        group_add(ch,class_table[ch->class].base_group,FALSE);
        ch->pcdata->learned[gsn_recall] = 50;
	write_to_buffer(d,"Do you wish to customize this character?\n\r",0);
	write_to_buffer(d,"Customization takes time, but allows a wider range of skills and abilities.\n\r",0);
	write_to_buffer(d,"Customize (Y/N)? ",0);
	d->connected = CON_DEFAULT_CHOICE;
	break;

case CON_DEFAULT_CHOICE:
	write_to_buffer(d,"\n\r",2);
        switch ( argument[0] )
        {
        case 'y': case 'Y': 
	    ch->gen_data = new_gen_data();
	    ch->gen_data->points_chosen = ch->pcdata->points;
	    do_help(ch,"group header");
	    list_group_costs(ch);
	    write_to_buffer(d,"You already have the following skills:\n\r",0);
	    do_skills(ch,"");
	    do_help(ch,"menu choice");
	    d->connected = CON_GEN_GROUPS;
	    break;
        case 'n': case 'N': 
	    group_add(ch,class_table[ch->class].default_group,TRUE);
            write_to_buffer( d, "\n\r", 2 );
	    write_to_buffer(d,
		"Please pick a weapon from the following choices:\n\r",0);
	    buf[0] = '\0';
	    for ( i = 0; weapon_table[i].name != NULL; i++)
		if (ch->pcdata->learned[*weapon_table[i].gsn] > 0)
		{
		    strcat(buf,weapon_table[i].name);
		    strcat(buf," ");
		}
	    strcat(buf,"\n\rYour choice? ");
	    write_to_buffer(d,buf,0);
            d->connected = CON_PICK_WEAPON;
            break;
        default:
            write_to_buffer( d, "Please answer (Y/N)? ", 0 );
            return;
        }
	break;

    case CON_PICK_WEAPON:
	write_to_buffer(d,"\n\r",2);
	weapon = weapon_lookup(argument);
	if (weapon == -1 || ch->pcdata->learned[*weapon_table[weapon].gsn] <= 0)
	{
	    write_to_buffer(d,
		"That's not a valid selection. Choices are:\n\r",0);
            buf[0] = '\0';
            for ( i = 0; weapon_table[i].name != NULL; i++)
                if (ch->pcdata->learned[*weapon_table[i].gsn] > 0)
                {
                    strcat(buf,weapon_table[i].name);
		    strcat(buf," ");
                }
            strcat(buf,"\n\rYour choice? ");
            write_to_buffer(d,buf,0);
	    return;
	}

	ch->pcdata->learned[*weapon_table[weapon].gsn] = 40;
	write_to_buffer(d,"\n\r",2);
	do_help(ch,"motd");
	d->connected = CON_READ_MOTD;
	break;

    case CON_GEN_GROUPS:
	send_to_char("\n\r",ch);
       	if (!str_cmp(argument,"done"))
       	{
	    sprintf(buf,"Creation points: %d\n\r",ch->pcdata->points);
	    send_to_char(buf,ch);
	    sprintf(buf,"Experience per level: %ld\n\r",
	            (long)exp_per_level(ch,ch->gen_data->points_chosen));
	    if (ch->pcdata->points < 40)
		ch->train = (40 - ch->pcdata->points + 1) / 2;
	    free_gen_data(ch->gen_data);
	    ch->gen_data = NULL;
	    send_to_char(buf,ch);
            write_to_buffer( d, "\n\r", 2 );
            write_to_buffer(d,
                "Please pick a weapon from the following choices:\n\r",0);
            buf[0] = '\0';
            for ( i = 0; weapon_table[i].name != NULL; i++)
                if (ch->pcdata->learned[*weapon_table[i].gsn] > 0)
                {
                    strcat(buf,weapon_table[i].name);
		    strcat(buf," ");
                }
            strcat(buf,"\n\rYour choice? ");
            write_to_buffer(d,buf,0);
            d->connected = CON_PICK_WEAPON;
            break;
        }

        if (!parse_gen_groups(ch,argument))
        send_to_char(
        "Choices are: list,learned,premise,add,drop,info,help, and done.\n\r"
        ,ch);

        do_help(ch,"menu choice");
        break;

    case CON_READ_IMOTD:

	write_to_buffer(d,"\n\r",2);
        do_help( ch, "motd" );
        d->connected = CON_READ_MOTD;
	break;

    case CON_INFO:
        break;

        /* states for new note system, (c)1995-96 erwin@pip.dknet.dk */
        /* ch MUST be PC here; have nwrite check for PC status! */

    case CON_NOTE_TO:
        handle_con_note_to (d, argument);
        break;

    case CON_NOTE_SUBJECT:
        handle_con_note_subject (d, argument);
        break; /* subject */

    case CON_NOTE_EXPIRE:
        handle_con_note_expire (d, argument);
        break;

    case CON_NOTE_TEXT:
        handle_con_note_text (d, argument);
        break;

    case CON_NOTE_FINISH:
        handle_con_note_finish (d, argument);
        break;

    case CON_READ_MOTD:
        if ( ch->pcdata == NULL || ch->pcdata->pwd[0] == '\0')
        {
            write_to_buffer( d, "Warning! Null password!\n\r",0 );
            write_to_buffer( d, "Please report old password with bug.\n\r",0);
            write_to_buffer( d,
                "Type 'password null <new password>' to fix.\n\r",0);
        }

        if(ch->level > 0)
        {
            sprintf(buf, "cp ../player/%s ../bkp/autobackup/player1/", 
                capitalize(d->character->name));
            system(buf);
        }


	write_to_buffer( d, 
    "\n\rWelcome to ROT 1.4.  Please do not feed the immortals.\n\r",
	    0 );
	ch->next	= char_list;
	char_list	= ch;
	ch->plr_next	= plr_list;
	plr_list	= ch;
	d->connected	= CON_PLAYING;
	reset_char(ch);

	if ( ch->level == 0 )
	{
	    ch->perm_stat[class_table[ch->class].attr_prime] += 3;

	    ch->level	= 1;
	    ch->exp	= exp_per_level(ch,ch->pcdata->points);
	    ch->hit	= ch->max_hit;
	    ch->mana	= ch->max_mana;
	    ch->move	= ch->max_move;
	    ch->train	 = 20;
	    ch->practice = 25;
	    sprintf( buf, "the %s",
		title_table [ch->class] [ch->level]
		[ch->sex == SEX_FEMALE ? 1 : 0] );
	    set_title( ch, buf );

            SET_BIT( ch->act, PLR_COLOUR );

	    do_outfit(ch,"");
	    obj_to_char(create_object(get_obj_index(OBJ_VNUM_MAP),0),ch);
	    obj_to_char(create_object(get_obj_index(OBJ_VNUM_WMAP),0),ch);
	    obj_to_char(create_object(get_obj_index(OBJ_VNUM_EMAP),0),ch);

	    obj_to_char(create_pack(),ch);

	    char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
	    send_to_char("\n\r",ch);
	    do_help(ch,"NEWBIE INFO");
	    send_to_char("\n\r",ch);
	}
	else if ( ch->in_room != NULL )
	{
	    char_to_room( ch, ch->in_room );
	}
	else if ( IS_IMMORTAL(ch) )
	{
	    char_to_room( ch, get_room_index( ROOM_VNUM_CHAT ) );
	}
	else
	{
	    if ( ch->alignment <0 )
		char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLEB ) );
	    else
		char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
	}

	act_new_2( "$n has entered the game.", ch, NULL, NULL, TO_ROOM, POS_RESTING, 0, ACTION_MUST_SEE );
	do_look( ch, "auto" );

	wiznet("$N has left real life behind.",ch,NULL,
		WIZ_LOGINS,WIZ_SITES,get_trust(ch));

	if (ch->pet != NULL)
	{
	    char_to_room(ch->pet,ch->in_room);
	    act("$n has entered the game.",ch->pet,NULL,NULL,TO_ROOM);
	}

	send_to_char("\n\r", ch);
	do_board(ch, "");  /* Show board status */

        if(!str_suffix(d->host,".mudconnect.com")
	|| !strcmp(d->host,"207.34.68.236"))
	    send_to_char("\n\r{RMUDConnector users{D:{G\n\r"
	"Please only connect from TMC if you are previewing this MUD.\n\r"
	"If you plan to stay, connect from your own computer or"
	"speak with the immortals.\n\r"
        "Failure to comply may result in {Ydeletion{G.\n\r"
        "If you MUST use a Java-based client, go to"
        "{Whttp://bota.betterbox.net/telnet{x\n\r\n\r", ch);
 
	break;
    }

    return;
}

bool check_valid_name( char *name )
{
    /*
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
	char *pc;
	bool fIll,adjcaps = FALSE,cleancaps = FALSE;
 	int total_caps = 0;

	fIll = TRUE;
	for ( pc = name; *pc != '\0'; pc++ )
	{
	    if ( !isalpha(*pc) )
		return FALSE;

	    if ( isupper(*pc)) /* ugly anti-caps hack */
	    {
		if (adjcaps)
		    cleancaps = TRUE;
		total_caps++;
		adjcaps = TRUE;
	    }
	    else
		adjcaps = FALSE;

	    if ( LOWER(*pc) != 'i' && LOWER(*pc) != 'l' )
		fIll = FALSE;
	}

	if ( fIll )
	    return FALSE;

	if (cleancaps || (total_caps > (strlen(name)) / 2 && strlen(name) < 3))
	    return FALSE;
    }

    return TRUE;
}

/*
 * Parse a name for acceptability.
 */
bool check_parse_name( char *name )
{
    char strsave[MAX_INPUT_LENGTH];
//    DESCRIPTOR_DATA *d;

    /*
     * Reserved words.
     */
    if ( is_name( name, 
	"all auto immortal immortals self someone something the you " 
        "demise balance circle loner honor none "
        "everything everyone anyone anybody anything somewhere "
        "god gods nothing nobody everybody somebody and for of "
        "north east south west door ooc imm backstab death "
        "quest questmaster questor admin administrator grenier ruler "
        "jason albert jasonlai albertwang lai wang quit save michaelwu anderthan"
        "thoth belan odin ") )
	return FALSE;

    /*
     * Cursing
     */

    if (!str_prefix(name,"platinum"))
        return FALSE;
    if (!str_prefix(name,"gold"))
        return FALSE;
    if (!str_prefix(name,"silver"))
        return FALSE;
    if (!str_prefix(name,"coin"))
        return FALSE;
    if (!str_prefix(name,"cubic"))
        return FALSE;


    if (!str_infix("immortal",name))
	return FALSE;
    if (!str_infix(" ", name))
	return FALSE;
    if (!str_infix("fuck",name))
	return FALSE;
    if (!str_infix("shit",name))
        return FALSE;
    if (!str_infix("asshole",name))
        return FALSE;
    if (!str_infix("pussy",name))
        return FALSE;
    /*
     * Length restrictions.
     */
     
    if ( strlen(name) <  2 )
	return FALSE;

#if defined(MSDOS)
    if ( strlen(name) >  8 )
	return FALSE;
#endif

#if defined(macintosh) || defined(unix)
    if ( strlen(name) > 12 )
	return FALSE;
#endif

    /*
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
	char *pc;
	bool fIll,adjcaps = FALSE,cleancaps = FALSE;
 	int total_caps = 0;

	fIll = TRUE;
	for ( pc = name; *pc != '\0'; pc++ )
	{
	    if ( !isalpha(*pc) )
		return FALSE;

	    if ( isupper(*pc)) /* ugly anti-caps hack */
	    {
		if (adjcaps)
		    cleancaps = TRUE;
		total_caps++;
		adjcaps = TRUE;
	    }
	    else
		adjcaps = FALSE;

	    if ( LOWER(*pc) != 'i' && LOWER(*pc) != 'l' )
		fIll = FALSE;
	}

	if ( fIll )
	    return FALSE;

	if (cleancaps || (total_caps > (strlen(name)) / 2 && strlen(name) < 3))
	    return FALSE;
    }
/*
    for (d = descriptor_list; d != NULL; d = d->next)
    {
        if(d->connected != CON_PLAYING
           && d->connected != CON_NOTE_TO
           && d->connected != CON_NOTE_TEXT
           && d->connected != CON_NOTE_EXPIRE
           && d->connected != CON_NOTE_SUBJECT
           && d->character
           && d->character->name[0]
           && !str_cmp(d->character->name,name))
            return FALSE;
    }
*/

    /*
     * Prevent players from naming themselves after mobs.
     */

    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( name ) );

    if ( !file_exists(strsave) )
    {
	extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
	MOB_INDEX_DATA *pMobIndex;
	int iHash;

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
	{
	    for ( pMobIndex  = mob_index_hash[iHash];
		  pMobIndex != NULL;
		  pMobIndex  = pMobIndex->next )
	    {
		if ( is_name( name, pMobIndex->player_name ) )
		    return FALSE;
	    }
	}

	if(clan_lookup(name))	
		return FALSE;
    }


    return TRUE;
}



/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect( DESCRIPTOR_DATA *d, char *name, bool fConn )
{
    CHAR_DATA *ch;
    char buf[MAX_STRING_LENGTH];

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
	if ( !IS_NPC(ch)
	&&   (!fConn || ch->desc == NULL)
	&&   !str_cmp( d->character->name, ch->name ) )
	{
	    if ( fConn == FALSE )
	    {
		free_string( d->character->pcdata->pwd );
		d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
	    }
	    else
	    {
		OBJ_DATA *obj;

		free_char( d->character );
		d->character = ch;
		ch->desc	 = d;
		ch->timer	 = 0;
		if(ch->tells)
		{
		    sprintf( buf, "Reconnecting.  You have {R%d{x tells waiting.\n\r",
			ch->tells );
		    send_to_char( buf, ch );
		    send_to_char("Type 'replay' to see tells.\n\r",ch);
		}
		else
		{
			send_to_char("Reconnecting.  You have no tells waiting.\n\r",ch);
		}
		act_new_2( "$n has reconnected.", ch, NULL, NULL, TO_ROOM, 
                          POS_RESTING, 0, ACTION_MUST_SEE );
		if ((obj = get_eq_char(ch,WEAR_LIGHT)) != NULL
		&&  obj->item_type == ITEM_LIGHT && obj->value[2] != 0)
		    --ch->in_room->light;

		sprintf( log_buf, "%s@%s reconnected.", ch->name, d->host );
		log_string(LOG_CONNECT, log_buf );
		wiznet("$N groks the fullness of $S link.",
		    ch,NULL,WIZ_LINKS,0,0);
		d->connected = CON_PLAYING;
		/* Inform the character of a note in progress and the possbility
		 * of continuation!
		 */
		if (ch->pcdata->in_progress)
		    send_to_char ("You have a note in progress. Type NWRITE to continue it.\n\r", ch);
	    }
	    return TRUE;
	}
    }

    return FALSE;
}



/*
 * Check if already playing.
 */
bool check_playing( DESCRIPTOR_DATA *d, char *name )
{
    DESCRIPTOR_DATA *dold;

    for ( dold = descriptor_list; dold; dold = dold->next )
    {
	if ( dold != d
	&&   dold->character != NULL
	&&   dold->connected != CON_GET_NAME
	&&   dold->connected != CON_GET_OLD_PASSWORD
	&&   !str_cmp( name, dold->original
	         ? dold->original->name : dold->character->name ) )
	{
	    write_to_buffer( d, "That character is already playing.\n\r",0);
	    write_to_buffer( d, "Do you wish to connect anyway (Y/N)? ",0);
	    d->connected = CON_BREAK_CONNECT;
	    return TRUE;
	}
    }

    return FALSE;
}



void stop_idling( CHAR_DATA *ch )
{
    if ( ch == NULL
    ||   ch->desc == NULL
    ||   ch->desc->connected != CON_PLAYING
    ||   ch->was_in_room == NULL 
    ||   ch->in_room != get_room_index(ROOM_VNUM_LIMBO))
	return;

    ch->timer = 0;
    char_from_room( ch );
    char_to_room( ch, ch->was_in_room );
    ch->was_in_room	= NULL;
    act( "$n has returned from the void.", ch, NULL, NULL, TO_ROOM );
    return;
}



/*
 * Write to one char.
 */
void send_to_char_bw( const char *txt, CHAR_DATA *ch )
{
    if ( txt != NULL && ch->desc != NULL )
        write_to_buffer( ch->desc, txt, strlen(txt) );
    return;
}

/*
 * Write to one char, new colour version, by Lope.
 */
void send_to_char( const char *txt, CHAR_DATA *ch )
{
    const	char 	*point;
    		char 	*point2;
    		char 	buf[ MAX_STRING_LENGTH*4 ];

    buf[0] = '\0';
    point2 = buf;

    if(!ch)
    {
        log_f("[BUG!] send_to_char: null ch: \"%10.10s\";", txt);
    }

    if( txt && ch->desc )
	{
	    if( IS_SET( ch->act, PLR_COLOUR ) )
	    {
		for( point = txt ; *point ; point++ )
	        {
		    if( *point == '{' )
		    {
			point++;
			strcat( buf, colour( *point, ch ) );
			for( point2 = buf ; *point2 ; point2++ )
			    ;
			continue;
		    }
		    *point2 = *point;
		    *++point2 = '\0';
		}			
		*point2 = '\0';
        	write_to_buffer( ch->desc, buf, point2 - buf );
	    }
	    else
	    {
		for( point = txt ; *point ; point++ )
	        {
		    if( *point == '{' )
		    {
			point++;
			if( *point != '{' )
			{
			    continue;
			}
		    }
		    *point2 = *point;
		    *++point2 = '\0';
		}
		*point2 = '\0';
        	write_to_buffer( ch->desc, buf, point2 - buf );
	    }
	}
    return;
}

/*
 * Send a page to one char.
 */
void page_to_char_bw( const char *txt, CHAR_DATA *ch )
{
    if ( txt == NULL || ch->desc == NULL)


    if (ch->lines == 0 )
    {
	send_to_char(txt,ch);
	return;
    }
	
#if defined(macintosh)
	send_to_char(txt,ch);
#else
    ch->desc->showstr_head = alloc_mem(strlen(txt) + 1);
    strcpy(ch->desc->showstr_head,txt);
    ch->desc->showstr_point = ch->desc->showstr_head;
    show_string(ch->desc,"");
#endif
}

/*
 * Page to one char, new colour version, by Lope.
 */
void page_to_char( const char *txt, CHAR_DATA *ch )
{
    const	char	*point;
    		char	*point2;
    		char	buf[ MAX_STRING_LENGTH * 4 ];

    buf[0] = '\0';
    point2 = buf;
    if( txt && ch->desc )
	{
	    if( IS_SET( ch->act, PLR_COLOUR ) )
	    {
		for( point = txt ; *point ; point++ )
	        {
		    if( *point == '{' )
		    {
			point++;
			strcat( buf, colour( *point, ch ) );
			for( point2 = buf ; *point2 ; point2++ )
			    ;
			continue;
		    }
		    *point2 = *point;
		    *++point2 = '\0';
		}			
		*point2 = '\0';
		free_string( ch->desc->showstr_head );
		ch->desc->showstr_head  = str_dup( buf );
		ch->desc->showstr_point = ch->desc->showstr_head;
		show_string( ch->desc, "" );
	    }
	    else
	    {
		for( point = txt ; *point ; point++ )
	        {
		    if( *point == '{' )
		    {
			point++;
			if( *point != '{' )
			{
			    continue;
			}
		    }
		    *point2 = *point;
		    *++point2 = '\0';
		}
		*point2 = '\0';
		free_string( ch->desc->showstr_head );
		ch->desc->showstr_head  = str_dup( buf );
		ch->desc->showstr_point = ch->desc->showstr_head;
		show_string( ch->desc, "" );
	    }
	}
    return;
}

/* string pager */
void show_string(struct descriptor_data *d, char *input)
{
    char buffer[4*MAX_STRING_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    register char *scan, *chk;
    int lines = 0, toggle = 1;
    int show_lines;

    one_argument(input,buf);
    if (buf[0] != '\0')
    {
	if (d->showstr_head)
	{
	    free_string(d->showstr_head);
	    d->showstr_head = 0;
	}
    	d->showstr_point  = 0;
	return;
    }

    if (d->character)
	show_lines = d->character->lines;
    else
	show_lines = 0;

    for (scan = buffer; ; scan++, d->showstr_point++)
    {
	if (((*scan = *d->showstr_point) == '\n' || *scan == '\r')
	    && (toggle = -toggle) < 0)
	    lines++;

	else if (!*scan || (show_lines > 0 && lines >= show_lines))
	{
	    *scan = '\0';
	    write_to_buffer(d,buffer,strlen(buffer));
	    for (chk = d->showstr_point; isspace(*chk); chk++);
	    {
		if (!*chk)
		{
		    if (d->showstr_head)
        	    {
            		free_string(d->showstr_head);
            		d->showstr_head = 0;
        	    }
        	    d->showstr_point  = 0;
    		}
	    }
	    return;
	}
    }
    return;
}
	

/* quick sex fixer */
void fix_sex(CHAR_DATA *ch)
{
    if (ch->sex < 0 || ch->sex > 2)
    	ch->sex = IS_NPC(ch) ? 0 : ch->pcdata->true_sex;
}

void act (const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2,
	  int type)
{
    /* to be compatible with older code */
    act_new(format,ch,arg1,arg2,type,POS_RESTING);
}

void act2 (const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2,
	  int type, long bits)
{
    /* to be compatible with older code */
    act_new_2(format,ch,arg1,arg2,type,POS_RESTING, 0, bits);
}

void act_new( const char *format, CHAR_DATA *ch, const void *arg1, 
	      const void *arg2, int type, int min_pos )
{
    act_new_2( format, ch, arg1, arg2, type, min_pos, 1, 0 );
}


/*
 * The colour version of the act( ) function, -Lope
 */
void act_new_2( const char *format, CHAR_DATA *ch, const void *arg1, 
	      const void *arg2, int type, int min_pos, int min_level, long bits )

{
    static char * const he_she  [] = { "it",  "he",  "she" };
    static char * const him_her [] = { "it",  "him", "her" };
    static char * const his_her [] = { "its", "his", "her" };
 
    CHAR_DATA 		*to;
    CHAR_DATA 		*vch = ( CHAR_DATA * ) arg2;
    OBJ_DATA 		*obj1 = ( OBJ_DATA  * ) arg1;
    OBJ_DATA 		*obj2 = ( OBJ_DATA  * ) arg2;
    const 	char 	*str;
    char 		*i;
    char 		*point;
    char 		*i2;
    char 		fixed[ MAX_STRING_LENGTH ];
    char 		buf[ MAX_STRING_LENGTH   ];
    char 		fname[ MAX_INPUT_LENGTH  ];
    bool		fColour = FALSE;
    bool 		global = FALSE;

    colour_buffer = 'x';
    last_colour = 'x';

    if( !format || !*format )
        return;

    if( !ch || !ch->in_room )
	return;

    if(IS_SET(bits, ACTION_GLOBAL)) global = TRUE;

    to = ch->in_room->people;
    if( type == TO_VICT )
    {
        if ( !vch )
        {
            bug( "Act: null vch with TO_VICT.", 0 );
            return;
        }

	if ( !vch->in_room )
	    return;

        to = vch->in_room->people;
    }
 
    for( ; to ; to = to->next_in_room )
    {
        if ( (!IS_NPC(to) && to->desc == NULL )
        ||   ( IS_NPC(to) && !HAS_TRIGGER(to, TRIG_ACT) && !to->desc )
        ||    to->position < min_pos )
            continue;
 
        if( type == TO_CHAR && to != ch )
            continue;
        if( type == TO_VICT && ( to != vch || to == ch ) )
            continue;
        if( type == TO_ROOM && to == ch )
            continue;
        if( type == TO_NOTVICT && ( to == ch || to == vch ) )
            continue;

	if( get_trust(to) < min_level ) continue;
 
        if( IS_SET(bits, ACTION_MUST_SEE) )
        {
            if(ch && (!can_see(to,ch) ||
               (!IS_SET(bits, ACTION_GLOBAL)
             && ch->ghost_level > get_trust(ch)) ) )
                continue;
        }

        if( IS_SET(bits, ACTION_BATTLE_NODAM)
         && IS_SET(to->comm2, COMM_NOBATTLESPAM) )
        {
            continue;
        }

        point   = buf;
        str     = format;
        while( *str )
        {
            if( *str != '$' && *str != '{' )
            {
                *point++ = *str++;
                continue;
            }

	    i = NULL;
	    switch( *str )
	    {
		case '$':
		    fColour = TRUE;
		    ++str;
		    i = " <@@@> ";
		    if ( !arg2 && *str >= 'A' && *str <= 'Z' && *str != 'G' )
		    {
			bug( "Act: missing arg2 for code %d.", *str );
			i = " <@@@> ";
		    }
		    else
		    {
			switch ( *str )
			{
			    default:  
				bug( "Act: bad code %d.", *str );
				i = " <@@@> ";                                
				break;

                            case '$': i = "$"; break;

			    case 't': 
				i = (char *) arg1;                            
				break;

			    case 'T': 
				i = (char *) arg2;                            
				break;

			    case 'n': 
				i = global ? GPERS(ch,to) : PERS( ch, to );
				break;

			    case 'N': 
				i = global ? GPERS(vch,to) : PERS( vch, to );
				break;

			    case 'e': 
				i = he_she  [URANGE(0, ch  ->sex, 2)];
				break;

			    case 'E': 
				i = he_she  [URANGE(0, vch ->sex, 2)];
				break;

			    case 'm': 
				i = him_her [URANGE(0, ch  ->sex, 2)];
				break;

			    case 'M': 
				i = him_her [URANGE(0, vch ->sex, 2)];        
				break;

			    case 's': 
				i = his_her [URANGE(0, ch  ->sex, 2)];
				break;

			    case 'S': 
				i = his_her [URANGE(0, vch ->sex, 2)];
				break;
 
			    case 'p':
				i = can_see_obj( to, obj1 )
				  ? obj1->short_descr
				  : "something";
				break;
 
			    case 'P':
				i = can_see_obj( to, obj2 )
				  ? obj2->short_descr
				  : "something";
				break;
 
			    case 'd':
				if ( !arg2 || ((char *) arg2)[0] == '\0' )
				{
				    i = "door";
				}
				else
				{
				    one_argument( (char *) arg2, fname );
				    i = fname;
				}
				break;

			    case 'G':
				if ( ch->alignment < 0 )
				{
				    i = "Belan";
				}
				else
				{
				    i = "Thoth";
				}
				break;

			}
		    }
		    break;

		case '{':
		    fColour = FALSE;
		    ++str;
		    i = NULL;
		    if( IS_SET( to->act, PLR_COLOUR ) )
		    {
			i = colour( *str, to );
		    }
		    break;

		default:
		    fColour = FALSE;
		    *point++ = *str++;
		    break;
	    }

            ++str;
	    if( fColour && i )
	    {
		fixed[0] = '\0';
		i2 = fixed;

		if( IS_SET( to->act, PLR_COLOUR ) )
		{
		    for( i2 = fixed ; *i ; i++ )
	            {
			if( *i == '{' )
			{
			    i++;
			    strcat( fixed, colour( *i, to ) );
			    for( i2 = fixed ; *i2 ; i2++ )
				;
			    continue;
			}
			*i2 = *i;
			*++i2 = '\0';
		    }			
		    *i2 = '\0';
		    i = &fixed[0];
		}
	        else
		{
		    for( i2 = fixed ; *i ; i++ )
	            {
			if( *i == '{' )
			{
			    i++;
			    if( *i != '{' )
			    {
				continue;
			    }
			}
			*i2 = *i;
			*++i2 = '\0';
		    }			
		    *i2 = '\0';
		    i = &fixed[0];
		}
	    }


	    if( i )
	    {
		while( ( *point = *i ) != '\0' )
		{
		    ++point;
		    ++i;
		}
	    }
        }
 
        *point++	= '\n';
        *point++	= '\r';
        *point		= '\0';
	buf[0]		= UPPER( buf[0] );
        if ( to->desc != NULL && to->desc->connected == CON_PLAYING )
	    write_to_buffer( to->desc, buf, point - buf );
        else
        if ( IS_NPC(to) && MOBtrigger )
            mp_act_trigger( buf, to, ch, arg1, arg2, TRIG_ACT );
    }
    return;
}


char *colour( char type, CHAR_DATA *ch )
{
    if( IS_NPC( ch ) )
	return ( "" );

    if(type == '=')
        type = colour_buffer;

    switch( type )
    {
	default:
	    sprintf(clcode, colour_clear( ch ));
	    break;
	case 'x':
	    sprintf(clcode, colour_clear( ch ));
	    break;
	case '+':
            colour_buffer = last_colour;
	    return "";
	case '=':
            break;
        case '0':
	    sprintf(clcode, colour_clear( ch ));
	    break;
	case 'z':
	    sprintf( clcode, BLINK );
	    break;
	case 'b':
	    sprintf( clcode, C_BLUE );
	    break;
        case '4':
            sprintf( clcode, C_BLUE );
            break;
	case 'c':
	    sprintf( clcode, C_CYAN );
	    break;
        case '6':
            sprintf( clcode, C_CYAN );
            break;
	case 'g':
	    sprintf( clcode, C_GREEN );
	    break;
        case '2':
            sprintf( clcode, C_GREEN );
            break;
	case 'm':
	    sprintf( clcode, C_MAGENTA );
	    break;
        case '5':
            sprintf( clcode, C_MAGENTA );
            break;
	case 'r':
	    sprintf( clcode, C_RED );
	    break;
        case '1':
            sprintf( clcode, C_RED );
            break;
	case 'w':
	    sprintf( clcode, C_WHITE );
	    break;
        case '7':
            sprintf( clcode, C_WHITE );
            break;
	case 'y':
	    sprintf( clcode, C_YELLOW );
	    break;
        case '3':
            sprintf( clcode, C_YELLOW );
            break;
	case 'B':
	    sprintf( clcode, C_B_BLUE );
	    break;
        case '$':
            sprintf( clcode, C_B_BLUE );
            break;
	case 'C':
	    sprintf( clcode, C_B_CYAN );
	    break;
        case '^':
            sprintf( clcode, C_B_CYAN );
            break;
	case 'G':
	    sprintf( clcode, C_B_GREEN );
	    break;
        case '@':
            sprintf( clcode, C_B_GREEN );
            break;
	case 'M':
	    sprintf( clcode, C_B_MAGENTA );
	    break;
        case '%':
            sprintf( clcode, C_B_MAGENTA );
            break;
	case 'R':
	    sprintf( clcode, C_B_RED );
	    break;
        case '!':
            sprintf( clcode, C_B_RED );
            break;
	case 'W':
	    sprintf( clcode, C_B_WHITE );
	    break;
        case '&':
            sprintf( clcode, C_B_WHITE );
            break;
	case 'Y':
	    sprintf( clcode, C_B_YELLOW );
	    break;
        case '#':
            sprintf( clcode, C_B_YELLOW );
            break;
	case 'D':
	    sprintf( clcode, C_D_GREY );
	    break;
        case '8':
            sprintf( clcode, C_D_GREY );
            break;
        case '*':
            sprintf( clcode, C_D_GREY );
            break;

	case 'A':	/* Auction Channel */
	    if (ch->color_auc)
	    {
		sprintf( clcode, colour_channel(ch->color_auc, ch));
	    }
	    else
	    {
		sprintf( clcode, C_B_GREEN );
	    }
	    break;
	case 'E':	/* Clan Gossip Channel */
	    if (ch->color_cgo)
	    {
		sprintf( clcode, colour_channel(ch->color_cgo, ch));
	    }
	    else
	    {
		sprintf( clcode, C_B_RED );
	    }
	    break;
	case 'F':	/* Clan Talk Channel */
	    if (ch->color_cla)
	    {
		sprintf( clcode, colour_channel(ch->color_cla, ch));
	    }
	    else
	    {
		sprintf( clcode, C_B_MAGENTA );
	    }
	    break;
	case 'H':	/* Gossip Channel */
	    if (ch->color_gos)
	    {
		sprintf( clcode, colour_channel(ch->color_gos, ch));
	    }
	    else
	    {
		sprintf( clcode, C_B_BLUE );
	    }
	    break;
	case 'J':	/* Grats Channel */
	    if (ch->color_gra)
	    {
		sprintf( clcode, colour_channel(ch->color_gra, ch));
	    }
	    else
	    {
		sprintf( clcode, C_YELLOW );
	    }
	    break;
	case 'K':	/* Group Tell Channel */
	    if (ch->color_gte)
	    {
		sprintf( clcode, colour_channel(ch->color_gte, ch));
	    }
	    else
	    {
		sprintf( clcode, C_CYAN );
	    }
	    break;
	case 'L':	/* Immortal Talk Channel */
	    if (ch->color_imm)
	    {
		sprintf( clcode, colour_channel(ch->color_imm, ch));
	    }
	    else
	    {
		sprintf( clcode, C_B_WHITE );
	    }
	    break;
	case 'N':	/* Music Channel */
	    if (ch->color_mus)
	    {
		sprintf( clcode, colour_channel(ch->color_mus, ch));
	    }
	    else
	    {
		sprintf( clcode, C_B_CYAN );
	    }
	    break;
	case 'P':	/* Question+Answer Channel */
	    if (ch->color_que)
	    {
		sprintf( clcode, colour_channel(ch->color_que, ch));
	    }
	    else
	    {
		sprintf( clcode, C_B_YELLOW );
	    }
	    break;
	case 'Q':	/* Quote Channel */
	    if (ch->color_quo)
	    {
		sprintf( clcode, colour_channel(ch->color_quo, ch));
	    }
	    else
	    {
		sprintf( clcode, C_GREEN );
	    }
	    break;
	case 'S':	/* Say Channel */
	    if (ch->color_say)
	    {
		sprintf( clcode, colour_channel(ch->color_say, ch));
	    }
	    else
	    {
		sprintf( clcode, C_MAGENTA );
	    }
	    break;
	case 'T':	/* Shout+Yell Channel */
	    if (ch->color_sho)
	    {
		sprintf( clcode, colour_channel(ch->color_sho, ch));
	    }
	    else
	    {
		sprintf( clcode, C_RED );
	    }
	    break;
	case 'U':	/* Tell+Reply Channel */
	    if (ch->color_tel)
	    {
		sprintf( clcode, colour_channel(ch->color_tel, ch));
	    }
	    else
	    {
		sprintf( clcode, C_CYAN );
	    }
	    break;
	case 'V':	/* Wiznet Messages */
	    if (ch->color_wiz)
	    {
		sprintf( clcode, colour_channel(ch->color_wiz, ch));
	    }
	    else
	    {
		sprintf( clcode, C_WHITE );
	    }
	    break;
	case 'a':	/* Mobile Talk */
	    if (ch->color_mob)
	    {
		sprintf( clcode, colour_channel(ch->color_mob, ch));
	    }
	    else
	    {
		sprintf( clcode, C_MAGENTA );
	    }
	    break;
	case 'e':	/* Room Title */
	    if (ch->color_roo)
	    {
		sprintf( clcode, colour_channel(ch->color_roo, ch));
	    }
	    else
	    {
		sprintf( clcode, C_B_BLUE );
	    }
	    break;
	case 'f':	/* Opponent Condition */
	    if (ch->color_con)
	    {
		sprintf( clcode, colour_channel(ch->color_con, ch));
	    }
	    else
	    {
		sprintf( clcode, C_B_RED );
	    }
	    break;
	case 'h':	/* Fight Actions */
	    if (ch->color_fig)
	    {
		sprintf( clcode, colour_channel(ch->color_fig, ch));
	    }
	    else
	    {
		sprintf( clcode, C_B_BLUE );
	    }
	    break;
	case 'i':	/* Opponents Fight Actions */
	    if (ch->color_opp)
	    {
		sprintf( clcode, colour_channel(ch->color_opp, ch));
	    }
	    else
	    {
		sprintf( clcode, C_CYAN );
	    }
	    break;
	case 'j':	/* Disarm Messages */
	    if (ch->color_dis)
	    {
		sprintf( clcode, colour_channel(ch->color_dis, ch));
	    }
	    else
	    {
		sprintf( clcode, C_B_YELLOW );
	    }
	    break;
	case 'k':	/* Witness Messages */
	    if (ch->color_wit)
	    {
		sprintf( clcode, colour_channel(ch->color_wit, ch));
	    }
	    else
	    {
		sprintf( clcode, colour_clear( ch ));
	    }
	    break;
	case 'l':	/* Quest Gossip */
            if (ch->color_qgo)
            {
                sprintf( clcode, colour_channel(ch->color_qgo, ch));
            }
            else
            {
                sprintf( clcode, C_B_CYAN);
            }
            break;

	case '{':
	    sprintf( clcode, "%c", '{' );
	    break;

	case '-':
	    sprintf( clcode, "%c", '~' );
	    break;
    }

    last_colour = type;
    return clcode;
}

char *colour_clear( CHAR_DATA *ch )
{

    if (ch->color)
    {
	if (ch->color == 1)
	    sprintf( clcode, R_RED );
	else if (ch->color == 2)
	    sprintf( clcode, R_GREEN );
	else if (ch->color == 3)
	    sprintf( clcode, R_YELLOW );
	else if (ch->color == 4)
	    sprintf( clcode, R_BLUE );
	else if (ch->color == 5)
	    sprintf( clcode, R_MAGENTA );
	else if (ch->color == 6)
	    sprintf( clcode, R_CYAN );
	else if (ch->color == 7)
	    sprintf( clcode, R_WHITE );
	else if (ch->color == 8)
	    sprintf( clcode, R_D_GREY );
	else if (ch->color == 9)
	    sprintf( clcode, R_B_RED );
	else if (ch->color == 10)
	    sprintf( clcode, R_B_GREEN );
	else if (ch->color == 11)
	    sprintf( clcode, R_B_YELLOW );
	else if (ch->color == 12)
	    sprintf( clcode, R_B_BLUE );
	else if (ch->color == 13)
	    sprintf( clcode, R_B_MAGENTA );
	else if (ch->color == 14)
	    sprintf( clcode, R_B_CYAN );
	else if (ch->color == 15)
	    sprintf( clcode, R_B_WHITE );
	else if (ch->color == 16)
	    sprintf( clcode, R_BLACK );
	else
	    sprintf( clcode, CLEAR );
    }
    else
    {
	sprintf( clcode, CLEAR );
    }

    return clcode;
}

char *colour_channel( int colornum, CHAR_DATA *ch )
{

    if (colornum == 1)
	    sprintf( clcode, C_RED );
    else if (colornum == 2)
	    sprintf( clcode, C_GREEN );
    else if (colornum == 3)
	    sprintf( clcode, C_YELLOW );
    else if (colornum == 4)
	    sprintf( clcode, C_BLUE );
    else if (colornum == 5)
	    sprintf( clcode, C_MAGENTA );
    else if (colornum == 6)
	    sprintf( clcode, C_CYAN );
    else if (colornum == 7)
	    sprintf( clcode, C_WHITE );
    else if (colornum == 8)
	    sprintf( clcode, C_D_GREY );
    else if (colornum == 9)
	    sprintf( clcode, C_B_RED );
    else if (colornum == 10)
	    sprintf( clcode, C_B_GREEN );
    else if (colornum == 11)
	    sprintf( clcode, C_B_YELLOW );
    else if (colornum == 12)
	    sprintf( clcode, C_B_BLUE );
    else if (colornum == 13)
	    sprintf( clcode, C_B_MAGENTA );
    else if (colornum == 14)
	    sprintf( clcode, C_B_CYAN );
    else if (colornum == 15)
	    sprintf( clcode, C_B_WHITE );
    else if (colornum == 16)
	    sprintf( clcode, C_BLACK );
    else
	sprintf( clcode, colour_clear( ch ));

    return clcode;
}


/*
 * Macintosh support functions.
 */
#if defined(macintosh)
int gettimeofday( struct timeval *tp, void *tzp )
{
    tp->tv_sec  = time( NULL );
    tp->tv_usec = 0;
}
#endif

void log_f (char * fmt, ...)
{
    char buf [2*MSL];
    va_list args;
    va_start (args, fmt);
    vsprintf (buf, fmt, args);
    va_end (args);

    log_string (LOG_GAME,buf);
}

void sig_handler(int sig)
{
  if(merc_down)
  {
     bug_f("sig_handler called, but merc_down");
     return;
  }

  switch(sig)
  {
  case SIGBUS:
    log_string(LOG_ERR,"Sig handler SIGBUS.");
    auto_shutdown();
    break;
  case SIGTERM:
    log_string(LOG_ERR,"Sig handler SIGTERM.");
//    auto_shutdown();
    break;
  case SIGABRT:
    log_string(LOG_ERR,"Sig handler SIGABRT");
    auto_shutdown();
    break;
  case SIGSEGV:
    log_string(LOG_ERR,"Sig handler SIGSEGV");
    auto_shutdown();
    break;
  case SIGXCPU:
    log_string(LOG_ERR,"Sig handler SIGXCPU");
    auto_shutdown();
    return;
    break;

  case SIGILL:
    log_string(LOG_ERR,"Sig handler SIGSEGV");
    auto_shutdown();
    break;
  }

  /* ! VERY IMPORTANT ! Lets system terminate program */
  signal( SIGABRT, SIG_DFL );
  signal( SIGTERM, SIG_DFL );
  signal( SIGSEGV, SIG_DFL );
  signal( SIGBUS, SIG_DFL );
  signal( SIGILL, SIG_DFL );
  signal( SIGXCPU, SIG_DFL );

  exit(0);

/*  abort(); */

  return;
}

void auto_shutdown() /* When the game terminates abnormally */
{
    extern bool merc_down;
    extern char last_command[MAX_STRING_LENGTH];
/*    extern int  last_hostip;*/

    DESCRIPTOR_DATA *d, *d_next;
    char buf[MAX_STRING_LENGTH];
    merc_down = TRUE;

    log_string(LOG_ERR,"auto_shutdown() called");

    log_string(LOG_ERR,"Last Command: %s", last_command);
    log_string(LOG_ERR,"Last From Buffer: %s", last_from_buffer);

    log_string(LOG_ERR,"Safe coredumping...");
    safe_coredump();
    log_string(LOG_ERR,"Done safe coredumping.");

    system("cp ../src/rot ../bin/deadrot");
    system("mv core ../bin/core-safe");

/*    if(last_hostip)
        auto_ban_ip(last_hostip);*/

/*    do_dump(NULL,""); */

    for ( d = descriptor_list; d != NULL; d = d_next)
    {
      if(d->character)
      {
          CHAR_DATA *ch = (d->original) ? d->original : d->character;

          write_to_desc(d,"\n\r-> System: Uh Oh ...\n\r",0);

          if(!IS_NPC(ch))
          {
            sprintf(buf, "cp ../player/%s ../bkp/autobackup/player/", 
                capitalize(d->character->name));
            system(buf);
          }

          do_save (d->character, "");
          send_to_char ( "\n\r\n\r{x#  Impending {RCR{ra{RSH{x!  #\n\r\n\r",
            d->character );
          /* Color won't work on the following line */
          write_to_desc(d,"\n\r** Brace for Impact! **\n\r",0);
      }
      d_next = d->next;
      close_socket(d);
    }

    log_string(LOG_GAME,"Auto Asaving Changed");
    do_asave(NULL,"changed");

    /*terminate_python();*/

    /* ! VERY IMPORTANT ! Lets system terminate program */
    signal( SIGABRT, SIG_DFL );
    signal( SIGTERM, SIG_DFL );
    signal( SIGSEGV, SIG_DFL );
    signal( SIGILL, SIG_DFL );
    signal( SIGBUS, SIG_DFL );
    signal( SIGXCPU, SIG_DFL );

    return;
}

/* nc_strlen
 * counts number of characters visible on screen
 *  by Thyrr
 */
int nc_strlen ( char *string )
{
    int count = 0;

    while ( *string != '\0' )
    {
        if( *string == '{' )
        {
            if(*(++string) == '\0') break;

            /* escaped - these translate to real characters */
            if(*string == '{' || *string == '-')
                count++;
        }
        else
            count++;

        string++;
    }

    return count;

}

void foreign_speech(CHAR_DATA *ch,const char *text,
				char *transpose, int language)
{
    ofuscate_text(text,transpose, UMAX(100 - ch->language_known[language],0));
    return;
}

void foreign_hearing(CHAR_DATA *ch,CHAR_DATA *sby,const char *text,
				char *transpose, int language)
{
    if(ch == sby)
    {
        ofuscate_text(text,transpose,0);
        return;
    }

    ofuscate_text(text,transpose, UMAX(85 - ch->language_known[language],0));
    return;
}

/*
 * ofuscate_text: ofuscates the text by a certain amount
 *                The higher the percent, the more it's ofuscated
 *                  - Written by Thyrr
 */
int ofuscate_text(const char *tochange,char *newstring,int percent)
{
    char lastchar;
    char temp;
    char consonants[30] = "BCDFGHJKLMNPQRSTVWXZ^";
    char vowels[10] = "AEIOUY^";
    char spaces[10] = "   '-`  ^";
    char newchars[100] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ^";

    lastchar = 'A';

    while(*tochange != '\0')
    {

        if(number_range(60,100) <= percent && number_range(1,19) < 4)
        {
	    *newstring = (isupper(lastchar)) ?
               newchars[number_range(0,25)] :
                tolower(newchars[number_range(0,25)]);
            newstring++;
        }

        if(number_range(5,100) <= percent)
        {
	    temp = tolower(*tochange);
	    if( temp == 'a' || temp == 'e' || temp == 'i' || temp == 'o'
			    || temp == 'u' || temp == 'y' )
            {
		*newstring = (isupper(*tochange)) ?
                    vowels[number_range(0,5)] :
                    tolower(vowels[number_range(0,5)]);
            }
            else if(isalpha(*tochange))
            {
		*newstring = (isupper(*tochange)) ?
                    consonants[number_range(0,19)] :
                    tolower(consonants[number_range(0,19)]);
            }
            else if(isspace(*tochange))
            {
		*newstring = spaces[number_range(0,7)];
            }
            else
            {
		*newstring = *tochange;
            }
        }
        else
            *newstring = *tochange;

        lastchar = *newstring;

        tochange++;
        newstring++;

        if(number_range(40,100) <= percent && number_range(1,20) < 4)
        {
	    *newstring = (isupper(lastchar)) ?
               newchars[number_range(0,25)] :
                tolower(newchars[number_range(0,25)]);
            newstring++;
        }

    }

    *newstring = '\0';

    return percent;
}

/*
 * For use with languages
 */
void act_lang( const char *format, CHAR_DATA *ch, const void *arg1, 
	      const void *arg2, int type, int min_pos, int lang )
{
    static char * const he_she  [] = { "it",  "he",  "she" };
    static char * const him_her [] = { "it",  "him", "her" };
    static char * const his_her [] = { "its", "his", "her" };
 
    CHAR_DATA 		*to;
    CHAR_DATA 		*vch = ( CHAR_DATA * ) arg2;
    OBJ_DATA 		*obj1 = ( OBJ_DATA  * ) arg1;
    OBJ_DATA 		*obj2 = ( OBJ_DATA  * ) arg2;
    const 	char 	*str;
    char 		*i = NULL;
    char 		*point;
    char 		*i2;
    char 		fixed[ MAX_STRING_LENGTH ];
    char 		buf[ MAX_STRING_LENGTH   ];
    char 		fname[ MAX_INPUT_LENGTH  ];
    char 		tra[ MAX_INPUT_LENGTH * 5 ];
    bool		fColour = FALSE;

    if( !format || !*format )
        return;

    if( !ch || !ch->in_room )
	return;

    to = ch->in_room->people;
    if( type == TO_VICT )
    {
        if ( !vch )
        {
            bug( "Act: null vch with TO_VICT.", 0 );
            return;
        }

	if ( !vch->in_room )
	    return;

        to = vch->in_room->people;
    }
 
    for( ; to ; to = to->next_in_room )
    {
        if ( (!IS_NPC(to) && to->desc == NULL )
        ||   ( IS_NPC(to) && !HAS_TRIGGER(to, TRIG_ACT) && !to->desc)
        ||    to->position < min_pos )
            continue;
 
        if( type == TO_CHAR && to != ch )
            continue;
        if( type == TO_VICT && ( to != vch || to == ch ) )
            continue;
        if( type == TO_ROOM && to == ch )
            continue;
        if( type == TO_NOTVICT && ( to == ch || to == vch ) )
            continue;
 
        point   = buf;
        str     = format;
        while( *str )
        {
            if( *str != '$' && *str != '{' )
            {
                *point++ = *str++;
                continue;
            }

	    i = NULL;
	    switch( *str )
	    {
		case '$':
		    fColour = TRUE;
		    ++str;
		    i = " <@@@> ";
		    if ( !arg2 && *str >= 'A' && *str <= 'Z' && *str != 'G' )
		    {
			bug( "Act: missing arg2 for code %d.", *str );
			i = " <@@@> ";
		    }
		    else
		    {
			switch ( *str )
			{
			    default:  
				bug( "Act: bad code %d.", *str );
				i = " <@@@> ";                                
				break;

                            case '$': i = "$"; break;

			    case 't': 
				foreign_hearing(to,ch,arg1,tra,lang);
				i = tra;
				break;

			    case 'T': 
				foreign_hearing(to,ch,arg2,tra,lang);
				i = tra;

				break;

			    case 'n': 
				i = PERS( ch,  to  );
				break;

			    case 'N': 
				i = PERS( vch, to  );                         
				break;

			    case 'e': 
				i = he_she  [URANGE(0, ch  ->sex, 2)];        
				break;

			    case 'E': 
				i = he_she  [URANGE(0, vch ->sex, 2)];        
				break;

			    case 'm': 
				i = him_her [URANGE(0, ch  ->sex, 2)];        
				break;

			    case 'M': 
				i = him_her [URANGE(0, vch ->sex, 2)];        
				break;

			    case 's': 
				i = his_her [URANGE(0, ch  ->sex, 2)];        
				break;

			    case 'S': 
				i = his_her [URANGE(0, vch ->sex, 2)];        
				break;
 
			    case 'p':
				i = can_see_obj( to, obj1 )
				  ? obj1->short_descr
				  : "something";
				break;
 
			    case 'P':
				i = can_see_obj( to, obj2 )
				  ? obj2->short_descr
				  : "something";
				break;
 
			    case 'd':
				if ( !arg2 || ((char *) arg2)[0] == '\0' )
				{
				    i = "door";
				}
				else
				{
				    one_argument( (char *) arg2, fname );
				    i = fname;
				}
				break;

			    case 'G':
				if ( ch->alignment < 0 )
				{
				    i = "Belan";
				}
				else
				{
				    i = "Thoth";
				}
				break;

			}
		    }
		    break;

		case '{':
		    fColour = FALSE;
		    ++str;
		    i = NULL;
		    if( IS_SET( to->act, PLR_COLOUR ) )
		    {
			i = colour( *str, to );
		    }
		    break;

		default:
		    fColour = FALSE;
		    *point++ = *str++;
		    break;
	    }

            ++str;
	    if( fColour && i )
	    {
		fixed[0] = '\0';
		i2 = fixed;

		if( IS_SET( to->act, PLR_COLOUR ) )
		{
		    for( i2 = fixed ; *i ; i++ )
	            {
			if( *i == '{' )
			{
			    i++;
			    strcat( fixed, colour( *i, to ) );
			    for( i2 = fixed ; *i2 ; i2++ )
				;
			    continue;
			}
			*i2 = *i;
			*++i2 = '\0';
		    }			
		    *i2 = '\0';
		    i = &fixed[0];
		}
	        else
		{
		    for( i2 = fixed ; *i ; i++ )
	            {
			if( *i == '{' )
			{
			    i++;
			    if( *i != '{' )
			    {
				continue;
			    }
			}
			*i2 = *i;
			*++i2 = '\0';
		    }			
		    *i2 = '\0';
		    i = &fixed[0];
		}
	    }


	    if( i )
	    {
		while( ( *point = *i ) != '\0' )
		{
		    ++point;
		    ++i;
		}
	    }
        }
 
        *point++	= '\n';
        *point++	= '\r';
        *point		= '\0';
	buf[0]		= UPPER( buf[0] );
        if ( to->desc != NULL && to->desc->connected == CON_PLAYING )
	    write_to_buffer( to->desc, buf, point - buf );
        else
        if ( IS_NPC(to) && MOBtrigger )
            mp_act_trigger( buf, to, ch, arg1, arg2, TRIG_ACT );

    }
    return;
}

void bug_f (char * fmt, ...)
{
    char buf [2*MSL];
    va_list args;
    va_start (args, fmt);
    vsprintf (buf, fmt, args);
    va_end (args);

    bug (buf, 0);
}

/*
 * Copies memory to child process and aborts it
 *  Suggested by Erwin Andreasen
 *
 */

void safe_coredump()
{
    switch (fork())
    {
        case -1:
            perror("safe_coredump:");
            break;
        case 0:
            /* child */
            signal( SIGABRT, SIG_DFL );
            signal( SIGTERM, SIG_DFL );
            signal( SIGSEGV, SIG_DFL );
            signal( SIGILL, SIG_DFL );
            signal( SIGBUS, SIG_DFL );
            abort();
            perror("safe_coredump:failed abort:"); /* EEK! */
            _exit(1);

        default:
           wait(NULL); /* wait for the child to finish */
    }
}

void numberbar ( char *buf, int width, int cur, int max )
{
    int pos,i;

    if(width < 0) width = 10;
    if(width > 70) width = 70;

    pos = (cur * width)/max;

    if((100*pos/width) <= 10) strcat(buf,"{D");
    else if((100*pos/width) <= 30) strcat(buf,"{R");
    else if((100*pos/width) <= 80) strcat(buf,"{Y");
    else strcat(buf,"{G");

    for(i = 1; i <= width; i++)
        strcat(buf,(i<pos)?"*":" ");
}

void init_signals()
{
    signal(SIGBUS,sig_handler);
    signal(SIGTERM,sig_handler);
    signal(SIGABRT,sig_handler);
    signal(SIGSEGV,sig_handler);
    signal(SIGILL,sig_handler);
    signal(SIGXCPU,sig_handler);

    return;
}


void bad_password(DESCRIPTOR_DATA *d, char *argument)
{
    int lfcount;
    int i;
    int lfempty = -1;
    bool lffound = FALSE;

    return;

    for(lfcount = 0; lfcount < MAX_LOGIN_FAILURE; lfcount++)
    {
        if(d->hostip == login_failures[lfcount].ip &&
             d->hostip != 2130706433)
        {
            if(login_failures[lfcount].lockedout &&
                (login_failures[lfcount].lockedout > current_time))
            {
                break;
            }

            if(login_failures[lfcount].lastattempt + 600 > current_time)
                login_failures[lfcount].attempts++;

            if(login_failures[lfcount].attempts >= 7)
            {
                login_failures[lfcount].lockedout = current_time + (10 * 60);
                login_failures[lfcount].attempts = 0;
                login_failures[lfcount].lastattempt = current_time;
                break;
            }

            lffound = TRUE;
            break;
        }

        if(!login_failures[lfcount].ip) lfempty = lfcount;
    }
    if(!lffound ) /* Not on list - add */
    {
        if(lfempty < 0) /* All slots taken, find one to boot out */
        {
            for(i = 0; i < 5; i++)
            {
                lfempty = number_range(0,MAX_LOGIN_FAILURE);
                if(!login_failures[lfempty].lockedout)
                    break;
            }
        }

        login_failures[lfempty].ip = d->hostip;
        login_failures[lfempty].attempts = 1;
        login_failures[lfempty].lockedout = 0;
        login_failures[lfempty].lastattempt = current_time;

    }
}

void print_last_command( void )
{
    char last_command[MAX_STRING_LENGTH];
    bug_f("Command: [%s]",last_command);
}

bool file_exists (char *name)
{
    struct stat filestat;

    /*fclose( fpReserve );*/

    if(stat(name,&filestat) == 0)
    {
//        fpReserve = fopen( NULL_FILE, "r" );
        return TRUE;
    }

//    fpReserve = fopen( NULL_FILE, "r" );

    return FALSE;
}


