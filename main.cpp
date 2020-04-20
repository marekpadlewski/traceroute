/*
 * Created by Marek Padlewski, 299778
 */

#include <zconf.h>

#include "packets_sender.h"
#include "packets_receiver.h"

#define MAX_TTL 30

void print_result_line(int ttl, const std::vector<std::string>& v_ips, int rec_packet_count, int resp_time_sum){
    std::cout << ttl << ". ";

    //print ip's
    for (int i = 0 ; i < rec_packet_count ; i++){
        if (i == 0)
            std::cout << v_ips[i] << " ";
        else if (i == 1 && v_ips[i] != v_ips[i-1])
                std::cout << v_ips[i] << " ";
        else if (i == 2 && v_ips[i] != v_ips[i-2] && v_ips[i] != v_ips[i-1])
            std::cout << v_ips[i] << " ";
    }

    if (rec_packet_count == NUMBER_OF_PACKETS){
        int avg_time = resp_time_sum / (NUMBER_OF_PACKETS * 1000);
        std::cout << avg_time << "ms" << std::endl;
    }
    else if (rec_packet_count > 0){
        std::cout << "???" << std::endl;
    }

    else
        std::cout << "*" << std::endl;
}

int main(int argc, char ** argv) {
    if (argc != 2){
        std::cout << "Please run program with argument (IPv4 address)." << std::endl;
        exit(1);
    }

    struct sockaddr_in recipient{};
    bzero(&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;

    //take ip address from program argument
    auto remote_ip_addr = argv[1];
    //convert IPv4 addresses from text to binary form
    //returns 1 when ip_addr is in good format
    if (inet_pton(AF_INET, remote_ip_addr, &recipient.sin_addr) != 1){
        std::cout << "You typed not valid IPv4 address." << std::endl;
        exit(1);
    }

    //we need to have this PID to later recognize which packets were sent by OUR program
    int pid = getpid();
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    for (int ttl = 1 ; ttl <= MAX_TTL ; ttl++){
        //lets send some packets
        int sp = send_packets(sockfd, pid, ttl, recipient);
        if (sp == -1)
            exit(1);

        int rec_packet_count = 0;
        int resp_time_sum = 0;
        std::vector<std::string> v_ips;
        int res = receive_all(sockfd, pid, ttl, v_ips, rec_packet_count, resp_time_sum);
        if (res == -1)
            exit(1);

        print_result_line(ttl, v_ips, rec_packet_count, resp_time_sum);

        if (res == 1)
            break;
    }
}