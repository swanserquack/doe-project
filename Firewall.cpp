//Semi-Automatic Firewall made by swanserquack

//Keep in mind I don't have any experience with C++
//and so this is probably bad but I want to get it to work and for it to be fast and reliable.


//Going to need to change these to files bundled with the exec maybe?
#include <iostream>
#include <tins/tins.h>
#include <sys/types.h>
#include "main.h"

using namespace Tins;
using namespace std;
int option;
void compare(string src, string dst);
struct sysinfo memInfo;
struct rlimit limits;

bool loop(const PDU &pdu) {
    const IP &ip = pdu.rfind_pdu<IP>();
    const TCP &tcp = pdu.rfind_pdu<TCP>();
    cout << ip.dst_addr() << endl;
    string src = to_string(ip.src_addr());
    string dst = to_string(ip.dst_addr());
    compare(src, dst);
    return true;
}

int main(){
    while (true){
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
            //Need to add somekind of settings storage (maybe a file?)
        }
        else if (option == 3){
            return 0;
        }
        else{
            cout << "Invalid option" << endl;
        }
    }
}