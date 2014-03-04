#include <iostream>
#include <fstream>
#include <stdexcept>
#include <json/reader.h>
#include <json/writer.h>

#include "Config.h"

Config::Config()
{
}

Config::~Config()
{
    std::deque<Game*>::const_iterator it = mGames.begin(), end = mGames.end();
    for ( ; it != end; ++it)
        delete *it;
}

bool Config::hasMoreGames() const
{
    return mGamesIterator != mGames.end();
}

void Config::start()
{
    mGamesIterator = mGames.begin();
}

const Game& Config::getNextGame()
{
    return **(mGamesIterator++);
}

bool Config::hasGame(std::string& name)
{
    std::deque<Game*>::const_iterator it = mGames.begin(), end = mGames.end();
    for ( ; it != end; ++it)
        if ((*it)->name == name)
            return true;
    return false;
}

const Game& Config::lookupGame(std::string& name)
{
    std::deque<Game*>::const_iterator it = mGames.begin(), end = mGames.end();
    for ( ; it != end; ++it)
        if ((*it)->name == name)
            return **it;
    throw std::logic_error(("Game " + name + " not found").c_str());
}

void Config::parseFile(const std::string& filename)
{
    std::filebuf fb;
    if (fb.open (filename.c_str() ,std::ios::in)) {
        std::istream is(&fb);
        parse(is);
        fb.close();
    } else {
        throw std::logic_error("File not found");
    }
}

void Config::parse(std::istream& is)
{
    Json::Reader reader;
    Json::Value library;
    if (!reader.parse(is, library))
        throw std::logic_error("Incorrect JSON");
    for (int i = 0; i < library.size(); i++)
        mGames.push_back(Game::create(library[i]));
}

void Config::stringify(std::ostream& os) const
{
    Json::Value root;
    std::deque<Game*>::const_iterator it = mGames.begin(), end = mGames.end();
    for ( ; it != end; ++it)
        root.append((*it)->serialize());
    os << root;
}
