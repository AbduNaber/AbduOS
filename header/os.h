#ifndef OS_H
#define OS_H

#include "file.h"

#define DISK_SIZE 10
#define MB 1024*1024 // 1 mb = 1024*1024 bytes
#define ASTERISK "*"


namespace OS{
    
    class os;
    class shell;

    
    class shell {

        public:
            shell(){};
            ~shell(){};
            void run(OS::os & os);
            std::vector < std::string > parseCommand(const std::string& command) const ;
            bool isValidCommand(const std::vector < std::string > commands,const std::vector<File *> files) const ;
            void callOS(const std::vector < std::string > commands, OS::os & os)const ;
            static File * findFile(const std::string& fileName, const std::vector<File*>& files) ;
            static std::string relativeToAbsolutePath(const std::string& path);
            friend class os;
            static File * currentFile ;
            static std::string currentPath ;
        private:
            
    };

    class os{
        
        public:
            os(){};
            ~os(){
               // saveDisk();
            };
            void setdiskName(std::string name) { diskName = name; };
            std::vector <File *> getFiles() const { return files; };
            void ls(std::string parameter,File * cf);
            void mkdir(std::string name);
            void rm(std::string name);
            void cp(std::string sourceFilePath, std::string targetFilePath);
            void link(std::string name, std::string path);
            void cat(std::string name);
            void cd(std::string path);
            void rmdir(std::string name);
            std::string getdate();
            void shutdown();

            std::string getContents(std::ifstream *diskFile,size_t size) const ;
            static void createDisk();
            std::string getNameFromPath(std::string path)const ;
            size_t getDiskSize()const ;
            void setDiskSize();

            void loadDisk();
            void saveDisk();
        private:
            std::string diskName = "disk1";
            std::vector<File *> files;
            size_t diskSize = 0;
                       
    };

}

#endif // OS_H
