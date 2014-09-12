import mud
import standard
from util import random

idle = "idle"
walking = "walking"
talking = "talking"

sarins_room = 9708
chamber = 9604
altar = 9605

healer = 9605
priest = 9581

class Sarin(standard.Character):
    def __init__(self):
        self.state = idle
        self.lastseen_priest = 0
        self.lastseen_healer = 0

    def on_think(self):
      if self.state is walking:
        if 0: # not self.path:
          self.say("Whoops! I'm a bit lost now...")
          mud.log("Sarin: walking without a path")
          self.state = idle
        return

      if self.state is talking:
        pass

      if self.state is idle:
        if(self.location.vnum == sarins_room):
          if( random.randint(0, 10) == 1 ):
            if self.lastseen_healer + 10 < mud.ticks():
              self.setPath("ddwn")
            elif self.lastseen_priest + 10 < mud.ticks():
              self.setPath("ddwwn")
            
            if self.path:
                self.state = walking
                self.say("Hmm, I guess I should walk around now ...")

            
        elif(self.location.vnum == chamber):
          self.lastseen_priest = mud.ticks()

          if( random.randint(0, 5) == 1 ):
            self.say("Hmm, I guess I should leave now ...")
            self.state = walking
            if self.lastseen_healer + 4 < mud.ticks():
                self.setPath("sen")
            else:
                self.setPath("seeuu")

        elif(self.location.vnum == altar):
          self.lastseen_healer = mud.ticks()

          if( random.randint(0, 5) == 1 ):
            self.say("Hmm, I guess I should leave now ...")
            self.state = walking
            if self.lastseen_priest + 4 < mud.ticks():
                self.setPath("swn")
            else:
                self.setPath("seu")

        else:
          # whoops! okay, go back to my room
          self.state = walking
          self.setPath(sarins_room)
          self.say("Whoa, what was I doing?")
          self.say("I better head back to my room.")

    def on_wake(self, ch):
        if canSee(ch):
            self.say("Oh, sorry, " + self.pers(ch) + "." )
        else:
            self.say("Huh? Who's there?")

    def on_arrive(self, fromRoom=None, dir=None):
        return

    def on_endOfPath(self):
        self.state = idle

    def on_detectEntry(self, ch, fromRoom=None, dir=None):
        #self.say("Hello")
        pass
