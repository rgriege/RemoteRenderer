#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <deque>

#include "Game.h"

class Config
{
public:
    Config();

    bool hasMoreGames() const;
    const Game& getNextGame();
    bool hasGame(std::string& name);
    const Game& lookupGame(std::string& name);

    void parse(const std::string& file);
    void parse(std::istream& is);
    void stringify(std::ostream& os) const;

private:
    std::deque<Game> mGames;
    std::deque<Game>::const_iterator mGamesIterator;
};

#endif
