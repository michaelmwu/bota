import standard

class Hunter(standard.Character):
    def __init__(self):
        self.hit_list = []

    def on_see_entry(self, char):
        if char.id in self.hit_list:
            attack(ch, "backstab") # not implemented
