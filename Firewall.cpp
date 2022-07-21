//Firewall.cpp by swanserquack on github (Still a major WIP)
#include "main.hpp"


int main(){

    download(14);

    //Creates the vector and fill it with the data from the file
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
            notify(ip_list);
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