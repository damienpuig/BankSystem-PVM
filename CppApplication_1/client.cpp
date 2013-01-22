/* 
 * File:   client.cpp
 * Author: damien
 * 
 * Client class implementation - see client.h for method description.
 * 
 */
#include <cstdlib>
#include <stdio.h>
#include <pvm3.h>
#include "client.h"
#include "../SHARED/command.h"
#include "../SHARED/config.h"
#include "bankmap.h"

client::client() {
}

client::client(const client& orig) {
}

client::~client() {
}

void client::init(){
    //Spawning master
    this->mytid = pvm_mytid();
    printf("Ready to spawn master ");
    
    int tids[10];
    this->info = pvm_spawn(masterntocall, (char**)0, PvmTaskHost, masterservern, 1, tids);
    this->mastertid = tids[0];
    
    printf("pvm_spawn=%d, master tid=%d\n", this->info, this->mastertid);
}

void client::switchaccount(){
    //get the new account context id.
   printf("Account number:\n");
   scanf("%d", &selectedaccountid);
    
   //Pack the new account context id and send to the current bank context using banktid task id.
    pvm_initsend(PvmDataDefault);
    pvm_pkint(&selectedaccountid, 1, 1);
    pvm_send(this->banktid, msgbankswitchcontextaccount);
    
    printf("awaiting reply..\n");
    this->info = pvm_recv(-1, msgbankresult);
    this->info = pvm_upkstr(this->formatedresult);
    printf("%s\n", this->formatedresult);
}

void client::getbalance(){
     printstate("Get Balance");
     
     //asks for context balance - not data to be sent
    pvm_initsend(PvmDataDefault);
    pvm_send(this->banktid, msgbankbalance);
    
    printf("awaiting reply..\n");
    
    pvm_recv(-1, msgbankresult);
    pvm_upkstr(this->formatedresult);
    
     printf("%s\n", this->formatedresult);
    
    scanf("transaction succesfully\n");
}

void client::performdeposit(){
    printstate("Deposit");
    
    //deposit amount to perform
    float amount;
    scanf("%f", &amount);
    
    
    pvm_initsend(PvmDataDefault);
    pvm_pkfloat(&amount, 1, 1);
    pvm_send(this->banktid, msgbankdeposit);
    
    printf("awaiting reply..\n");
    
    pvm_recv(-1, msgbankresult);
    pvm_upkstr(this->formatedresult);
    
    printf("%s\n", this->formatedresult);
    
    printf("transaction finished :deposit:\n");  
}

void client::performwithdraw(){
     printstate("Withdraw");
     
     //withdraw amount to perform
    float amountr;
    scanf("%f", &amountr);
    
    
    pvm_initsend(PvmDataDefault);
    pvm_pkfloat(&amountr, 1, 1);
    pvm_send(this->banktid, msgbankwithdraw);
    
    printf("awaiting reply..\n");
    
    pvm_recv(-1, msgbankresult);
    pvm_upkstr(this->formatedresult);
    
    printf("%s\n", this->formatedresult);
    printf("transaction finished :remove:\n");  
}

void client::initbankchoices(){
     
    pvm_initsend(PvmDataDefault);
    pvm_send(this->mastertid, msgmasterlistbanks);
    
    printf("get banks performing..\n");
    
    //this->banks[0].tid
    
    pvm_recv(-1, msgmasterresult);
    
    
    pvm_upkint(&banks[0].tid,1,1);
    pvm_upkint(&banks[1].tid,1,1);
    //If there is only 2 accounts, the value of bank[2] isn't affected.
    pvm_upkint(&banks[2].tid,1,1);
    
    //Hard coded values... pvm doesn't allow to pack multiple string..
    this->banks[0].name = "HSBC";
     this->banks[1].name = "Loyds";
      this->banks[2].name = "Natwest";
    
    
    printf("Choose %s-%d Choose 0\n",this->banks[0].name, this->banks[0].tid);
    printf("Choose %s-%d Choose 1\n",this->banks[1].name, this->banks[1].tid);
    printf("Choose %s-%d Choose 2\n",this->banks[2].name, this->banks[2].tid);
}

void client::bankchoices(){
    printf("Choose %s-%d Choose 0\n",this->banks[0].name, this->banks[0].tid);
    printf("Choose %s-%d Choose 1\n",this->banks[1].name, this->banks[1].tid);
    printf("Choose %s-%d Choose 2\n",this->banks[2].name, this->banks[2].tid);
}

void client::initaccountsstates(){
    //Get the bank - set the bank to the client context
    printf("bank:\n");
    int bid;
    scanf("%d", &bid);
    this->banktid = banks[bid].tid;
    this->selectedbankid = bid;
    
    //Get the user id - set the user id to the client context
    printf("user:\n");
    int uid;
    scanf("%d", &uid);
    this->userid = uid;
    
    //Send packed values to init on bank process
    pvm_initsend(PvmDataDefault);
    pvm_pkint(&bid, 1, 1);
    pvm_pkint(&this->userid, 1, 1);
    pvm_send(this->banktid, msgbankinituseraccounts);
    
    printf("awaiting reply..\n");
    
    pvm_recv(-1, msgbankresult);
    pvm_upkstr(this->formatedresult);
    pvm_upkint(&selectedaccountid, 1, 1);
    printf("%s\n", this->formatedresult);
}

void client::performtransfert(){
    //get the current amount
    printf("Transfert from current account %d, bank %d, enter amount:\n", this->selectedaccountid, this->selectedbankid);
    float value;
    scanf("%f", &value);
    
    //get the account id receiving the transfert
    printf("Account targeted:\n");
    int accid;
    scanf("%d", &accid);
    
    pvm_initsend(PvmDataDefault);
    pvm_pkfloat(&value, 1, 1);
    pvm_pkint(&accid, 1, 1);
    pvm_send(this->banktid, msgbanktransfert);
    
    printf("awaiting reply..\n");
    
    pvm_recv(-1, msgbankresult);
    pvm_upkstr(this->formatedresult);
    printf("%s\n", this->formatedresult);
}

void client::performintertransfert(){
    //Get the transaction amount
    printf("Transfert from current account %d, enter amount:\n", this->selectedaccountid, this->selectedbankid);
    float value;
    scanf("%f", &value);
    
    //Get the bank number
    printf("Bank targeted:\n");
    int btobbanknumber;
    scanf("%d", &btobbanknumber);
    
    //Get the account number
    printf("Account targeted:\n");
    int accnumber;
    scanf("%d", &accnumber);
    
    
    printf("values targeted: value %f, bank TID %d, account %d\n", value, this->banks[btobbanknumber].tid, accnumber);
    
    pvm_initsend(PvmDataDefault);
    pvm_pkfloat(&value, 1, 1);
    pvm_pkint(&this->banks[btobbanknumber].tid, 1, 1);
    pvm_pkint(&accnumber, 1, 1);
    pvm_send(this->banktid, msgbankintertransfert);
    
    printf("awaiting reply..\n");
    
    pvm_recv(-1, msgbankresult);
    pvm_upkstr(this->formatedresult);
    printf("%s\n", this->formatedresult); 
}

//Propagate the exit to the master process
void client::propagateexit(){
    pvm_initsend(PvmDataDefault);
    pvm_send(this->mastertid, msgmasterexit);
    pvm_recv(-1, msgmasterresult);
    pvm_upkstr(this->formatedresult);
    printf(formatedresult); 
}

//menu shortcut
void client::menu(){
    printf("Choice an action (Currently on bank %d, account %d):\n",this->selectedbankid, this->selectedaccountid);
    printf("===== Deposit - 0\n");
    printf("===== Withdraw - 1\n");
    printf("===== Transfert - 2\n");
    printf("===== Transfert to another bank - 3\n");
    printf("===== Balance - 4\n");
    printf("===== Switch account - 5\n");
    printf("===== Exit - 6\n");
}

//Shortcut state of the current account context.
void client::printstate(char* actionstr){
    printf("%s to current account %d, bank %d, enter amount:\n",actionstr, this->selectedaccountid, this->selectedbankid);
}