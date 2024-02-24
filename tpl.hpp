#pragma once
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

    std::vector<variable> variables;

    variable* getVariable(const char* var)
    {
        size_t index=0;
        for (variable& i : variables)
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
        for (variable& i : variables)
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
        bool skipping_if=false, skipping_else=false;
        std::ifstream file_s(path);

        std::string line;
        while (std::getline(file_s, line, ';'))
        {
            line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
            line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());

            if(line=="else")
            {
                if(!skipping_if)
                {
                    skipping_else=true;
                }
                else
                {
                    skipping_if=false;
                }
                continue;
            }
            if(line=="endif")
            {
                skipping_if=false;
                skipping_else=false;
                continue;
            }

            if(skipping_if || skipping_else)
            {
                continue;
            }

            std::vector<std::string> args;
            
            tokenize(line, args);

            if(args[0]=="out")
            {
                if(args[1]=="\\n")
                {
                    std::cout<<'\n';
                }
                else
                {
                    std::cout<<args[1];
                }
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
            if(args[0]=="sys")
            {
                system(args[1].c_str());
            }
            if(args[0]=="free")
            {
                size_t len = variables.size();
                for(size_t i=0; i<len; i++)
                {
                    if(variables[i].name == args[1])
                    {
                        variables.erase(variables.begin() + i);
                        break;
                    }
                }
            }
            if(args[0]=="if")
            {
                if(args[1]!=args[2])
                {
                    skipping_if=true;
                }
            }
            if(args[0]=="getvars")
            {
                for(variable var : variables)
                {
                    std::cout<<var.name<<": "<<var.data<<'\n';
                }
            }
            
            if(args[0]=="add")
            {
                int sum = std::stoi(getVariable(args[1].c_str())->data);
                sum += std::stoi(args[2]);
                setVariable(args[1].c_str(), std::to_string(sum).c_str());
            }
            if(args[0]=="sub")
            {
                int sum = std::stoi(getVariable(args[1].c_str())->data);
                sum -= std::stoi(args[2]);
                setVariable(args[1].c_str(), std::to_string(sum).c_str());
            }
            if(args[0]=="mul")
            {
                int sum = std::stoi(getVariable(args[1].c_str())->data);
                sum *= std::stoi(args[2]);
                setVariable(args[1].c_str(), std::to_string(sum).c_str());
            }
            if(args[0]=="div")
            {
                int sum = std::stoi(getVariable(args[1].c_str())->data);
                sum /= std::stoi(args[2]);
                setVariable(args[1].c_str(), std::to_string(sum).c_str());
            }
        }
    }
}
