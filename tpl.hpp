#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>


namespace tpl
{
    struct variable
    {
        variable(const char* name, const char* data)
        {
            this->name = name;
            this->data = data;
        }
        std::string name;
        std::string data;
    };

    std::vector<tpl::variable> variables;

    variable* getVariable(const char* var)
    {
        size_t index=0;
        for (tpl::variable& i : variables)
        {
            if (i.name == var)
            {
                return &i;
            }
        }
        
        return NULL;
    }

    void setVariable(const char* var, const char* to)
    {
        bool found = false;
        for (tpl::variable& i : variables)
        {
            if (i.name == var)
            {
                i.data = to;
                found = true;
            }
        }
                
        if (!found) variables.emplace_back(var, to);
    }

    void tokenize(std::string& str, std::vector<std::string>& to)
    {
        bool in_string=false;
        std::string token;


        for(char ch : str)
        {
            if(ch=='"')
            {
                if(in_string==false) {in_string=true; continue;}
                if(in_string==true)
                {
                    in_string=false;
                    to.push_back(token);
                    token.clear();
                    continue;
                }
            }
            if(ch==' ' && !in_string)
            {
                if(token[0]=='%')
                {
                    token.erase(0,1);
                    to.push_back(getVariable(token.c_str())->data);
                }
                else
                {
                    to.push_back(token);
                }

                token.clear();
                continue;
            }
            token.push_back(ch);
        }
        if(token!="")
        {
            if(token[0]=='%')
            {
                token.erase(0,1);
                to.push_back(getVariable(token.c_str())->data);
            }
            else
            {
                to.push_back(token);
            }

            token.clear();
        }
    }

    void runFromFile(const char* path)
    {
        std::ifstream file_s(path);

        std::string line;
        while (std::getline(file_s, line, ';'))
        {
            line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
            line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
            std::vector<std::string> args;
            
            tokenize(line, args);

            if(args[0]=="out")
            {
                std::cout<<args[1];
            }
            if(args[0]=="set")
            {
                setVariable(args[1].c_str(), args[2].c_str());
            }
            if(args[0]=="in")
            {
                std::string input;
                std::getline(std::cin, input);
                setVariable(args[1].c_str(), input.c_str());
            }
        }
    }
}