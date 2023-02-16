///@file virtualMachine.h
///@author Hossein Molavi (hmolavi@uwaterloo.ca)
///@brief Class Declerations
///@version 1.0
///@date 2023-02-09
///
///@copyright Copyright (c) 2023
///

#include <string>
#include <vector>
#include <iostream>

/// @brief Virtual Machine / PID
class ProcessID {
    friend class VirtualMachineManager;
    public:
    
    ProcessID(){}

    private:
    int* p_mem_index {nullptr};  // Pointer to appropriate index in p_Physical_Mem (ie RAM)
    unsigned int key{0};
    bool isDeleted{false};
};

/// @brief RAM | 'Physical' memory handled by the VMM 
class RandomAccessMemory {  
    friend class VirtualMachineManager;
    public:

    RandomAccessMemory(unsigned int memory_size);
    ~RandomAccessMemory();
        
    private:
    int* p_Physical_Mem {nullptr};
};  

/// @brief VMM
class VirtualMachineManager {
    public:

    VirtualMachineManager(){};
    VirtualMachineManager(unsigned int memory_size, unsigned int page_size, int mode);
    ~VirtualMachineManager();

    std::string insertPID(unsigned int PID);                                        // INSERT
    void printPID(unsigned int PID_key);                                            // SEARCH
    void printPosition(unsigned int m);                                             // PRINT
    void printMem(unsigned int PID_key, unsigned int ADDRESS);                      // READ
    std::string writeMem(unsigned int PID_key, unsigned int ADDRESS, int value);    // WRITE
    std::string deletePID(unsigned int PID_key);                                    // DELETE


    private:
    unsigned int hash_function1 (unsigned int PID_key);
    unsigned int hash_function2 (unsigned int PID_key);
    unsigned int double_hashing_function (unsigned int h1, unsigned int h2, unsigned int idx);

    RandomAccessMemory* RAM {nullptr};
    unsigned int pageSize; 
    ProcessID* p_PIDArray{nullptr};                 // An array of ProcessID
    std::vector<ProcessID>*p_PIDVector{nullptr};    // An array of Vectors, Created based on COLLISIONS_RESOLVE_MODE
    unsigned int maxPIDCapacity;                    // Size of hash table, m = N/P
    unsigned int curPIDCount{0};                    // Used capacity always start at 0, table full if curPIDCount = maxPIDCapacity
    
    int COLLISIONS_RESOLVE_MODE {-1};               // Deciding factor between Chaining and Open addressing.
                                                    // 0 if Open addressing, 1 if Chaining
};
