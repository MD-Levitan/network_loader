#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "packet.h"

const char *usage = "Usage: loader <interface> <destination> <speed> <length>\n";

/**
 * Usage: loader <interface> <destination> <speed> <length>
 */
int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        fprintf(stderr, "%s", usage);
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

    speed = strtol(argv[3], NULL, 10);
    size = strtol(argv[4], NULL, 10);

    packet.size = size;
    packet.speed = speed;
    packet.interface = argv[1];
    memset(packet.ip, buf, sizeof(struct in_addr));

    return 0;
}