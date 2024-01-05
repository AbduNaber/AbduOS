#ifndef OS_H
#define OS_H

#include "file.h"

#define DISK_SIZE 10
#define MB 1024*1024 // 1 mb = 1024*1024 bytes
#define ASTERISK "*"


namespace OS{
    
    class os{
        
        public:
            os(){};
            ~os(){
               // saveDisk();
            };
            
            void ls(std::string parameter);
            void mkdir(std::string name);
            void rm(std::string name);
            void cp(std::string sourceFilePath, std::string targetFilePath);
            void link(std::string name, std::string path);
            void cd(std::string path);
            std::string getContents(std::ifstream *diskFile);
            static void createDisk();
            void loadDisk();
            void saveDisk();

        //private: for debug
            std::vector<File *> files;
            
            
    };

    class shell {

        public:
            shell(){};
            ~shell(){};
            void run(OS::os & os);
            std::vector < std::string > parseCommand(const std::string& command) const ;
            bool isValidCommand(const std::vector < std::string > commands,const std::vector<File *> files) const ;
            void callOS(const std::vector < std::string > commands, OS::os & os)const ;
            File * findFile(const std::string& fileName, const std::vector<File*>& files) const ;

        private:
            
            std::map <std::string, std::string> commandParameters = {
                {"ls","[-R]"},
                {"mkdir","<name>"},
                {"rm","<name>"},
                {"cp","<sourceFilePath> <targetFilePath>"},
                {"link","<name> <path>"},
                {"cd","<path>"},
                {"cat","<name>"}
            };

    };
}

#endif // OS_H
