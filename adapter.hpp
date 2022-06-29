#include <iostream>
#include <viface/viface.hpp>

//Will get rid of this later
using namespace std;

//The arguements are the arguements provided at the command line
int adapter(int argc, const char* argv[])
{
    string name = "viface0";
    if (argc > 1) {
        name = string(argv[1]);
    }

    string ip = "192.168.25.46";
    if (argc > 2) {
        ip = string(argv[2]);
    }

    string mac = "ec:f1:f8:d5:47:6b";
    if (argc > 3) {
        mac = string(argv[3]);
    }

    try {
        // Create interface
        viface::VIface iface(name);

        // Configure interface
        iface.setIPv4(ip);
        iface.setMAC(mac);

        // Bring up interface
        iface.up();

        //Prints something to indicate that the interface is available
        cout << "Interface " << name << endl;
        cout << iface.getIPv4() << endl;
        cout << iface.getMAC() << endl;
        cout << "" << endl;
    } 
    
    //Error catching
    catch(exception const & ex) {
        cerr << ex.what() << endl;
        return -1;
    }

    return 0;
}