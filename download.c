#include <stdio.h>
#include <stdlib.h>
#include <libsocket/libinetsocket.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

# define BUFFER 1000

int main(int argc, char * argv[]) {

    // potential software design
    //char design[] = {'\\', '-', '.', '-', '/'};
    int count = 0;

    int fstSize = 10;
    char serverName[BUFFER] = ".cs.sierracollege.edu";
    char *serverSelect = (char *)malloc(fstSize * sizeof(char));

    char request[3];

    printf("Connect to Server --> (newark, london):  ");
    
     while(fgets(serverSelect, BUFFER, stdin)){

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
    char response[BUFFER];
    char fileName[BUFFER];
    char garbageBuff[3];
    long size;
    unsigned char buffer[BUFFER];
    long transfered;
    int remaining;

    printf("\nOption\nL) --> list of files\nD) --> download files\nQ) --> Quit\n");

    fflush(s);
    
    while(fgets(request, 3, stdin)){

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
            
            while(fgets(response, BUFFER, s)){

                if(*response == '.'){
                break;

                }

                printf("%s", response);
                fflush(s);

            }

        }

        else if(*request == 'D'){

           printf("\nFile to Download: ");
           
           fgets(fileName, BUFFER, stdin);

            char *nl = strchr(fileName, '\n');

            if(nl){

                *nl = '\0';

            }

            FILE *localFile = fopen(fileName, "wb");

            fprintf(s,"SIZE %s\n", fileName);

            fgets(response, BUFFER, s);

            sscanf(response, "+OK %ld", &size);

            fprintf(s, "GET %s\n", fileName);

            if(size == 0){

                fread(buffer, 0, size, s);
                fwrite(buffer, 0, size,localFile);
                fflush(s);

            }

        if(size < 1000 && size != 0){

                fread(garbageBuff, 3, 3, s);
                fread(buffer, 1, size, s); // --> -5 for size, same as below
                fwrite(buffer, 1, size,localFile);
                fflush(s);

            }

            if(size >= 1000){

                fread(garbageBuff, 3, 3, s);
                
                while(transfered <= remaining){

                    transfered++;
                    remaining = size - ((transfered)*(BUFFER));

                    fread(buffer, 1, BUFFER, s);
                    fwrite(buffer, 1, BUFFER,localFile);
                    
                }

                fread(buffer, 1, remaining, s);
                fwrite(buffer, 1, remaining,localFile);

            }

            fflush(s);
        }

        else{

            printf("\nERROR --> not a valid option");
            
        }
 
        fflush(s);
        printf("\nOption\nL) --> list of files\nD) --> download files\nQ) --> Quit\n");


    } 


}
