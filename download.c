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

            FILE *localFile = fopen(fileName, "w");

            fprintf(s,"SIZE %s\n", fileName);

            fgets(response, BUFFER, s);

            sscanf(response, "+OK %ld", &size);

            fprintf(s, "GET %s\n", fileName);

            fread(garbageBuff, 3, 3, s);

            printf("successfully captured --> %s", garbageBuff);

            fread(buffer, 1, size-5, s);
            fwrite(buffer, 1, size-5,localFile);

            //if(size <= 1000){

               // printf("size was smaller than 1000");
              //  char *buffer = malloc(size);
                //int readByte = fread(buffer, 1, size, s);

                //if(readByte == size){

                //printf("successfully have read %d of Bytes", readByte);

                //}

            //}




        }

        else{

            printf("\nERROR --> not a valid option");
            
        }
 
        fflush(s);
        printf("\nOption\nL) --> list of files\nD) --> download files\nQ) --> Quit\n");


    } 


}
