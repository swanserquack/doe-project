//Firewall.cpp by swanserquack on github
#include "main.hpp"


int main(){
    if (getuid()) {
        cout << "You must be root to run this program. Try again running as root." << endl;
        return EXIT_SUCCESS;
    }
    mem();
    download(14);

    while (true){

        int option;
        
        cout << "\nSelect what to do" << "\n1. Enable/Disable" << "\n2. Exit\n" << endl;
        std::cin >> option;

        if (option == 1){
            filtersetup(ip_list);
            if (filtersetup(ip_list) == -1){
                cout << "Some kind of error occured, you should have got an error message. If you got no message open an issue on github." << endl;
                return EXIT_FAILURE;
            }
        }

        else if (option == 2){
            return EXIT_SUCCESS;
        }

        else{
            cout << "\nInvalid option" << endl;
            break;
        }
    }
}