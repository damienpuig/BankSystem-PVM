/* 
 * File:   main.cpp
 * Author: damien
 *
 * 
 */
#include <cstdlib>
#include <stdio.h>
#include <pvm3.h>
#include "../SHARED/command.h"
#include "../SHARED/config.h"
#include "bank.h"

using namespace std;

/*
 * Master attempt to deliver available bank to the user, and propagate the exit statut among banks and account recursively.
 * All pvm requests are non-blocked using pvm_nrecv instead of pvm_recv
 */
int main(int argc, char** argv) {
    int usertid;
    //list of 3 bank objects. Bank object contains tid banks, and server used to contact the bank.
    //A bank asks for a userid in order to initialize connection between user and banks.
    bank banks[3];
    
    //Root user.
    usertid = pvm_parent();
    
    //Banks are spawned once the user connects to the master
    // That part can use a database in order to retreive potential banks. Here banks are hard coded.
    banks[0].initBootsptapping(bankservern, usertid);
    banks[1].initBootsptapping(bankservern, usertid);
    banks[2].initBootsptapping(bankservern, usertid);
    
    
    //While loop in order to keep master processing on potential exit.
    // Once a message is sent has msgmasterexit, the master propagate the exit status to all banks before exiting
    while(pvm_nrecv(-1, msgmasterexit) == 0)
      {
       
        //msgmasterlistbanks is used by the client to get the bank list
       if(pvm_nrecv(-1, msgmasterlistbanks) > 0)
       { 
            pvm_initsend(PvmDataDefault);
            
            pvm_pkint(&banks[0].tid,1,1);
            pvm_pkint(&banks[1].tid,1,1);
            pvm_pkint(&banks[2].tid,1,1);
            
            //Send packed banks tids back to user, using msgmasterresult
            pvm_send(usertid, msgmasterresult);
       }
       
       
     }
    
    //If the user asks for an exit, master propagates exit on banks.
    
    for (int i = 0; i < sizeof(banks); i++) {
        banks[i].propagateexit();
    }
    
    //Message to send before exiting master server.
    char *formatedresult = new char[255];
    sprintf(formatedresult, "Exit propagated to banks and accounts");
    
     pvm_initsend(PvmDataDefault);
     pvm_pkstr(formatedresult);
     pvm_send(usertid, msgbankresult);
     
    pvm_exit();
    
    return 0;
}

