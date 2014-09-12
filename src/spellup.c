void do_spellup(CHAR_DATA *ch,char *argument)
{
    CHAR_DATA *vch;
    char arg[MAX_INPUT_LENGTH];
    sh_int stype;

    argument = one_argument(argument,arg);

    if (IS_NPC(ch))
        return;

    if (arg[0] == '\0')
    {
        send_to_char("Spellup whom?\n\r",ch);    
        return;
    }

    act("You snap your fingers.",ch,NULL,NULL,TO_CHAR);
    act("$n snaps $s fingers.",ch,NULL,NULL,TO_ROOM);

    if (!str_cmp("room",arg))
    {
        vch = ch->in_room->people;
    }
    else
    {
        if ((vch = get_char_room(ch,arg)) == NULL
             && (vch=get_char_world(ch,arg)) == NULL )
        {
            send_to_char("They aren't here.\n\r",ch);
            return;
        }

        stype = 1;
    }

    for (vch = ch->in_room->people;vch;vch = vch->next_in_room)
        {
            if (IS_NPC(vch) || vch == ch)
                continue;

    spell_shockshield(skill_lookup("shockshield"),ch->level,ch,vch,TARGET_CHAR);
    spell_fireshield(skill_lookup("fireshield"),ch->level,ch,vch,TARGET_CHAR);
    spell_iceshield(skill_lookup("iceshield"),ch->level,ch,vch,TARGET_CHAR);
    spell_shield(skill_lookup("shield"),ch->level,ch,vch,TARGET_CHAR);
    spell_armor(skill_lookup("armor"),ch->level,ch,vch,TARGET_CHAR);
    spell_sanctuary(skill_lookup("sanctuary"),ch->level,ch,vch,TARGET_CHAR);
    spell_fly(skill_lookup("fly"),ch->level,ch,vch,TARGET_CHAR);
    spell_frenzy(skill_lookup("frenzy"),ch->level,ch,vch,TARGET_CHAR);
    spell_giant_strength(skill_lookup("giant strength"),ch->level,ch,vch,TARGET_CHAR);
    spell_bless(skill_lookup("bless"),ch->level,ch,vch,TARGET_CHAR);
    spell_invis(skill_lookup("invis"),ch->level,ch,vch,TARGET_CHAR);
    spell_haste(skill_lookup("haste"),ch->level,ch,vch,TARGET_CHAR);

    affect_strip(vch,gsn_plague);
    affect_strip(vch,gsn_poison);
    affect_strip(vch,gsn_blindness);
    affect_strip(vch,gsn_sleep);
    affect_strip(vch,gsn_curse);
    affect_strip(vch,gsn_fire_breath);
    affect_strip(vch,gsn_weaken);

    if ( stype = 1 )
        break;
                          
    return;
}  
                                                                             
