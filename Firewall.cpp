//Going to need to change these to files bundled with the exec maybe?
#include <iostream>
#include <tins/tins.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>

struct rlimit limits;

using namespace Tins;
using namespace std;
int option;
struct sysinfo memInfo;

bool loop(const PDU &pdu) {
    const IP &ip = pdu.rfind_pdu<IP>();
    const TCP &tcp = pdu.rfind_pdu<TCP>();
    cout << ip.src_addr() << endl;
    return ip.src_addr();
    return ip.dst_addr();
    return true;
}

int main(){
    limits.rlim_max = 5368709120;
    int err = setrlimit(RLIMIT_DATA, &limits);
    if (err){
        cout << "Memory limit breached, running killswitch" << endl;
        void _exit(int status);
    }

    cout << "Select what to do" << endl;
    cout << "1. Enable/Disable" << endl;
    cout << "2. Settings" << endl;
    cout << "3. Exit" << endl << endl;
    cin >>option;

    if (option == 1){
        //The wlan0 is temporary and will be changed in the future
        Sniffer("wlan0").sniff_loop(loop);
    }
    else if (option == 2){
        sysinfo (&memInfo);
        long long physMemUsed = memInfo.totalram - memInfo.freeram;
        physMemUsed *= memInfo.mem_unit;
        cout << "Physical Memory Used: " << physMemUsed << endl;
    }
    else if (option == 3){
        return 0;
    }
}