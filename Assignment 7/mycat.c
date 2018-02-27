#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
void get_cwd(char *cwd){
    FILE *fp;
    fp = fopen("cwd.txt", "r");
    if(fp==NULL){
        perror("File error");
    }
    fgets(cwd, 1024, (FILE*)fp);
    fclose(fp);
}
void convert_absolute(char *cwd,char *path){
    if(path[0]=='/'){//absolute
        strcpy(cwd,path);
    }else{
        get_cwd(cwd);
        strcat(cwd,"/");
        strcat(cwd,path);
        strcat(cwd,"\000");
    }
}
int main(int argc, char * argv[])
{
    if(argc!=2)
    {   char buffer[1];
        if(read(STDIN_FILENO,buffer,1)){//if there is something in the input
            printf("%s",buffer);//printing and reading from buffer
            while(read(STDIN_FILENO,buffer,1))//gets a chunk of data from buffer
                printf("%s",buffer);//printing and reading from buffer

        }else{
            printf("Only one argument is allowed");

        }
        exit(0);
    }
    FILE * file;
    char buffer[30];
    char cwd[1024];
    struct stat my_stat;
    convert_absolute(cwd,argv[1]);
    stat(cwd, &my_stat);//getting information about current file
    if(S_ISDIR(my_stat.st_mode)){//checking if current file is a directory
        printf("%s is a directory\n",argv[1]);
        return (-1);
    }
    file = fopen(cwd, "r");
    if (NULL == file)
    {
        perror("file error");
        return (-1);
    }
    while (EOF != fscanf(file, "%30[^\n]\n", buffer))
    {
        printf("%s\n", buffer);//printing each line
    }
    fclose(file);
    return (0);
}