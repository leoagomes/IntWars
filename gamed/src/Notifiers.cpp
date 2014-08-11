#include "stdafx.h"
#include "Game.h"
#include "Packets.h"

#include <iostream>

using namespace std;

void Game::notifyMinionSpawned(Minion* m) {
   MinionSpawn ms(m);
   broadcastPacket(reinterpret_cast<uint8*>(&ms),sizeof(ms), CHL_S2C);
   notifySetHealth(m);
}

void Game::notifySetHealth(Unit* u) {
   SetHealth sh(u);
   broadcastPacket(sh, CHL_S2C);
}

void Game::notifyUpdatedStats(Unit* u) {
   UpdateStats us(u);
   broadcastPacket(us, CHL_LOW_PRIORITY, 2);
}

void Game::notifyTeleport(Unit* u){
    
   // TeleportRequest first(u->getNetId(), u->teleportToX, u->teleportToY, true);
    
   // sendPacket(currentPeer, first, CHL_S2C);
       
    TeleportRequest second(u->getNetId(), u->teleportToX, u->teleportToY, false);
    broadcastPacket(second, CHL_S2C);
    
    u->needsToTeleport = false;
       
    //todo check for vision? and send to each player 
    
}

void Game::notifyMovement(Object* o) {
   const std::vector<MovementVector>& waypoints = o->getWaypoints();
   MovementAns *answer = MovementAns::create(waypoints.size()*2);
   
   for(size_t i = 0; i < waypoints.size(); i++) {
      printf("     Vector %lu, x: %f, y: %f\n", i, 2.0 * waypoints[i].x + MAP_WIDTH, 2.0 * waypoints[i].y + MAP_HEIGHT);
   }
   
   answer->nbUpdates = 1;
   answer->netId = o->getNetId();
   for(size_t i = 0; i < waypoints.size(); i++) {
      answer->getVector(i)->x = waypoints[i].x;
      answer->getVector(i)->y = waypoints[i].y;
   }
   
   broadcastPacket(reinterpret_cast<uint8 *>(answer), answer->size(), 4);
   MovementAns::destroy(answer);
}

void Game::notifyDamageDone(Unit* source, Unit* target, float amount) {
   DamageDone dd(source, target, amount);
   broadcastPacket(dd, CHL_S2C);
}

void Game::notifyAutoAttack(Unit* attacker, Unit* victim, uint32 futureProjNetId) {
   AutoAttack aa(attacker, victim, futureProjNetId);
   broadcastPacket(aa, CHL_S2C);
}

void Game::notifyProjectileSpawn(Projectile* p) {
   SpawnProjectile sp(p);
   broadcastPacket(sp, CHL_S2C);
}

void Game::notifyProjectileDestroy(Projectile* p) {
   DestroyProjectile dp(p);
   broadcastPacket(dp, CHL_S2C);
}

void Game::notifyParticleSpawn(Champion* source, Target* target, const std::string& particleName) {
   SpawnParticle sp(source, target, particleName);
   broadcastPacket(sp, CHL_S2C);
}

void Game::notifyModelUpdate(Unit* object) {
    UpdateModel mp(object->getNetId(), object->getModel().c_str());
    broadcastPacket(reinterpret_cast<uint8 *>(&mp), sizeof(UpdateModel), CHL_S2C);
}

void Game::notifyInventory(Champion* c) {

   auto items = c->getInventory().getItems();

   for(int i = 0; i < 7 ; ++i) {
      if(items[i].first == 0) {
         continue;
      }
      
      BuyItemAns response;
      response.header.netId = c->getNetId();
      response.itemId = items[i].first->getTemplate()->getId();
      response.slotId = i;
      response.stack = c->getInventory().getItems()[i].second;
      broadcastPacket(reinterpret_cast<uint8 *>(&response), sizeof(response), CHL_S2C);
   }
}