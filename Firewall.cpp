//Firewall.cpp by swanserquack on github (Still a major WIP)

//Going to need to change these to files bundled with the exec maybe? (You can really tell I'm new to this)
#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <curl/curl.h>
#include "main.h"

CURL *easy_handle = curl_easy_init();

//Network capture functions
void capture(){
    signal(SIGINT, stop_capture);
    signal(SIGTERM, stop_capture);
    signal(SIGQUIT, stop_capture);
    handle = create_pcap_handle(device, filter);
    get_link_header_len(handle);
    
    if (handle == NULL or linkhdrlen == 0) {
        return;
    }

    if (pcap_loop(handle, count, packet_handler, (u_char*)NULL) == PCAP_ERROR) {
        fprintf(stderr, "pcap_loop failed: %s\n", pcap_geterr(handle));
        return;
    }
    
    //Won't need this but will need to return results somehow to analyse them
    stop_capture(0);
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
            capture();
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