#include <iostream>
#include <sys/sysinfo.h>
#include <sys/resource.h>

//Defineing a bunch of stuff
using namespace std;
struct rlimit limits;
int option;
void compare(string src, string dst);
struct sysinfo memInfo;

//Actual compare function (Need to improve the memory killswitch later)
void compare(string src, string dst){
    limits.rlim_max = 5368709120; //Sets hard mem limit to 5Gb
    int err = setrlimit(RLIMIT_DATA, &limits);
    if (err == 1){
        cout << "Memory limit breached, running killswitch" << endl;
        void _exit(int status);
    }
    cout << src << endl;
    cout << dst << endl;
    src.clear();
    dst.clear();
}