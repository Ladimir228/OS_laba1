#include <unistd.h>
#include <errno.h>

int main(void) {
    char buffer[16];

    ssize_t received;

    for (;;) {
        received = read(0, buffer, sizeof(buffer));

        if (received == -1 && errno == EINTR) {
            continue;
        }

        if (received <= 0) {
            break;
        }

        if (received > 0) {
            for (ssize_t i = 0; i < received; i++) {
                if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
                    buffer[i] = buffer[i] - 'A' + 'a';
                }
            }

            ssize_t sent = 0;
            while (sent < received) {
                ssize_t written = write(1, buffer + sent, (size_t)(received - sent));

                if (written > 0) {
                    sent += written;
                }
                else if (written == -1 && errno == EINTR) {
                    continue;
                }
                else {
                    return 1;
                }
            }
        }
    }
    return  received == -1 ? 1 : 0;
}
