//Firewall.cpp by swanserquack on github (Still a major WIP)

//Going to need to change these to files bundled with the exec maybe? (You can really tell I'm new to this)
#include <iostream>
#include <curl/curl.h>
#include <fstream>
#include <string.h>
#include "main.hpp"

CURL *easy_handle = curl_easy_init();

int main(){
    //Will change this to the API URL later on and make sure it saves the ip list somewhere
    curl_easy_setopt(easy_handle, CURLOPT_URL, "https://example.com");
    //Counts the lines in the iplist in order to set the array length (Not currently working)
    line_count();
    std::string array[count];
    while (true){
        cout << "Select what to do" << endl;
        cout << "1. Enable/Disable" << endl;
        cout << "2. Settings" << endl;
        cout << "3. Exit" << endl << endl;
        std::cin >> option;

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