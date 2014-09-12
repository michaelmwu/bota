from mud import *

DEFENSE = 0
OFFENSE = 1

KING_ROOM = 458
NEAR_TENT = 406
TENT_ROOM = 407
GARRISON  = 417

def on_begin():
    # spawn team-allied NPCs
    king = spawnMob(400)
    king.setTeam(DEFENSE)
    king.moveTo(KING_ROOM)

    # king's guards
    guard = spawnMob(406)
    guard.setTeam(DEFENSE)
    guard.moveTo(KING_ROOM)

    guard = spawnMob(406)
    guard.setTeam(DEFENSE)
    guard.moveTo(KING_ROOM)

    lieutenant = spawnMob(401)
    lieutenant.setTeam(OFFENSE)
    lieutenant.moveTo(TENT_ROOM)

    # lieutenant should be well-protected, in case of sneak attacks
    guard = spawnMob(407)
    guard.setTeam(OFFENSE)
    guard.moveTo(NEAR_TENT)

    guard = spawnMob(407)
    guard.setTeam(OFFENSE)
    guard.moveTo(NEAR_TENT)

    guard = spawnMob(407)
    guard.setTeam(OFFENSE)
    guard.moveTo(TENT_ROOM)

    guard = spawnMob(407)
    guard.setTeam(OFFENSE)
    guard.moveTo(TENT_ROOM)

    garguard = spawnMob(402)
    garguard.setTeam(DEFENSE)
    garguard.moveTo(GARRISON)
