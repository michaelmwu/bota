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

/*****************************************************
 File: pathfind.c

 This code is copyright (C) 2001 by Brian Graversen.
 It may be used, modified and distributed freely, as
 long as you do not remove this copyright notice.

 Feel free to mail me if you like the code.

 E-mail: jobo@daimi.au.dk
******************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "merc.h"


#define RID ROOM_INDEX_DATA

/* local varibles */
bool gFound;

/* local functions */
bool  examine_room         args (( RID *pRoom, RID *tRoom, AREA_DATA *pArea, int steps ));
void  dijkstra             args (( RID *chRoom, RID *victRoom ));
RID  *heap_getMinElement   args (( HEAP *heap ));
HEAP *init_heap            args (( RID *root ));


/*
 * This function will return a shortest path from
 * room 'from' to room 'to'. If no path could be found,
 * it will return NULL. This function will only return
 * a path if both rooms where in the same area, feel
 * free to remove that restriction, and modify the rest
 * of the code so it will work with this.
 */
char *pathfind(RID *from, RID *to)
{
  int const exit_names [] = { 'n', 'e', 's', 'w', 'u', 'd' };
  RID *pRoom;
  AREA_DATA *pArea;
  static char path[MAX_STRING_LENGTH]; // should be plenty.
  int iPath = 0, vnum, door;
  bool found;

  if (from == to) return "";
  if ((pArea = from->area) != to->area) return NULL;

  /* initialize all rooms in the area */
  for (vnum = pArea->min_vnum; vnum < pArea->max_vnum; vnum++)
  {
    if ((pRoom = get_room_index(vnum)))
    {
      pRoom->visited = FALSE;
      for (door = 0; door < 6; door++)
      {
        if (pRoom->exit[door] == NULL) continue;
        pRoom->exit[door]->pf_visited = FALSE;
      }
    }
  }

  /* initialize variables */
  pRoom = from;
  gFound = FALSE;

  /* In the first run, we only count steps, no coloring is done */
  dijkstra(pRoom, to);
     
  /* If the target room was never found, we return NULL */
  if (!gFound) return NULL;

  /* in the second run, we color the shortest path using the step counts */
  if (!examine_room(pRoom, to, pArea, 0))
    return NULL;

  /* then we follow the trace left by examine_room() */
  while (pRoom != to)
  {
    found = FALSE;
    for (door = 0; door < 6 && !found; door++)
    {
      if (pRoom->exit[door] == NULL) continue;
      if (pRoom->exit[door]->u1.to_room == NULL) continue;
      if (!pRoom->exit[door]->pf_visited) continue;

      pRoom->exit[door]->pf_visited = FALSE;
      found = TRUE;
      path[iPath] = exit_names[door];
      iPath++;
      pRoom = pRoom->exit[door]->u1.to_room;
    }
    if (!found)
    {
      bug("Pathfind: Fatal Error in %d.", pRoom->vnum);
      return NULL;
    }
  }

  path[iPath] = '\0';
  return path;
} 
       
/*
 * This function will examine all rooms in the same area as chRoom,
 * setting their step count to the minimum amount of steps needed
 * to walk from chRoom to the room. If victRoom is encountered, it
 * will set the global bool gFound to TRUE. Running time for this
 * function is O(n*log(n)), where n is the amount of rooms in the area.
 */
void dijkstra(RID *chRoom, RID *victRoom)
{
  RID *pRoom;
  RID *tRoom;
  RID *xRoom;
  HEAP *heap;
  int door, x;
  bool stop;

  heap = init_heap(chRoom);
  while (heap->iVertice)
  {
    if ((pRoom = heap_getMinElement(heap)) == NULL)
    {
      bug("Dijstra: Getting NULL room", 0);
      return;
    }
    if (pRoom == victRoom)
      gFound = TRUE;

    /* update all exits */
    for (door = 0; door < 6; door++)
    {
      if (pRoom->exit[door] == NULL) continue;
      if (pRoom->exit[door]->u1.to_room == NULL) continue;

      /* update step count, and swap in the heap */
      if (pRoom->exit[door]->u1.to_room->steps > pRoom->steps + 1)
      {
        xRoom = pRoom->exit[door]->u1.to_room;
        xRoom->steps = pRoom->steps + 1;
        stop = FALSE;
        while ((x = xRoom->heap_index) != 1 && !stop)
        {
          if (heap->knude[x/2]->steps > xRoom->steps)
          {
            tRoom                       = heap->knude[x/2];
            heap->knude[x/2]            = xRoom;
            xRoom->heap_index           = xRoom->heap_index/2;
            heap->knude[x]              = tRoom;
            heap->knude[x]->heap_index  = x;
          }
          else stop = TRUE;
        }
      }
    }
  }

  /* free the heap */
  free(heap);
}


/*
 * This function walks through pArea, searching for tRoom,
 * while it colors the path it takes to get there. It will
 * return FALSE if tRoom was never encountered, and TRUE if
 * it does find the room.
 */
bool examine_room(RID *pRoom, RID *tRoom, AREA_DATA *pArea, int steps)
{
  int door;

  /* been here before, out of the area or can we get here faster */
  if (pRoom->area != pArea) return FALSE;
  if (pRoom->visited) return FALSE;
  if (pRoom->steps < steps) return FALSE;

  /* Have we found the room we are searching for */
  if (pRoom == tRoom)
    return TRUE;

  /* mark the room so we know we have been here */
  pRoom->visited = TRUE;

  /* Depth first traversel of all exits */
  for (door = 0; door < 6; door++)
  {
    if (pRoom->exit[door] == NULL) continue;
    if (pRoom->exit[door]->u1.to_room == NULL) continue;

    /* assume we are walking the right way */
    pRoom->exit[door]->pf_visited = TRUE;

    /* recursive return */
    if (examine_room(pRoom->exit[door]->u1.to_room, tRoom, pArea, steps + 1))
      return TRUE;

    /* it seems we did not walk the right way */
    pRoom->exit[door]->pf_visited = FALSE;
  }
  return FALSE;
}

/* 
 * This function will initialize a heap used for the
 * pathfinding algorithm. It will return a pointer to
 * the heap if it succedes, and NULL if it did not.
 */
HEAP *init_heap(RID *root)
{
  AREA_DATA *pArea;
  RID *pRoom;
  HEAP *heap;
  int i, size, vnum;
      
  if ((pArea = root->area) == NULL) return NULL;
  size = pArea->max_vnum - pArea->min_vnum;
     
  /* allocate memory for heap */
  heap = calloc(1, sizeof(*heap));
  heap->knude = calloc(2 * (size + 1), sizeof(ROOM_INDEX_DATA *));
  heap->size  = 2 * (size + 1);

  /* we want the root at the beginning */
  heap->knude[1]             = root;
  heap->knude[1]->steps      = 0;
  heap->knude[1]->heap_index = 1;

  /* initializing the rest of the heap */   
  for (i = 2, vnum = pArea->min_vnum; vnum < pArea->max_vnum; vnum++)
  {
    if ((pRoom = get_room_index(vnum)))
    {
      if (pRoom == root) continue;
      heap->knude[i]             = pRoom;
      heap->knude[i]->steps      = 2 * heap->size;
      heap->knude[i]->heap_index = i;
      i++;
    }
  }

  heap->iVertice = i-1;
    
  /* setting the rest to NULL */
  for (; i < heap->size; i++)
    heap->knude[i] = NULL;
 
  return heap;
}

/* 
 * This function will return the smallest element in the heap,
 * remove the element from the heap, and make sure the heap
 * is still complete.
 */
RID *heap_getMinElement(HEAP *heap)
{
  RID *tRoom;
  RID *pRoom;
  bool done = FALSE;
  int i = 1;

  /* this is the element we wish to return */
  pRoom = heap->knude[1];

  /* We move the last vertice to the front */
  heap->knude[1] = heap->knude[heap->iVertice];
  heap->knude[1]->heap_index = 1;

  /* Decrease the size of the heap and remove the last entry */
  heap->knude[heap->iVertice] = NULL;
  heap->iVertice--;

  /* Swap places till it fits */
  while(!done)
  {
    if (heap->knude[i] == NULL)
      done = TRUE;
    else if (heap->knude[2*i] == NULL)
      done = TRUE;
    else if (heap->knude[2*i+1] == NULL)
    {
      if (heap->knude[i]->steps > heap->knude[2*i]->steps)
      {
        tRoom                        = heap->knude[i];
        heap->knude[i]               = heap->knude[2*i]; 
        heap->knude[i]->heap_index   = i;
        heap->knude[2*i]             = tRoom;
        heap->knude[2*i]->heap_index = 2*i;
        i = 2*i;
      }
      done = TRUE;
    }
    else if (heap->knude[i]->steps <= heap->knude[2*i]->steps &&
        heap->knude[i]->steps <= heap->knude[2*i+1]->steps)
      done = TRUE;
    else if (heap->knude[2*i]->steps <= heap->knude[2*i+1]->steps)
    {
      tRoom                          = heap->knude[i];
      heap->knude[i]                 = heap->knude[2*i];
      heap->knude[i]->heap_index     = i;
      heap->knude[2*i]               = tRoom;
      heap->knude[2*i]->heap_index   = 2*i;
      i = 2*i;
    }
    else
    {
      tRoom                          = heap->knude[i];
      heap->knude[i]                 = heap->knude[2*i+1];
      heap->knude[i]->heap_index     = i;
      heap->knude[2*i+1]             = tRoom;
      heap->knude[2*i+1]->heap_index = 2*i+1;
      i = 2*i+1;
    }
  }

  /* return the element */
  return pRoom;
}

void do_pathfind(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  char *path;
    
  one_argument(argument, arg);
  if ((victim = get_char_world(ch, arg)) == NULL) return;
    
  if ((path = pathfind(ch->in_room, victim->in_room)) != NULL)
    sprintf(buf, "Path: %s\n\r", path);
  else
    sprintf(buf, "Path: Unknown.\n\r");
  send_to_char(buf, ch);
  
  return;
}
