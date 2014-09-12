EVENT *new_event();
void event_update args (( void ));
void event_add args(( EVENT *e ));
void event_add_char(EVENT *e, CHAR_DATA *ch);
void event_add_system args(( EVENT *e ));
void event_remove_local args(( EVENT **event_list, EVENT *e ));
void event_remove args(( EVENT *e ));
void event_list_kill args(( EVENT *list ));
EVENT* event_function_remove_local args (( EVENT **event_list,
					   EVENT_FUN *func ));
EVENT* find_event_type(EVENT *list, EVENT_FUN *func);

void interrupt_char(CHAR_DATA *ch, long flags);

#define EVENT_ACTOR_CHAR        1
#define EVENT_ACTOR_OBJ         2
#define EVENT_ACTOR_ROOM        3
#define EVENT_ACTOR_MOB_INDEX   4
#define EVENT_ACTOR_AREA        5
#define EVENT_ACTOR_SYSTEM      6

EVENT_FUN event_test;
EVENT_FUN event_hit_gain;
EVENT_FUN event_mana_move_gain;
EVENT_FUN event_iotdm;

#define MAX_EVENT_HASH 4096
extern EVENT *event_hash[MAX_EVENT_HASH];

#define EVENT_REPEAT			(A)
#define EVENT_DELETED			(B)
#define EVENT_PLAYER_ACTION		(C)
#define EVENT_INTERRUPT_ON_ACTION	((D)|(E)|(F)|(G))
#define EVENT_INTERRUPT_ON_COMMAND	(E)
#define EVENT_INTERRUPT_ON_FIGHT	(F)
#define EVENT_INTERRUPT_ON_MOVE		(G)

struct event_data
{
    EVENT *next_global;
    EVENT *next_local;

    bool valid;

    unsigned int when;
    int actor_type;
    int event_type;
    unsigned long executions;
    long flags;

    union
    {
        CHAR_DATA *ch;
        OBJ_DATA *obj;
        MOB_INDEX_DATA *mob;
        ROOM_INDEX_DATA *room;
        AREA_DATA *area;
        PyObject *pyobject;
	void *typeless;
    } actor;

    EVENT_FUN *call;

    union
    {
        CHAR_DATA	*ch;
        OBJ_DATA	*obj;
        MOB_INDEX_DATA	*mob;
        EXIT_DATA	*exit;
        ROOM_INDEX_DATA	*room;
        AREA_DATA	*area;
	PyObject	*pyobject;
	char		*string;
	void		*typeless;
	int		 num;
	long		 long_num;
    } data[5];

    char *interupt_char_msg;
    char *interupt_room_msg;
    EVENT_FUN *interrupt_func;

};
