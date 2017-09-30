#include "usronlinestate.h"

UsrOnlineState::UsrOnLineMap UsrOnlineState::usrMap;
ll_list_t UsrOnlineState::usrList = LLIST_INIT2(&UsrOnlineState::usrList);

UsrOnlineState::UsrOnlineState()
{

}

bool UsrOnlineState::addUser(UsrOnState_t &u)
{
    removeUser(u.uid);
    std::pair<UsrOnLineMap::iterator, bool> ret = usrMap.insert(std::pair<int, UsrOnState_t>(u.uid, u));
    if (ret.second){
       LLIST_ADD_TAIL(&usrList, &ret.first->second.list);
    }
    return ret.second;
}

bool UsrOnlineState::setUserRemAddr(int uid, unsigned ip, unsigned short port)
{
    UsrOnLineMap::iterator it = usrMap.find(uid);

    if (it != usrMap.end()){
        UsrOnState_t *u = &it->second;

        u->srcaddr = ip;
        u->srcport = port;
        return 1;
    }
    return 0;
}

void UsrOnlineState::removeUser(int uid)
{
    UsrOnLineMap::iterator it = usrMap.find(uid);

    if (it != usrMap.end()){
        LLIST_DEL(&(it->second.list));
        usrMap.erase(it);
    }
}

int UsrOnlineState::getOnlineUsrInfo(UsrOnState_t *u, int maxn, ll_list_t **lastEnd)
{
    ll_list_t *l= 0;
    int n = 0;
    UsrOnState_t *p = 0;

    if (lastEnd && (*lastEnd)){
        l = *lastEnd;
    }else{
        l = usrList.next;
    }
    while (l != &usrList && maxn > 0){
        p = MEMBER_ENTRY(l, UsrOnState_t, list);
        *(u+n) = *p;
        ++n;
        --maxn;
        l = l->next;
    }
    *lastEnd = l;
    return n;
}











