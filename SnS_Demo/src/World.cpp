#include "World.h"
#include <sstream>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include "btBulletDynamicsCommon.h"
#include "SoundManager.h"
#include "Plane.h"
#include "StringHelper.h"
#include "GameState.h"
#include "ItemDatabase.h"
#include <sstream>

bool HandleContacts(btManifoldPoint& point, btCollisionObject* body0, btCollisionObject* body1);

extern ContactProcessedCallback gContactProcessedCallback;

#define INT(data) (*static_cast<int*>(data))
#define CHAR(data) (*static_cast<char*>(data))
#define UCHAR(data) (*static_cast<unsigned char*>(data))
#define FLOAT(data) (*static_cast<float*>(data))
#define INCR(data, num) ((data) = (static_cast<char*>(data) + num))

World::World(Ogre::SceneManager* m_pSceneMgr)
{
  gContactProcessedCallback = (ContactProcessedCallback)HandleContacts;
  btBroadphaseInterface *BroadPhase = new btAxisSweep3(btVector3(-1000,-1000,-1000), btVector3(1000,1000,1000));
  btDefaultCollisionConfiguration *CollisionConfiguration = new btDefaultCollisionConfiguration();
  btCollisionDispatcher *Dispatcher = new btCollisionDispatcher(CollisionConfiguration);
  btSequentialImpulseConstraintSolver *Solver = new btSequentialImpulseConstraintSolver();
  btWorld = new btDiscreteDynamicsWorld(Dispatcher, BroadPhase, Solver, CollisionConfiguration);

  paused = false;
  sceneMgr = m_pSceneMgr;

  //Sets skydome
  m_pSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

  // Create Plane
  Plane *floor = new Plane("floorNode", m_pSceneMgr, Ogre::Vector3::UNIT_Y, Ogre::Vector3::UNIT_Z, 0);
  addObject(floor);

  // Create Cubes
  for (int i = 0; i < 51; i++) {
    if(i < 50)
    {
	    std::stringstream name;
	    name << "cube" << i;
	    Enemy* e = new Enemy(name.str(), m_pSceneMgr, 10.f, CUBE);
	    addObject(e);
	    //chars.push_back(e);
	    chars[i] = e;
    }
    else
    {
	Enemy* e = new Enemy("Boss", m_pSceneMgr, 50.f, BOSS);
	addObject(e);
	//chars.push_back(e);
	chars[i] = e;
    }
  }

  // Create Robots
  /*for (int i = 0; i < 5; i++) {
    std::stringstream name;
    name << "robot" << i;
    Robot* r = new Robot(name.str(), m_pSceneMgr, 100);
    addObject(r);
    //chars.push_back(r);
    chars[name.str()] = r;
  }*/

  loadMap();
}

World::World(Ogre::SceneManager* m_pSceneMgr, void* data)
{
  gContactProcessedCallback = (ContactProcessedCallback)HandleContacts;
  btBroadphaseInterface *BroadPhase = new btAxisSweep3(btVector3(-1000,-1000,-1000), btVector3(1000,1000,1000));
  btDefaultCollisionConfiguration *CollisionConfiguration = new btDefaultCollisionConfiguration();
  btCollisionDispatcher *Dispatcher = new btCollisionDispatcher(CollisionConfiguration);
  btSequentialImpulseConstraintSolver *Solver = new btSequentialImpulseConstraintSolver();
  btWorld = new btDiscreteDynamicsWorld(Dispatcher, BroadPhase, Solver, CollisionConfiguration);

  paused = false;
  sceneMgr = m_pSceneMgr;

  //Sets skydome
  sceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

  // Create Plane
  Plane *floor = new Plane("floorNode", sceneMgr, Ogre::Vector3::UNIT_Y, Ogre::Vector3::UNIT_Z, 0);
  addObject(floor);

  if(!SetData(data))
    printf("bad world setup data\n");

  loadMap();
}

World::~World(void)
{
}

void World::loadMap()
{
  pugi::xml_document mapDoc;
  pugi::xml_parse_result result = mapDoc.load_file("xml/map.xml");
  std::cout << "XML Load result: " << result.description() << " " << result.status << std::endl;
  if (result.status != 0) {
    result = mapDoc.load_file("server/xml/map.xml");
    std::cout << "XML Load result 2: " << result.description() << " " << result.status << std::endl;
  }

  pugi::xml_node root = mapDoc.child("map");
  int treeCount = 0;
  for( pugi::xml_node node = root.child("tree"); node; node = node.next_sibling("tree")){
    std::stringstream treeName; treeName << "tree" << treeCount;
    std::string treeMesh = node.child_value("mesh");
    std::string treeMaterial = node.child_value("material");
    Ogre::Radian treeYaw = Ogre::Radian(::atoi(node.child_value("yaw")));
    pugi::xml_node positionNode = node.child("position");
    pugi::xml_node scaleNode = node.child("scale");
    Ogre::Vector3 treePosition = Ogre::Vector3(positionNode.attribute("x").as_float(),
                                               positionNode.attribute("y").as_float(),
                                               positionNode.attribute("z").as_float());
    Ogre::Vector3 scaleVector = Ogre::Vector3(scaleNode.attribute("x").as_float(),
                                              scaleNode.attribute("y").as_float(),
                                              scaleNode.attribute("z").as_float());

    Ogre::Entity* entity = sceneMgr->createEntity(treeName.str(), treeMesh);
    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("treeMat"+treeCount,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::TextureUnitState* tuisTexture = mat->getTechnique(0)->getPass(0)->createTextureUnitState(treeMaterial);
    Ogre::SceneNode* scenenode = sceneMgr->getRootSceneNode()->createChildSceneNode(treeName.str());
    scenenode->attachObject(entity);
    scenenode->setPosition(treePosition);
    scenenode->yaw(treeYaw);
    scenenode->scale(scaleVector);

    treeCount++;
  }
}

void World::addObject(GameObject *obj)
{
  btWorld->addRigidBody(obj->getRigidBody());
}

void World::step(Ogre::Real dt)
{
  if (!paused){
    //btWorld->stepSimulation(dt);
    for(std::map<int, Character*>::iterator it = chars.begin(); it != chars.end(); ) {
      Character* c = it->second;
      c->update(dt);
      if (c->isFinished()) {
        chars.erase(it);
        delete c;
      } else {
        it++;
      }
    }
    for(std::map<int, Projectile*>::iterator it = projectiles.begin(); it != projectiles.end(); ) {
      Projectile* p = it->second;
      p->update(dt);
      if (p->getContactWithTarget()) {
        projectiles.erase(it);
        delete p;
      }
      it++;
    }
    for(std::map<int, Player*>::iterator it = players.begin(); it != players.end(); ++it) {
      // bandage - change with caution
      if (!it->second) {
        players.erase(it);
        printf("erased null pointer\n");
      } else {
        Player* p = it->second;
        p->update(dt);
        if (p->isDead()) {
          printf("player %i died\n", it->first);
          p->setGold(0);
          p->clearInventory();
          p->clearEquipItems();
          for(std::map<int, Character*>::iterator it2 = chars.begin(); it2 != chars.end(); it2++) {
            Enemy* e = (Enemy*)(it2->second);
            e->removePlayerEnemy(p);
          }
          players.erase(it);
          delete p;
        }
      }
    }
  }
}

Character* World::getObjectByName(std::string name)
{
  Character* result = NULL;
  for(std::map<int, Character*>::iterator it = chars.begin(); it != chars.end(); it++) {
    if( (it->second)->getName().compare(name) == 0 ) {
      result = it->second;
    }
  }
  if (result == NULL) {
    for(std::map<int, Player*>::iterator it = players.begin(); it != players.end(); it++) {
      if(it->second->getName().compare(name) == 0 ) {
        result = it->second;
      }
    }
  }
  return result;
}

void World::injectInput(Ogre::Vector3 step, Ogre::Radian rad, int idx)
{
  players[idx]->translate(step);
  players[idx]->yaw(rad);
}

void World::addPlayer(int idx, std::string name)
{
  // Create Player Character
  Player* p = new Player(name, sceneMgr, 10.f);
  std::cout << "new player '" << name << "' (" << idx << ")" << std::endl;
  addObject(p);
  players[idx] = p;
  for(std::map<int, Character*>::iterator it = chars.begin(); it != chars.end(); it++) {
      Enemy* e = (Enemy*)(it->second);
      e->addPlayerEnemy(p);
  }
}

Player* World::getPlayer(int idx)
{
  return players[idx];
}

void World::removePlayer(int idx)
{
  Player* p = getPlayer(idx);
  for(std::map<int, Character*>::iterator it = chars.begin(); it != chars.end(); it++) {
    Enemy* e = (Enemy*)(it->second);
    e->removePlayerEnemy(p);
  }
  players.erase(idx);
  delete p;
  printf("player %i deleted from world\n", idx);
  printf("%i players left %i\n", players.size(), players.begin() == players.end());
}

void World::setPlayer(pugi::xml_node data, int idx)
{
  players[idx]->setInGameName(data.attribute("name").as_string());
  players[idx]->setDamage(atoi(data.attribute("damage").as_string()));
  players[idx]->setLevel(data.attribute("level").as_int());
  players[idx]->setMaxHealth(data.attribute("health").as_float());
  players[idx]->setHealth(players[idx]->getMaxHealth());
  players[idx]->setExp(data.attribute("experience").as_int());
  players[idx]->setDamage(data.attribute("damage").as_int());
}

void World::addEnemy(int idx)
{
  std::stringstream ss;
  ss << "cube" << idx;
  if(idx < 50)
  	chars[idx] = new Enemy(ss.str(), sceneMgr, 10.f, CUBE);
  else
	chars[idx] = new Enemy("Boss", sceneMgr, 50.f, BOSS);
}

void World::addProjectile(int idx)
{
  printf("adding projectile %i\n", idx);
  projectiles[idx] = new Projectile(sceneMgr, Ogre::Vector3::ZERO, NULL, DEATHCOIL);
  projectiles[idx]->index = idx;
}

void World::addProjectile(char spell, Player* player)
{
  Projectile* proj = new Projectile(sceneMgr, player->getPosition(), player->getTarget(), spell);
  projectiles[proj->index] = proj;
  printf("adding projectile %i\n", proj->index);
}

/* Format:
 * (int msgLength) ('S')
 * (int #players) | (int idx) (opt char* name) (char changes) (float[3] position) (float rotation) (int health) 
                                               (Inventory) (Equip Items) (char level) (int experience) (int gold)
 * (int #enemies) | (int idx) (char changes) (float[3] position) (float rotation) (int health) (Inventory)
 * (int #projectiles) | (int spell) (float[3] position)
 *
 * Inventory Format:
 * (int #items) | (char idx)
 */
void* World::GetData()
{
  using namespace std;

  // get the message length
  int len = 17 + players.size()*5 + chars.size()*5 + projectiles.size()*16;
  // first check for new players
  newPlayer = false;
  for(std::map<int, Player*>::iterator it = players.begin(); it != players.end(); it++)
    if (it->second->getChanges() == 255)
      newPlayer = true;
  // if there is a new player, send everything
  if (newPlayer) {
    for(std::map<int, Player*>::iterator it = players.begin(); it != players.end(); it++) {
      it->second->changeAll();
      len += it->second->getName().length()+1;
    }
  }
  for(std::map<int, Player*>::iterator it = players.begin(); it != players.end(); it++) {
    unsigned char delta = it->second->getChanges();
    if (delta & (1 << POSITION_CHANGE))
      len += 12;
    if (delta & (1 << ROTATION_CHANGE))
      len += 4;
    if (delta & (1 << HEALTH_CHANGE))
      len += 4;
    if (delta & (1 << INVENTORY_CHANGE))
      len += 4 + it->second->getInventory().size();
    if( delta & (1 << EQUIP_CHANGE))
      len += 4 + it->second->getEquipItems().size()*2;
    if (delta & (1 << LEVEL_CHANGE))
      len += 1;
    if (delta & (1 << EXP_CHANGE))
      len += 4;
    if (delta & (1 << GOLD_CHANGE))
      len += 4;
  }
  for(std::map<int, Character*>::iterator it = chars.begin(); it != chars.end(); it++) {
    char delta = it->second->getChanges();
    if (delta & (1 << POSITION_CHANGE))
      len += 12;
    if (delta & (1 << ROTATION_CHANGE))
      len += 4;
    if (delta & (1 << HEALTH_CHANGE))
      len += 4;
    if (delta & (1 << INVENTORY_CHANGE))
      len += 4 + it->second->getInventory().size();
    if (delta & (1 << GOLD_CHANGE))
      len += 4;
  }

  // now allocate and fill the buffer
  void* data = (void*)malloc(len);
  //printf("allocating %i bytes\n", len);
  void* orig = data;
  *(int*)data = len;
  //int _len = INT(data);
  INCR(data, 4);
  *(char*)data = 'S';
  //char _c = CHAR(data);
  INCR(data, 1);

  *(int*)data = players.size();
  //cout << "#players " << INT(data) << endl;
  INCR(data, 4);

  void* overhead = data;

  void* player;
  for(std::map<int, Player*>::iterator it = players.begin(); it != players.end(); it++) {
    player = data;
    *(int*)data = it->first;
    INCR(data, 4);
    if (newPlayer) {
      int nameLen = it->second->getName().length()+1;
      memcpy(data, it->second->getName().c_str(), nameLen);
      printf("sending player %i name data %s\n", it->first, (char*)data);
      INCR(data, nameLen);
    }
    unsigned char delta = it->second->getChanges();
    *(unsigned char*)data = delta;
    INCR(data, 1);
    if (delta & (1 << POSITION_CHANGE)) {
      memcpy(data, it->second->getPosition().ptr(), 12);
      INCR(data, 12);
    }
    if (delta & (1 << ROTATION_CHANGE)) {
      *(float*)data = it->second->getYaw().valueRadians();
      INCR(data, 4);
    }
    if (delta & (1 << LEVEL_CHANGE)) {
      *(unsigned char*)data = (unsigned char)it->second->getLevel();
      INCR(data, 1);
    }
    if (delta & (1 << HEALTH_CHANGE)) {
      *(int*)data = it->second->getHealth();
      INCR(data, 4);
    }
    if (delta & (1 << INVENTORY_CHANGE)) {
      std::vector<Item*> items = it->second->getInventory();
      *(int*)data = items.size();
      INCR(data, 4);
      for (std::vector<Item*>::iterator it = items.begin(); it != items.end(); ++it) {
        *(char*)data = (char)((*it)->getIndex());
        INCR(data, 1);
      }
    }
    if (delta & (1 << EQUIP_CHANGE)) {
      std::map<int, Item*> equipItems = it->second->getEquipItems();
      *(int*)data = equipItems.size();
      INCR(data, 4);
      for (std::map<int, Item*>::iterator it = equipItems.begin(); it != equipItems.end(); ++it) {
        *(char*)data = (char)it->first;
        INCR(data, 1);
        *(char*)data = (char)(it->second->getIndex());
        INCR(data, 1);
      }
    }
    if (delta & (1 << EXP_CHANGE)) {
      *(int*)data = it->second->getExp();
      INCR(data, 4);
    }
    if (delta & (1 << GOLD_CHANGE)) {
      *(int*)data = it->second->getGold();
      INCR(data, 4);
    }
    it->second->clearChanges();
    //ptrdiff_t x = (char*)data - (char*)player;
  }

  *(int*)data = chars.size();
  int _chars = INT(data);
  //cout << "#enemies " << INT(data) << endl;
  INCR(data, 4);
  for(std::map<int, Character*>::iterator it = chars.begin(); it != chars.end(); it++) {
    *(int*)data = it->first;
    INCR(data, 4);
    unsigned char delta = it->second->getChanges();
    *(unsigned char*)data = delta;
    INCR(data, 1);
    if (delta & (1 << POSITION_CHANGE)) {
      memcpy(data, it->second->getPosition().ptr(), 12);
      INCR(data, 12);
    }
    if (delta & (1 << ROTATION_CHANGE)) {
      *(float*)data = it->second->getYaw().valueRadians();
      INCR(data, 4);
    }
    if (delta & (1 << HEALTH_CHANGE)) {
      *(int*)data = it->second->getHealth();
      INCR(data, 4);
    }
    if (delta & (1 << INVENTORY_CHANGE)) {
      std::vector<Item*> items = it->second->getInventory();
      *(int*)data = items.size();
      INCR(data, 4);
      for (std::vector<Item*>::iterator it = items.begin(); it != items.end(); ++it) {
        *(char*)data = (char)((*it)->getIndex());
        INCR(data, 1);
      }
    }
    if (delta & (1 << GOLD_CHANGE)) {
      *(int*)data = it->second->getGold();
      INCR(data, 4);
    }
    it->second->clearChanges();
  }

  *(int*)data = projectiles.size();
  INCR(data, 4);
  for(std::map<int, Projectile*>::iterator it = projectiles.begin(); it != projectiles.end(); it++) {
    *(int*)data = it->second->index;
    INCR(data, 4);
    memcpy(data, it->second->getPosition().ptr(), 12);
    //cout << "receive projectile position " << FLOAT(data) << " " << *(static_cast<float*>(data)+1) << " " << *(static_cast<float*>(data)+2) << endl;
    INCR(data, 12);
  }

  //d = (char*)data - (char*)orig;
  //printf("message len: %td\n", d);
  return orig;
}

bool World::CanSetData(void* data)
{
  void* orig = data;
  int len = INT(data);
  INCR(data, 4);
  char c = CHAR(data);
  if (c != 'S') {
    std::cout << "cannot set world data: " << len << " " << c << "(" << CHAR(orig) << "?)" << std::endl;
    return false;
  }
  return true;
}

/* Format:
 * (int msgLength) ('S')
 * (int #players) | (int idx) (opt char* name) (char changes) (float[3] position) (float rotation) (int health) (Inventory) (Equip Items) (int level) (int experience)
 * (int #enemies) | (int idx) (char changes) (float[3] position) (float rotation) (int health) (Inventory) (NULL)      (NULL)
 * (int #projectiles) | (int spell) (float[3] position)
 *
 * Inventory Format:
 * (int #items) | (char idx)
 */
bool World::SetData(void* data)
{
  using namespace std;
  void* orig = data;
  int len = INT(data);
  INCR(data, 4);
  if(CHAR(data) != 'S') {
    free(orig);
    return false;
  }
  INCR(data, 1);

  int playerCount = INT(data);
  INCR(data, 4);

  newPlayer = players.size() < playerCount;
  if(newPlayer)
    cout << "new player needs to be added to world" << endl;

  for(std::map<int, Player*>::iterator it = players.begin(); it != players.end(); it++) {
    it->second->updated = false;
  }
  for(int i = 0; i < playerCount; i++) {
    int idx = INT(data);
    INCR(data, 4);
    if (newPlayer) {
      std::string name = std::string((char*)data);
      INCR(data, name.length()+1);
      if (players.find(idx) == players.end()) {
        printf("receiving new player %s (%i) data\n", name.c_str(), idx);
        addPlayer(idx, name);
      }
    }
    players[idx]->clearChanges();
    players[idx]->updated = true;
    unsigned char delta = UCHAR(data);
    INCR(data, 1);
    if (delta & (1 << POSITION_CHANGE)) {
      players[idx]->setPosition(Ogre::Vector3(static_cast<float*>(data)));
      INCR(data, 12);
    }
    if (delta & (1 << ROTATION_CHANGE)) {
      players[idx]->setYaw(Ogre::Radian(FLOAT(data)));
      INCR(data, 4);
    }
    if (delta & (1 << LEVEL_CHANGE)) {
      players[idx]->setLevel((unsigned int)UCHAR(data));
      players[idx]->setMaxHealth(players[idx]->getHealth());
      INCR(data, 1);
    }
    if (delta & (1 << HEALTH_CHANGE)) {
      players[idx]->setHealth(INT(data));
      INCR(data, 4);
    }
    if (delta & (1 << INVENTORY_CHANGE)) {
      players[idx]->clearInventory();
      int numItems = INT(data);
      INCR(data, 4);
      for (int j = 0; j < numItems; j++) {
        char itemIdx = CHAR(data);
        INCR(data, 1);
        players[idx]->addItem(ItemDatabase::GetInstance()->getItem(itemIdx));
      }
    }
    if (delta & (1 << EQUIP_CHANGE)) {
      players[idx]->clearEquipItems();
      int numItems = INT(data);
      INCR(data, 4);
      for (int j = 0; j < numItems; j++) {
        char slot = CHAR(data);
        INCR(data, 1);
        char itemIdx = CHAR(data);
        INCR(data, 1);
        players[idx]->setEquipItem(slot, ItemDatabase::GetInstance()->getItem(itemIdx));
      }
    }
    if (delta & (1 << EXP_CHANGE)) {
      players[idx]->setExp(INT(data));
      INCR(data, 4);
    }
    if (delta & (1 << GOLD_CHANGE)) {
      players[idx]->setGold(INT(data));
      INCR(data, 4);
    }
  }
  for(std::map<int, Player*>::iterator it = players.begin(); it != players.end(); it++) {
    if (!it->second->updated) {
      removePlayer(it->first);
    }
  }

  int enemyCount = INT(data);
  INCR(data, 4);
  for(int i = 0; i < enemyCount; i++) {
    int idx = INT(data);
    INCR(data, 4);
    if (chars.find(idx) == chars.end())
      addEnemy(idx);
    chars[idx]->clearChanges();
    char delta = CHAR(data);
    INCR(data, 1);
    if (delta & (1 << POSITION_CHANGE)) {
      chars[idx]->setPosition(Ogre::Vector3(static_cast<float*>(data)));
      INCR(data, 12);
    }
    if (delta & (1 << ROTATION_CHANGE)) {
      chars[idx]->setYaw(Ogre::Radian(FLOAT(data)));
      INCR(data, 4);
    }
    if (delta & (1 << HEALTH_CHANGE)) {
      chars[idx]->setHealth(INT(data));
      INCR(data, 4);
    }
    if (delta & (1 << INVENTORY_CHANGE)) {
      chars[idx]->clearInventory();
      int numItems = INT(data);
      INCR(data, 4);
      for (int j = 0; j < numItems; j++) {
        char itemIdx = CHAR(data);
        INCR(data, 1);
        chars[idx]->addItem(ItemDatabase::GetInstance()->getItem(itemIdx));
      }
    }
    if (delta & (1 << GOLD_CHANGE)) {
      chars[idx]->setGold(INT(data));
      INCR(data, 4);
    }
  }

  for(std::map<int, Projectile*>::iterator it = projectiles.begin(); it != projectiles.end(); it++) {
    it->second->clearStatus();
  }
  int projectileCount = INT(data);
  INCR(data, 4);
  for(int i = 0; i < projectileCount; i++) {
    int idx = INT(data);
    INCR(data, 4);
    if (projectiles.find(idx) == projectiles.end())
      addProjectile(idx);
    //cout << "Attempting to send projectile position " << FLOAT(data) << " " << *(static_cast<float*>(data)+1) << " " << *(static_cast<float*>(data)+2) << endl;
    projectiles[idx]->setPosition(Ogre::Vector3(static_cast<float*>(data)));
    //cout << "Finished sending projectile position " << FLOAT(data) << " " << *(static_cast<float*>(data)+1) << " " << *(static_cast<float*>(data)+2) << endl;
    INCR(data, 12);
  }
  for(std::map<int, Projectile*>::iterator it = projectiles.begin(); it != projectiles.end(); ) {
    Projectile* p = it->second;
    if(!p->isActive()) {
      projectiles.erase(it);
      delete p;
    }
    it++;
  }

  free(orig);
  return true;
}

void World::SetPaused(bool _pause)
{
  paused = _pause;
}

bool HandleContacts(btManifoldPoint& point, btCollisionObject* body0, btCollisionObject* body1)
{
  /*  GameObject *obj0 = ((GameObject*)body0->getUserPointer());
    GameObject *obj1 = ((GameObject*)body1->getUserPointer());
  //printf("contact %s %s\n", obj0->getName().c_str(), obj1->getName().c_str());

  if(obj0->getName().compare("BallNode") == 0 && obj1->getName().compare("floorNode") == 0) {
    SoundManager::GetInstance()->PlaySound("GameOver.wav");
   }*/
  return false;
}

std::vector<Character*> World::checkCharacterHealth()
{
  std::vector<Character*> result;
  for(std::map<int, Character*>::iterator it = chars.begin(); it != chars.end(); it++) {
    Character* c = it->second;
    if (c->healthChange != 0) {
      result.push_back(c);
    }
  }
  return result;
}

bool World::checkPlayerGold(int idx)
{
  if (players[idx]->getChanges() & (1 << GOLD_CHANGE))
    return true;
}

std::map<int, Player*> World::checkNewPlayers()
{
  if (newPlayer)
    return players;
  else
    return std::map<int, Player*>();
}
