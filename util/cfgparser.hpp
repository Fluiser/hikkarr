#pragma once

#include <fstream>
#include <string>
#include <map>
#include <iostream>
#include <cstring>

std::map<std::string, std::string> cfg_parse(const char* path)
{
    std::ifstream ifs(path, std::ios::in);
    std::string srcText((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    ifs.close();

    int keyStart = -1;
    int keyEnd = -1;
    int valStart = -1;
    int valEnd = -1;

    std::map<std::string, std::string> values;

    for(int i = 0; i < srcText.size(); ++i)
    {
        const char& c = srcText[i];
        if(c == '\n' || c == '\r'){
            if(keyStart < 0 || valStart < 0) continue;
            for(int j = valEnd+valStart; j>=0; --j){
                if(srcText[j] != ' ' && srcText[j] != '\n' && srcText[j] != '\r') {
                    valEnd = j-valStart+1;
                    break;
                }
            }
            values.emplace(
                    srcText.substr(keyStart, keyEnd-keyStart+1),
                    srcText.substr(valStart, valEnd)
            );

            keyStart = keyEnd = valStart = valEnd = -1;
        } else if(c == '=') {
            for(int j = i-1; j > 0; --j)
            {
                if(srcText[j] != ' ') {
                    keyEnd = j;
                    break;
                }
            }
        } else {
            if (c == ' ' && keyStart < 0) continue;
            if(c == ' ' && valStart < 0 && keyStart > -1) continue;
            if(keyStart < 0) keyStart = i;
            if(keyEnd > -1) {
                if(valStart < 0) valStart = i;
                if(valStart > -1) {
                    if(valEnd < 0) {
                        valEnd = 1;
                    } else {
                        ++valEnd;
                    }
                }
            }
        }
    }

    if(keyStart > -1 || valStart > -1) {
        for(int j = valEnd+valStart; j>=0; --j){
            if(srcText[j] != ' ' && srcText[j] != '\n' && srcText[j] != '\r') {
                valEnd = j-valStart+1;
                break;
            }
        }
        values.emplace(
                srcText.substr(keyStart, keyEnd-keyStart+1),
                srcText.substr(valStart, valEnd)
        );

        keyStart = keyEnd = valStart = valEnd = -1;
    }

    memset(srcText.data(),  0, srcText.size());

    return values;
}
