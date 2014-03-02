#ifndef GAME_H
#define GAME_H

#include <string>
#include <map>
#include <deque>

struct Game
{
    void stringify(std::ostream& os) const;

    static Game parse(std::istream& is);
    static Game create(std::map<std::string, std::string> params);

    const std::string name;
    const std::string path;
    const std::string summary;
    const std::string preview;

private:
    Game(std::string name, std::string path, std::string summary, std::string preview);
};

#endif // GAME_H
