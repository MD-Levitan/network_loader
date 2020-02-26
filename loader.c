#include <fcntl.h>
#include "time.h"
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <linux/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <memory.h>
#include <unistd.h>
#include "stdio.h"
#include <sys/ioctl.h>
#include "stdlib.h"

typedef struct ping_struct_t ping_struct_t;
typedef struct packet_t packet_t;

struct ping_struct_t {
    /* Interface throw which send packets */
    char *      interface;

    /* IP address of destination */
    uint8_t 	ip[4];
    
    /* Size of packets */
    uint16_t	size;

    /* Number of packets per sec */
    uint64_t 	speed;
};

struct packet_t {
    struct icmphdr hdr;
    uint8_t msg[1024];
};

/*--------------------------------------------------------------------*/
/*--- checksum - standard 1s complement checksum                   ---*/
/*--------------------------------------------------------------------*/
unsigned short checksum(void *b, int len)
{
    unsigned short *buf = b;
    unsigned int sum=0;
    unsigned short result;

    for ( sum = 0; len > 1; len -= 2 )
        sum += *buf++;
    if ( len == 1 )
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}


/*--------------------------------------------------------------------*/
/*--- ping - Create message and send it.                           ---*/
/*    return 0 is ping Ok, return 1 is ping not OK.                ---*/
/*--------------------------------------------------------------------*/
void ping_(ping_struct_t *ping)
{

    char msg[sizeof(packet_t)];

    const len = strnlen(ping->interface, 16);

    if (len == 16) {
        fprintf(stderr, "Too long interface name");
        return;
    }

    uint8_t cnt = 0;
    uint16_t sd, val = 255;

    packet_t pckt;
    struct sockaddr_in r_addr;
    struct sockaddr_in addr_ping;
    
    addr_ping.sin_family = AF_INET;
    addr_ping.sin_port = 0;
    memcpy((char *) &addr_ping.sin_addr, ping->ip, 4);

    sd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sd < 0)
    {
        fprintf(stderr, "Can't open socket");
        return;
    }
    if (setsockopt(sd, SOL_IP, IP_TTL, &val, sizeof(val)) != 0)
    {
        fprintf(stderr, "Set TTL option");
        return;
    }

    if (setsockopt(sd, SOL_SOCKET, SO_BINDTODEVICE, ping->interface, len) != 0)
    {
       fprintf(stderr, "Set network device name");
       return;
   }

    if (fcntl(sd, F_SETFL, O_NONBLOCK) != 0)
    {
        fprintf(stderr, "Request nonblocking I/O");
        return;
    }

    /* Calcualte sleep time beetween package */
    uint64_t sleep_time = 1000000 / ping->speed;

    while (1)
    {

        memset(&pckt, 0, sizeof(pckt));
        pckt.hdr.type = ICMP_ECHO;

        for (uint8_t i = 0; i < ping->size - 1; i++ )
            pckt.msg[i] = i+'0';

        pckt.msg[ping->size - 1] = 0;

        pckt.hdr.un.echo.sequence = cnt++;

        pckt.hdr.checksum = checksum(&pckt, sizeof(struct icmphdr) + ping->size);

        memcpy(msg, &pckt, sizeof(struct icmphdr) + ping->size);

        sendto(sd, msg, ping->size + sizeof(struct icmphdr), 0, (struct sockaddr*)&addr_ping, sizeof(addr_ping));

        usleep(sleep_time);

        int len = sizeof(r_addr);

        /* if ( recvfrom(sd, msg, ping.size + sizeof(struct icmphdr), 0, (struct sockaddr*)&r_addr, &len) > 0 ) */
    
    }
    return;
}

/*--------------------------------------------------------------------*/
/*--- main - look up host and start ping processes.                ---*/
/*--------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    ping_struct_t ping;

    uint8_t addr[4] = {52, 128, 128, 3};
    memcpy(ping.ip, addr, 4);

    ping.size = 20;
    ping.speed = 10;
    ping.interface = "ipsec0";

    ping_(&ping);
    return 0;
}