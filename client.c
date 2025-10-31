#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(9999),
        .sin_addr.s_addr = inet_addr("127.0.0.1")
    };

    connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("connected to server!\n");

    struct pollfd fds[2] = {
        { .fd = STDIN_FILENO, .events = POLLIN },
        { .fd = sock_fd, .events = POLLIN }
    };

    for (;;) {
        char buffer[256] = {0};

        poll(fds, 2, -1);

        if (fds[0].revents & POLLIN) {
            ssize_t bytes = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
            if (bytes > 0)
                send(sock_fd, buffer, bytes, 0);
        }

        if (fds[1].revents & POLLIN) {
            ssize_t bytes = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
            if (bytes <= 0) {
                printf("server closed connection.\n");
                break;
            }
            printf("server: %s", buffer);
        }
    }

    close(sock_fd);
    return 0;
}
