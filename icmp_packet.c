#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <linux/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <memory.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#include "icmp_packet.h"

typedef struct ping_struct_t ping_struct_t;
typedef struct icmp_packet_t icmp_packet_t;

struct icmp_packet_t
{
    struct icmphdr hdr;
    uint8_t msg[1024];
};

unsigned short checksum(void *b, int len)
{
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void icmp_loader(packet_t *packet)
{

    char msg[sizeof(icmp_packet_t)];

    const len = strnlen(packet->interface, 16);

    if (len == 16)
    {
        fprintf(stderr, "Too long interface name");
        return;
    }

    uint8_t cnt = 0;
    uint16_t sd, val = 255;

    icmp_packet_t pckt;
    struct sockaddr_in r_addr;
    struct sockaddr_in addr_ping;

    addr_ping.sin_family = AF_INET;
    addr_ping.sin_port = 0;
    memcpy((char *)&addr_ping.sin_addr, packet->ip, 4);
    

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

    if (setsockopt(sd, SOL_SOCKET, SO_BINDTODEVICE, packet->interface, len) != 0)
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
    uint64_t sleep_time = 1000000 / packet->speed;

    while (1)
    {

        memset(&pckt, 0, sizeof(pckt));
        pckt.hdr.type = ICMP_ECHO;

        for (uint8_t i = 0; i < packet->size - 1; i++)
            pckt.msg[i] = i + '0';

        pckt.msg[packet->size - 1] = 0;

        pckt.hdr.un.echo.sequence = cnt++;

        pckt.hdr.checksum = checksum(&pckt, sizeof(struct icmphdr) + packet->size);

        memcpy(msg, &pckt, sizeof(struct icmphdr) + packet->size);

        sendto(sd, msg, packet->size + sizeof(struct icmphdr), 0, (struct sockaddr *)&addr_ping, sizeof(addr_ping));

        usleep(sleep_time);

        int len = sizeof(r_addr);

        /* if ( recvfrom(sd, msg, ping.size + sizeof(struct icmphdr), 0, (struct sockaddr*)&r_addr, &len) > 0 ) */
    }
    return;
}