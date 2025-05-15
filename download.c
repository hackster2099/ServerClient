#include <stdio.h>
#include <stdlib.h>
#include <libsocket/libinetsocket.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1000
#define min(a, b) (((a) < (b)) ? (a) : (b))

int main(int argc, char *argv[]) {
    int fstSize = 10;
    char serverName[BUFFER_SIZE] = ".cs.sierracollege.edu";
    char *serverSelect = (char *)malloc(fstSize * sizeof(char));

    char request[10];

    printf("Connect to Server --> (newark, london):  ");
    while (fgets(serverSelect, fstSize, stdin)) {
        char *nl = strchr(serverSelect, '\n');
        if (nl) {
            *nl = '\0';
            break;
        }
    }

    strcat(serverSelect, serverName);

    int fd = create_inet_stream_socket(serverSelect, "3456", LIBSOCKET_BOTH, 0);
    if (fd < 0) {
        fprintf(stderr, "\nERROR --> No connection to the port\n");
        exit(1);
    }

    printf("\nConnected to the Socket\n");

    FILE *s = fdopen(fd, "r+");
    if (!s) {
        perror("fdopen failed");
        close(fd);
        return 1;
    }

    char response[BUFFER_SIZE];
    char fileName[BUFFER_SIZE];
    unsigned char buffer[BUFFER_SIZE];
    long size = 0;

    printf("\nOption\nL) --> list of files\nD) --> download files\nQ) --> Quit\n");

    while (fgets(request, sizeof(request), stdin)) {
        char *nl = strchr(request, '\n');
        if (nl) *nl = '\0';

        if (*request == 'Q') {
            fprintf(s, "QUIT\n");
            fflush(s);
            break;
        } 
        else if (*request == 'L') {
            fprintf(s, "LIST\n");
            fflush(s);
            while (fgets(response, BUFFER_SIZE, s)) {
                if (*response == '.')
                    break;
                printf("%s", response);
            }
        } 
        else if (*request == 'D') {
            printf("\nFile to Download: ");
            fgets(fileName, BUFFER_SIZE, stdin);
            nl = strchr(fileName, '\n');
            if (nl) *nl = '\0';

            // Request file size
            fprintf(s, "SIZE %s\n", fileName);
            fflush(s);
            if (!fgets(response, BUFFER_SIZE, s)) {
                fprintf(stderr, "Failed to get response from server.\n");
                
            }

            if (sscanf(response, "+OK %ld", &size) != 1 || size <= 0) {
                printf("Invalid file size or file doesn't exist.\n");
                
            }

            FILE *localFile = fopen(fileName, "wb");
            if (!localFile) {
                printf("File creation failed");
                
            }

            fprintf(s, "GET %s\n", fileName);
            fflush(s);

            // Read and discard the "+OK\n" line
            if (!fgets(response, BUFFER_SIZE, s)) {
                printf("Failed to receive +OK for GET.\n");
                fclose(localFile);
                continue;
            }

            long totalRec = 0;
            while (totalRec < size) {
                long remaining = size - totalRec;
                size_t bytesToRead = min(remaining, BUFFER_SIZE);
                size_t bytesRead = fread(buffer, 1, bytesToRead, s);
                fwrite(buffer, 1, bytesRead, localFile);
                totalRec += bytesRead;
            }

            fclose(localFile);

        } 
        else {
            printf("\nERROR --> not a valid option");
        }

        fflush(s);
        printf("\nOption\nL) --> list of files\nD) --> download files\nQ) --> Quit\n");
    }

    fclose(s);
    close(fd);
    free(serverSelect);
    return 0;
}
