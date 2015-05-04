#ifndef BUFF_H
#define BUFF_H

#include "stdafx.h"
#include <string>

class Unit;
class LuaScript;

enum BuffDuration {
   BUFFTYPE_ETERNAL,
   BUFFTYPE_TEMPORARY
};

enum BuffFlag {
// Generic Properties
   BUFFFLAG_BUFF,
   BUFFFLAG_DEBUFF,
   BUFFFLAG_DISABLE,

// Crowd Control
   BUFFFLAG_AIRBORNE,
   BUFFFLAG_BLIND,
   BUFFFLAG_DISARM,
   BUFFFLAG_FORCED,
   BUFFFLAG_ROOT,
   BUFFFLAG_SILENCE,
   BUFFFLAG_SLOW,
   BUFFFLAG_STASIS,
   BUFFFLAG_STUN,
   BUFFFLAG_SUSPENSION,
   BUFFFLAG_SUPPRESION,

// On Death
   BUFFFLAG_TRANSFER
};

class Buff{
protected:
   float duration;
   float movementSpeedPercentModifier;
   float timeElapsed;
   bool remove;
   Unit* attachedTo;
   Unit* attacker; // who added this buff to the unit it's attached to
   BuffType buffType;
   LuaScript* buffScript;
   
   void init();
   std::string name;

   std::vector <StatMod> statMods;
   std::vector <BuffFlag> flags;

   bool enabled;
public:

   BuffType const getBuffType(){return buffType;}
   Unit* const getUnit(){
      return attachedTo;
   }
   
   Unit* const getSource () {
      return attacker;
   }

   void setName(const std::string& name){
      this->name = name;
   }
   
    
   bool needsToRemove() { return remove; }
   Buff(std::string buffName, float dur, BuffType type, Unit* u, Unit* attacker);
   Buff(std::string buffName, float dur, BuffType type, Unit* u) : Buff(buffName, dur, type, u, u) { } //no attacker specified = selfbuff, attacker aka source is same as attachedto

   float getMovementSpeedPercentModifier() { return movementSpeedPercentModifier;}

   void setMovementSpeedPercentModifier(float modifier){ movementSpeedPercentModifier = modifier; }

   void setEnabled (bool state) {
      this->enabled = state;
   }
   
   std::string getName() const {
      return name;
   }
   
   void setTimeElapsed(float time){
      timeElapsed = time;
   }

   void update(int64 diff);

   std::vector <BuffFlag> getBuffFlags () {
      return this->flags;
   }
   void addFlag (BuffFlag flag) {
      this->flags.push_back (flag);
   }

   std::vector <StatMod>  getStatMods () {
      return this->statMods;
   }
   void addStatMod (StatMod statMod) {
      this->statMods.push_back (statMod);
   }

   void apply (); 
};


#endif	/* BUFF_H */

