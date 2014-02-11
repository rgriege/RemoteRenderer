#include "ItemDatabase.h"
#include <iostream>
#include <cstdlib>

ItemDatabase* ItemDatabase::instance = NULL;

ItemDatabase::ItemDatabase()
{
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file("xml/items.xml");
  std::cout << "XML Load result: " << result.description() << std::endl;
  if (result.status != 0) {
  	result = doc.load_file("server/xml/items.xml");
  	std::cout << "XML Load result 2: " << result.description() << " " << result.status << std::endl;
  }
  root = doc.child("database");

  for( pugi::xml_node itemNode = root.child("item"); itemNode; itemNode = itemNode.next_sibling("item")) {
  	std::string name = itemNode.attribute("name").value();
  	int value = itemNode.attribute("value").as_int();
    std::string image = itemNode.attribute("image").value();
    int slot = itemNode.attribute("slot").as_int();
  	Item* i = new Item(name, value, slot, image);
  	for (pugi::xml_node attrNode = itemNode.child("attribute"); attrNode; attrNode = attrNode.next_sibling("attribute")) {
      Attribute* attr = new Attribute();
      attr->stat = attrNode.attribute("stat").as_int();
      attr->amount = attrNode.attribute("amount").as_int();
      attr->duration = attrNode.attribute("duration").as_int();
      i->addAttribute(attr);
  	}
  	items.push_back(i);
  }
  std::cout << items.size() << " items loaded into database" << std::endl;
}

ItemDatabase* ItemDatabase::GetInstance()
{
  if (!instance)
  	instance = new ItemDatabase();
  return instance;
}

Item* ItemDatabase::getItem(int idx)
{
  return items[idx];
}

// gets a random (non-gold) item
Item* ItemDatabase::getRandomItem()
{
  return getItem(rand() % items.size());
}