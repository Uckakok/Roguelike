#pragma once
#include "Defines.h"



class Entity {
private:
	int HP;
	int MaxHP;
	int Damage;
	bool bIsDead = false;
	bool bIsPlayer = false;
	int Speed;
public:
	Entity() = default;
	Entity(const Entity& NewEntity) : HP(NewEntity.HP), Damage(NewEntity.Damage), bIsDead(NewEntity.bIsDead), bIsPlayer(NewEntity.bIsPlayer), Location(NewEntity.Location), Type(NewEntity.Type), MaxHP(NewEntity.MaxHP), Speed(NewEntity.Speed) { ; };
	Position Location;
	EntityTypes Type;
	//stats but who cares

	Entity(EntityTypes NewType, Position NewPosition, int NewHP, int NewMaxHP, int NewDamage, int NewSpeed) : Type(NewType), Location(NewPosition), HP(NewHP), MaxHP(NewMaxHP), Damage(NewDamage), Speed(NewSpeed) { 
		if (NewType == EntityTypes::PlayerEntity) {
			bIsPlayer = true;
		}
	};

	void ReceiveDamage(int Damage);
	bool IsDead();
	bool IsPlayer();
	int GetDamage();
	int GetHP();
	int GetMaxHP();
	int GetSpeed();
	void AddMaxHP(int Bonus);
	void AddDamage(int Bonus);
	void Heal(int Bonus);
};