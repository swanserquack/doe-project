//Firewall.cpp by swanserquack on github (Still a major WIP)

//Going to need to change these to files bundled with the exec maybe? (You can really tell I'm new to this)
#include <iostream>
#include <curl/curl.h>
#include "main.h"

CURL *easy_handle = curl_easy_init();

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
            compare();
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