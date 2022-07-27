//Firewall.cpp by swanserquack on github (Still a major WIP)
#include "main.hpp"


int main(){
    if (getuid()) {
        cout << "You must be root to run this program. Try again running as root." << endl;
        return 0;
    }
    mem();
    //download(14);

    //Creates the vector and fill it with the data from the file
    create_vector(ip_list);

    while (true){

        int option;
        
        //Maybe I can put this into a single cout?
        cout << "" << endl;
        cout << "Select what to do" << endl;
        cout << "1. Enable/Disable" << endl;
        cout << "2. Exit" << endl << endl;
        std::cin >> option;

        if (option == 1){
            filtersetup(ip_list);
        }

        else if (option == 2){
            return 0;
        }

        else{
            cout << endl << "Invalid option" << endl;
        }
    }
}