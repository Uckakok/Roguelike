#include "pch.h"
#include "Entity.h"

void Entity::ReceiveDamage(int Damage)
{
    HP -= Damage;
    if (HP <= 0) {
        bIsDead = true;
    }
}

bool Entity::IsDead()
{
    return bIsDead;
}

bool Entity::IsPlayer()
{
    return bIsPlayer;
}

int Entity::GetDamage()
{
    return Damage;
}

int Entity::GetHP()
{
    return HP;
}

int Entity::GetMaxHP()
{
    return MaxHP;
}

int Entity::GetSpeed()
{
    return Speed;
}

void Entity::AddMaxHP(int Bonus)
{
    MaxHP += Bonus;
}

void Entity::AddDamage(int Bonus)
{
    Damage += Bonus;
}

void Entity::Heal(int Bonus)
{
    HP += Bonus;
    if (HP > MaxHP) {
        HP = MaxHP;
    }
}
