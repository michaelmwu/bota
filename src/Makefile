CC      = gcc
PROF    = -ggdb -pg
NOCRYPT =
C_FLAGS =  -Wall $(PROF) `cat cflags` -march=pentium4
P_LINK_FLAGS = -lm -ldl -lpthread -Xlinker -export-dynamic -lpython2.3 -lz
L_FLAGS =  $(PROF) $(P_LINK_FLAGS)
P_FLAGS = -I/usr/include/python2.3/

ROT_FILES = act_comm.o act_enter.o act_info.o act_move.o act_obj.o act_wiz.o \
          alias.o ban.o comm.o const.o curse.o db.o db2.o drunk.o effects.o \
	  fight.o finger.o flags.o forget.o guild.o handler.o healer.o interp.o \
	  lookup.o magic.o magic2.o music.o recycle.o repent.o save.o \
	  skills.o special.o tables.o update.o wizlist.o omni.o \
	  mob_cmds.o mob_prog.o olc.o olc_act.o olc_save.o bit.o mem.o \
	  string.o olc_mpcode.o copyover.o quest.o arena.o board.o \
	  scan.o fishing.o event.o bank.o grant.o pathfind.o quest2.o \
          python.o python_mud.o python_mob.o python_obj.o python_room.o \
	  util.o vfind.o cprintf.o mccp.o

all: rot

.PHONY : clean depend
clean:
	rm *.o

ROT_C_FILES = $(subst .o,.c,$(ROT_FILES))

depend:
	@echo -n Making dependencies ...
	@gcc $(P_FLAGS) $(ROT_C_FILES) -MM > .depend
	@echo -e . "\E[35mDONE\E[0m"

include .depend

rot: $(ROT_FILES)
	@rm -f rot
	@echo -n Linking object files ...
	@$(CC) $(L_FLAGS) -o rot $(ROT_FILES) -lcrypt
	@echo -e . "\E[35mDONE\E[0m"

.c.o: merc.h tables.h
	@echo Compiling $<
	@$(CC) -c $(C_FLAGS) $(P_FLAGS) $<
