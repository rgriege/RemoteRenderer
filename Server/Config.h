#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <deque>

struct Game
{
    void stringify(std::ostream& os) const;

    static Game read(std::istream& is);
    static Game create(std::map<std::string, std::string> params);

    const std::string name;
    const std::string path;
    const std::string summary;
    const std::string preview;

private:
    Game(std::string name, std::string path, std::string summary, std::string preview);
};

class Config
{
public:
    Config();
    void load(const std::string& file);
    bool hasMoreGames() const;
    const Game& getNextGame();
    bool hasGame(std::string& name);
    const Game& lookupGame(std::string& name);
    void stringify(std::ostream& os) const;

private:
    std::deque<Game> mGames;
    std::deque<Game>::const_iterator mGamesIterator;
};

#endif