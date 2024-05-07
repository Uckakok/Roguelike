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
