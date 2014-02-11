#include "Item.h"

int Item::num = 0;

Item::Item(std::string _name, int _value, int _slot, std::string _image)
{
  value = _value;
  name = _name;
  slot = _slot;
  image = _image;
  index = num++;
  equipable = false;
}

Item::~Item()
{
}

int Item::getValue()
{
  return value;
}

std::string Item::getName()
{
  return name;
}

int Item::getIndex()
{
  return index;
}

std::string Item::getImage()
{
	return image;
}

int Item::getSlot()
{
  return slot;
}

bool Item::isEquipable()
{
  return equipable;
}

std::vector<Attribute*> Item::getAttributes()
{
	return attributes;
}

void Item::addAttribute(Attribute* attr)
{
  if (attr->duration == MAX_DURATION)
    equipable = true;
  attributes.push_back(attr);
}
