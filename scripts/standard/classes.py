import mud

class Character(mud.CharacterType):
    pass

class Mob(Character):
    pass

class Player(Character):
    pass

class Object(mud.ObjectType):
    def getDesc(self):
        return self.description

class Room(mud.RoomType):
    def getDesc(self):
        return self.description

class Event(object):
    pass

