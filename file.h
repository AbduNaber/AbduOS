#ifndef _FILE_H_
#define _FILE_H_

#include <vector>
#include <string>
#include <map>
#include <stdexcept>

namespace OS{


    class File{
        private:
            std::string name;
            std::string path;
            size_t size;
            std::string date;
            std::string type;

        public:
            File(){};
            File(std::string name, std::string path, size_t size, std::string date, std::string type);
            ~File(){};

            std::string getName();
            std::string getPath();
            size_t getSize();
            std::string getDate();
            std::string getType();

           
            virtual void rm() = 0;
            virtual void cat() = 0;
            
            virtual void setData(std::string data) = 0;
        
            void setName(std::string name);
            void setPath(std::string path);
            void setSize(size_t size);
            void setDate(std::string date);
            
            void setType(std::string type);


    };


    class Directory : public File{
        private:
            // files and directories inside this directory
            std::vector<File *> files; 
            

        public:
            Directory(){};
            Directory(std::string name, std::string path, size_t size, std::string date, std::string type);
            ~Directory(){};

           void setData(std::string data){
                throw std::invalid_argument("This is a directory!");
           };
           
           void rm(){
                throw std::invalid_argument("This is a directory!");
           };
           void cat(){
                throw std::invalid_argument("This is a directory!");
           };
           void addFile(File * file){
               files.push_back(file);
           };
           void searchSubFiles(const std::vector<File *> & files);
           const std::vector < File *>  getFiles(){ return files ;}
           void deleteFile(File * file);

    };

    class RegularFile : public File{
        private:
            std::string data;

        public:
            RegularFile(){};
            RegularFile(std::string name, std::string path, size_t size, std::string date, std::string type, std::string data);
            ~RegularFile(){};

            void setData(std::string data);
            std::string getData();
            void rm();
            void cat();

    };

    class linkedFile : public File{
        private:
            File * _mainFile = nullptr;
            std::string _mainFilePath ;             
        public:
            linkedFile(){};
            linkedFile(std::string name, std::string path, size_t size, std::string date, std::string type, std::string mainFilePath);
            ~linkedFile(){};

            void setMainFile(File * mainFile){
                _mainFile = mainFile;
            };
            File * getMainFile(){
                return _mainFile;
            };
            File * getMainFile() const{
                return _mainFile;
            };
            File * findMainFile(const std::vector<File *> & files){
                for(auto file : files){
                    if(file->getPath() == _mainFilePath){
                        return file;
                    }
                }
                throw std::invalid_argument("Main file not found!");
            };
            std::string getMainFilePath(){
                return _mainFilePath;
            };

            void setData(std::string data){
                _mainFile->setData(data);

            };
            
            void rm(){
                _mainFile->rm();
            };

            void cat(){
                _mainFile->cat();
            };

    };

            
    };





#endif // _FILE_H_