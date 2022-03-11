bool spec_guard( NPCharacter *ch )
{
   char buf[MAX_STRING_LENGTH];
   Character *victim;
   Character *v_next;
   Character *ech;
   char *crime;
   int max_evil;

   if (!IS_AWAKE(ch) || ch->fighting != 0) 
      return false;

   max_evil = 300;
   ech      = 0;
   crime    = "";

   for ( victim = ch->in_room->people; victim != 0; victim = v_next )
   {
      v_next = victim->next_in_room;
	
	   /*
	   * говорливые стражники в хометаунах, реагируют даже на мобов
	   */
      if (IS_SET( ch->in_room->area->area_flag, AREA_HOMETOWN )
		 && number_percent() < 2
		 && !victim->is_immortal( ))
      { 
	     do_say( ch, "Я Вас знаю?");

	     Room *room = get_room_index( victim->is_npc() ? 0 : victim->getPC()->getHometown( )->getRecall() );
	    
	     /* victim прописан в другой зоне */
	     if (!room || ch->in_room->area != room->area)
	     {
		    do_say( ch, "Я не знаю тебя. Уходи прочь!");
	     }
	     else /* ба, да мы соседи */
	     {
		    do_say(ch, "Ну ладно, мой друг. Я попытаюсь вспомнить.");
		    interpret( ch, "smile");
	     }
	   }
	
	   /*
	   * увидели преступника, это первоочередная цель, потому обрываем цикл.
	   * переменная crime жива еще с тех пор, когда туда кроме "CRIMINAL"
	   * писалось "THIEF" или "KILLER" (в предках анатолии, ROM-mud).
	   */
	   if ( !victim->is_npc()
		   && ( IS_SET(victim->act, PLR_WANTED)
			|| victim->isAffected(gsn_jail ) ) )
      {
	     crime = "CRIMINAL";
	     break;
      }
	
	   /*
	   * не будем вмешиваться, если victim - квестовый или иной спец-моб
	   */
	   if (victim->is_npc( ) 
	      && victim->getNPC()->behavior
	      && victim->getNPC()->behavior->hasDestiny())
	      continue;
	
	   /*
	   * все мирно, либо поздняк метаться
	   */
	   if (victim->fighting == 0 || victim->fighting == ch)
	     continue;
	
	   /*
	    * не будем вмешиваться в драку с квестовыми или спец-мобами
	   */
	   if (victim->fighting->is_npc( ) 
	     && victim->fighting->getNPC()->behavior
	     && victim->fighting->getNPC()->behavior->hasDestiny())
	     continue;
	
	   /*
	   * незамысловатый алгоритм выбора самого злого из сражающихся в переменну ech.
	   * этос и прочее нифига не учитывается.
	   */
	   if (victim->alignment < max_evil)
	   {
	     if ( IS_EVIL(victim) )
	     {
		    max_evil = victim->alignment;
		    ech      = victim;
	     }
	     else
		    ech = victim;
	   }
    }//end for
    
    /*
     * нашли преступника. стучим в клантолк правителям и атакуем, если левел позволяет
     */
    if (victim != 0)
    {
	     ch->setClan( clan_ruler );
	     interpret_raw(ch, "cb", "ВНИМАНИЕ!!! %s находится %s в районе %s",
		    victim->getNameP(), 
			 ch->in_room->name, 
			 ch->in_room->area->name);

	       if ( ( ch->getModifyLevel() + 8 > victim->getModifyLevel() )
		       && !is_safe_nomessage( ch, victim ))
	       {
	           interpret_raw( ch, "yell", "%s %s! ЗАЩИЩАЙ НЕВИННЫХ!! СМЕРТЬ ПРЕСТУПНИКАМ!!", victim->getNameP( ), crime );
	           multi_hit( ch, victim );
	       }
	       else
	       {
	          act("$c1 кричит '$t! ТЫ ЕЩЕ ОТВЕТИШЬ ЗА СВОИ ПРЕСТУПЛЕНИЯ!'", ch, victim->getNameP( ), 0, TO_ROOM);
	       }
	       return true;
    }
    
    /*
     * нашли самого злого козла отпущения, атакуем
     */
    if ( ech != 0 && ch->can_see(ech) )
    {
	   act( "$c1 кричит 'ЗАЩИЩАЙ НЕВИННЫХ!! СМЕРТЬ ПРЕСТУПНИКАМ!!", ch, 0, 0, TO_ROOM);
	   multi_hit( ch, ech );
	   return true;
    }

    return false;
}

