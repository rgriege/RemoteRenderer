#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <deque>

#include "LibraryConfigPrereqs.h"
#include "Game.h"

class LIBRARY_CONFIG_API Config
{
public:
    Config();
    ~Config();

    void start();
    const Game& getNextGame();
    bool hasMoreGames() const;

    bool hasGame(std::string& name);
    const Game& lookupGame(std::string& name);

    void parseFile(const std::string& filename = "games.cfg");
    void parse(std::istream& is);
    void stringify(std::ostream& os) const;

private:
    std::deque<Game*> mGames;
    std::deque<Game*>::const_iterator mGamesIterator;
};

#endif
