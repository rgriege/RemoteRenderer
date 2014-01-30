#include <iostream>
#include <fstream>

#include "Config.h"

Game::Game(std::string name, std::string path, std::string summary, std::string preview)
    : name(name), path(path), summary(summary), preview(preview)
{
}

void Game::stringify(std::ostream& os) const
{
    os << "{\"Name\":\"" << name << "\",\"";
    if (summary != "")
        os << "Summary\":\"" << summary << "\",\"";
    if (preview != "")
    {
        os << "Preview\":\"";
        std::ifstream previewIs(preview);
        while(previewIs)
            os << char(previewIs.get());
    }
    os << "\"}";
}

Game Game::create(std::map<std::string, std::string> params)
{
    if (params.find("Name") == params.end() || params.find("Path") == params.end())
        throw std::exception("Name or Path missing");

    {
        if (params.find("Preview") != params.end() && !std::ifstream(params["Preview"]))
            throw std::exception("Invalid Preview");
    }

    return Game(params["Name"], params["Path"], params["Summary"], params["Preview"]);
}

Game Game::read(std::istream& is)
{
    std::string buffer;
    std::map<std::string, std::string> params;
    while(is)
    {
        char first = is.peek();
        // Stop if end of file or start of new Game; ignore comments and empty lines
        if (!is)
            break;
        else if (first == '[' && params.find("Name") != params.end())
            break;
        else if (first == '\n' || first == '#')
        {
            getline(is, buffer);
            continue;
        }

        // Parse parameter
        getline(is, buffer);
        if (first == '[')
        {
            params["Name"] = buffer.substr(1, buffer.size() - 2);
        }
        else
        {
            unsigned splitIdx = buffer.find('=');
            if (splitIdx == buffer.npos)
                throw std::exception((std::string("Invalid Parameter: ") + buffer).c_str());
            params[buffer.substr(0, splitIdx)] = buffer.substr(splitIdx+1, buffer.size() - splitIdx);
        }
    }
    return create(params);
}

Config::Config()
{
    mGamesIterator = mGames.begin();
}

void Config::load(const std::string& file)
{
    std::filebuf fb;
    if (fb.open ("games.cfg",std::ios::in))
    {
        std::istream is(&fb);
        Game* game = NULL;
        while (is)
        {
            Game game = Game::read(is);
            mGames.push_back(game);
            std::cout << "Found game: " << game.name << std::endl;
        }
        mGamesIterator = mGames.begin();
        fb.close();
    }
    else
    {
        throw std::exception("File not found");
    }
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
    throw std::exception(("Game " + name + " not found").c_str());
}

void Config::stringify(std::ostream& os) const
{
    os << "{\"Games\":[";
    std::deque<Game>::const_iterator it = mGames.begin(), end = mGames.end();
    bool first = true;
    for ( ; it != end; ++it) {
        if (first)
            first = false;
        else
            os << ',';
        it->stringify(os);
    }
    os << "]}";
}