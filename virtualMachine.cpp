///@file virtualMachine.cpp
///@author Hossein Molavi (hmolavi@uwaterloo.ca)
///@brief Class Definitions
///@version 1.0
///@date 2023-02-09
///
///@copyright Copyright (c) 2023
///

#include "virtualMachine.h"

RandomAccessMemory::RandomAccessMemory(unsigned int memory_size){p_Physical_Mem = new int [memory_size]{911};}
RandomAccessMemory::~RandomAccessMemory(){ delete[] p_Physical_Mem; }

VirtualMachineManager::VirtualMachineManager(unsigned int memory_size, unsigned int page_size, int mode){
    RAM = new RandomAccessMemory(memory_size);
    maxPIDCapacity = memory_size/page_size;
    pageSize = page_size;
    COLLISIONS_RESOLVE_MODE = mode;

    p_PIDArray = new ProcessID [maxPIDCapacity];
    if ( COLLISIONS_RESOLVE_MODE ) 
        p_PIDVector = new std::vector<ProcessID>[maxPIDCapacity]; //array of vectors of PIDs
}

VirtualMachineManager::~VirtualMachineManager(){
    delete RAM;
    delete[] p_PIDArray;
    if ( COLLISIONS_RESOLVE_MODE ) delete[] p_PIDVector;
}

unsigned int VirtualMachineManager::hash_function1(unsigned int PID_key){
    return PID_key % maxPIDCapacity;
}

unsigned int VirtualMachineManager::hash_function2(unsigned int PID_key){
    unsigned int h2 = (PID_key/maxPIDCapacity) % maxPIDCapacity;
    if (h2%2 == 0) return ++h2;
    return h2;
}

unsigned int VirtualMachineManager::double_hashing_function(unsigned int h1, 
                                                            unsigned int h2, 
                                                            unsigned int idx){
    return (h1 + idx*h2) % maxPIDCapacity;
}

std::string VirtualMachineManager::insertPID(unsigned int PID_key){
    if (maxPIDCapacity == curPIDCount ) return "failure";
    unsigned int h1 = hash_function1(PID_key);

    /// Open addressing 
    if ( !COLLISIONS_RESOLVE_MODE ){    
        if ( p_PIDArray[h1].key == PID_key) return "failure";
        if ( p_PIDArray[h1].key == 0 && p_PIDArray[h1].isDeleted==false ){
            p_PIDArray[h1].key = PID_key;
            p_PIDArray[h1].p_mem_index = &RAM->p_Physical_Mem[h1*pageSize];
            ++curPIDCount;
            return "success";
        }
        
        unsigned int h2 = hash_function2(PID_key);
        unsigned int h_ki = double_hashing_function(h1, h2, 0);
        unsigned int deleted_idx;
        bool deleted_found {false};


        for ( unsigned int i{1}; i < maxPIDCapacity + 1 ; i++ ){
            if ( p_PIDArray[h_ki].isDeleted == true && deleted_found==false){ 
                deleted_idx = h_ki; 
                deleted_found = true; 
            }
            if ( p_PIDArray[h_ki].key == PID_key ) return "failure";
            h_ki =  double_hashing_function(h1, h2, i);
        }

        if( deleted_found ){
            p_PIDArray[deleted_idx].key = PID_key;
            p_PIDArray[deleted_idx].isDeleted = false;
            p_PIDArray[deleted_idx].p_mem_index = &RAM->p_Physical_Mem[deleted_idx*pageSize];
        }
        else {
            h_ki = double_hashing_function(h1, h2, 0);  
        
            for ( unsigned int i{1} ; p_PIDArray[h_ki].key != 0 ; i++)
                h_ki =  double_hashing_function(h1, h2, i);

            p_PIDArray[h_ki].key = PID_key;
            p_PIDArray[h_ki].p_mem_index = &RAM->p_Physical_Mem[h_ki*pageSize];
        }
    } 
    
    /// Chaining
    else if ( COLLISIONS_RESOLVE_MODE ){

        for(unsigned int i{0}; i < p_PIDVector[h1].size(); i++)
            if ( p_PIDVector[h1][i].key == PID_key ) return "failure";
        
        int arrIndex = h1;                          // finding empty memory
        while( p_PIDArray[arrIndex].key != 0 ){
            arrIndex++;
            if( arrIndex == maxPIDCapacity ) arrIndex = 0;
        }

        p_PIDArray[arrIndex].key = PID_key;
        p_PIDArray[arrIndex].p_mem_index = &RAM->p_Physical_Mem[arrIndex*pageSize];        

        if (p_PIDVector[h1].size() ==0)
            p_PIDVector[h1].push_back(p_PIDArray[arrIndex]);

        else{
            for(int i{0}; i < p_PIDVector[h1].size()+1; i++){
                if( i == p_PIDVector[h1].size() ){
                    p_PIDVector[h1].push_back(p_PIDArray[arrIndex]);
                    break;
                }
                if( PID_key > p_PIDVector[h1][i].key ){
                    p_PIDVector[h1].insert(p_PIDVector[h1].begin() + i, p_PIDArray[arrIndex]);
                    break;
                }
            }
        }
    }
    ++curPIDCount;
    return "success";
}

void VirtualMachineManager::printPosition(unsigned int m){
    if ( !COLLISIONS_RESOLVE_MODE ) return;

    if ( p_PIDVector[m].size() > 0 ){
        for(unsigned int i{0}; i < p_PIDVector[m].size(); i++){
            if ( p_PIDVector[m][i].key != 0 ){
                std::cout << p_PIDVector[m][i].key << " ";
            }
        }
        std::cout << std::endl;
        return;
    } 
    std::cout << "chain is empty" << std::endl;
}

void VirtualMachineManager::printPID(unsigned int PID_key){
    if ( curPIDCount == 0 ) {
        std::cout << "not found" << std::endl;
        return;
    }

    unsigned int h1 = hash_function1(PID_key);

    /// Open addressing 
    if ( !COLLISIONS_RESOLVE_MODE ){    
        if ( p_PIDArray[h1].key == PID_key){
            std::cout << "found " << PID_key << " in " << h1 << std::endl;
            return;
        }
        unsigned int h2 = hash_function2(PID_key);
        unsigned int h_ki = double_hashing_function(h1, h2, 0);
        
        for ( unsigned int i{1} ; i < maxPIDCapacity ; i++){
            h_ki = double_hashing_function(h1, h2, i);
            if ( p_PIDArray[h_ki].key == PID_key ){
                std::cout << "found " << PID_key << " in " << h_ki << std::endl;
                return;
            } 
        }
    } 

    /// Chaining
    else if ( COLLISIONS_RESOLVE_MODE ){
        for(unsigned int i{0}; i < p_PIDVector[h1].size(); i++){
            if ( p_PIDVector[h1][i].key == PID_key ){
                std::cout << "found " << PID_key << " in " << h1 << std::endl;
                return;
            }
        }
    }

    std::cout<< "not found" << std::endl;
}


void VirtualMachineManager::printMem(unsigned int PID_key, unsigned int ADDRESS){
    if ( curPIDCount == 0 ) {
        std::cout << "failure" << std::endl;
        return;
    }
    if (ADDRESS > pageSize-1) {
        std::cout << "failure" << std::endl;
        return;
    }

    unsigned int h1 = hash_function1(PID_key);

    /// Open addressing 
    if ( !COLLISIONS_RESOLVE_MODE ){    
        if ( p_PIDArray[h1].key == PID_key){
            std::cout << ADDRESS << " " << *(p_PIDArray[h1].p_mem_index + ADDRESS) << std::endl;
            return;
        }
        unsigned int h2 = hash_function2(PID_key);
        unsigned int h_ki = double_hashing_function(h1, h2, 0);
        
        for ( unsigned int i{1} ; i < maxPIDCapacity; i++){
            h_ki = double_hashing_function(h1, h2, i);
            if ( p_PIDArray[h_ki].key == PID_key ){
                std::cout << ADDRESS << " " << *(p_PIDArray[h_ki].p_mem_index + ADDRESS) << std::endl;
                return;
            } 
        }
    } 

    /// Chaining
    else if ( COLLISIONS_RESOLVE_MODE ){
        for(unsigned int i{0}; i < p_PIDVector[h1].size(); i++){
            if ( p_PIDVector[h1][i].key == PID_key ){
                std::cout << ADDRESS << " " << *(p_PIDVector[h1][i].p_mem_index + ADDRESS) << std::endl;
                return;
            }
        }
    }
    
    std::cout<< "failure" << std::endl;
    return;
}

std::string VirtualMachineManager::writeMem(unsigned int PID_key, unsigned int ADDRESS, int value){
    if ( curPIDCount == 0 ) return "failure";

    if (ADDRESS > pageSize-1) return "failure";
    unsigned int h1 = hash_function1(PID_key);

    /// Open addressing 
    if ( !COLLISIONS_RESOLVE_MODE ){    
        if ( p_PIDArray[h1].key == PID_key){
            *(p_PIDArray[h1].p_mem_index + ADDRESS) = value;
            return "success";
        }
        unsigned int h2 = hash_function2(PID_key);
        unsigned int h_ki = double_hashing_function(h1, h2, 0);
        
        for ( unsigned int i{1} ; i < maxPIDCapacity != 0; i++){
            h_ki = double_hashing_function(h1, h2, i);
            if ( p_PIDArray[h_ki].key == PID_key ){
                *(p_PIDArray[h_ki].p_mem_index + ADDRESS) = value;
                return "success";
            } 
        }
    } 

    /// Chaining
    else if ( COLLISIONS_RESOLVE_MODE ){
        for(unsigned int i{0}; i < p_PIDVector[h1].size(); i++){
            if ( p_PIDVector[h1][i].key == PID_key ){
                *(p_PIDVector[h1][i].p_mem_index + ADDRESS) = value;
                return "success";;
            }
        }
    }
    
    return "failure";
}

std::string VirtualMachineManager::deletePID(unsigned int PID_key){
    if ( curPIDCount == 0 ) return "failure";

    unsigned int h1 = hash_function1(PID_key);

    /// Open addressing 
    if ( !COLLISIONS_RESOLVE_MODE ){    
        if ( p_PIDArray[h1].key == PID_key){
            p_PIDArray[h1].key = 0;
            p_PIDArray[h1].isDeleted = true;
            --curPIDCount;
            return "success";
        }
        
        unsigned int h2 = hash_function2(PID_key);
        unsigned int h_ki = double_hashing_function(h1, h2, 0);
        
        for ( unsigned int i{1} ; i < maxPIDCapacity+1; i++){
            if ( p_PIDArray[h_ki].key == PID_key ) {
                p_PIDArray[h_ki].key = 0;
                p_PIDArray[h_ki].isDeleted = true;
                --curPIDCount;
                return "success"; 
            }
            h_ki = double_hashing_function(h1, h2, i);
        }
    } 
    /// Chaining
    else if ( COLLISIONS_RESOLVE_MODE ){        
        for(unsigned int i{0}; i < p_PIDVector[h1].size(); i++){
            if ( p_PIDVector[h1][i].key == PID_key ){  
                p_PIDVector[h1].erase(p_PIDVector[h1].begin() + i);
                
                int arrIndex = h1;                          // free allocated array_idx
                while( p_PIDArray[arrIndex].key != PID_key ){
                    arrIndex++;
                    if( arrIndex == maxPIDCapacity ) arrIndex = 0;
                }
                p_PIDArray[arrIndex].key = 0;

                --curPIDCount;
                return "success";   
            }   
        }
    }
    return "failure";
}