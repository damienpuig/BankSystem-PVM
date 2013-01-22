/* 
 * File:   main.cpp
 * Author: damien
 *
 * Client process:
 * the client uses a client context in order to perform request (see client class), and a bank mapper
 * the initialise the bank sent back to it from master.
 * 
 * Once the client is connected, the client object is initialised, and a request is sent to the master asking for bank availability.
 * 
 */

#include <cstdlib>
#include <stdio.h>
#include <pvm3.h>
#include "client.h"
#include "../SHARED/command.h"
#include "../SHARED/config.h"

using namespace std;


/*
 * 
 */

int main(int argc, char** argv) {

    //Client context object.
    client currentclient;
    
    //Initialisation of context (spwaning master)
    currentclient.init();
    
    //The client asks for available banks.
    currentclient.initbankchoices();
    
    printf("Choice a bank:\n");
    
    //The client has to enter a bank id and a user id.
    //The number of accounts depends on the user id and bank id.
    currentclient.initaccountsstates();
 
    //Client choices.
    currentclient.menu();
    
    //Chosen action
    int action;
     
    while(true)
    {
        scanf("%d", &action);
        switch(action)
        {
                case 0:
                    currentclient.performdeposit();
                    break;
                case 1:
                    currentclient.performwithdraw();
                    break;
                    
                case 2:
                    currentclient.performtransfert();
                    break;
                case 3:
                      currentclient.performintertransfert();
                    break;
                case 4:
                    currentclient.getbalance();
                    break;
                case 5:
                    currentclient.switchaccount();
                    break;
                case 6:
                    currentclient.propagateexit();
                    pvm_exit();
                    return 0;
                    break;
        }
                    
        currentclient.menu();
    }
}
