#include <string.h>

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
}