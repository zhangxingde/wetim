#ifndef USRONLINESTATE_H
#define USRONLINESTATE_H
#include <map>
#include "poll/llist.h"

class UsrOnlineState
{
public:
    #pragma pack(1)
    typedef struct {
        ll_list_t list;
        int uid;
        unsigned int srcaddr;
        unsigned short srcport;
    }UsrOnState_t;
    #pragma pack()

    typedef std::map<int, UsrOnState_t> UsrOnLineMap;

    UsrOnlineState();

    bool addUser (UsrOnState_t &u);
    void removeUser (int uid);

    int getOnlineUsrInfo (UsrOnState_t *u, int maxn, int begin);
private:
    static UsrOnLineMap usrMap;
    static ll_list_t usrList;
};

#endif // USRONLINESTATE_H
