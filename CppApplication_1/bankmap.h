/* 
 * File:   bankmap.h
 * Author: damien
 *
 * bank map representation:
 * a bank map represent a available bank given by the master server.
 * Properties are id of the bank (bid), task id of the bank (tid), and name of the bank (name)
 */

#ifndef BANKMAP_H
#define	BANKMAP_H

class bankmap {
public:
    bankmap();
    bankmap(const bankmap& orig);
    virtual ~bankmap();
    int tid;
    int bid;
    char* name;
};

#endif	/* BANKMAP_H */

