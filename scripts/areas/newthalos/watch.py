import mud
from time import *
import standard

class Watch(standard.Object):
    def getRealTime(self):
        return (divmod(localtime()[3],12)[1] or 12, localtime()[4])

    def getDesc(self, viewer=None):
        rtime = self.getRealTime()
        return ("It's a shiny golden pocket watch with smooth glass"
               + " covering the face.\nThe short hand points to "
               + mud.toRoman(rtime[0])
               + " and the long hand points to "
               + `rtime[1]` + ".")
