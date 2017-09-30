#ifndef USRONLINESTATE_H
#define USRONLINESTATE_H
#include <map>
#include <netinet/in.h>
#include "poll/llist.h"

class UsrOnlineState
{
public:
    #pragma pack(1)
    typedef struct {
        ll_list_t list;
        int uid;
        //struct sockaddr_in inAddr;
        unsigned int srcaddr;//网络字节序
        unsigned short srcport;//网络字节序
    }UsrOnState_t;
    #pragma pack()

    typedef std::map<int, UsrOnState_t> UsrOnLineMap;

    UsrOnlineState();

    bool addUser (UsrOnState_t &u);
    bool setUserRemAddr (int uid, unsigned ip, unsigned short port);
    void removeUser (int uid);
    const UsrOnState_t* getUserOnstateByUid (int uid)
    {
        UsrOnState_t *u = 0;
        UsrOnLineMap::iterator it = usrMap.find(uid);

        if (it != usrMap.end()){
            u = &it->second;
        }
        return u;
    }

    int getOnlineUsrInfo (UsrOnState_t *u, int maxn, ll_list_t **lastEnd);
private:
    static UsrOnLineMap usrMap;
    static ll_list_t usrList;
};

#endif // USRONLINESTATE_H
