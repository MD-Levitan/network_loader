#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <arpa/inet.h>

#include "packet.h"

#include "icmp_packet.h"

typedef struct private_packet_t private_packet_t;
typedef void (*private_loader)(packet_t *);

struct private_packet_t
{
    packet_type_t type;
    const char *name;
    private_loader loader;

} private_packets[] = {
    {
        .type = ICMP,
        .name = "ICMP",
        .loader = icmp_loader,
    },

};

private_packet_t *get_private_by_str(const char *str)
{
    for (uint8_t i = 0; i < sizeof(private_packets) / sizeof(private_packets[0]); ++i)
    {
        if (!strcmp(private_packets[i].name, str))
        {
            return private_packets;
        }
    }
    return NULL;
}

private_packet_t *get_private_by_type(packet_type_t type)
{
    for (uint8_t i = 0; i < sizeof(private_packets) / sizeof(private_packets[0]); ++i)
    {
        if (private_packets[i].type == type)
        {
            return private_packets;
        }
    }
    return NULL;
}

packet_type_t parse_string(const char *str)
{
    private_packet_t *private;
    private = get_private_by_str(str);
    if (private)
    {
        return private->type;
    }
    return UNDEFINED;
}

void load(packet_t *packet)
{
    private_packet_t *private;
    private = get_private_by_type(packet->type);
    if (private)
    {
        private->loader(packet);
    }
    else
    {
        fprintf(stderr, "incorrect packet\n");
    }
}

void print_types(FILE *fd)
{
    for (uint8_t i = 0; i < sizeof(private_packets) / sizeof(private_packets[0]); ++i)
    {
        fprintf(fd, "\t%s\n", private_packets[i].name);
    }
    return NULL;
}

void print_interfaces(FILE *fd)
{
    struct ifaddrs *ifaddr, *ifa;
    int error;
    
    if (fd == NULL)
    {
        _exit(1);
    }

    if (getifaddrs(&ifaddr) == -1) {
        error = errno;
        fprintf(fd, "error in getifaddrs: %s", strerror(error));
        return;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        char addres[INET6_ADDRSTRLEN];

        if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family & AF_INET & AF_INET6)
        {
            inet_ntop(ifa->ifa_addr->sa_family, ifa->ifa_addr->sa_family == AF_INET ? 
                                                ifa->ifa_addr->sa_data + 2 : ifa->ifa_addr->sa_data + 6,
                                                addres, INET6_ADDRSTRLEN);
            fprintf(fd, "\t%s: %s\n", ifa->ifa_name, addres);
        }
    }

  freeifaddrs(ifaddr);
  return;
} 