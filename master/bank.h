/* 
 * File:   bank.h
 * Author: damien
 *
 * Bank declaration
 */

#ifndef BANK_H
#define	BANK_H

//A bank can be initialised or exited.
class bank {
public:
    int initBootsptapping(char*, int);
    void propagateexit();
    int tid;
    char* bservername;
    bank();
    bank(const bank& orig);
    virtual ~bank();
    

};

#endif	/* BANK_H */

