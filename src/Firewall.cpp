//Semi-Automatic Firewall made by swanserquack

//Going to need to change these to files bundled with the exec maybe?
#include <iostream>
#include <tins/tins.h>
#include <sys/types.h>
#include "main.h"
#include <curl/curl.h>

using namespace Tins;
using namespace std;
CURL *easy_handle = curl_easy_init();

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
    //Will change this to the API URL later on and make sure it saves the ip list somewhere
    curl_easy_setopt(easy_handle, CURLOPT_URL, "https://example.com");
    while (true){
        cout << "Select what to do" << endl;
        cout << "1. Enable/Disable" << endl;
        cout << "2. Settings" << endl;
        cout << "3. Exit" << endl << endl;
        cin >>option;
        if (option == 1){
            //The wlan0 is temporary and will be changed in the future in order to automatically detect the correct interface
            Sniffer("wlan0").sniff_loop(loop);
        }
        else if (option == 2){
            //Need to add somekind of settings storage + actual setting ideas (maybe a file?)
        }
        else if (option == 3){
            return 0;
        }
        else{
            cout << "Invalid option" << endl;
        }
    }
}