#include <fstream>
#include <websocketpp/base64/base64.hpp>

#include "Game.h"

Game::Game(std::string name, std::string path, std::string summary, std::string preview)
    : name(name), path(path), summary(summary), preview(preview)
{
}

Game* Game::create(Json::Value object)
{
    return new Game(object.get("Name", "").asString(),
                    object.get("Path", "").asString(),
                    object.get("Summary", "").asString(),
                    object.get("Preview", "").asString());
}

Json::Value Game::serialize() const
{
    Json::Value object;
    if (!name.empty())
        object["Name"] = name;
    if (runnable())
        object["Path"] = path;
    if (!summary.empty())
        object["Summary"] = summary;
    if (!preview.empty())
        object["Preview"] = preview;
    return object;
}

bool Game::runnable() const
{
    return !!std::ifstream(path.c_str());
}
