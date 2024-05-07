#pragma once
#include "Defines.h"



class Entity {
public:
	EntityTypes Type;
	Position Location;
	//stats but who cares

	Entity(EntityTypes NewType, Position NewPosition, int NewHP, int NewDamage) : Type(NewType), Location(NewPosition), HP(NewHP), Damage(NewDamage) { 
		if (NewType == EntityTypes::PlayerEntity) {
			bIsPlayer = true;
		}
	};

	void ReceiveDamage(int Damage);
	bool IsDead();
	bool IsPlayer();
	int GetDamage();
	int GetHP();
private:
	int HP;
	int Damage;
	bool bIsDead = false;
	bool bIsPlayer = false;
};