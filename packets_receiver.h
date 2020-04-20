/*
 * Created by Marek Padlewski, 299778
 */

#pragma once

#include <netinet/in.h>
#include <netinet/ip.h>
#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <netinet/ip_icmp.h>
#include <vector>

int receive_all(int sockfd, int pid, int ttl, std::vector<std::string>& v_ips, int& rec_packet_count, int& resp_time_sum);