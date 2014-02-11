#ifndef __Item_h__
#define __Item_h__

#include <string>
#include <vector>

#define ATTR_HEALTH 0
#define ATTR_DAMAGE 1

#define MAX_DURATION 65535

struct Attribute {
  char stat;
  unsigned short int amount;
  unsigned short int duration;
};

class Item
{
public:
  Item(std::string _name, int _value, int _slot, std::string _image);
  virtual ~Item();
  int getValue();
  std::string getName();
  int getIndex();
  std::string getImage();
  int getSlot();
  void addAttribute(Attribute* attr);
  std::vector<Attribute*> getAttributes();
  bool isEquipable();

private:
  int value;
  std::string name;
  int index;
  int slot;
  std::string image;
  std::vector<Attribute*> attributes;
  bool equipable;

  static int num;
};

#endif //__Item_h__
