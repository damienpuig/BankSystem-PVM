/* 
 * File:   command.h
 * Author: damien
 *command.h is used by each part of the project: account program, bank program, master program and client program.
 * It shares command between each program, speficic to the right process.
 * the message tag define where the message is going to be processed on a given task id.
 * 
 * Example:
 * The user asks for initialisation, sending a request with the message tag msgmasterlistbanks.
 * 
 * The user asks for a deposit account request. the request is sent to the right bank (task id) using msgbankdeposit message tag
 * 
 * the account process processed a bank request, then the bank wait on the returns msgaccountresult message tag. Once the account sent back on the 
 * msgaccountresult, the bank can send on the msgbankresult the informations back to the user, waiting a response on msgbankresult.
 * 
 * the master process sends an exit status to the bank using msgaccountexit.
 * Once the request is reached by the pvm_nrecv bank, it stop the while loop and send an exit status to the accounts using the message tag  
 * msgaccountexit. The account process uses the same implementation.
 */

#ifndef COMMAND_H
#define	COMMAND_H

//Enumeration of action available on bank process.
enum msgtagbank {
        msgbankexit = 0,
        msgbankbalance = 100,
        msgbankdeposit = 200,
        msgbankwithdraw = 300,
        msgbanktransfert = 400,
        msgbankresult = 500,
        msgbankswitchcontextaccount = 700,
        
        msgbankintertransfert = 800,
        msgbanktobanktransfert = 1000,
        
         msgbankinituseraccounts = 900,
         msgbankactiveuser=1000
    };
    
//Enumeration of action available on master process. 
enum msgtagmaster {
        
        msgmasterexit = 0,
        msgmasterlistbanks = 200,
        msgmasterresult = 500
    };

//Enumeration of action available on account process.
enum msgtagaccount {
        msgaccountexit = 0,
        msgaccountadd = 100,
        msgaccountremove = 200,
        msgaccountresult = 500
    };

#endif	/* COMMAND_H */

