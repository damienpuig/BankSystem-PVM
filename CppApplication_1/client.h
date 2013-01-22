/* 
 * File:   client.h
 * Author: damien
 *
 * Client class representation.
 * The client class representation is used in order to manage the client context.
 * Each client choice are represented by a method.
 * There is no method asking for an entry as parameter, because the method perform scans itself.
 * 
 * The client object uses a bankmap object if the user has the change from one bank into another one.
 * A bankmap object contains the task id of a bank, and the name.
 * 
 * Changing from one bank into another isn't implemented. But if the client needs to change, 
 * then the implementation would be the following:
 * The client asks from another bank, then a method dedicated to it propagate an exit on the current master implementation.
 * Therefore, the client can initiate a new connection to master and, asks for the right bank.
 */

#ifndef CLIENT_H
#define	CLIENT_H
#include "bankmap.h"

class client {
public:
    //User id in the bank.
    int userid;
    //account id context.
    int selectedaccountid;
    //bank id context
    int selectedbankid;
    client();
    client(const client& orig);
    virtual ~client();
    //Spawn the master server in order to initiate bank availability.
    void init();
    //Get the balance of the user context.
    void getbalance();
    //Perform a deposit on the user context.
    void performdeposit();
    //Perform a withdraw on the user context.
    void performwithdraw();
    //take bank user choice.
    void bankchoices();
    //initiate bank user choice.
    void initbankchoices();
    //Context switching in the bank (from an account into another account).
    void switchaccount();
    //Init user account states.
    void initaccountsstates();
    //Perform a transfert on a given account
    void performtransfert();
    //Perform a transfert on a given bank and a given account.
    void performintertransfert();
    //Progagate exit from the master to all the implementation
    void propagateexit();
    //Menu asking for actions.
    void menu();

private:
    //array of available banks.
    bankmap banks[3];
    
    //client task id
    int mytid;
    
    //current master task id
    int mastertid;
    //current bank task id
    int banktid;
    
    // information about master spawn
    int info;
    
    //Last result notified from any sources.
    char formatedresult[255];
    
    //Shortcut on printing. ( tacking in parameter the type of the request )
    void printstate(char*);
};

#endif	/* CLIENT_H */

