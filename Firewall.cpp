//Firewall.cpp by swanserquack on github (Still a major WIP)
#include "main.hpp"


int main(){
    if (getuid()) {
        cout << "You must be root to run this program. Try again running as root." << endl;
        return 0;
    }
    mem();
    download(14);

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
            if (filtersetup(ip_list) == -1){
                cout << "Some kind of error occured, you should have got an error message. If you got no message open an issue on github." << endl;
                return -1;
            }
        }

        else if (option == 2){
            return 0;
        }

        else{
            cout << endl << "Invalid option" << endl;
            break;
        }
    }
}