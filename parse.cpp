#ifndef _Parse_Json
#define _Parse_Json

#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<fstream>
using namespace std;

map<string,string> parse_json(string path){
    fstream file(path,ios::in);
    if(!file.is_open()){
        throw std::runtime_error(path+" not found !");
    }
    char ch;
    string s;
    map<string,string> urls;
    while(file.get(ch)){
        if(ch=='\n' || ch==' '){
            continue;
        }
        s+=ch;
    }
    int length=s.size();
    int i=2;
    bool FIRST=true;
    string t1,t2;
    do{
        if(s.at(i)==','){
            i+=2;
            while(s.at(i)!='\"'){
                t1+=s.at(i++);
            }
            i+=3;
            while(s.at(i)!='\"'){
                t2+=s.at(i++);
            }
            urls[t1]=t2;
        }
        else if(FIRST){
            FIRST=false;
            while(s.at(i)!='\"'){
                t1+=s.at(i++);
            }
            i+=3;
            while(s.at(i)!='\"'){
                t2+=s.at(i++);
            }
            urls[t1]=t2;
        }
        t1="";t2="";
        i++;
    }while(s.at(i)!='}' && i<length);

    return urls;
}

#endif