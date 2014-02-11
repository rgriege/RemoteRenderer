#ifndef __ItemDatabase_h__
#define __ItemDatabase_h__

#include "Item.h"
#include "pugixml.hpp"

class ItemDatabase
{
private:
  ItemDatabase();
  ItemDatabase(ItemDatabase const&) {};
  ItemDatabase& operator=(ItemDatabase const&) {};
  virtual ~ItemDatabase() {};

public:
  static ItemDatabase* GetInstance();
  Item* getItem(int idx);
  Item* getRandomItem();

private:
  static ItemDatabase* instance;

  std::string file;
  std::vector<Item*> items;
  pugi::xml_node root;
};

#endif