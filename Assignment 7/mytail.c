#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int args,char **argv)
{   int pipeFlag=0;
    if(args<3){
        char buffer[1];
        FILE *fp;

        fp = fopen("temptail.txt", "w+");
        if(read(STDIN_FILENO,buffer,1)){//if there is something in the input
            pipeFlag=1;
            fprintf(fp,buffer);//printing and reading from buffer
            while(read(STDIN_FILENO,buffer,1))//gets a chunk of data from buffer
                fprintf(fp,buffer);//printing and reading from buffer
            fclose(fp);
            if(args!=2){
                printf("\n There should be atleast 1 argument which tells the last n number of lines to print");
                exit(0);
            }
        }else{
        printf("There should be exactly 2 arguments");
        exit(0);
        }
    }else if(args>3){
        printf("\nInvalid number of arguments\n");
        exit(0);
    }

    FILE* fp;
    int n;

    sscanf(argv[1],"-%d",&n);
    // Open file in binary mode
    // wb+ mode for reading and writing simultaneously
    if(pipeFlag)
        fp = fopen("temptail.txt", "r");
    else
        fp = fopen(argv[2], "r");
    if (fp == NULL)
    {
        printf("Error while opening file %s",argv[2]);
        exit(0);
    }
    int count = 0;  // To count '\n' characters
    long position;
    char str[200];
    // Go to End of file
    if (fseek(fp, 0, SEEK_END))
        perror("fseek() failed");
    else
    {
        // position will contain no. of chars in
        // input file.
        position = (long)ftell( fp);

        // search for '\n' characters
        while (position)
        {
            // Move 'position' away from end of file.
            if (!fseek( fp, --position, SEEK_SET))
            {
                if (fgetc( fp) == '\n')

                    // stop reading when n newlines
                    // is found
                    if (count++ == n)
                        break;
            }
            else
                perror("fseek() failed");
        }

        // print last n lines
        while (fgets(str, sizeof(str),  fp))
            printf("%s", str);
    }
    printf("\n\n");
    fclose(fp);
    if(pipeFlag)
        remove("temptail.txt");
    return 0;
}
