import standard

# Advanced pathfinding support goes here

class Pathfinder(standard.Character):
    def __init__(self):
        pass

    def findAndGet(self, obj):
        setPath( pathfind(self.location, obj.in_room) )

    def seekAndDestroy(self, target):
        setPath( pathfind(self.location, target) )

    def on_arriveInRoom(self, room):
        pass # not implemented
