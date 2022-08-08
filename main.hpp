#include "include.hpp"


//Lets me use cout and endl without extra code
using std::cout;
using std::endl;
using pcpp::IPFilter;


//Think I've got this better now
void mem(){
    struct rlimit rl;
    rl.rlim_cur = 2147483648;
    rl.rlim_max = 5368709120;
    setrlimit (RLIMIT_AS, &rl);
}


std::vector <std::string> ip_list;
void create_vector(){
    std::ifstream in("iplist.txt");
    if(!in){
        cout << "Error opening file in input mode, check the iplist.txt file permissions" << endl;
        return;
    }
    std::string test;
    while(std::getline(in, test)){
        if(test.size() > 0){
            ip_list.push_back(test);
        }
    }
    in.close();
}




// PCAP++

std::string adapter(){
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    sockaddr_in loopback;

    if (sock == -1) {
        std::cerr << "Could not socket\n";
        return "-1";
    }

    std::memset(&loopback, 0, sizeof(loopback));
    loopback.sin_family = AF_INET;
    loopback.sin_addr.s_addr = 1337;   // can be any IP address
    loopback.sin_port = htons(9);      // using the debug port

    if (connect(sock, reinterpret_cast<sockaddr*>(&loopback), sizeof(loopback)) == -1) {
        close(sock);
        std::cerr << "Could not connect\n";
        return "-1";
    }

    socklen_t addrlen = sizeof(loopback);
    if (getsockname(sock, reinterpret_cast<sockaddr*>(&loopback), &addrlen) == -1) {
        close(sock);
        std::cerr << "Could not getsockname\n";
        return "-1";
    }

    close(sock);

    char buf[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &loopback.sin_addr, buf, INET_ADDRSTRLEN) == nullptr) {
        std::cerr << "Could not inet_ntop\n";
        return "-1";
    } 
    else {
        std::string str(buf);
        return str;
    }
}


static void onPacketArrives(pcpp::RawPacket*, pcpp::PcapLiveDevice*, void*)
{
    //Will add proper messaging here
    cout << "Packet arrived or been sent" << endl;
}



int filtersetup(std::vector<std::string> & ip_list_filter){

    std::vector<pcpp::GeneralFilter*> portFilterVec;

    std::string interfaceIPAddr = adapter();

    if (interfaceIPAddr == "-1") {
        std::cerr << "An error occured in the ip address getting, try changing network types, check you have internet and if nothing works open an issue on github.\n" << endl;
        return EXIT_FAILURE;
    }

    std::string stopcapture;

    pcpp::PcapLiveDevice* dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(interfaceIPAddr);

    if (dev == nullptr){
        std::cerr << "Could not find interface with IPv4 address of '" << interfaceIPAddr << "'" << endl;
        return EXIT_FAILURE;
    }
    
    if (!dev->open()) {
        std::cerr << "Could not open device " << dev->getName() << endl;
        return EXIT_FAILURE;
    }

    for (int i = 0; i < ip_list_filter.size(); i++){
        portFilterVec.push_back(new IPFilter(ip_list_filter[i], pcpp::SRC_OR_DST));
    }
    
    pcpp::OrFilter orfilter(portFilterVec);
    
    if (!dev->setFilter(orfilter)){
        std::cerr << "Could not set filter, try running the program again as root" << endl;
        return EXIT_FAILURE;
    }

    dev->startCapture(onPacketArrives, nullptr);
    cout << "Capturing packets..." << endl;
    cout << "\nEnter 'q' to stop capture: ";
    std::cin >> stopcapture;
    do{
    if (std::cin.fail()){
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    else if(stopcapture != "q"){
        cout << "Invalid input, try again" << endl;
        cout << "\nEnter 'q' to stop capture: ";
        std::cin >> stopcapture;
    }
    }
    while(!std::cin.fail() && (stopcapture != "q"));{
        dev->stopCapture();
        dev->close();
        return EXIT_SUCCESS;
    }
}





//CURL 

std::string fullfile = "iplist.txt";
void file_write(std::string readBuffer){
    std::ifstream cl;
    cl.open("iplist.txt", std::fstream::out | std::fstream::trunc);
    cl.close();
    
    std::fstream fs;
    fs.open("iplist.txt", std::fstream::out | std::fstream::app);
    fs << readBuffer;
    fs.close();
    return;
}

//Callback to turn the response from the website into a string
static size_t WriteCallback(char* contents, size_t size, size_t nmemb, std::string* userp)
{
    userp->append(contents, size * nmemb);
    return size * nmemb;
}

void download(int)
{
    std::string readBuffer;
    CURL *curl;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://rules.emergingthreats.net/blockrules/compromised-ips.txt");
        curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        file_write(readBuffer);
    }
    create_vector();
    signal(SIGALRM, download);
    alarm(1800);
}