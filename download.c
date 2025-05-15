#include <stdio.h>
#include <stdlib.h>
#include <libsocket/libinetsocket.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

# define BUFFER_SIZE 1000
# define min(a,b) (((a) < (b)) ? (a) : (b))

int main(int argc, char * argv[]) {

    // potential software design
    //char design[] = {'\\', '-', '.', '-', '/'};
    int count = 0;

    int fstSize = 10;
    char serverName[BUFFER_SIZE] = ".cs.sierracollege.edu";
    char *serverSelect = (char *)malloc(fstSize * sizeof(char));

    char request[10];

    printf("Connect to Server --> (newark, london):  ");
    
     while(fgets(serverSelect, BUFFER_SIZE, stdin)){

        char *nl = strchr(serverSelect,'\n');

        if(nl){

            *nl = '\0';
            break;

        }
        
    }

    strcat(serverSelect, serverName);

    // opening a file discriptor
    int fd = create_inet_stream_socket(serverSelect, "3456", LIBSOCKET_BOTH, 0);
    
    while(fd < 0){

        printf("\nERROR --> No connection to the port");
        exit(1);
    }

    printf("\nConnected to the Socket");

    FILE *s = fdopen(fd, "r+");
    char response[BUFFER_SIZE];
    char fileName[BUFFER_SIZE];
    char garbageBuff[3];
    long size;
    unsigned char buffer[BUFFER_SIZE];
    long totalRec = 0;
    long remaining;
    long bytes_wanted;
    long bytes_recieved;

    printf("\nOption\nL) --> list of files\nD) --> download files\nQ) --> Quit\n");
    
    while(fgets(request, 10, stdin)){

        char *nl = strchr(request, '\n');

        if(nl){

            *nl = '\0';

        }

        if(*request == 'Q'){

            fprintf(s, "QUIT\n");
            break;    

        }

        else if(*request == 'L'){

            fprintf(s, "LIST\n");
            
            while(fgets(response, BUFFER_SIZE, s)){

                if(*response == '.'){
                break;

                }

                printf("%s", response);
                fflush(s);

            }

        }

        else if(*request == 'D'){

           printf("\nFile to Download: ");
           
           fgets(fileName, BUFFER_SIZE, stdin);

            char *nl = strchr(fileName, '\n');

            if(nl){

                *nl = '\0';

            }

            FILE *localFile = fopen(fileName, "wb");

            fprintf(s,"SIZE %s\n", fileName);

            fgets(response, BUFFER_SIZE, s);

            sscanf(response, "+OK %ld", &size);

            fprintf(s, "GET %s\n", fileName);

            fread(garbageBuff, 3, 3, s);

            while(totalRec < size){

                remaining = size - totalRec;
                bytes_wanted = min(remaining, BUFFER_SIZE);
                bytes_recieved = fread(buffer, 1, bytes_wanted, s);
                fwrite(buffer,1, bytes_recieved, localFile);
                totalRec = totalRec + bytes_recieved;
                printf("End of Loop\n");

                fflush(s);
                
            }

        }

        else{

            printf("\nERROR --> not a valid option");
            
        }
 
        fflush(s);
        printf("\nOption\nL) --> list of files\nD) --> download files\nQ) --> Quit\n");


    } 


}
