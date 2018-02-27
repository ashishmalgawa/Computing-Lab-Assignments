#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include<string.h>
using namespace std;
void createChild(string username,string hashtags){
    int pid=fork();//creating child 
    if(pid==0){//pid will be 0 for the newly created child only since 
        string command="python 17CS60R81_summary_creater.py ";//generating command line command
        command.append(username);
        command.append(" ");
        command.append(hashtags);
        system(command.c_str());//since system only accepts character array 
        exit(0);//child will exit after successfull summary creation
    }
}
int main(){
    int parent=getpid();//process id of the parent
    while(true){
        string username;
        string hashtags="\"";
        if(getpid()==parent){ //checking if the current process is parent
            cout<<"Enter User (enter STOP to stop)"<<endl;
            cin>>username;
            if(username=="STOP")
                break;
            while(true){
                cout<<"Enter Hashtag \n NOTE:\n 1.Enter STOP to stop\n 2.Use # to start hashtags"<<endl;
                string hashtag;
                cin>>hashtag;
                if(hashtag=="STOP")
                    break;
                hashtags.append(hashtag);
            }
        }
        hashtags.append("\"");
        createChild(username,hashtags);//calling method to create concurrent child process 
    }
    return 0;
}