#pragma once
#include "Defines.h"



class Entity {
private:
	int HP;
	int Damage;
	bool bIsDead = false;
	bool bIsPlayer = false;
public:
	Entity() = default;
	Position Location;
	EntityTypes Type;
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
};