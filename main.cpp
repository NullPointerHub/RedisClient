#include <iostream>
#include <cstdio>
#include "redisclient.h"
using namespace std;


int main(int argc, char** argv){
    redisclient rc;
    rc.auth("666");
    string  back{};
    back = rc.get("key");
    cout << back << endl;
    return 0;
}

