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
	   * ���������� ��������� � ����������, ��������� ���� �� �����
	   */
      if (IS_SET( ch->in_room->area->area_flag, AREA_HOMETOWN )
		 && number_percent() < 2
		 && !victim->is_immortal( ))
      { 
	     do_say( ch, "� ��� ����?");

	     Room *room = get_room_index( victim->is_npc() ? 0 : victim->getPC()->getHometown( )->getRecall() );
	    
	     /* victim �������� � ������ ���� */
	     if (!room || ch->in_room->area != room->area)
	     {
		    do_say( ch, "� �� ���� ����. ����� �����!");
	     }
	     else /* ��, �� �� ������ */
	     {
		    do_say(ch, "�� �����, ��� ����. � ��������� ���������.");
		    interpret( ch, "smile");
	     }
	   }
	
	   /*
	   * ������� �����������, ��� �������������� ����, ������ �������� ����.
	   * ���������� crime ���� ��� � ��� ���, ����� ���� ����� "CRIMINAL"
	   * �������� "THIEF" ��� "KILLER" (� ������� ��������, ROM-mud).
	   */
	   if ( !victim->is_npc()
		   && ( IS_SET(victim->act, PLR_WANTED)
			|| victim->isAffected(gsn_jail ) ) )
      {
	     crime = "CRIMINAL";
	     break;
      }
	
	   /*
	   * �� ����� �����������, ���� victim - ��������� ��� ���� ����-���
	   */
	   if (victim->is_npc( ) 
	      && victim->getNPC()->behavior
	      && victim->getNPC()->behavior->hasDestiny())
	      continue;
	
	   /*
	   * ��� �����, ���� ������� ��������
	   */
	   if (victim->fighting == 0 || victim->fighting == ch)
	     continue;
	
	   /*
	    * �� ����� ����������� � ����� � ���������� ��� ����-������
	   */
	   if (victim->fighting->is_npc( ) 
	     && victim->fighting->getNPC()->behavior
	     && victim->fighting->getNPC()->behavior->hasDestiny())
	     continue;
	
	   /*
	   * �������������� �������� ������ ������ ����� �� ����������� � ��������� ech.
	   * ���� � ������ ������ �� �����������.
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
     * ����� �����������. ������ � �������� ���������� � �������, ���� ����� ���������
     */
    if (victim != 0)
    {
	     ch->setClan( clan_ruler );
	     interpret_raw(ch, "cb", "��������!!! %s ��������� %s � ������ %s",
		    victim->getNameP(), 
			 ch->in_room->name, 
			 ch->in_room->area->name);

	       if ( ( ch->getModifyLevel() + 8 > victim->getModifyLevel() )
		       && !is_safe_nomessage( ch, victim ))
	       {
	           interpret_raw( ch, "yell", "%s %s! ������� ��������!! ������ ������������!!", victim->getNameP( ), crime );
	           multi_hit( ch, victim );
	       }
	       else
	       {
	          act("$c1 ������ '$t! �� ��� �������� �� ���� ������������!'", ch, victim->getNameP( ), 0, TO_ROOM);
	       }
	       return true;
    }
    
    /*
     * ����� ������ ����� ����� ���������, �������
     */
    if ( ech != 0 && ch->can_see(ech) )
    {
	   act( "$c1 ������ '������� ��������!! ������ ������������!!", ch, 0, 0, TO_ROOM);
	   multi_hit( ch, ech );
	   return true;
    }

    return false;
}

