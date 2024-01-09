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


           class Iterator {
                private:
                    const Directory& directory;
                    size_t position;

                public:
                    Iterator(const Directory& directory, size_t position) : directory(directory), position(position) {}

                    // Overloading the * operator to get the current element
                    File* operator*() const {
                        return directory.files[position];
                    }

                    // Overloading the ++ operator to move to the next element
                    Iterator& operator++() {
                        ++position;
                        return *this;
                    }

                    // Overloading the != operator to check for inequality
                    bool operator!=(const Iterator& other) const {
                        return position != other.position;
                    }
            };

            Iterator begin() const {
                return Iterator(*this, 0);
            }

            Iterator end() const {
                return Iterator(*this, files.size());
            }

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

            class Iterator {
               
                public:
                    Iterator(const RegularFile& file, size_t position) : file(file), position(position) {}

                    // Overloading the * operator to get the current element
                    char operator*() const {
                        return file.data[position];
                    }

                    // Overloading the ++ operator to move to the next element
                    Iterator& operator++() {
                        ++position;
                        return *this;
                    }

                    // Overloading the != operator to check for inequality
                    bool operator!=(const Iterator& other) const {
                        return position != other.position;
                    }
                private:
                    const RegularFile& file;
                    size_t position;
                };

            Iterator begin() const {
                return Iterator(*this, 0);
            }

            Iterator end() const {
                return Iterator(*this, data.size());
            }

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