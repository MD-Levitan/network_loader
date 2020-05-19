#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "packet.h"

const char *usage = "Usage: loader <interface> <destination> <packet_type> <speed> <length>\n";

/**
 * Usage: loader <interface> <destination> <packet_type> <speed> <length>
 */
int main(int argc, char *argv[])
{
    if (argc < 6)
    {
        fprintf(stderr, "%s", usage);
        fprintf(stderr, "\nInterfaces:\n", usage);
        print_interfaces(stderr);
        
        fprintf(stderr, "\nTypes:\n", usage);
        print_types(stderr);
        /* TODO: add help */
        exit(EXIT_FAILURE);
    }

    packet_t packet;
    uint32_t size, speed;
    unsigned char buf[sizeof(struct in_addr)];
    uint8_t status;

    status = inet_pton(AF_INET, argv[2], buf);
    if (status <= 0)
    {
        if (status == 0)
        {
            fprintf(stderr, "destination in wrong format\n");
        }
        else
        {
            perror("inet_pton");
        }
        exit(EXIT_FAILURE);
    }

    speed = strtol(argv[4], NULL, 10);
    size = strtol(argv[5], NULL, 10);

    packet.type = parse_string(argv[3]);
    packet.size = size;
    packet.speed = speed;
    packet.interface = argv[1];
    memcpy(packet.ip, buf, sizeof(struct in_addr));
 
    load(&packet);
    return 0;
}