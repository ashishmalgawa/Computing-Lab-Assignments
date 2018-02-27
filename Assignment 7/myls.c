#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
static char perms_buff[30];
const char *get_perms(mode_t mode)
{
    char ftype = '?';

    if (S_ISREG(mode)) ftype = '-';
    if (S_ISLNK(mode)) ftype = 'l';
    if (S_ISDIR(mode)) ftype = 'd';
    if (S_ISBLK(mode)) ftype = 'b';
    if (S_ISCHR(mode)) ftype = 'c';
    if (S_ISFIFO(mode)) ftype = '|';

    sprintf(perms_buff, "%c%c%c%c%c%c%c%c%c%c %c%c%c", ftype,
            mode & S_IRUSR ? 'r' : '-',
            mode & S_IWUSR ? 'w' : '-',
            mode & S_IXUSR ? 'x' : '-',
            mode & S_IRGRP ? 'r' : '-',
            mode & S_IWGRP ? 'w' : '-',
            mode & S_IXGRP ? 'x' : '-',
            mode & S_IROTH ? 'r' : '-',
            mode & S_IWOTH ? 'w' : '-',
            mode & S_IXOTH ? 'x' : '-',
            mode & S_ISUID ? 'U' : '-',
            mode & S_ISGID ? 'G' : '-',
            mode & S_ISVTX ? 'S' : '-');

    return (const char *)perms_buff;
}
void get_cwd(char *cwd){
    FILE *fp;
    fp = fopen("cwd.txt", "r");
    fgets(cwd, 1024, (FILE*)fp);
    fclose(fp);
}
int main(int argc, char** argv){
    char *pointer=NULL;
    char datestring[256];
    DIR *dp=NULL,*for_count=NULL;
    struct stat my_stat;
    struct dirent *sd=NULL,*for_count_dirent=NULL;
    if(argc==1){ //if there are no arguments (1st argument is ./a.out)
        char cwd[1024];
        get_cwd(cwd); //getting current working directory of myshell
        pointer = cwd;
        dp=opendir((const char*)pointer);
        if(dp==NULL){
            perror("directory error");
            exit(0);
        }
    }else if(argc>2) {
        printf("\n \033[31m You can only give one argument with myls command\n");
        exit(0);
    }else if (argv[1][0]=='/'){ //if the path is absolute
        pointer=argv[1];
        dp=opendir((const char*)argv[1]);
        if(dp==NULL){
            perror("directory error");
            exit(0);
        }
    }
    else if(argv[1][0]!='/'){//if path is relative
        char cwd[1024];
        get_cwd(cwd);
        strcpy(cwd,"/");
        strcpy(cwd,argv[1]);
        pointer = cwd;
        dp=opendir((const char*)pointer);
        if(dp==NULL){
            perror("directory error");
            exit(0);
        }
    }
    char path[1024];
    char buf[1024];
    struct group grp;
    struct group *grpt;
    struct passwd pwent;
    struct passwd *pwentp;
    strcpy(path,pointer);
    while((sd=readdir(dp))!=NULL)
    {   if(sd->d_name[0]=='.')//if the file is hidden
            continue;
        //---adding the current file to path---
        strcat(path,"/");
        strcat(path,sd->d_name);
        strcat(path,"\0");
        //---adding the current file to path finished---

        stat(path, &my_stat);//getting information about the current file

        printf("%s  \t ",get_perms(my_stat.st_mode));//changing the mode in the required format
        printf("%2d  \t ",(int)my_stat.st_nlink);//number of hardlinks
        if (!getpwuid_r(my_stat.st_uid, &pwent, buf, sizeof(buf), &pwentp))
            printf("%s  \t", pwent.pw_name);//username of owner
        else
            printf("id:%d  \t",my_stat.st_uid);
        if (!getgrgid_r (my_stat.st_gid, &grp, buf, sizeof(buf), &grpt))
            printf("%s  \t", grp.gr_name);//groupname of owner
        else
            printf("id:%d  \t", my_stat.st_gid);
        printf("%6d \t", (int)my_stat.st_size);//size of file
        struct tm time;
        localtime_r(&my_stat.st_atime, &time); //last data modification time
        strftime(datestring, sizeof(datestring), "%F %T", &time); /* Get localized date string. */
        printf("%s  \t", datestring);
        printf("%s  \t ",sd->d_name);//file name
        printf("\n");
        //removing current file from path
        int x= (int) (strlen(path) - strlen(sd->d_name));
        path[x-1]='\0';
        //removed
    }
}
