#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
void get_cwd(char *cwd){
    FILE *fp;
    fp = fopen("cwd.txt", "r");
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
void recursive_remove(int args,char *argv){
    struct stat my_stat;
    struct dirent *sd=NULL;
    DIR *dp;
    char cwd[1024];
    convert_absolute(cwd,argv);//converting the path to absolute and if it is absolute then taking as it is path and adding it to cwd
    stat(cwd, &my_stat);//getting information about current file
    if(!S_ISDIR(my_stat.st_mode)){//if it is not a directory then deleting the file no recursive algorithm is needed for the file
        if(remove(cwd)==0){
            printf("successfully deleted %s\n",cwd);
        }else{
            printf("%s\n",cwd);
            perror("remove directory error");
        }
        return;
    }
    dp=opendir(cwd);
    if(dp==NULL){
        perror("directory error");
        return;
    }
    char path[1024];
    strcpy(path,cwd);//adding the current path of the particular directory into path
    while((sd=readdir(dp))!=NULL) {
	if(sd->d_name[0]=='.'&&sd->d_name[1]==NULL)//checking if it is.
		continue;	
	if(sd->d_name[0]=='.'&&sd->d_name[1]=='.')//checking if it is ..
            continue;
        //adding current file in path
        strcat(path,"/");
        strcat(path,sd->d_name);
        strcat(path,"\0");
        //added
        stat(path, &my_stat);
        printf("removing :%s: \n",path);
        if(S_ISDIR(my_stat.st_mode)){//if it is a directory then recursively calling the same function to remove the directory in recursive manner
            recursive_remove(args,path);
        }else{
            if(remove(path)==0){
                printf("successfully deleted %s\n",cwd);
            }else{
                printf("%s\n",cwd);
                perror("remove error");
            }
        }
        //resetting path start
        int x= (int) (strlen(path) - strlen(sd->d_name));
        path[x-1]='\0';
        //resetting path over
    }
    //deleting original directory
    if(remove(cwd)==0){
        printf("successfully deleted %s\n",cwd);
    }else{
        printf("%s\n",cwd);
        perror("remove directory error");
    }
}
int main(int args,char **argv){
    if(args==1){
        printf("you should enter some argument in rm\n");
    }else if(args>=2&&strcmp(argv[1],"-r")==0){
        for (int i = 2; i <args ; ++i) {
            recursive_remove(args,argv[i]);
        }
    }else{//if not recursive then deleting the files
        for (int i = 1; i <args ; ++i) {
            char cwd[1024];
            convert_absolute(cwd,argv[i]);
            if(remove(cwd)==0){
                printf("successfully deleted %s\n",cwd);
            }else{
                printf("%s\n",cwd);
                perror("remove error");
            }
        }
    }
    return 0;
}
