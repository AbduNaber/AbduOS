#include <iostream>
#include <string>
#include <fstream>
#include "file.h"


namespace OS{
        
        File::File(std::string name, std::string path, size_t size, std::string date, std::string type): name(name), path(path), size(size), date(date), type(type)
        {}


        std::string File::getName(){
            return this->name;
        }
    
        std::string File::getPath(){
            return this->path;
        }
    
        size_t File::getSize(){
            return this->size;
        }
    
        std::string File::getDate(){
            return this->date;
        }
    
        std::string File::getType(){
            return this->type;
        }
    
        void File::setName(std::string name){
            this->name = name;
        }
    
        void File::setPath(std::string path){
            this->path = path;
        }
    
        void File::setSize(size_t size){
            this->size = size;
        }
    
        void File::setDate(std::string date){
            this->date = date;
        }
    
        void File::setType(std::string type){
            this->type = type;
        }
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Directory::Directory(std::string name, std::string path, size_t size, std::string date, std::string type): File(name,path,size,date,type){
            this->files = std::vector<File *>();
        }

        void Directory::searchSubFiles(const std::vector<File *> & files){
            
            for(auto file : files){
                if(file->getPath().substr(0,this->getPath().size())== this->getPath()){
                    this->files.push_back(file);
                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        RegularFile::RegularFile(std::string name, std::string path, size_t size, std::string date, std::string type, std::string data): File(name,path,size,date,type){
            this->data = data;

        }
        void RegularFile::setData(std::string data){
            this->data = data;
        }
        std::string RegularFile::getData(){
            return this->data;
        }
        void RegularFile::rm(){
            std::cout << "rm called on regular file" << std::endl;
        }

        void RegularFile::cat(){
            std::cout << this->getData()<< std::endl;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        linkedFile::linkedFile(std::string name, std::string path, size_t size, std::string date, char * data, std::string type, std::string mainFilePath): File(name,path,size,date,type){
            this->_mainFilePath = _mainFilePath;
        }
        
    
      
      
        
    


}