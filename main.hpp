#include "include.hpp"


//Lets me use cout and endl without extra code
using std::cout;
using std::endl;
using pcpp::IPFilter;


//THIS NEEDS TO BE IMPROVED TO BE MORE EFFICIENT AND BETTER
void mem(){

    struct rlimit limits;

    limits.rlim_cur = 5368709120; //5 Gb (Soft limit)
    limits.rlim_max = 10737418240; //10 Gb (Hard limit)
    if(setrlimit(RLIMIT_DATA, &limits) == -1){
        fprintf(stderr, "&s\n", strerror(errno));
    }
    else if(getrlimit(RLIMIT_STACK, &limits) == -1){
        fprintf(stderr, "&s\n", strerror(errno));
    }
    else if(getrlimit(RLIMIT_MEMLOCK, &limits) == -1){
        fprintf(stderr, "&s\n", strerror(errno));
    }
}


std::vector <std::string> ip_list;
void create_vector(std::vector<std::string> & ip_list){
    std::ifstream in("iplist.txt");
    if(!in){
        cout << "Error opening file" << endl;
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
        return "-1"; // Need to make a handler for this
    }

    std::memset(&loopback, 0, sizeof(loopback));
    loopback.sin_family = AF_INET;
    loopback.sin_addr.s_addr = 1337;   // can be any IP address
    loopback.sin_port = htons(9);      // using debug port

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

static void onPacketArrives(pcpp::RawPacket* packet, pcpp::PcapLiveDevice* dev, void* cookie)
{
    //Will add proper messaging here
    cout << "Packet arrived or been sent" << endl;
}



int filtersetup(std::vector<std::string> & ip_list){

    std::vector<pcpp::GeneralFilter*> portFilterVec;

    std::string interfaceIPAddr = adapter();

    std::string stopcapture;

    //getInstance causes memory leak will need to look into it later
    pcpp::PcapLiveDevice* dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(interfaceIPAddr);

    if (dev == nullptr){
        std::cerr << "Could not find interface with IPv4 address of '" << interfaceIPAddr << "'" << endl;
        return 1;
    }

    cout << "   Interface name:" << dev->getName() << endl; // get interface name

    
    if (!dev->open()) {
        std::cerr << "Could not open device " << dev->getName() << endl;
        return 1;
    }

    for (int i = 0; i < ip_list.size(); i++){
        portFilterVec.push_back(new IPFilter(ip_list[i], pcpp::SRC_OR_DST));
    }
    
    pcpp::OrFilter orfilter(portFilterVec);
    
    if (!dev->setFilter(orfilter)){
        std::cerr << "Could not set filter, try running the program again as root" << endl;
        return 1;
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
    else if((stopcapture != "q") || (std::cin.get()=='\n')){
        cout << "Invalid input, try again" << endl;
        cout << "\nEnter 'q' to stop capture: ";
        std::cin >> stopcapture;
    }
    }
    while(!std::cin.fail() && (stopcapture != "q"));{
        dev->stopCapture();
        dev->close();
        return 0;
    }
}





//CURL 


//Callback to turn the response from the website into a string
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int fileupdate(std::string readBuffer){
    std::ofstream list;
    list.open ("iplist.txt");
    list << readBuffer;
    list.close();
    return 0;
}


void download(int signum)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://rules.emergingthreats.net/blockrules/compromised-ips.txt");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
        curl_easy_cleanup(curl);
        fileupdate(readBuffer);
    }
    signal(SIGALRM, download);
    alarm(1800);
}