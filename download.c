#include <stdio.h>
#include <stdlib.h>
#include <libsocket/libinetsocket.h>
#include <unistd.h>
#include <string.h>

// Defining the buffer size as well as min function --> searched up the min function
#define BUFFER_SIZE 1000
#define min(a, b) (((a) < (b)) ? (a) : (b))

int main(int argc, char *argv[]) {

    // creating an array for the address and allocating memory for the user input for selecting the server
    int fstSize = 10;
    char serverName[BUFFER_SIZE] = ".cs.sierracollege.edu";
    char *serverSelect = (char *)malloc(fstSize * sizeof(char));

    char request[10];

    // obtaining the chosen network with the fgets function, as well as trimming the newline
    printf("Connect to Server --> (newark, london):  ");
    while (fgets(serverSelect, fstSize, stdin)) {
        char *nl = strchr(serverSelect, '\n');
        if (nl) {
            *nl = '\0';
            break;
        }
    }

    // copying the server to the server address
    strcat(serverSelect, serverName);


    // setting up a connection to the socket
    int fd = create_inet_stream_socket(serverSelect, "3456", LIBSOCKET_BOTH, 0);
    if (fd < 0) {
        fprintf(stderr, "\nERROR --> No connection to the port\n");
        exit(1);
    }

    
    printf("\nConnected to the Socket\n");

    // openning up a file descriptor
    FILE *s = fdopen(fd, "r+");
    if (!s) {
        printf("fdopen failed");
        close(fd);
    }

    // response array is for containing the server response from the file descriptor
    // creating a buffer array for large scaled files
    char response[BUFFER_SIZE];
    char fileName[BUFFER_SIZE];
    unsigned char buffer[BUFFER_SIZE];
    long size = 0;

    // presenting the menu
    printf("\nOption\nL) --> list of files\nD) --> download files\nQ) --> Quit\n");

    // 
    while (fgets(request, sizeof(request), stdin)) {

        // trimming the new line 
        char *nl = strchr(request, '\n');
        if (nl) { 
            *nl = '\0';
        }

        // If the user inputs Q, the program will quit and file descriptor content will be flushed
        if (*request == 'Q') {
            fprintf(s, "QUIT\n");
            fflush(s);
            break;
        } 

        // L is for list, which it will present all the files available until it reaches the . at the end
        else if (*request == 'L') {
            fprintf(s, "LIST\n");
            fflush(s);
            while (fgets(response, BUFFER_SIZE, s)) {
                if (*response == '.')
                    break;
                printf("%s", response);
            }
        } 

        // D is for download, which length prefix framing algorithm will be used to download the contents
        else if (*request == 'D') {
            
            // requesting the file name from the user
            printf("\nFile to Download: ");
            fgets(fileName, BUFFER_SIZE, stdin);
            
            // trimming the line if the file was valid
            nl = strchr(fileName, '\n');
            if (nl){   
                *nl = '\0';
            }
            // Request file size
            fprintf(s, "SIZE %s\n", fileName);
            fflush(s);
            if (!fgets(response, BUFFER_SIZE, s)) {
                fprintf(stderr, "Failed to get response from server.\n");
                
            }

            // utilizing the sscanf to trim out the +OK response from the server, just to contain
            // the number inside of the side variable
            if (sscanf(response, "+OK %ld", &size) != 1 || size <= 0) {
                printf("Invalid file size or file doesn't exist.\n");
                
            }

            // open a file in the mode for writing binary, as well as checking if it 
            // openned properly
            FILE *localFile = fopen(fileName, "wb");
            if (!localFile) {
                printf("File creation failed");
                
            }

            // requesting the file from the server by sending GET plus the file name
            fprintf(s, "GET %s\n", fileName);
            fflush(s);

            // Read and discard the "+OK\n" line
            if (!fgets(response, BUFFER_SIZE, s)) {
                printf("Failed to receive +OK for GET.\n");
                fclose(localFile);
            }

            // This is the length prefix framing algorithm that was mentioned in the video
            // but something to note is that is best to decleare variables ushc as remaining 
            // inside of the loop, otherwise the program will freeze for some reason
            long totalRec = 0;
            while (totalRec < size) {
                long remaining = size - totalRec;
                size_t bytesToRead = min(remaining, BUFFER_SIZE);
                size_t bytesRead = fread(buffer, 1, bytesToRead, s);
                fwrite(buffer, 1, bytesRead, localFile);
                totalRec += bytesRead;
            }

            // closing the localfile for writing binary data
            fclose(localFile);

        } 
        else {
            printf("\nERROR --> not a valid option");
        }

        // flushing out all the content from the file descriptor to make sure the program 
        // doesnt freeze
        fflush(s);
        printf("\nOption\nL) --> list of files\nD) --> download files\nQ) --> Quit\n");
    }

    // closing the file descriptor and setting the allocated memory free
    fclose(s);
    close(fd);
    free(serverSelect);
}
