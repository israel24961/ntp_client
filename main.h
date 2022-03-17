#define _POSIX_C_SOURCE 200112L //Glibc getaddrinfo

#define DEBUG_NTP 1
#include "ntp_packet.h"

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

#include <argp.h>


//OTF (one time functions)
void setupNTPPacket(ntp_packet* np);
char *get_ip_str(struct sockaddr *sa, char *s, size_t maxlen);
void info_addrinfo(struct addrinfo* i);
void info_allAddrInfo(struct addrinfo* i);
void getIP(struct sockaddr_in *serverIP,char* serverURL);
void connect2Server(int udp_socket,struct sockaddr_in *ntpServerIp);
void sendPacket(int udp_socket,ntp_packet *np );
void wait4Response(int udp_socket,ntp_packet *np);


