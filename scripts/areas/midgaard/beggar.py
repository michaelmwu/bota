import mud
import standard
from misc.mobs.chatty import *

class Beggar(ChattyMob):
    def __init__(self):
        self.chat_msgs_idle += [
            "A few coins for a blind old beggar?",
            "Fail college and you'll end up just like me!"
            "Coins, booze, whatever. I want it."
            "Looks like nasty weather"]
