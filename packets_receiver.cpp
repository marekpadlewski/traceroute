/*
 * Created by Marek Padlewski, 299778
 */

#include "packets_receiver.h"

//return -1 if some error happen
//return 0 if we are on the the way
//return 1 if we reached target
int receive_all(int sockfd, int pid, int ttl, std::vector<std::string>& v_ips, int& rec_packet_count, int& resp_time_sum) {
    struct timeval tv{};
    tv.tv_sec = 1; //wait only 1 second
    tv.tv_usec = 0;
    fd_set descriptors;
    FD_ZERO (&descriptors);
    FD_SET(sockfd, &descriptors);

    bool target_reached = false;

    struct sockaddr_in sender{};
    socklen_t sender_len = sizeof(sender);

    while (true) {
        int ready = select(sockfd + 1, &descriptors, NULL, NULL, &tv);
        if (ready == -1) {
            std::cerr << "select error: " << std::strerror(errno) << std::endl;
            return -1;
        }
        else if (ready == 0 || rec_packet_count == 3) {
            //timeout reached after 1 sec
            if (target_reached)
                return 1;
            else
                break;
        }

        //take packet from socket
        u_int8_t buffer[IP_MAXPACKET + 1];
        if (recvfrom(sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr *) &sender, &sender_len) == -1) {
            std::cerr << "recvfrom error: " << std::strerror(errno) << std::endl;
            return -1;
        }

        //convert address to IPv4
        char sender_ip_ch[20];
        if (inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_ch, sizeof(sender_ip_ch)) == nullptr) {
            std::cerr << "inet_ntop error: " << std::strerror(errno) << std::endl;
            return -1;
        }
        std::string sender_ip_str = sender_ip_ch;

        struct ip *ip_header = (struct ip *) buffer;
        u_int8_t *icmp_packet = buffer + 4 * ip_header->ip_hl;
        struct icmp *icmp_header = (struct icmp *) icmp_packet;

        if (icmp_header->icmp_type == ICMP_TIME_EXCEEDED) {
            //take from payload packet with ttl equal to 0
            //we need to go 8 bytes deeper to get origin icmp header
            struct ip *origin_ip = (struct ip *) ((uint8_t *) icmp_header + 8);
            struct icmp *origin_icmp = (struct icmp *) ((uint8_t *) origin_ip + origin_ip->ip_hl * 4);
            if (origin_icmp->icmp_seq/3 == ttl && origin_icmp->icmp_id == pid) {
                //add ip to set and count time
                v_ips.push_back(sender_ip_str);
                rec_packet_count++;
                resp_time_sum += 1000000 - tv.tv_usec;
            }
        }

        if (icmp_header->icmp_type == ICMP_ECHOREPLY && icmp_header->icmp_seq/3 == ttl && icmp_header->icmp_id == pid) {
            //we reached target ip
            v_ips.push_back(sender_ip_str);
            rec_packet_count++;
            resp_time_sum += 1000000 - tv.tv_usec;
            target_reached = true;
        }
    }

    return 0;
}