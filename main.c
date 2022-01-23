#define _POSIX_C_SOURCE 200112L //Glibc getaddrinfo

#define DEBUG_NTP 1
#include "ntp_packet.h"


//Standard
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


//Glibc DNS resolver
#include <sys/types.h>
#include <sys/socket.h>
#include  <netdb.h>

//Filedescriptor
#include <unistd.h>
#include <sys/select.h>

#include <arpa/inet.h>

void setupNTPPacket(ntp_packet* np)
{
    ntp_setLi(np,0);
    ntp_setVn(np,3);
    ntp_setMode(np,3);
    ntp_setPoll(np,2);
    ntp_setPrecision(np,0);
    ntp_setRootdelay_s(np,0);
    ntp_setRootDispersion_s(np,0);
    ntp_setRefClkId(np,0);
    ntp_setRef_s(np,0);
    ntp_setRef_ds(np,0);
    ntp_setOri_s(np,0);
    ntp_setOri_ds(np,0);
    ntp_setRx_s(np,0);
    ntp_setRx_ds(np,0);
    ntp_setTx_s(np,0);
    ntp_setTx_ds(np,0);
}

char *get_ip_str(struct sockaddr *sa, char *s, size_t maxlen)
{//Credits to: https://gist.github.com/jkomyno/45bee6e79451453c7bbdc22d033a282e
    switch(sa->sa_family) {
        case AF_INET:
            inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
                    s, maxlen);
            break;

        case AF_INET6:
            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
                    s, maxlen);
            break;

        default:
            strncpy(s, "Unknown AF", maxlen);
            return NULL;
    }

    return s;
}
void info_addrinfo(struct addrinfo* i)
{
    char addr[40]={0};
    get_ip_str(i->ai_addr,addr,i->ai_addrlen);
    fprintf(stderr,"AddrInfo:%p \n"
            "  ai_flags:%x\n"
            "  ai_family:%i %s\n"
            "  ai_socktype:%i %s\n"
            "  ai_protocol:%i %s\n"
            "  ai_addrlen:%u\n"
            "  ai_addr: %s:%i\n"
            "  ai_canonname:%s\n"
            "  ai_next->%p\n",
            i,
            i-> ai_flags,
            i-> ai_family, i-> ai_family==AF_INET6?"IPv6":"IPv4(if 2)",
            i-> ai_socktype,i->ai_socktype==SOCK_STREAM?"SOCK_STREAM":
                            i->ai_socktype==SOCK_DGRAM?"SOCK_DGRAM":
                            "Not STREAM nor DGRAM",
            i-> ai_protocol, i-> ai_protocol==IPPROTO_TCP?"TCP":
                             i-> ai_protocol==IPPROTO_UDP? "UDP":
                                "No TCP/UDP",
            i-> ai_addrlen,
            addr, htons(((struct sockaddr_in*)i->ai_addr)->sin_port),
            i->ai_canonname,
            i->ai_next);
}
void info_allAddrInfo(struct addrinfo* i)
{
    for (struct addrinfo* temp=i;temp!=NULL;temp=temp->ai_next)
    {
        info_addrinfo(temp);
    }
}
void getIP(struct sockaddr_in *serverIP,char* serverURL)
{

    struct addrinfo hints={0};
    hints.ai_flags=AI_CANONNAME|AI_PASSIVE|AI_NUMERICSERV;
    hints.ai_family=AF_INET;//IPv4 results
    hints.ai_socktype=SOCK_DGRAM;
    hints.ai_protocol=IPPROTO_UDP;

    struct addrinfo* res=NULL;//Array of results(IPs)
    int addrInfo= getaddrinfo(serverURL,
            "123",//Services associated to this port
            &hints,
            &res);

    if (addrInfo)
    {
        const char* A=gai_strerror(addrInfo);
        fprintf(stderr,"addrInfo:%i\t%s\n",addrInfo,A);
        exit(1) ;
    }
    else
        fprintf(stderr,"DNS server successfully queried\n");

    info_allAddrInfo(res);
    char ipstr[30]={0};
    memcpy(serverIP,res->ai_addr,sizeof(struct sockaddr_in));
    inet_ntop(AF_INET, &(serverIP->sin_addr),ipstr,30);
    fprintf(stderr,"IP selected %s\n",ipstr);
    freeaddrinfo(res);
}
void connect2Server(int udp_socket,struct sockaddr_in *ntpServerIp)
{

    if (udp_socket==-1)
    {
        fprintf(stderr,"Socket couldn't be made %i\n",udp_socket);
        exit(1);
    }
    else
        fprintf(stderr,"Socket successfully made:[%i]...\n",udp_socket);

    int connecting= connect(udp_socket,(struct sockaddr *)ntpServerIp,
                            sizeof(struct sockaddr_in));

    if (connecting)
    {
        fprintf(stderr,"Socket couldn't be made\n");
        exit(1);
    }
    else
        fprintf(stderr,"Succesfully connected....\n");


}
void wait4Response(int udp_socket,ntp_packet *np)
{


    /* Params for select*/
    fd_set rfds;
    struct timeval tv;
    int retval;

    /* Watch stdin (fd udp_socket) to see when it has input. */

    FD_ZERO(&rfds);
    FD_SET(udp_socket, &rfds);

    /* Wait up to five seconds. */

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    retval = select(udp_socket+1, &rfds, NULL, NULL, &tv);

    if (retval == -1)
    {
        fprintf(stderr,"select() returned -1\n");
        exit(-1);
    }
    else if (retval)
    {
        uint8_t* buffer = malloc(50);
        int Ef = read(udp_socket, buffer, 50);
        if (Ef != sizeof(ntp_packet))
        {
            fprintf(stderr,"Error: Number of bytes received are: %i, then it's not interpretted as a NTPv3 packet\n",Ef);
            exit(1) ;
        }
        memcpy(np,buffer,sizeof(ntp_packet));
        fprintf(stderr,"=============Received packet================\n");
        ntp_info(np);
    }
    /* FD_ISSET(0, &rfds) will be true. */
    else
    {
        fprintf(stderr,"No data within five seconds. Probably the address isn't a NTP server\n");
        exit(1);
    }
}
void sendPacket(int udp_socket,ntp_packet *np )
{
    int n=write(udp_socket, (char*)np, sizeof(ntp_packet));
    if (n  < 0)
    {
        fprintf(stderr,"Error writting from socket");
        exit(1);
    }
}
int main(int args, char** argc)
{
    fprintf(stderr,"Arguments: %i\n",args);
    ntp_packet np={0};
    setupNTPPacket(&np);
    ntp_info(&np);

    struct sockaddr_in ntpServerIp={0};//IPv4
    getIP(&ntpServerIp,argc[1]);

    int udp_socket=socket(AF_INET,SOCK_DGRAM,0);
    connect2Server(udp_socket,&ntpServerIp);
    sendPacket(udp_socket,&np);

    ntp_packet Rnp={0};
    wait4Response(udp_socket,&Rnp);
    close(udp_socket);
    uint32_t ReceivedTimeStamp=Rnp.Rx_s;

    const uint32_t EpochTo1900=2208988800;//seconds between 1900 and 1970
    uint32_t tm2Epoch=ntohl(ReceivedTimeStamp)-EpochTo1900;//Remember byte order in packet
    fprintf(stderr,"Timestamp seconds:%u seconds, epoch:%u \n",ReceivedTimeStamp,tm2Epoch);

    printf("%u\n",tm2Epoch);

    return 0;
}
