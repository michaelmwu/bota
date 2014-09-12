import standard
import random

randgen = random.Random()

class ChattyMob(standard.Character):
    chat_msgs_idle = [
         "Hey, how are you doing today?",
         "Peppermints are great, aren't they?",
         "I could really go for a hot meal, eh?",
         "Good adventuring to ya!" ]
    chat_msgs_weather = {
      'sunny': ["It's a wonderful day, isn't it?",
                "Oh my, it's so bright!"],
      'cloudy': ["Miserable cloud cover today, eh?"],
      'rainy': ["Oh, how long will this miserable day last!",
                "I'm siiiingggin' in the rain, just singing in the rain!"],
      }

    def on_think(self):
        num = randgen.randrange(0,6)
        if(num is 0):
            self.say(randgen.choice(self.chat_msgs_idle))
        elif(num is 5):
            self.say(randgen.choice(self.chat_msgs_weather['sunny']))


