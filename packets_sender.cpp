/*
 * Created by Marek Padlewski, 299778
 */

#include "packets_sender.h"

int send_packets(int sockfd, int pid, int ttl, struct sockaddr_in recipient){
    //we need to update TTL value
    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));

    //we need to send three packets with the same TTL
    for (int i = 0 ; i < NUMBER_OF_PACKETS ; i++){
        //code copied from lecture
        struct icmp header{};
        header.icmp_type = ICMP_ECHO;
        header.icmp_code = 0;
        header.icmp_hun.ih_idseq.icd_id = pid;
        header.icmp_hun.ih_idseq.icd_seq = 3 * ttl + i;
        header.icmp_cksum = 0;
        header.icmp_cksum = compute_icmp_checksum((u_int16_t*)&header, sizeof(header));

        ssize_t bytes_sent = sendto(sockfd, &header, sizeof(header),0,
                (struct sockaddr*)&recipient, sizeof(recipient));

        if (bytes_sent < 0){
            std::cerr << "sendto error: " << std::strerror(errno) << std::endl;
            return -1;
        }
    }

    return 0;
}

//function copied from lecture
u_int16_t compute_icmp_checksum (const void *buff, int length)
{
    u_int32_t sum;
    const u_int16_t* ptr = (u_int16_t *) buff;
    //assert (length % 2 == 0);
    for (sum = 0; length > 0; length -= 2)
        sum += *ptr++;
    sum = (sum >> 16) + (sum & 0xffff);
    return (u_int16_t)(~(sum + (sum >> 16)));
}

