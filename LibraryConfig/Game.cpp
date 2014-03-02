#include <iostream>
#include <fstream>
#include <stdexcept>
#include <websocketpp/base64/base64.hpp>

#include "Game.h"

Game::Game(std::string name, std::string path, std::string summary, std::string preview)
    : name(name), path(path), summary(summary), preview(preview)
{
}

void Game::stringify(std::ostream& os) const
{
    os << "{\"Name\":\"" << name << "\"";
    if (summary != "")
        os << ",\"Summary\":\"" << summary << "\"";
    if (preview != "")
    {
        os << ",\"Preview\":\"data:image/png;base64,";
        std::ifstream previewIs(preview.c_str(), std::ios_base::binary);
        size_t len = 15;
        char buffer[15];
        while(previewIs.read(buffer, len))
            os << base64_encode(reinterpret_cast<unsigned char*>(buffer), len);
        os << base64_encode(reinterpret_cast<unsigned char*>(buffer), previewIs.gcount()) << "\"";
    }
    os << "}";
}

Game Game::create(std::map<std::string, std::string> params)
{
    if (params.find("Name") == params.end() || params.find("Path") == params.end())
        throw std::logic_error("Name or Path missing");

    {
        if (params.find("Preview") != params.end() && !std::ifstream(params["Preview"].c_str()))
            throw std::logic_error("Invalid Preview");
    }

    return Game(params["Name"], params["Path"], params["Summary"], params["Preview"]);
}

Game Game::parse(std::istream& is)
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
                throw std::logic_error((std::string("Invalid Parameter: ") + buffer).c_str());
            params[buffer.substr(0, splitIdx)] = buffer.substr(splitIdx+1, buffer.size() - splitIdx);
        }
    }
    return create(params);
}

