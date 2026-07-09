#include "parser/URLParser.h"
#include <cctype> //isalpha , tolower
#include <string>

enum class State{
    Scheme,
    Slash1,
    Slash2,
    Authority,
    Port,
    Path,
    Query,
    Done 
};


ParsedURL URLParser::parse(std::string_view url)const{
    ParsedURL result;
    State state = State::Scheme;
    size_t start=0;
    bool inIPv6 = false;
    bool IPv6started = false;
    bool credseen =false;//to handle double @ 
    for(size_t i=0;i<url.length();i++)
    {
        char c = url[i];
        switch(state)
        {
            //for the http part
            case State::Scheme:
                if(c==':')
                {
                    result.scheme = url.substr(start,i-start);
                    state = State::Slash1;
                }
                //for other language chars out of the -128 to 127 range
                else if(i == 0) 
                {
                    if(!std::isalpha(static_cast<unsigned char>(c)))//for first letter in scheme which should be a alphabet
                        return result;
                }
                else
                {
                    if(!std::isalnum(static_cast<unsigned char>(c)) &&c != '+' &&c != '-' &&c != '.')//after it the scheme could be numerical too
                        return result;
                }
                break;
            
            case State::Slash1:
                if(c!='/')return result;
                state = State::Slash2;
                break;
            
            case State::Slash2:
                if(c!='/')return result;
                state = State::Authority;
                start = i+1;
                break;
                
            case State::Authority:
                if(std::isspace(static_cast<unsigned char>(c)))return result;
                if(c=='/'&& inIPv6 == false)
                {  
                    if(i==start)return result; 
                    result.host = url.substr(start,i-start);
                    state = State::Path;
                    start = i;
                    break;   
                }
                else if(c=='[')
                {
                    if(IPv6started)return result;
                    if(i!=start)return result; //to avoid [ in between host
                    inIPv6=true;
                    IPv6started=true;
                }
                else if(c==']')
                {
                    if(!inIPv6)return result;
                    inIPv6=false;
                }
                else if(c=='@'&& inIPv6 == false)
                {
                    if(credseen)return result;
                    if(i==start)return result; //return in https://@google.com
                    result.credentials = url.substr(start,i-start);
                    credseen=true;
                    if(result.credentials.empty())return result;
                    start = i+1;
                }
                else if(c==':' && inIPv6 == false)
                {
                    if(i==start)return result;
                    result.host = url.substr(start,i-start);
                    result.hasport = true;
                    state = State::Port;
                    start=i+1;
                }
                else if(c=='?'&& inIPv6 == false)
                {
                    if(i==start)return result;
                    result.host = url.substr(start,i-start);
                    state = State::Query;
                    start=i+1;
                }
                else if(c=='#'&& inIPv6 == false)
                {
                    if(i==start)return result;
                    result.host = url.substr(start,i-start);
                    state = State::Done;
                }
                break;

            case State::Path:
                if(c=='?')
                {
                    result.path = url.substr(start,i-start);
                    state = State::Query;
                    start=i+1;
                }
                else if(c=='#')
                {
                    result.path = url.substr(start,i-start);
                    state = State::Done;
                }
                break;

            case State::Port:
                if(c=='/')
                {
                    if(i==start)return result;
                    result.port = url.substr(start,i-start);
                    state = State::Path;
                    start=i;
                }
                else if(c=='?')
                {
                    if(i==start)return result;
                    result.port = url.substr(start,i-start);
                    state = State::Query;
                    start=i+1;
                }
                else if(c=='#')
                {
                    if(i==start)return result;
                    result.port = url.substr(start,i-start);
                    state = State::Done;
                }
                else if(!std::isdigit(static_cast<unsigned char>(c)))
                {
                    return result;
                }
                break;
            
            case State::Query:
                if(c=='#')
                {
                    result.query = url.substr(start,i-start);
                    state = State::Done;
                }
                break;
            case State::Done:
                break;
            }
        if(state == State::Done)
        {
            break;
        }
        }
    //cleanup
    if(state== State::Authority)
    {
        result.host = url.substr(start);
    }
    else if(state== State::Path)
    {
        result.path = url.substr(start);
    }
    else if(state == State::Port)
    {
        result.port = url.substr(start);
        if(result.port.empty())return result;
    }
    else if(state == State::Query)
    {
        result.query = url.substr(start);
    }

    //validation
    
    if(!result.port.empty())
    {
        int port = 0;
        for(char c : result.port)
        {
            port = port * 10 + (c - '0');
            if(port > 65535)
                return result;
        }
        if(port == 0)
            return result;
    }
    if(inIPv6)return result; //[ this brac was not closed
    if(IPv6started) // reject malformed IPv6
    {
        if(result.host.empty())return result;
        if(result.host.front()!='[' || result.host.back()!=']')return result; 
    }
    std::string scheme(result.scheme);
    for(char& c : scheme)
    {
        c = std::tolower(static_cast<unsigned char>(c));
    }

    if((scheme == "http" ||scheme == "https") &&!result.host.empty())
    {
        result.valid = true;
    }
    return result;
}
