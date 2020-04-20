/*
 * Created by Marek Padlewski, 299778
 */

#pragma once

#include <iostream>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <cstring>

#define NUMBER_OF_PACKETS 3

int send_packets(int sockfd, int pid, int ttl, struct sockaddr_in recipient);
u_int16_t compute_icmp_checksum (const void *buff, int length);