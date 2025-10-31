#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(9999),
        .sin_addr.s_addr = INADDR_ANY
    };

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 10);

    printf("waiting for connection on port 9999...\n");
    int client_fd = accept(server_fd, NULL, NULL);
    printf("client connected!\n");

    struct pollfd fds[2] = {
        { .fd = STDIN_FILENO, .events = POLLIN },
        { .fd = client_fd, .events = POLLIN }
    };

    for (;;) {
        char buffer[256] = {0};

        poll(fds, 2, -1);

        if (fds[0].revents & POLLIN) {
            ssize_t bytes = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
            if (bytes > 0)
                send(client_fd, buffer, bytes, 0);
        }

        if (fds[1].revents & POLLIN) {
            ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            if (bytes <= 0) {
                printf("client disconnected.\n");
                break;
            }
            printf("client: %s", buffer);
        }
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
