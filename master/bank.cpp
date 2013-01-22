/* 
 * File:   bank.cpp
 * Author: damien
 * 
 * Bank implementation on master side
 */

#include <cstdlib>
#include <stdio.h>
#include <pvm3.h>
#include "../SHARED/command.h"
#include "../SHARED/config.h"
#include "bank.h"

bank::bank() {
}

bank::bank(const bank& orig) {
}

bank::~bank() {
    
}

//initBootsptapping asks the banks to start it process.
//the bank process is call worker. the master spawn a bank, and sends imediately the user potentially asking for the bank.
int bank::initBootsptapping(char* servername, int usertid){
    this->bservername = servername;
    int tids[10];
    pvm_spawn(bankntocall, (char**)0, PvmTaskHost, this->bservername, 1, tids);
      
    this->tid = tids[0];
    
    //msgbankactiveuser is the action receiving the user id for initialisation.
    pvm_initsend(PvmDataDefault);
    pvm_pkint(&usertid, 1, 1);
    pvm_send(this->tid, msgbankactiveuser);
    
    pvm_recv(-1, msgmasterresult);
    
   return this->tid;
}

//the Master can ask a bank to exit and stop current processes.
//msgbankexit is used in that case.
void bank::propagateexit(){
    pvm_initsend(PvmDataDefault);
    pvm_send(this->tid, msgbankexit);
}


