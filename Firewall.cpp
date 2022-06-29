//Firewall.cpp by swanserquack on github (Still a major WIP)

//Going to need to change these to files bundled with the exec maybe? (You can really tell I'm new to this)
#include <curl/curl.h>
#include "main.hpp"
#include "adapter.hpp"

CURL *easy_handle = curl_easy_init();

int main(int argc, const char* argv[]){
    //Will change this to the API URL later on and make sure it saves the ip list somewhere
    curl_easy_setopt(easy_handle, CURLOPT_URL, "https://example.com");

    //Creates the vector and fill it with the data from the file
    std::vector <std::string> ip_list;
    create_vector(ip_list);

    while (true){

        int option;
        
        //Maybe I can put this into a single cout?
        cout << "" << endl;
        cout << "Select what to do" << endl;
        cout << "1. Enable/Disable" << endl;
        cout << "2. Settings" << endl;
        cout << "3. Exit" << endl << endl;
        std::cin >> option;

        if (option == 1){
            adapter(argc, argv);

            //Shouldn't I just create a linux packet filter using PF_PACKET?

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