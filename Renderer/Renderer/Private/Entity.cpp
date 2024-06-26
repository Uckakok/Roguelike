#include "pch.h"
#include "Entity.h"


Entity::Entity(EntityTypes NewType, Position NewPosition, int NewHP, int NewMaxHP, int NewDamage, int NewSpeed) 
    : Type(NewType), Location(NewPosition), m_hp(NewHP), m_maxHp(NewMaxHP), m_Damage(NewDamage), m_speed(NewSpeed) 
{
    if (NewType == EntityTypes::PlayerEntity) 
    {
        m_bIsPlayer = true;
    }
};

Entity::Entity(const Entity& NewEntity) : m_hp(NewEntity.m_hp), m_Damage(NewEntity.m_Damage),
    m_bIsDead(NewEntity.m_bIsDead), m_bIsPlayer(NewEntity.m_bIsPlayer), Location(NewEntity.Location),
    Type(NewEntity.Type), m_maxHp(NewEntity.m_maxHp), m_speed(NewEntity.m_speed)
{
    ;
}

void Entity::ReceiveDamage(int Damage)
{
    m_hp -= Damage;
    if (m_hp <= 0) 
    {
        m_bIsDead = true;
    }
}

bool Entity::IsDead() const
{
    return m_bIsDead;
}

bool Entity::IsPlayer() const
{
    return m_bIsPlayer;
}

int Entity::GetDamage() const
{
    return m_Damage;
}

int Entity::GetHP() const
{
    return m_hp;
}

int Entity::GetMaxHP() const
{
    return m_maxHp;
}

int Entity::GetSpeed() const
{
    return m_speed;
}

void Entity::AddMaxHP(int Bonus)
{
    m_maxHp += Bonus;
}

void Entity::AddDamage(int Bonus)
{
    m_Damage += Bonus;
}

void Entity::Heal(int Bonus)
{
    m_hp += Bonus;
    if (m_hp > m_maxHp) 
    {
        m_hp = m_maxHp;
    }
}
