#ifndef GAME_H
#define GAME_H

#include <string>
#include <map>
#include <deque>
#include <json/value.h>

struct Game
{
    Game(std::string name, std::string path, std::string summary, std::string preview);

    static Game create(Json::Value object);
    Json::Value serialize() const;

    bool runnable() const;

    const std::string name;
    const std::string path;
    const std::string summary;
    const std::string preview;
};

#endif // GAME_H
