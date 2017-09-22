﻿#include "usronlinestate.h"

UsrOnlineState::UsrOnLineMap UsrOnlineState::usrMap;
ll_list_t UsrOnlineState::usrList = LLIST_INIT2(&UsrOnlineState::usrList);

UsrOnlineState::UsrOnlineState()
{

}

bool UsrOnlineState::addUser(UsrOnState_t &u)
{
    std::pair<UsrOnLineMap::iterator, bool> ret = usrMap.insert(std::pair<int, UsrOnState_t>(u.uid, u));

    if (ret.second){
       LLIST_ADD_TAIL(&usrList, &ret.first->second.list);
    }
    return ret.second;
}

void UsrOnlineState::removeUser(int uid)
{
    UsrOnLineMap::iterator it = usrMap.find(uid);

    if (it != usrMap.end()){
        LLIST_DEL(&(it->second).list);
        usrMap.erase(it);
    }
}

int UsrOnlineState::getOnlineUsrInfo(UsrOnState_t *u, int maxn, int begin)
{
    ll_list_t * l= usrList.next;
    int n = 0;
    UsrOnState_t *p = 0;

    while ((l != &usrList) && (begin > 0)){
        l = l->next;
        --begin;
    }
    while (l != &usrList && maxn > 0){
        p = MEMBER_ENTRY(l, UsrOnState_t, list);
        *u = *p;
        ++n;
        --maxn;
        l = l->next;
    }
    return n;
}