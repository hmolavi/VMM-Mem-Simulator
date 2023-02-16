///@file mainDriver.cpp
///@author Hossein Molavi (hmolavi@uwaterloo.ca)
///@brief Parser and Main Driver 
///@version 2.0
///@date 2023-01-27
///
///@copyright Copyright (c) 2023
///


#include "virtualMachine.cpp"

using namespace std;

int main(){
    string cmd;	
    unsigned int PID;
    VirtualMachineManager* VMM {nullptr};

    cin >> cmd;
    int MODE = (cmd == "ORDERED") ? 1 : 0 ; 

    while (cin >> cmd){
        if("M"==cmd){
            unsigned int memory_size, page_size;
            cin >> memory_size;
            cin >> page_size;
            VMM = new VirtualMachineManager(memory_size, page_size, MODE);
            cout << "success" << endl;
        }

        else if ("INSERT" == cmd){
            cin >> PID;
            cout << VMM->insertPID(PID) << endl;
        }
        
        else if ("SEARCH" == cmd){
            cin >> PID;
            VMM->printPID(PID);
        }

        else if ("WRITE" == cmd){
            int ADDR, x;
            cin >> PID;
            cin >> ADDR;
            cin >> x;
            cout << VMM->writeMem(PID, ADDR, x) << endl;
        }
        
        else if ("READ" == cmd){
            int ADDR;
            cin >> PID;
            cin >> ADDR;
            VMM->printMem(PID, ADDR);
        }

        else if ("PRINT" == cmd && MODE){
            cin >> PID;
            VMM->printPosition(PID);
        }

        else if ("DELETE" == cmd){
            cin >> PID;
            cout << VMM->deletePID(PID) << endl;
        }

        else if ("END" == cmd){
            delete VMM;
            return 0;
        }
    }
}