/* 
 * File:   account.cpp
 * Author: damien
 * Description of method and done in account.h file.
 * account implementation.
 * 
 */
#include <cstdlib>
#include <stdio.h>
#include <pvm3.h>
#include "../SHARED/command.h"
#include "../SHARED/config.h"
#include "account.h"


account::account(){
}

void account::init(int number, int inicialBalance){
    //account initialisation.
    this->accountnumber = number;
    this->balance = float(inicialBalance);
    pvm_spawn(accountntocall, (char**)0, PvmTaskHost, accountservern, 1, tids);
}

account::account(const account& orig) {
}

account::~account() {
}

//Use of send method with the messagetag msgaccountadd 
float account::add(float value){
   return this->send(value, msgaccountadd);
}

//Use of send method with the messagetag msgaccountremove 
float account::remove(float value){
    return this->send(value, msgaccountremove);
}

void account::notifybalanceorerror(int& dtid){
    this->formatedresult = new char[255];
    
    //balance result already checker in send method.
    //If the result generated an error message, send the error message back to the user.
    if(this->errorinfo != NULL)
    {
        sprintf(formatedresult, this->errorinfo);
    }
    else
    {
        sprintf(formatedresult, "Account number %d, balance = %f, working on account tid = %d \n", this->accountnumber, this->balance, this->tids[0]);
    }
    
     pvm_initsend(PvmDataDefault);
     
     pvm_pkstr(formatedresult);
     pvm_send(dtid, msgbankresult);
}

void account::propagateexit(){
    pvm_initsend(PvmDataDefault);
    pvm_send(tids[0], msgaccountexit);
}


float account::send(float value, int msgtag){
    //Before processing an operation, the error message is set to NULL
    this->errorinfo = NULL;
    
    pvm_initsend(PvmDataDefault);
    pvm_pkfloat(&balance, 1, 1);
    pvm_pkfloat(&value, 1, 1);
    
    pvm_send(tids[0], msgtag);
    
    
    pvm_recv(-1, msgaccountresult);
    
    //Temporary result coming from account process.
    //This value is checked by the checker in order to affect the balance.
    float tempresult;
    pvm_upkfloat(&tempresult, 1, 1);
    
    //If the checker is false, the error message is asigned and the notification check in notifybalanceorerror is affected.
    if(checker(tempresult) != true)
    {
        this->errorinfo = "The current operation cannot be performed. The current account cannot be under 0";
    }
    
    return this->balance;
}

bool account::checker(float result){
    //If the result is less than 0, the balance isn't affected.
    if(result < 0)
    {
        return false;
    }
    
    this->balance = result;
    return true;
}
