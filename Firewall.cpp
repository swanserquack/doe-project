#include <iostream>
#include <tins/tins.h>
#include <sys/time.h>
#include <sys/resource.h>

using namespace Tins;
using namespace std;
int option;

bool loop(const PDU &pdu) {
    const IP &ip = pdu.rfind_pdu<IP>();
    const TCP &tcp = pdu.rfind_pdu<TCP>();
    return ip.src_addr();
    return ip.dst_addr();
    return true;
}

int main(){
    while(true){
        //Going to put in a temporary memory killswitch here in case of variable overload
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

    }
    else if (option == 3){
        return 0;
    }
}