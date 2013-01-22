/* 
 * File:   account.h
 * Author: damien
 *
 * Account class:
 * The class account is the representation of an account on a bank server, object model is used.
 * The bank receives user request and dispatch it the account context. 
 * The account object uses method in order to process request on a account process dedicated to it.
 */

#ifndef ACCOUNT_H
#define	ACCOUNT_H

class account {
public:
    //An account has 3 properties.
    // the balance is contained in the object. it is used by account process
    // and change the value once the process has been successfully done.
    int accountnumber;
    float balance;
    char* errorinfo;
    
    account();
    account(const account& orig);
    //The init method initialise the account object with two parameter:
    // the accountnumber and the initial balance ( both can be set by a database instance).
    void init(int, int);
    virtual ~account();
    //add request allows banks to add a specific amount to the balance.
    float add(float);
    //add request allows banks to remove a specific amount to the balance.
    float remove(float);
    //notifybalanceorerror notify the user back and take the task user id in parameter.
    void notifybalanceorerror(int&);
    //Method used to propagate the exit status to the account.
    void propagateexit();
private:
    //checker is a method used to check if the result processed on account instance is valid.
    bool checker(float);
    
    //the sender is a method used by add and remove operations. once an operation is asked by the
    //bank, the account request is process in the send request, taking the entry of the user, and the message tag
    // assigned by the bank for the right operation process.
    //This method is also checked by the checker (above) and directly affect the balance result for the account process.
    //If the checker is false, the balance isn't affect.
    float send(float, int);
    
    // account task id to process add and remove request
    int tids[10];
    
    //result set and sent back to the user.
    char* formatedresult;
    
};

#endif	/* ACCOUNT_H */

