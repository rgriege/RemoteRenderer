#include "Character.h"
#include "MotionState.h"
#include "ItemDatabase.h"
#include <Ogre.h>

Character::Character(Ogre::String _name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height, std::string _objName) : GameObject(_name)
{
  height = _height;
  speed = 0.02f;
  mesh = _objName;
  obj = _objName;
  finished = false;
  deathTimer = 0;
  givenExp = false;
  level = 1;
  experience = 0;
  healthChange = 0;
  gold = 0;
  changes = 255;

  if (_sceneMgr)
    addToGraph(_sceneMgr);

  // set up Bullet properties
  btTransform* transform = new btTransform(btMatrix3x3::getIdentity());
  MotionState *motionState = new MotionState(node, *transform);

  btCollisionShape *shape = new btBoxShape(btVector3(height/2, height/2, height/2));
  //btCollisionShape *shape = GenerateShape(obj, 1.f);

  btScalar mass(1.f);
  btVector3 localInertia(0,0,0);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
  body = new btRigidBody(rbInfo);
  body->setLinearFactor(btVector3(1.f,0.f,1.f));
  body->forceActivationState(DISABLE_DEACTIVATION);
  body->setRestitution(1.f);
  body->setFriction(0.f);
  body->setUserPointer((void *)this);
}

Character::Character(Ogre::String _name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height) : GameObject(_name)
{
  height = _height;
  speed = 0.02f;
  mesh = "cube.mesh";
  finished = false;
  deathTimer = 0;
  givenExp = false;
  level = 1;
  experience = 0;
  healthChange = 0;
  gold = 0;
  changes = 255;

  if (_sceneMgr) {
    addToGraph(_sceneMgr);
    node->translate(Ogre::Vector3(0, height/2.f, 0));
  }

  // set up Bullet properties
  btTransform* transform = new btTransform(btMatrix3x3::getIdentity());
  MotionState *motionState = new MotionState(node, *transform);

  btCollisionShape *shape = new btBoxShape(btVector3(height/2, height/2, height/2));

  btScalar mass(1.f);
  btVector3 localInertia(0,0,0);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
  body = new btRigidBody(rbInfo);
  body->setLinearFactor(btVector3(1.f,0.f,1.f));
  body->forceActivationState(DISABLE_DEACTIVATION);
  body->setRestitution(1.f);
  body->setFriction(0.f);
  body->setUserPointer((void *)this);
  body->translate(btVector3(0, height/2.f, 0));
}

void Character::addToGraph(Ogre::SceneManager* _sceneMgr)
{
  sceneMgr = _sceneMgr;

  // set up Ogre properties
  entity = sceneMgr->createEntity(name, mesh);

  // Create Character materials
  /*Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(name+"Mat",
    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  Ogre::TextureUnitState* tuisTexture = mat->getTechnique(0)->getPass(0)->createTextureUnitState("checker.mtl");
  entity->setMaterial(mat);*/

  node = sceneMgr->getRootSceneNode()->createChildSceneNode(name);
  node->attachObject(entity);

  Ogre::Real cur_height = entity->getBoundingBox().getSize().y;
  node->scale(height/cur_height, height/cur_height, height/cur_height);
  //std::string str = Ogre::StringConverter::toString(radius);
  //TRACE(str);
}

Character::~Character(void)
{
  if (node) {
    node->detachObject(entity);
    sceneMgr->destroyEntity(entity);
    sceneMgr->destroySceneNode(node);
  }
}

void Character::translate(const Ogre::Vector3 displacement)
{
  if (node)
    node->translate(displacement);
  body->translate(btVector3(displacement.x, displacement.y, displacement.z));
  changes = changes | (1 << POSITION_CHANGE);
}

void Character::setPosition(const Ogre::Vector3 position)
{
  if (node) {
    Ogre::Vector3 diff = node->getPosition() - position;
    node->setPosition(position);
    body->translate(btVector3(diff.x, diff.y, diff.z));
  } else {
    btTransform transform = body->getCenterOfMassTransform();
    transform.setOrigin(btVector3(position.x, position.y, position.z));
    body->setCenterOfMassTransform(transform);
  }
  changes = changes | (1 << POSITION_CHANGE);
}

void Character::yaw(const Ogre::Radian radians)
{
  if (node)
      node->yaw(radians);
  btTransform t = body->getCenterOfMassTransform();
  t.setRotation(btQuaternion(0, 1, 0, radians.valueRadians()));
  body->setCenterOfMassTransform(t);
  changes = changes | (1 << ROTATION_CHANGE);
}

void Character::setYaw(const Ogre::Radian radians)
{
  if (node) {
    node->setOrientation(node->getInitialOrientation());
  }
  yaw(radians);
}


void Character::loadXML(pugi::xml_node node)
{
  inGameName = node.child_value("name");
  health = atoi(node.child_value("health"));
  maxHealth = health;
  damage = atoi(node.child_value("damage"));
  experience = atoi(node.child_value("experience"));
  attackspeed = atoi(node.child_value("attackspeed"));
  std::cout << inGameName << " " << health << " " << damage << std::endl;
}

void Character::setMesh(std::string _meshName)
{
  mesh = _meshName;
}

Ogre::Real Character::getHeight()
{
  return height;
}

void Character::update(Ogre::Real dt)
{
}

int Character::getDamage()
{
  return damage;
}

int Character::getHealth()
{
  return health;
}

int Character::getExp()
{
  return experience;
}

int Character::getAttackSpeed()
{
  return attackspeed;
}

float Character::getMaxHealth()
{
  return (float)maxHealth;
}

int Character::getGold()
{
  return gold;
}

std::string Character::getInGameName()
{
  return inGameName;
}

unsigned int Character::getLevel()
{
  return level;
}

int Character::getEquipDamage()
{
  int equipDamage = 0;
  for(std::map<int, Item*>::iterator it = equipItems.begin(); it != equipItems.end(); it++) {
    std::vector<Attribute*> attributes = it->second->getAttributes();
    for(std::vector<Attribute*>::iterator it = attributes.begin(); it != attributes.end(); it++) {
      equipDamage += (*it)->amount;
    }
  }
  return equipDamage;
}

void Character::setDamage(int _damage)
{
  damage = _damage;
}

void Character::setHealth(int _health)
{
  if (_health < 0) _health = 0;
  if (_health >= maxHealth) _health = maxHealth;
  healthChange = _health - health;
  health = _health;
  changes = changes | (1 << HEALTH_CHANGE);
}

void Character::setExp(int _exp)
{
  experience = _exp;
  changes = changes | (1 << EXP_CHANGE);
}

void Character::setAttackSpeed(int _attackspeed)
{
  attackspeed = _attackspeed;
}

void Character::setMaxHealth(float _maxhealth)
{
  maxHealth = _maxhealth;
}

void Character::setGold(int _gold)
{
  gold = _gold;
  changes = changes | (1 << GOLD_CHANGE);
}

bool Character::isDead()
{
  return health <= 0;
}

void Character::setInGameName(std::string _inGameName)
{
  inGameName = _inGameName;
} 

void Character::setLevel(unsigned int _level)
{
  maxHealth = 200 * pow(1.5, (_level - level));
  level = _level;
  changes = changes | (1 << LEVEL_CHANGE);
}

bool Character::isFinished()
{
  return finished;
}

void Character::hasAlreadyGivenExp()
{
  givenExp = true;
}

bool Character::hasGivenExp()
{
  return givenExp;
}

std::vector<Item*> Character::getInventory()
{
  return inventory;
}

void Character::addItem(Item* item)
{
  changes = changes | (1 << INVENTORY_CHANGE);
  inventory.push_back(item);
  printf("item added\n");
}

void Character::addItem(int idx)
{
  addItem(ItemDatabase::GetInstance()->getItem(idx));
}

Item* Character::getItem(std::string name)
{
  for(std::vector<Item*>::iterator it = inventory.begin(); it != inventory.end(); it++) {
    if ((*it)->getName().compare(name) == 0) {
      return (*it);
    }
  }
  return NULL;
}

Item* Character::getItemByIndex(int index)
{
  int i = 0;
  for(std::vector<Item*>::iterator it = inventory.begin(); it != inventory.end(); it++) {
    if (i == index) {
      return (*it);
    }
    i++;
  }
  return NULL;
}

void Character::removeItem(std::string name)
{
  bool found = false;
  for(std::vector<Item*>::iterator it = inventory.begin(); it != inventory.end(); it++) {
    if ((*it)->getName().compare(name) == 0) {
      it = inventory.erase(it);
      changes = changes | (1 << INVENTORY_CHANGE);
      printf("item removed\n");
      found = true;
      break;
    }
  }
  if (!found) printf("item %s not found\n", name.c_str());
}

void Character::removeItemByIndex(int index)
{
  bool found = false;
  int i = 0;
  for(std::vector<Item*>::iterator it = inventory.begin(); it != inventory.end(); it++) {
    if (i == index) {
      it = inventory.erase(it);
      changes = changes | (1 << INVENTORY_CHANGE);
      printf("item removed\n");
      found = true;
      break;
    }
    i++;
  }
  if (!found) printf("item %s not found\n", name.c_str());
}

void Character::clearInventory()
{
  inventory.clear();
  changes = changes | (1 << INVENTORY_CHANGE);
}

std::map<int, Item*> Character::getEquipItems()
{
  return equipItems;
}

Item* Character::getEquipItem(int index)
{
  return equipItems[index];
}

void Character::setEquipItem(int index, Item* item)
{
  printf("Setting Equip Items\n");
  equipItems[index] = item;
  std::cout << "Added: " << equipItems[index]->getName() << std::endl;
  changes = changes | (1 << EQUIP_CHANGE);
}

void Character::removeEquipItem(int index)
{
  equipItems.erase(index);
  changes = changes | (1 << EQUIP_CHANGE);
}

void Character::clearEquipItems()
{
  equipItems.clear();
  changes = changes | (1 << EQUIP_CHANGE);
  printf("Done with clear\n");
}

unsigned char Character::getChanges()
{
  return changes;
}

void Character::clearChanges()
{
  changes = 0;
}

void Character::changeAll()
{
  changes = 255;
}