#ifndef _GTUSet_H_
#include <vector>

template <class T>
class GTUSet{
    public:
        GTUSet();
        GTUSet(const GTUSet<T >&o);
    
        void add(T j);
        bool isIn(T j);
        GTUSet& operator=(const GTUSet<T>& o);
        private:
            std::vector<T> setelems;

};
#ifndef // end of header

#include "GTUSet.h"

template <class T>
GTUSet<T>::GTUSet(){}

template <class T>
GTUSet<T>::GTUSet(const GTUSet<T>& o){
    setelems = o.setelems;}


template <class T>
GTUset& GTUSet<T>::operator=(const GTUSet<T>& o){
    setelems = o.setelems;}

template <class T>
void GTUSet<T>::add(T j){
    for(auto i: setelems){
        if(i == j)
            throw std::invalid_argument("duplicated");
    }
        
    setelems.push_back(j);}

template <class T>
bool GTUSet<T>::isIn(T j){
    for(auto i: setelems){
        if(i == j){
            return true;}
           
    }
    else return false;
}

int main(){
    GTUSet <char> S;
    GTUSet <char> S1;
    try {
        S.add('a');
        s.add(2.1);
    }
    catch (std::invalid_argument& e){
        std::cout << e.what() ;}
    std::cout <<"a: " <<S.isIn('a');
    return 0;}