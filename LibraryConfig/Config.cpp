#include <iostream>
#include <fstream>
#include <stdexcept>

#include "Config.h"

Config::Config()
{
    mGamesIterator = mGames.begin();
}

bool Config::hasMoreGames() const
{
    return mGamesIterator != mGames.end();
}

const Game& Config::getNextGame()
{
    return *mGamesIterator;
}

bool Config::hasGame(std::string& name)
{
    std::deque<Game>::const_iterator it = mGames.begin(), end = mGames.end();
    for ( ; it != end; ++it)
        if (it->name == name)
            return true;
    return false;
}

const Game& Config::lookupGame(std::string& name)
{
    std::deque<Game>::const_iterator it = mGames.begin(), end = mGames.end();
    for ( ; it != end; ++it)
        if (it->name == name)
            return *it;
    throw std::logic_error(("Game " + name + " not found").c_str());
}

void Config::parse(const std::string& file)
{
    std::filebuf fb;
    if (fb.open ("games.cfg",std::ios::in))
    {
        std::istream is(&fb);
        parse(is);
        fb.close();
    }
    else
    {
        throw std::logic_error("File not found");
    }
}

void Config::parse(std::istream& is)
{
    Game* game = NULL;
    while (is) {
        Game game = Game::parse(is);
        mGames.push_back(game);
        std::cout << "Found game: " << game.name << std::endl;
    }
    mGamesIterator = mGames.begin();
}

void Config::stringify(std::ostream& os) const
{
    os << "[";
    std::deque<Game>::const_iterator it = mGames.begin(), end = mGames.end();
    bool first = true;
    for ( ; it != end; ++it) {
        if (first)
            first = false;
        else
            os << ',';
        it->stringify(os);
    }
    os << "]";
}
