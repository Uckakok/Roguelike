#pragma once
#include "Defines.h"



class Entity 
{
private:
	int m_hp;
	int m_maxHp;
	int m_Damage;
	bool m_bIsDead = false;
	bool m_bIsPlayer = false;
	int m_speed;
public:
	Entity() = default;
	Entity(const Entity& NewEntity);
	Entity(EntityTypes NewType, Position NewPosition, int NewHP, int NewMaxHP, int NewDamage, int NewSpeed);

	Position Location;
	EntityTypes Type;


	void ReceiveDamage(int Damage);
	bool IsDead() const;
	bool IsPlayer() const;
	int GetDamage() const;
	int GetHP() const;
	int GetMaxHP() const;
	int GetSpeed() const;
	void AddMaxHP(int Bonus);
	void AddDamage(int Bonus);
	void Heal(int Bonus);
};