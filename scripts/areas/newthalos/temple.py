import standard
import mud
from util import random

class Memory(object):
    def __init__(self, ch):
        self.last_seen = 0

class Safie(standard.Mob):
    def __init__(self):
        self.memory = {}

    def on_detectEntry(self, ch):

        if ch.isNPC:
            if(ch.vnum == 9708):
                self.say("How'ya doing, Sar'n?");
                return

            if self.memory.has_key(ch.id):
                elapsed = mud.ticks() - self.memory[ch.id].last_seen
            else:
                self.memory[ch.id] = Memory(ch)
                elapsed = 100

            if ch.isPet:
                if elapsed > 2:
                    self.do("pat " + ch.name)
        else:
            if self.memory.has_key(ch.id):
                elapsed = mud.ticks() - self.memory[ch.id].last_seen

                if elapsed > 50:
                    self.say("Hey, haven't seen you around much, " + 
                             self.pers(ch) + ".")
                elif elapsed > 15:
                    self.say("Hey, welcome back! Wow, it's been like ..."
                             " %d hours? " % elapsed)
                elif elapsed > 5:
                    self.say("Hi again, " + self.pers(ch) + "!")
                elif elapsed > 2:
                    self.do("smile " + ch.name)
            else:
                self.memory[ch.id] = Memory(ch)
                self.say("Hi, " + self.pers(ch) + "!")

            if random.randint(0, 6) == 5 and not ch.isNPC:
                self.praise_eq(ch)

        self.memory[ch.id].last_seen = mud.ticks()

    def on_think(self):
        if random.randint(0, 15) < 13:
            return

    def on_speech(self, ch, text):
        #self.say( self.pers(ch) + " said: " + text )
        pass

    def praise_eq(self, ch):
        items = ch.items

        fancy_items = [item for item in items if item.cost > 50000 and
                                                 item.isWorn]
        if not fancy_items: return

        item = random.choice(fancy_items)

        desc = item.short_desc
        if desc.startswith("an"): desc = desc[3:]
        elif desc.startswith("a"): desc = desc[2:]
        elif desc.startswith("the"): desc = desc[4:]

        self.say("Oh, that's a nice {+%s{= you have there, %s." %
                      (desc, self.pers(ch)))
