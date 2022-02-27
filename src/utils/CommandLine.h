#pragma once

#include <string>
#include <unordered_map>
#include <optional>

class CommandLine
{
public:
    typedef std::unordered_map<std::string, std::string> TStorage;
public:
    CommandLine(int argc, char ** argv);

    inline const std::string& operator[] (const char* key) const
    {
        return operator[](std::string(key));
    }
    inline const std::string& operator[] (const std::string& key) const
    {
        auto it = myParameters.find(key);
        if(it == myParameters.end())
        {
            return ourDisabledString;
        }
        return (it->second);
    }

    inline auto begin() const {return myParameters.cbegin();}
    inline auto end() const {return myParameters.cend();}

    inline auto size() const {return myParameters.size();}

    std::string myExecutableName;
private:
    TStorage myParameters;

    static const std::string ourEnabledString;
    static const std::string ourDisabledString;
};