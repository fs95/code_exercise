/*
 * Get network IPv4 address, subnet mask, broadcast address, MAC address.
 */
#include <iostream>
#include <net/if.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

int main()
{
    int fd = 0;
    struct ifconf ifc{};
    struct ifreq ifrs[5];

    // IPv4 UDP socket fd
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Initialize the ifconf structure
    ifc.ifc_len = sizeof(ifrs);
    ifc.ifc_ifcu.ifcu_buf = (caddr_t)ifrs;

    // Get the list of interfaces and the number
    if (ioctl(fd, SIOCGIFCONF, (char *)&ifc) == -1) {
        perror("ioctl SIOCGIFCONF");
        exit(EXIT_FAILURE);
    }
    int if_len = ifc.ifc_len/sizeof(struct ifreq); // Number of interfaces

    // Travers each interface
    while (if_len-- > 0) {
        cout << endl;
        cout << "Interface name: " << ifrs[if_len].ifr_ifrn.ifrn_name << endl;

        // Get IPv4 address
        if (ioctl(fd, SIOCGIFADDR, (char*)&ifrs[if_len]) == -1) {
            perror("SIOCGIFADDR");
            exit(EXIT_FAILURE);
        } else {
            cout << "IP address:"
            << inet_ntoa(((struct sockaddr_in*)(&ifrs[if_len].ifr_ifru.ifru_addr))->sin_addr) << endl;
        }

        // Subnet mask
        if (ioctl(fd, SIOCGIFNETMASK, (char*)&ifrs[if_len]) == -1) {
            perror("SIOCGIFNETMASK");
            exit(EXIT_FAILURE);
        } else {
            cout << "Subnet mask:"
            << inet_ntoa(((struct sockaddr_in*)(&ifrs[if_len].ifr_ifru.ifru_addr))->sin_addr) << endl;
        }

        // Broadcast address
        if (ioctl(fd, SIOCGIFBRDADDR, (char*)&ifrs[if_len]) == -1) {
            perror("SIOGIFBRSADDR");
            exit(EXIT_FAILURE);
        } else {
            cout << "Broadcast address:"
            << inet_ntoa(((struct sockaddr_in*)(&ifrs[if_len].ifr_ifru.ifru_addr))->sin_addr) << endl;
        }

        // MAC address
        if (ioctl(fd, SIOCGIFHWADDR, (char*)&ifrs[if_len]) == -1) {
            perror("SIOCGIFHWADDR");
            exit(EXIT_FAILURE);
        } else {
            printf("MAC address:%02x:%02x:%02x:%02x:%02x:%02x\n",
            (unsigned char)ifrs[if_len].ifr_ifru.ifru_hwaddr.sa_data[0],
            (unsigned char)ifrs[if_len].ifr_ifru.ifru_hwaddr.sa_data[1],
            (unsigned char)ifrs[if_len].ifr_ifru.ifru_hwaddr.sa_data[2],
            (unsigned char)ifrs[if_len].ifr_ifru.ifru_hwaddr.sa_data[3],
            (unsigned char)ifrs[if_len].ifr_ifru.ifru_hwaddr.sa_data[4],
            (unsigned char)ifrs[if_len].ifr_ifru.ifru_hwaddr.sa_data[5]);
        }
    }

    close(fd);

    exit(EXIT_SUCCESS);
}
