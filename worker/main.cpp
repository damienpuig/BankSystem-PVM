/* 
 * File:   main.cpp
 * Author: damien
 *
 * Head office implementation.
 */

#include <cstdlib>
#include <stdio.h>
#include <pvm3.h>
#include "../SHARED/command.h"
#include "../SHARED/config.h"
#include "account.h"

using namespace std;

/*
 * the worker is a potential head office process.
 * It attempts to be connected to the user and responding requests.
 * A bank uses a context called context. a Bank process only uses a account at the same time, in terms of requests.
 * The bank answers to a given account. An account is an abject mapping the bank and the account processes.
 * Once a request is sent to the bank, the bank asks for the given context operations. The context is an account class
 * processing add or remove on the balance.
 *  All pvm requests are non-blocked using pvm_nrecv instead of pvm_recv
 *
 */
int main() {
    //masterid is used in order to respond to the server that spawned the current bank.
    //userid is used to type the bank instance by user.
    //accountid is the id of the account context.
    int mastertid, usertid, userid, accountid;
    //List of user accounts. An user can have only 3 accounts per banks.
    account accounts[3];
    
    //Entry of the user for a given account context.
    //All entries are processed in float and notified in string.
    float entry;
    
    //The request context, of type account.
    // Once an user asks for bank accounts, the context is set depending on the last account instanciated.
    account context;
    
    //Initialisation of bank spawner (Master).
    mastertid = pvm_parent();
    
    

    //The while depends on the propagation of master exit.
     while(pvm_nrecv(-1, msgbankexit) == 0)
    {
         //Each request owns an errorinfo if the transaction fails
         context.errorinfo = NULL;
         
         //request asked by the master parent to set the userid used to return informations and statements
         if(pvm_nrecv(-1, msgbankactiveuser) > 0)
        {
            pvm_upkint(&usertid, 1, 1);
            pvm_initsend(PvmDataDefault);
            pvm_send(mastertid, msgmasterresult);
         }
         
         //Initialisation of bank account.
         // This part is the bad part of my project. The user request the bank to initialise bank account
         // per specific bank, using the bank id and user account id.
         // The current actions retreive accounts depending on the bank id and account id.
         // This part is hard coded.
       if(pvm_nrecv(-1, msgbankinituseraccounts) > 0)
        {
            int bankid;
            pvm_upkint(&bankid, 1, 1);
            pvm_upkint(&userid, 1, 1);
            
            char* switchresult = new char[255];
            
            //If the bank has id 0, then check userid. If the user id is ok, instanciate accounts
            if(bankid == 0)
            { 
            if(userid == 1000)
                 {
                accounts[0].init(0, 800);
                accounts[1].init(1, 1000);
                accounts[2].init(2, 200);
                
                accountid = 2;
                context = accounts[2];
                 
                sprintf(switchresult, "Accounts for User: %d\n-----Account %d, balance %f\n-----Account %d, balance %f\n-----Account %d, balance %f\n You are on account %d\n",
                        userid,
                        accounts[0].accountnumber,accounts[0].balance, 
                        accounts[1].accountnumber, accounts[1].balance,
                        accounts[2].accountnumber, accounts[2].balance,
                        accountid);
                 }
            //If the user didn't success to authenticate in bank 0, then set error message
            else
            {
                 sprintf(switchresult, "Accounts not found for bank %d and user id %d", bankid, userid);
            }
            }
            else if(bankid == 1)
            {
                 if(userid == 2000)
            {
                accounts[0].init(0, 554);
                accounts[1].init(1, 200);
                accountid = 1;
                context = accounts[1];
                
                sprintf(switchresult, "Accounts for User: %i\n-----Account %d, balance %f\n------Account %d, balance %f\n You are on account %d\n", userid,
                        accounts[0].accountnumber,accounts[0].balance, accounts[1].accountnumber, accounts[1].balance, accountid);
            }
            else
            {
                 sprintf(switchresult, "Accounts not found for bank %d and user id %d", bankid, userid);
            }
            }
            else if(bankid == 2)
            {
            if(userid == 3000)
            {
                accounts[0].init(0, 300);
                accounts[1].init(1, 900);
                accountid = 1;
                context = accounts[1];
                
                sprintf(switchresult, "Accounts for User: %d\n-----Account %d, balance %f\n------Account %d, balance %f\nYou are on account %d\n", userid,
                        accounts[0].accountnumber,accounts[0].balance, accounts[1].accountnumber, accounts[1].balance, accountid);
            }
            else
            {
                 sprintf(switchresult, "Accounts not found for bank %d and user id %d", bankid, userid);
            }
            
            }
            
            //send context (last account instanciated) back to the user, and notification about initialisation.
            pvm_initsend(PvmDataDefault);
            pvm_pkstr(switchresult);
            pvm_pkint(&accountid, 1, 1);
            pvm_send(usertid, msgbankresult);
        } 
       
         //Once an user is connected to the bank and owns bank accounts, the account context is set on the bank process.
         // If the user wants to change from an account to another, the user asks for switching context.
        if(pvm_nrecv(-1, msgbankswitchcontextaccount) > 0)
        {
            //asked account
            int newcontext;
            pvm_upkint(&newcontext, 1, 1);
            
            //context switching.
            accountid = newcontext;
            context = accounts[newcontext];
            
            //notification sent back to the user.
            char* switchresult = new char[255];
            sprintf(switchresult, "You are on account %d", context.accountnumber);
            
            pvm_initsend(PvmDataDefault);
            pvm_pkstr(switchresult);
            pvm_send(usertid, msgbankresult);
        } 
         
         // The user can request the balance of the context.
        if(pvm_nrecv(-1, msgbankbalance) > 0)
        {
            //sends the balance back to the user.
           context.notifybalanceorerror(usertid);
         }
                 
         //The user can request for a deposit action.
         // The user sends a float to the bank process, the bank sending 
         //the float to the context for processing
         // the balance is notified once the action is perfomed, successfully or not (see account implementation).
         if(pvm_nrecv(-1, msgbankdeposit) > 0)
        {
             pvm_upkfloat(&entry, 1, 1);
             context.add(entry);
             context.notifybalanceorerror(usertid);
         }
         
         //The user can request for a withdraw action.
         // The user sends a float to the bank process, the bank sending 
         //the float to the context for processing.
         // the balance is notified once the action is perfomed, successfully or not (see account implementation).
        if(pvm_nrecv(-1, msgbankwithdraw) > 0)
        {
             pvm_upkfloat(&entry, 1, 1);
             context.remove(entry);
             context.notifybalanceorerror(usertid);
         }
        
          //The user can request for a transfert between account instances.
         // The user sends a float and a account id (same bank) to the bank process, the bank sending 
         //the float for processing.
         // the action is separated in two phases:
         //The bank try to withdraw the amount from the current account, and try to add the amount on the other account.
         // the balance is notified once the action is perfomed, successfully or not (see account implementation).
         if(pvm_nrecv(-1, msgbanktransfert) > 0)
        {
            int targetedid;
            pvm_upkfloat(&entry, 1, 1);
            pvm_upkint(&targetedid, 1, 1);
            
            //attempt to withdraw entry on context.
            context.remove(entry);
            
            //if error, send the error back to the user, and break.
            if(context.errorinfo == NULL)
            {
                context.notifybalanceorerror(usertid);
                break;
            }
            
            accounts[targetedid].add(entry);
          
            context.notifybalanceorerror(usertid);
             
         }
       
         //The user can request for an inter bank transfert.
         //The user sends the amount of the trasaction, the trageted bank, and the targeted account.
        if(pvm_nrecv(-1, msgbankintertransfert) > 0)
        {
            FILE *op;
            op = fopen(LOGPATHINTERTRANSFERT, "w");
            
            fprintf(op, "entering in b2b transfert\n");
            
            //Getting user entries
            char* intertransfertresult = new char[255];
            int targetedaccountid;
            int targetedbanktid;
            int banksourse = pvm_mytid();
            
            pvm_upkfloat(&entry, 1, 1);
            pvm_upkint(&targetedbanktid, 1, 1);
            pvm_upkint(&targetedaccountid, 1, 1);
            
            fprintf(op, "sent parameters: bank %d, account %d, value %f\n", targetedbanktid, targetedaccountid, entry);
            
            //attempt to remove the amount on the context.
            context.remove(entry);
            
            //If error, break the inter transfert and notify the user.
            if(context.errorinfo != NULL)
            {
                context.notifybalanceorerror(usertid);
                break;
            }
            
            //Packing information to send to the targeted bank.
            pvm_initsend(PvmDataDefault);
            pvm_pkfloat(&entry, 1, 1);
            pvm_pkint(&banksourse, 1, 1);
            pvm_pkint(&targetedaccountid, 1, 1);
            
            fprintf(op, "message packed\n");
            
            //Once the information are sent, the user need to know if the transfert reached the targeted bank.
            //info helps to know the bank activity. If the bank is shutdown between user initialisation and inter transfert,
            //the request status is less than 0.
            //A bank uses the messagetag msgbanktobanktransfert to communicate a request to another bank.
            int info = pvm_send(targetedbanktid, msgbanktobanktransfert);
            
            fprintf(op, "value of the sent message info = %d\n", info);
            
            fprintf(op, "awaiting reply..\n");
             
            //If the request status is less than 0
            //recover amount on context.
            if(&info < 0)
            { 
                 fprintf(op, "error on sending -> info %d\n", info);
                 fprintf(op, "recovering amount -> entry %f\n", entry);
                 
             //recovering money on context.
            context.add(entry);
             
            if(context.errorinfo != NULL)
            {
                context.notifybalanceorerror(usertid);
                break;
            }
            
            //if bank recovered successfully the amount, a message is sent to the user about the connection between banks.
            sprintf(intertransfertresult, "error on sending transfert to bank id=%d", targetedbanktid);

            }
            else
            {
                 fprintf(op, "if else passed");
            //if the message is successfully sent, get the targeted bank notification 
            pvm_recv(-1, msgbankresult);
            
             fprintf(op, "received message from targeted bank -> info %d\n", info);
            
            pvm_upkstr(intertransfertresult);
            
            fprintf(op, "Successfully transfered -> info %d\n", info);
            
           
            
            }
            
            //send the result ( transfert successfull or error back to the user.
             pvm_initsend(PvmDataDefault);
            pvm_pkstr(intertransfertresult);
            pvm_send(usertid, msgbankresult);
            
            
            fclose(op);
         }
         
         
       //This part is used by a bank for a bank request only.
         //The instanciated bank ( by master ) receive a btobbanksource (bank to bank) source id,
         // a btobaccountid account id, and a btobentry value. A transfert is always an add.
        if(pvm_nrecv(-1, msgbanktobanktransfert) > 0)
        {
            FILE *op;
            op = fopen(LOGPATHBTOBTRANSFERT, "w");

            char* btobresult = new char[255];
            float btobentry;
            int btobaccountid;
            int btobbanksource;
             
            fprintf(op, "bank sending info %d\n", btobbanksource);
             
            fprintf(op, "bank receiving info %d\n", pvm_mytid());
            
            //Unpacking received data from parent bank.
            pvm_upkfloat(&btobentry, 1, 1);
            pvm_upkint(&btobbanksource, 1, 1);
            pvm_upkint(&btobaccountid, 1, 1);
            
            fprintf(op, "info received : entry %d, account id %d, \n", btobentry, btobaccountid);
           
            fprintf(op, "Account instanciation : entry %d, initial amount 100\n", btobaccountid, btobaccountid);
            
            //CONNECTION TO THE TARGETED ACCOUNT.
            //This part is hard coded.
            // Once an user asks for a account on another bank, the bank instanciate the account,
            //perfoms the operation and close the instance.
            //That connection has to be secure and the account don't have to be used (bad part of the project, with hard coded data.)
            //the account is instanciated and contexted.
            accounts[btobaccountid].init(btobaccountid, 100);
            context = accounts[btobaccountid];
            
            fprintf(op, "awaiting reply.. performing transfert, balance at 100\n");
            
            //the bank try to add the amount the to current account.
            context.add(btobentry);
            
            //If the error is null, the account has successfully added the value to it balance.
            // The balance doesn't send the balance back to the bank parent, for privacy purpose, but the balance is changed.
            //The result is computed only in a log file.
            if(context.errorinfo == NULL)
            {
            
            fprintf(op, "transfert performed on account %d, balance at %f\n", context.accountnumber, context.balance);
            
            sprintf(btobresult, "transfert performed successfully on bank id=%d, account=%d, ",  pvm_mytid(), btobaccountid);
            
             fprintf(op, "string %s ready to be sent\n", btobresult);
             

            }
            
            //If the transfert state is an error. send the notified result back to the parent back.
            else
            {
                btobresult = context.errorinfo;
            }
            
            pvm_initsend(PvmDataDefault);
            pvm_pkstr(btobresult);
            pvm_send(btobbanksource, msgbankresult);
            
            
             fprintf(op, "Message sent back to bank source %d\n", btobbanksource);
             
            
            fclose(op);
        }
       
       
     }
    
    //If master propagates the exit status, the bank instance propagates to exit status to account instances.
       for (int i = 0; i < sizeof(accounts); i++) {
        accounts[i].propagateexit();
    }
    
    pvm_exit();
    
    return 0;
}


