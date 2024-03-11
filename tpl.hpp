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
                    continue;
                }
            }
            if(ch==' ' && !in_string)
            {
                if(token=="\\n") { to.push_back("\n"); }
                else if(token=="\\t") { to.push_back("\t"); }
                else if(token[0]=='%')
                {
                    token.erase(0,1);
                    to.push_back(getVariable(token.c_str())->data);
                }
                else { to.push_back(token); }

                token.clear();
                continue;
            }
            token.push_back(ch);
        }
        if(token!="")
        {
            if(token=="\\n") { to.push_back("\n"); }
            else if(token=="\\t") { to.push_back("\t"); }
            else if(token[0]=='%')
            {
                token.erase(0,1);
                to.push_back(getVariable(token.c_str())->data);
            }
            else { to.push_back(token); }

            token.clear();
        }
    }

    bool interpret(std::string& line, bool& skipping_if, bool& skipping_else)
    {
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
            return true;
        }
        if(line=="endif")
        {
            skipping_if=false;
            skipping_else=false;
            return true;
        }

        if(skipping_if || skipping_else)
        {
            return true;
        }

        std::vector<std::string> args;
            
        tokenize(line, args);

        if(args[0]=="out")
        {
            bool first=true;
            for(std::string arg : args)
            {
                if(first) { first=false; continue; }
                std::cout<<arg;
            }
        }
        if(args[0]=="set")
        {
            setVariable(args[1].c_str(), args[2].c_str());
        }
        if(args[0]=="cat")
        {
            variable* var = getVariable(args[1].c_str());
            
            bool first=true;
            bool second=false;
            for(std::string arg : args)
            {
                if(first) { first=false; second=true; continue; }
                if(second) { second=false; continue; }
                var->data += arg;
            }
        }
        if(args[0]=="in")
        {
            std::string input;
            std::getline(std::cin, input);
            setVariable(args[1].c_str(), input.c_str());
        }
        if(args[0]=="read")
        {
            std::ifstream file(args[1]);
            std::string file_read;

            char c; 
            while (file >> std::noskipws >> c) 
            { 
                file_read.push_back(c);
            }
            
            file.close();

            setVariable(args[2].c_str(), file_read.c_str());
        }
        if(args[0]=="write")
        {
            std::ofstream file(args[1]);
            file<<args[2];
            file.close();
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
            variable* var = getVariable(args[1].c_str());
            int sum = std::stoi(var->data);
            sum += std::stoi(args[2]);
            var->data = std::to_string(sum);
        }
        if(args[0]=="sub")
        {
            variable* var = getVariable(args[1].c_str());
            int sum = std::stoi(var->data);
            sum -= std::stoi(args[2]);
            var->data = std::to_string(sum);
        }
        if(args[0]=="mul")
        {
            variable* var = getVariable(args[1].c_str());
            int sum = std::stoi(var->data);
            sum *= std::stoi(args[2]);
            var->data = std::to_string(sum);
        }
        if(args[0]=="div")
        {
            variable* var = getVariable(args[1].c_str());
            int sum = std::stoi(var->data);
            sum /= std::stoi(args[2]);
            var->data = std::to_string(sum);
        }

        return false;
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

            if(interpret(line, skipping_if, skipping_else)) continue;
        }

        file_s.close();
    }
}