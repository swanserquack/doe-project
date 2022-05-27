#include <iostream>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <pcap/pcap.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>

//Defineing a bunch of stuff
using std::cout;
using std::endl;
using std::cin;
struct rlimit limits;
int option;
pcap_t* handle;
int linkhdrlen;
int packets;
int count = 0;
char device[256];
char filter[256];
void compare();
void capture();
struct sysinfo memInfo;

//Actual compare function (Need to improve the memory killswitch later)
void compare(){
    limits.rlim_max = 5368709120; //Sets hard mem limit to 5Gb
    int err = setrlimit(RLIMIT_DATA, &limits);
    if (err == 1){
        cout << "Memory limit breached, running killswitch" << endl;
        void _exit(int status);
    }
}

pcap_t* create_pcap_handle(char* device, char* filter)
{
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = NULL;
    pcap_if_t* devices = NULL;
    struct bpf_program bpf;
    bpf_u_int32 netmask;
    bpf_u_int32 srcip;

    // If no network interface (device) is specfied, get the first one.
    if (!*device) {
    	if (pcap_findalldevs(&devices, errbuf)) {
            fprintf(stderr, "pcap_findalldevs(): %s\n", errbuf);
            return NULL;
        }
        strcpy(device, devices[0].name);
    }

    // Get network device source IP address and netmask.
    if (pcap_lookupnet(device, &srcip, &netmask, errbuf) == PCAP_ERROR) {
        fprintf(stderr, "pcap_lookupnet: %s\n", errbuf);
        return NULL;
    }

    // Open the device for live capture.
    handle = pcap_open_live(device, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "pcap_open_live(): %s\n", errbuf);
        return NULL;
    }

    // Convert the packet filter epxression into a packet filter binary.
    if (pcap_compile(handle, &bpf, filter, 1, netmask) == PCAP_ERROR) {
        fprintf(stderr, "pcap_compile(): %s\n", pcap_geterr(handle));
        return NULL;
    }

    // Bind the packet filter to the libpcap handle.    
    if (pcap_setfilter(handle, &bpf) == PCAP_ERROR) {
        fprintf(stderr, "pcap_setfilter(): %s\n", pcap_geterr(handle));
        return NULL;
    }

    return handle;
}

void get_link_header_len(pcap_t* handle)
{
    int linktype;
 
    // Determine the datalink layer type.
    if ((linktype = pcap_datalink(handle)) == PCAP_ERROR) {
        printf("pcap_datalink(): %s\n", pcap_geterr(handle));
        return;
    }
 
    // Set the datalink layer header size.
    switch (linktype)
    {
    case DLT_NULL:
        linkhdrlen = 4;
        break;
 
    case DLT_EN10MB:
        linkhdrlen = 14;
        break;
 
    case DLT_SLIP:
    case DLT_PPP:
        linkhdrlen = 24;
        break;
 
    default:
        printf("Unsupported datalink (%d)\n", linktype);
        linkhdrlen = 0;
    }
}

void packet_handler(u_char *user, const struct pcap_pkthdr *packethdr, const u_char *packetptr)
{
    struct ip* iphdr;
    struct icmp* icmphdr;
    struct tcphdr* tcphdr;
    struct udphdr* udphdr;
    char iphdrInfo[256];
    char srcip[256];
    char dstip[256];
 

    // Skip the datalink layer header and get the IP header fields.
    packetptr += linkhdrlen;
    iphdr = (struct ip*)packetptr;
    strcpy(srcip, inet_ntoa(iphdr->ip_src));
    strcpy(dstip, inet_ntoa(iphdr->ip_dst));
    sprintf(iphdrInfo, "ID:%d TOS:0x%x, TTL:%d IpLen:%d DgLen:%d",
            ntohs(iphdr->ip_id), iphdr->ip_tos, iphdr->ip_ttl,
            4*iphdr->ip_hl, ntohs(iphdr->ip_len));
 
    // Advance to the transport layer header then parse and display
    // the fields based on the type of hearder: tcp, udp or icmp.
    packetptr += 4*iphdr->ip_hl;
    switch (iphdr->ip_p)
    {
    case IPPROTO_TCP:
        tcphdr = (struct tcphdr*)packetptr;
        printf("TCP  %s:%d -> %s:%d\n", srcip, ntohs(tcphdr->th_sport),
               dstip, ntohs(tcphdr->th_dport));
        printf("%s\n", iphdrInfo);
        printf("%c%c%c%c%c%c Seq: 0x%x Ack: 0x%x Win: 0x%x TcpLen: %d\n",
               (tcphdr->th_flags & TH_URG ? 'U' : '*'),
               (tcphdr->th_flags & TH_ACK ? 'A' : '*'),
               (tcphdr->th_flags & TH_PUSH ? 'P' : '*'),
               (tcphdr->th_flags & TH_RST ? 'R' : '*'),
               (tcphdr->th_flags & TH_SYN ? 'S' : '*'),
               (tcphdr->th_flags & TH_SYN ? 'F' : '*'),
               ntohl(tcphdr->th_seq), ntohl(tcphdr->th_ack),
               ntohs(tcphdr->th_win), 4*tcphdr->th_off);
        printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n");
        packets += 1;
        break;
 
    case IPPROTO_UDP:
        udphdr = (struct udphdr*)packetptr;
        printf("UDP  %s:%d -> %s:%d\n", srcip, ntohs(udphdr->uh_sport),
               dstip, ntohs(udphdr->uh_dport));
        printf("%s\n", iphdrInfo);
	    printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n");
        packets += 1;
        break;
 
    case IPPROTO_ICMP:
        icmphdr = (struct icmp*)packetptr;
        printf("ICMP %s -> %s\n", srcip, dstip);
        printf("%s\n", iphdrInfo);
        printf("Type:%d Code:%d ID:%d Seq:%d\n", icmphdr->icmp_type, icmphdr->icmp_code,
               ntohs(icmphdr->icmp_hun.ih_idseq.icd_id), ntohs(icmphdr->icmp_hun.ih_idseq.icd_seq));
	    printf("+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n");
        packets += 1;
        break;
    }
}

void stop_capture(int signo)
{
    struct pcap_stat stats;
 
    if (pcap_stats(handle, &stats) >= 0) {
        printf("\n%d packets captured\n", packets);
        printf("%d packets received by filter\n", stats.ps_recv); 
        printf("%d packets dropped\n\n", stats.ps_drop);
    }
    pcap_close(handle);
    exit(0);
}