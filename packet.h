#include <inttypes.h>

#ifndef PACKET_H_
#define PACKET_H_

typedef struct packet_t packet_t;
typedef enum packet_type_t packet_type_t;

enum packet_type_t
{
    ICMP = 0x01,

    UNDEFINED = 0x99,
};

struct packet_t
{
    /* Type of packets */
    packet_type_t type;
    /* Interface throw which send packets */
    char *interface;
    /* IP address of destination */
    uint8_t ip[4];
    /* Size of packets */
    uint16_t size;
    /* Number of packets per sec */
    uint64_t speed;
};

/**
 * Create loader 
 */
void load(packet_t *packet);

/* Utils */
packet_type_t parse_string(const char *str);

#endif //PACKET_H_