#include "include.hpp"


//Lets me use cout and endl without extra code
using std::cout;
using std::endl;

//Struct

struct PacketStats
{
	int ethPacketCount;
	int ipv4PacketCount;
	int ipv6PacketCount;
	int tcpPacketCount;
	int udpPacketCount;
	int dnsPacketCount;
	int httpPacketCount;
	int sslPacketCount;


	/**
	 * Clear all stats
	 */
	void clear() { ethPacketCount = 0; ipv4PacketCount = 0; ipv6PacketCount = 0; tcpPacketCount = 0; udpPacketCount = 0; tcpPacketCount = 0; dnsPacketCount = 0; httpPacketCount = 0; sslPacketCount = 0; }

	/**
	 * C'tor
	 */
	PacketStats() { clear(); }

	/**
	 * Collect stats from a packet
	 */
	void consumePacket(pcpp::Packet& packet)
	{
		if (packet.isPacketOfType(pcpp::Ethernet))
			ethPacketCount++;
		if (packet.isPacketOfType(pcpp::IPv4))
			ipv4PacketCount++;
		if (packet.isPacketOfType(pcpp::IPv6))
			ipv6PacketCount++;
		if (packet.isPacketOfType(pcpp::TCP))
			tcpPacketCount++;
		if (packet.isPacketOfType(pcpp::UDP))
			udpPacketCount++;
		if (packet.isPacketOfType(pcpp::DNS))
			dnsPacketCount++;
		if (packet.isPacketOfType(pcpp::HTTP))
			httpPacketCount++;
		if (packet.isPacketOfType(pcpp::SSL))
			sslPacketCount++;
	}

	/**
	 * Print stats to console
	 */
	void printToConsole()
	{
		std::cout
			<< "Ethernet packet count: " << ethPacketCount << std::endl
			<< "IPv4 packet count:     " << ipv4PacketCount << std::endl
			<< "IPv6 packet count:     " << ipv6PacketCount << std::endl
			<< "TCP packet count:      " << tcpPacketCount << std::endl
			<< "UDP packet count:      " << udpPacketCount << std::endl
			<< "DNS packet count:      " << dnsPacketCount << std::endl
			<< "HTTP packet count:     " << httpPacketCount << std::endl
			<< "SSL packet count:      " << sslPacketCount << std::endl;
	}
};







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
    else{
        cout << "Limit set to " << limits.rlim_cur << " bytes" << endl;
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
    if (inet_ntop(AF_INET, &loopback.sin_addr, buf, INET_ADDRSTRLEN) == 0x0) {
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
	// extract the stats object form the cookie
	PacketStats* stats = (PacketStats*)cookie;

	// parsed the raw packet
	pcpp::Packet parsedPacket(packet);

	// collect stats from packet
	stats->consumePacket(parsedPacket);

    cout << "Packet arrived" << endl;
}



int filtersetup(std::vector<std::string> & ip_list){
    PacketStats stats;
    std::vector<pcpp::GeneralFilter*> portFilterVec;

    std::string interfaceIPAddr = adapter();

    //getInstance causes memory leak will need to look into it later
    pcpp::PcapLiveDevice* dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(interfaceIPAddr);

    if (dev == NULL){
        std::cerr << "Could not find interface with IPv4 address of '" << interfaceIPAddr << "'" << endl;
        return 1;
    }

    cout << "   Interface name:        " << dev->getName() << endl; // get interface name

    
    if (!dev->open()) {
        std::cerr << "Could not open device " << dev->getName() << endl;
        return 1;
    }

    for (int i = 0; i < ip_list.size(); i++){
        portFilterVec.push_back(new pcpp::IPFilter(ip_list[i], pcpp::SRC_OR_DST));
        cout << ip_list[i] << endl;
    }
    
    pcpp::OrFilter orfilter(portFilterVec);
    if (!dev->setFilter(orfilter)){
        std::cerr << "Could not set filter, try running the program again as root" << endl;
        return 1;
    }
    
    dev->startCapture(onPacketArrives, &stats);
    pcpp::multiPlatformSleep(10);
    dev->stopCapture();
    cout << "Results:" << endl;
	stats.printToConsole();
    dev->close();
    return 0;

}





//CURL (WORKS AGAINNNNNNNN)


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
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fileupdate(readBuffer);
    }
    signal(SIGALRM, download);
    alarm(1800);
}