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

            static void createDisk();
            void loadDisk();
            void saveDisk();

        private:
            std::vector<File *> files;
            
            
    };
}

#endif // OS_H
