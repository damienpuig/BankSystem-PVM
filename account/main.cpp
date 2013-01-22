/* 
 * File:   main.cpp
 * Author: damien
 *
 * account process implementation.
 * This process represents the add and remove action for a specific account.
 */

#include <cstdlib>
#include <stdio.h>
#include <pvm3.h>
#include "../SHARED/command.h"

using namespace std;

/*
 * 
 */
int main() {
    //account process implementation properties.
    int ptid;
    //sent balance and sent entry to affect the balance, on a specific message tag.
    float balance, entry, result;

    ptid = pvm_parent();
    
    //If the bank sends exit status to the account process, the account process will exit.
    while(pvm_nrecv(-1, msgaccountexit) == 0)
    {
        //Add process
        if(pvm_nrecv(-1, msgaccountadd) > 0)
        {
            pvm_upkfloat(&balance, 1, 1);
            pvm_upkfloat(&entry, 1, 1);
            
            result = balance + entry;
            
            pvm_initsend(PvmDataDefault);
            pvm_pkfloat(&result, 1, 1);
            pvm_send(ptid, msgaccountresult);
        }
        
        //Remove process
        if(pvm_nrecv(-1, msgaccountremove) > 0)
        {
            pvm_upkfloat(&balance, 1, 1);
            pvm_upkfloat(&entry, 1, 1);
            
            result = balance - entry;
            
            pvm_initsend(PvmDataDefault);
            pvm_pkfloat(&result, 1, 1);
            pvm_send(ptid, msgaccountresult);
        }
    }
    
    pvm_exit();
    return 0;
}

