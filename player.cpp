#include "player.h"

std::string Player::getName() const
{
    return name;
}

void Player::setName(const std::string &newName)
{
    name = newName;
}

Inventory *Player::getInventory() const
{
    return inventory;
}

void Player::setInventory(Inventory *newInventory)
{
    inventory = newInventory;
}

Player::Player()
{
    name = "John";
    inventory = new Inventory();
}

Player::~Player()
{
    delete inventory;
}
