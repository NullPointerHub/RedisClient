#include <vector>
#include <iostream>
#include <regex>
#include "redisclient.h"

using namespace std;

redisclient::redisclient(const string &ip , int port, const string &password) : ip(ip), port(port), password(password) {
    redisclient::init();
}

redisclient::redisclient() :ip("127.0.0.1"), port(6379), password("666") {
    redisclient::init();
}


string redisclient::ping(const string & mess) const{
    string command;
    if(mess == "ping"){
        command = redisclient::command({"ping"});
    } else{
        command = redisclient::command({"ping",mess});
    }
    send(command);
    string feedback = redisclient::recv();
    return redisclient::parse(feedback);
}

const string & redisclient::set(string key,string value) const {
    string command = redisclient::command({"set",key,value});
    send(command);
    string feedback = redisclient::recv();
    const string & fb = redisclient::parse(feedback);
    return fb;
}
const string & redisclient::get(string key) const {
    string command = redisclient::command({"get",key});
    send(command);
    string feedback = redisclient::recv();
    const string & fb = redisclient::parse(feedback);
    return fb;
}
const string & redisclient::del(string key) const {
    string command = redisclient::command({"del",key});
    send(command);
    string feedback = redisclient::recv();
    const string & fb = redisclient::parse(feedback);
    return fb;
}

const string & redisclient::commandkey(string command,string key) const {
    string comm = redisclient::command({command,key});
    send(comm);
    string feedback = redisclient::recv();
    const string & fb = redisclient::parse(feedback);
    return fb;
}

string  redisclient::hmset(string key,initializer_list<string> parameters) const {
    vector<string> items;
    items.push_back("HMSET");
    items.push_back(key);
    for (string item:parameters) {
        items.push_back(item);
    }
    copy(begin(items),end(items),ostream_iterator<string>{cout," "});
    const string &command = redisclient::command(items);
    redisclient::send(command);
    string feedback = redisclient::recv();
    string fb = redisclient::parse(feedback);
    return fb;
}


string redisclient::command(initializer_list<string> parameters) const {
    string mess{};
    int num{0};
    int len{0};
    stringstream ss;
    for (string parameter:parameters) {
        num = num + 1;
        len = parameter.length();
        ss << "$" << parameter.length() << "\r\n" << parameter << "\r\n";
    }
    mess = ss.str();
    ss.str("");
    ss << "*" << num << "\r\n";
    string head = ss.str();
    mess.insert(0, head);
    return mess;
};


string redisclient::command(const vector<string> & parameters) const {
    string mess{};
    int num{0};
    int len{0};
    stringstream ss;
    for (string parameter:parameters) {
        num = num + 1;
        len = parameter.length();
        ss << "$" << parameter.length() << "\r\n" << parameter << "\r\n";
    }
    mess = ss.str();
    ss.str("");
    ss << "*" << num << "\r\n";
    string head = ss.str();
    mess.insert(0, head);
    return mess;
}

const string & redisclient::lpush(const string & key,const string & value) const {
    string command = redisclient::command({"lpush",key,value});
    send(command);
    string feedback = redisclient::recv();
    const string & fb = redisclient::parse(feedback);
    return fb;
}

const string & redisclient::sadd(const string & key,const string & value) const {
    string command = redisclient::command({"sadd",key,value});
    send(command);
    string feedback = redisclient::recv();
    const string & fb = redisclient::parse(feedback);
    return fb;
}

string redisclient::zadd(const string key,const initializer_list<string> & parameters) const {
    vector<string> items;
    items.push_back("zadd");
    items.push_back(key);
    for (string item:parameters) {
        items.push_back(item);
    }
    copy(begin(items),end(items),ostream_iterator<string>{cout," "});
    const string &command = redisclient::command(items);
    redisclient::send(command);
    string feedback = redisclient::recv();
    string fb = redisclient::parse(feedback);
    return fb;
}

const string & redisclient::pfadd(const string & key,const string & value) const {
    string command = redisclient::command({"pfadd",key,value});
    send(command);
    string feedback = redisclient::recv();
    const string & fb = redisclient::parse(feedback);
    return fb;
}

const string & redisclient::pfcount(const string & key) const {
    string command = redisclient::command({"pfcount",key});
    send(command);
    string feedback = redisclient::recv();
    const string & fb = redisclient::parse(feedback);
    return fb;
}

const string & redisclient::multi() const {
    string command = redisclient::command({"multi"});
    send(command);
    string feedback = redisclient::recv();
    const string & fb = redisclient::parse(feedback);
    return fb;
}
const string & redisclient::exec() const {
    string command = redisclient::command({"exec"});
    send(command);
    string feedback = redisclient::recv();
    const string &fb = redisclient::analyse(feedback);
    return fb;
}

string redisclient::analyse(string feedback) const {
    int f = feedback.find('\n') + 1;
    string sub = feedback.substr(f);
    string mess;
    std::pair<string, string> pair{};
    do{
        char &c = sub.at(0);
        switch (c) {
            case '+':{
                int f = sub.find('+') + 1;
                int s = sub.find('\r');
                string m = sub.substr(f,s - f);
                mess.append(m);
                mess.append(" ");
                sub = sub.substr(s+2);
                break;
            }
            case '$': {
                pair = redisclient::getmessandsubwithtoken(sub,c);
                mess = mess.append(pair.first);
                mess.append(" ");
                sub = pair.second;
                break;
            }
            default:{
                break;
            }
        }
    }while (sub != "");
    return mess;
}

const string &  redisclient::configget(const string & key) const {
    string command = redisclient::command({"config","get",key});
    send(command);
    string feedback = redisclient::recv();
    const string & fb = redisclient::parse(feedback);
    return fb;
}
const string & redisclient::configset(const string & key,const string & value) const {
    string command = redisclient::command({"config","set",key,value});
    send(command);
    string feedback = redisclient::recv();
    const string & fb = redisclient::parse(feedback);
    return fb;
}



