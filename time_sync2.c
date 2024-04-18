
#include <stdio.h> // this header file is for basic input/output operation
#include <stdlib.h> // this header file defines many variable types micro and general functions 
#include <string.h> // this is for string handling function
#include <unistd.h> // it gives access to POSIX operating API 
#include <time.h> // this header file is to get a system's time
#include <sys/socket.h>// this header helps us networking programming function
#include <netinet/in.h> // this header file defines the micro for porpuse 
#include <arpa/inet.h> // this header file is for upper protocol 
#include <netdb.h>

// these constants for the NTP server
#define NTP_SERVER "pool.ntp.org"
#define NTP_PORT "123"  // Port should be string for getaddrinfo
#define NTP_TIMESTAMP_DELTA 2208988800ull

//This is the structure for NTP data type
typedef struct {
    uint8_t li_vn_mode;
    uint8_t stratum;
    uint8_t poll;
    uint8_t precision;
    uint32_t root_delay;
    uint32_t root_dispersion;
    uint32_t reference_id;
    uint32_t ref_ts_sec;
    uint32_t ref_ts_frac;
    uint32_t orig_ts_sec;
    uint32_t orig_ts_frac;
    uint32_t recv_ts_sec;
    uint32_t recv_ts_frac;
    uint32_t trans_ts_sec;
    uint32_t trans_ts_frac;
} ntp_packet;

// this function helps us in print error massage and program exits
void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}


int main() {
// variable and pointers declearation
    int sockfd, n;
    struct addrinfo hints, *servinfo, *p;
    ntp_packet packet;
    time_t current_time;
    struct tm *local_time;

    // Local Time
    current_time = time(NULL);
    local_time = localtime(&current_time);
    printf("Local time: %s", asctime(local_time));

    // NTP Time
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // either IPV4 or IPV6
    hints.ai_socktype = SOCK_DGRAM; // UDP

    if (getaddrinfo(NTP_SERVER, NTP_PORT, &hints, &servinfo) != 0) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(EXIT_FAILURE);
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to create socket\n");
        return 2;
    }

    memset(&packet, 0, sizeof(packet));
    packet.li_vn_mode = (0x1b); // LI=0, VN=3, Mode=3

    if (sendto(sockfd, &packet, sizeof(packet), 0, p->ai_addr, p->ai_addrlen) == -1) {
        close(sockfd);
        perror("sendto");
        exit(EXIT_FAILURE);
    }

    n = recvfrom(sockfd, &packet, sizeof(packet), 0, NULL, NULL);
    if (n == -1) {
        close(sockfd);
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }

    close(sockfd);
    freeaddrinfo(servinfo);

    current_time = (ntohl(packet.trans_ts_sec) - NTP_TIMESTAMP_DELTA);
    printf("NTP time: %s", ctime(&current_time));

    // Calculate and print time difference
    time_t diff = current_time - time(NULL);
    printf("Time difference: %ld seconds\n", diff);

    return 0;
}
