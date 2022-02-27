#include "CommandLine.h"
const std::string CommandLine::ourEnabledString ("1");
const std::string CommandLine::ourDisabledString;
CommandLine::CommandLine(int argc, char ** argv)
{
    myExecutableName = argv[0];
    
    for (int i = 1; i<argc; i++)
    {
        std::string s = argv[i];
        if(s.length()!=0)
        {
            if(s[0]=='-')
            {
                int st = 1;
                if(s[1]=='-')
                {
                    st++;
                }
                size_t hasValue = s.find('=',st);
                if(hasValue!=std::string::npos)
                {
                    if(hasValue!=st)
                    {
                        if(hasValue!=s.length())
                        {
                            myParameters[s.substr(st,hasValue-st)] = s.substr(hasValue+1);
                        }
                        else
                        {
                            myParameters[s.substr(st,hasValue-st)] = ourEnabledString;
                        }
                    }
                }
                else
                {
                    myParameters[s.substr(st)] = ourEnabledString;
                }
            }
            else
            {
                myParameters[s] = ourEnabledString;
            }
        }
    }
}

