#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;
struct shared{
    int readCount;
    int writeFlag=0;
    sem_t mutex;
    sem_t writer;
    sem_t finalWriter;
    sem_t childMutex;
    sem_t coutMutex;
    int numChild=0;
};//shared memory structure
typedef struct shared shared;
static shared *shared1;
//User defined class to manage all the files
class Files{
private:
    int reporters; //number of reporter or crawler processes
    fstream alltweets;//original tweet file
public:
    vector<fstream> tweetFiles;//vector of tweet files for each reporter
    fstream newtweets;//centralised file which will be read and write by processes

    Files(int reporters){
        this->reporters=reporters;
        alltweets.open("input/tweets_with_hashtags.txt",ios::in);//original tweet file
        remove("finaloutput.txt");//deleting ouput file if already exists
        mkdir("reader", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        mkdir("output", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        mkdir("reporters", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    }
    //creates files for each reporter
    int createFiles(){
        for (int i=0;i<reporters;i++){
            string filename;
            filename="reporters/";
            std::ostringstream ss;
            ss<<i;
            filename.append(ss.str());
            filename.append(".txt");
            tweetFiles.emplace_back(fstream{filename});//since we cannot use copy constructor in case of fstream objects
            tweetFiles[i].open(filename,ios::app);
        }

    }
    //adds tweets in each reporter file by distributing each line one by one in each file
    int setFiles(){
        string line;
        if (alltweets.is_open())
        { int i=0;
            while ( getline (alltweets,line) )
            {   if(tweetFiles[i%reporters].is_open())
                    tweetFiles[i%reporters]<<line<<"\n";
                else {
                    sem_wait(&shared1->coutMutex);
                    cout << "Unable to open file"<< endl;
                    sem_post(&shared1->coutMutex);
                }
                i++;
            }
            alltweets.close();
            for(int i=0;i<reporters;i++) {
                tweetFiles[i].close();
            }//closing all reporter files
        }
        else {
            sem_wait(&shared1->coutMutex);
            cout << "Unable to open file"<< endl;
            sem_post(&shared1->coutMutex);
        }
    }
    void deleteFiles(){
        //Closing the files
        for (int i=0;i<reporters;i++){
            tweetFiles[i].close();
            string filename;
            filename="reporters/";
            std::ostringstream ss;
            ss<<i;
            filename.append(ss.str());
            filename.append(".txt");
            remove(filename.c_str());//deleting reporter files
        }
        newtweets.close();
        remove("newtweets.txt");
        alltweets.close();
    }
};
//this process writes data in centralised file
void reporter(int i,Files *files){
    //calculating number of childs so that the parent doesn't exit if there is any child still alive
    //creating filename of current reporter
    string filename;
    filename="reporters/";
    std::ostringstream ss;
    ss<<i;
    filename.append(ss.str());
    filename.append(".txt");
    //filename created of current reporter
    fstream file;
    file.open(filename,ios::in);//opening reporter file
    string line;
    if (file.is_open())
    { int i=0;
        while ( getline (file,line) )
        {   sem_wait(&shared1->writer);
            //critical section open
            shared1->writeFlag=1;
            files->newtweets.open("newtweets.txt",ios::app);
            files->newtweets<<line<<"\n";
            files->newtweets.close();
            usleep(100000);//sleep in seconds
            //critical section close
            sem_post(&shared1->writer);
        }
    }
    else {
        sem_wait(&shared1->coutMutex);
        cout << "Unable to open file"<< endl;
        sem_post(&shared1->coutMutex);
    }

    //child will exit when every line is over
    sem_wait(&shared1->childMutex);
    shared1->numChild--;
    sem_post(&shared1->childMutex);
    exit(0);
}
void reader(string username,Files *files){
    string line;
    fstream file;
    while (shared1->writeFlag==0);//if there is no writer
    sem_wait(&shared1->mutex);
    shared1->readCount++;
    if (shared1->readCount == 1)
        sem_wait(&shared1->writer);
    sem_post(&shared1->mutex);
    //critical section begins
    files->newtweets.open("newtweets.txt",ios::in);
    file.open("reader/"+username+".txt",ios::app);
    if(files->newtweets.is_open()){

        while(getline(files->newtweets, line))
        {
            file << line << "\n";//adding data line by line into an intermediate file
        }

        files->newtweets.close();
        file.close();
    }else{
        sem_wait(&shared1->coutMutex);
        cout << "Unable to new tweets in reader"<<username<< endl;
        sem_post(&shared1->coutMutex);
    }
    //critical section over
    sem_wait(&shared1->mutex);
    shared1->readCount--;
    if (shared1->readCount == 0)
        sem_post(&shared1->writer);
    sem_post(&shared1->mutex);

}
//appending data of intermediate output file into final output
void finalWrite(string username){
    fstream file;
    string line;
    file.open("output/"+username+".txt",ios::in);
    if(file.is_open()){
        sem_wait(&shared1->finalWriter);
        //critical section
        fstream output;
        output.open("finaloutput.txt",ios::app);
        while(getline(file,line)){
            output<<line<<"\n";
        }
        output.close();
        file.close();
        //critical section over
        sem_post(&shared1->finalWriter);
    }
}
void createChild(string username,string hashtags,Files *files){
    sem_wait(&shared1->childMutex);
    shared1->numChild++;
    int pid=fork();//creating child
    sem_post(&shared1->childMutex);
    if(pid==0){//pid will be 0 for the newly created child only since

        string command="python 17CS60R81_summary_creater.py ";//generating command line command
        command.append(username);
        command.append(" ");
        command.append(hashtags);
        reader(username,files);
        system(command.c_str());//since system only accepts character array
        //writing into final output file
        finalWrite(username);
        //giving user a message that the file is added in final output
        sem_wait(&shared1->coutMutex);
        string temp="reader/"+username+".txt";
        remove(temp.c_str());
        temp="output/"+username+".txt";
        remove(temp.c_str());
        cout<<"Summary created for "<<username<<endl;
        sem_post(&shared1->coutMutex);
        sem_wait(&shared1->childMutex);
        shared1->numChild--;
        sem_post(&shared1->childMutex);
        exit(0);//child will exit after successful summary creation
    }
}
void cleanup(Files *files){
    //int munmap(void *addr, size_t length);
    sem_destroy(&shared1->writer);
    sem_destroy(&shared1->mutex);
    sem_destroy(&shared1->childMutex);
    sem_destroy(&shared1->coutMutex);
    sem_destroy(&shared1->finalWriter);
    munmap(shared1, sizeof(*shared1));
    //deleting the files
    files->deleteFiles();
    system("rmdir reader");
    system("rmdir output");
    system("rmdir reporters");

}
int main () {
    int reporters,ans;
    int parent=getpid();//process id of the parent
    cout<<"\n Input should be :input/tweets_with_hashtags.txt"<<endl;
    cout<<"Enter number of reporters"<<endl;
    cin>>reporters;
    Files files(reporters);
    files.createFiles();
    files.setFiles();
    //-----------------------------------------------------
    //  file is split into multiple files by now
    //-----------------------------------------------------

    shared1 = (shared *)mmap(0, sizeof(*shared1), PROT_READ | PROT_WRITE,
                             MAP_SHARED | MAP_ANON, -1, 0);
    //-----------------------------------------------------

    /*void *mmap(void *addr, size_t lengthint " prot ", int " flags ,
           int fd, off_t offset);
     int sem_init(sem_t *sem, int pshared, unsigned int value);
     */
    //shared memory
    //-----------------------------------------------------

    sem_init(&shared1->mutex, 1, 1);
    sem_init(&shared1->writer, 1, 1);
    sem_init(&shared1->childMutex, 1, 1);
    sem_init(&shared1->childMutex, 1, 1);
    sem_init(&shared1->coutMutex, 1, 1);
    sem_init(&shared1->finalWriter, 1, 1);

    //-----------------------------------------------------
    //semaphores are created now
    //-----------------------------------------------------

    for(int i=0;i<reporters;i++){
        sem_wait(&shared1->childMutex);
        shared1->numChild++;
        pid_t pid=fork();
        sem_post(&shared1->childMutex);
        if(pid==0){
            reporter(i,&files);
        }
    }
    while(true){
        string username;
        string hashtags="\"";

        if(getpid()==parent){ //checking if the current process is parent
            //so that no other cout interferes with the cin
            sem_wait(&shared1->coutMutex);
            cout<<"Enter User (enter STOP to stop)"<<endl;
            cin>>username;
            sem_post(&shared1->coutMutex);
            if(username=="STOP")
                break;
            while(true){
                sem_wait(&shared1->coutMutex);
                cout<<"Enter Hashtag \n NOTE:\n 1.Enter STOP to stop\n 2.Use # to start hashtags"<<endl;
                string hashtag;
                cin>>hashtag;
                sem_post(&shared1->coutMutex);
                if(hashtag=="STOP")
                    break;
                hashtags.append(hashtag);
            }
        }
        hashtags.append("\"");
        createChild(username,hashtags,&files);//calling method to create concurrent child process
    }
    //waiting for all the children to exit
    if(getpid()==parent) {
        while(1){
            sem_wait(&shared1->childMutex);
            if(shared1->numChild==0)
                break;
            usleep(200);
            sem_post(&shared1->childMutex);
        }
    }
    //deleting files and deleting shared memory and semaphores
    cleanup(&files);
    return 0;
}
