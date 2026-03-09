#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

#define default_dict "txt"

class Dict
{
public:
    std::string Translate(std::string word)
    {
    }

    Dict(const std::string &dict_path=default_dict)
    {
    }
    ~Dict()
    {
    }

private:
    std::string _dict_path;
    std::unordered_map<std::string, std::string> _dict;
}