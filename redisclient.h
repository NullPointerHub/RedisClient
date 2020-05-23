#ifndef LEARNING_REDISCLIENT_H
#define LEARNING_REDISCLIENT_H

#include <string>
#include <sys/socket.h>
#include <cstring>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <zconf.h>
#include <sstream>
#include <vector>
using namespace std;

class redisclient {
public:
    redisclient(const string &ip, int port, const string &password);

    redisclient();

    ~redisclient() {
        close(socketfd);
    }

    redisclient(const redisclient &cl) = delete;

    void operator=(const redisclient &cl) = delete;

    string auth(const string &password) const {
        const string &comm = command({"auth", password});
        send(comm);
        string feedback = recv();
        return parse(feedback);
    }

    string ping(const string &mess = "ping") const ;

    const string & set(string key,string value) const ;
    const string & get(string key) const ;
    const string & del(string key) const ;
    const string & commandkey(string command,string key) const ;
    string hmset(string key,initializer_list<string> parameters) const ;
    const string & lpush(const string & key,const string & value) const ;
    const string & sadd(const string & key,const string & value) const ;
    string  zadd(const string key,const initializer_list<string> & parameters) const ;
    const string & pfadd(const string & key,const string & value) const ;
    const string & pfcount(const string & key) const ;
    const string & multi() const ;
    const string & exec() const ;

    const string & configget(const string & key) const ;
    const string & configset(const string & key,const string & value) const ;




    long int send(const string &message) const {
        const char *buf = message.c_str();
        unsigned long len = message.length();
        int flags{0};
        int value{0};
        if (value = ::send(socketfd, buf, len, flags) < 0) {
            return 0;
        }
        return value;
    }

    string recv() const {
        char buff[1024] = {0};
        long int len{0};
        int flags{0};
        len = ::recv(socketfd, buff, 1024, flags);
        return string(buff, len);
    }

    string command(initializer_list<string> parameters) const ;

    string command(const vector<string> & parameters) const;


    string sendAndReceive(string mess) {
        send(mess);
        return recv();
    }

private:
    string ip;
    int port;
    string password;
    int socketfd;
    struct sockaddr_in serveraddr{0};

    void init() {
        createSocket();
        initServerAddress();
        connect_remote();
    }

    int createSocket() {
        if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            cout << "error: " << errno << " strerror(errno): " << strerror(errno) << endl;
            return 0;
        }
        return socketfd;
    }

    int initServerAddress() {
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(port);
        int value{0};
        if (value = inet_pton(AF_INET, ip.c_str(), &serveraddr.sin_addr) <= 0) {
            return 0;
        }
        return value;
    }

    int connect_remote() {
        int value{0};
        if (connect(socketfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
            cout << "error: " << errno << " strerror(errno): " << strerror(errno) << endl;
            return 0;
        }
        return value;
    }

    int getnum(char c, string feedback) const {
        int f = feedback.find(c) + 1;
        int s = feedback.find('\r');
        string sub = feedback.substr(f, s);
        stringstream ss(sub);
        int num = 0;
        ss >> num;
        return num;
    }

    pair<string, string> getmessandsub(string sub) const {
        pair<string, string> p;
        int num = getnum('$', sub);
        if (num == -1) {
            p.first = "nil";
            if (sub.size() == 5) {
                p.second = "";
            } else {
                p.second = sub.substr(5);
            }
            return p;
        }
        int f = sub.find('\n') + 1;
        p.first = sub.substr(f, num);
        p.second = sub.substr(f + num + 2);
        return p;
    }

    pair<string, string> getmessandsubwithtoken(string sub,char c) const {
        pair<string, string> p;
        int num = getnum(c, sub);
        //cout << "num:" << num << endl;
        if (num == -1) {
            p.first = "nil";
            if (sub.size() == 5) {
                p.second = "";
            } else {
                p.second = sub.substr(5);
            }
            return p;
        }
        int f = sub.find('\n') + 1;
        p.first = sub.substr(f, num);
        p.second = sub.substr(f + num + 2);
        return p;
    }

    string parse(string feedback) const {
        char c = feedback.at(0);
        switch (c) {
            case '*': {
                string mess;
                int f = feedback.find('\n') + 1;
                string sub = feedback.substr(f);
                pair<string, string> pair;
                do {
                    pair = getmessandsub(sub);
                    mess = mess.append(pair.first);
                    mess.append(" ");
                    sub = pair.second;
                } while (sub != "");
                return mess;
            }
            case '+': {
                int f = feedback.find('+') + 1;
                int s = feedback.find('\r');
                return feedback.substr(f, s - f);
            }
            case '-': {
                int f = feedback.find(' ') + 1;
                string sub = feedback.substr(f);
                f = sub.find('\r');
                return sub.substr(0, f);
            }
            case ':': {

                int num = getnum(':', feedback);
                stringstream ss;
                ss << "(integer)";
                ss << num;
                return ss.str();
            }
            case '$': {
                int num = getnum('$', feedback);
                if (num == -1) {
                    return "nil";
                }
                int f = feedback.find('\n') + 1;
                string mess = feedback.substr(f, num);
                return mess;
            }
        }
    }


    string analyse(string feedback) const;
};


#endif
