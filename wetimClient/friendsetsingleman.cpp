#include "friendsetsingleman.h"

FriendSetSingleMan::FriendSetSingleMan()
{
    frdsMap.clear();
}

FriendSetSingleMan::~FriendSetSingleMan()
{

}

bool FriendSetSingleMan::instOneFriend2Map(int uid, const char *name, int avicon)
{
    FrdDetaiInfo_t frd;
    std::pair<FriendsMap::iterator, bool> ret;

    memset(&frd, 0, sizeof (frd));
    frd.baseinfo.uid = uid;
    strncpy(frd.baseinfo.name, name, sizeof(frd.baseinfo.name));
    frd.baseinfo.avicon = avicon;

    frd.state.chatBorwerWindowNum = -1;

    ret = frdsMap.insert(std::pair<int, FrdDetaiInfo_t>(uid, frd));
    return ret.second;
}

void FriendSetSingleMan::removeOneFriend(int uid)
{
    eraseOneFriendFromMap(uid);
}

bool FriendSetSingleMan::setChatBrowserWindwNum(int uid, int winNum)
{
    FrdDetaiInfo_t *f = findFriendDetaiInfoByUid(uid);

    if (f){
        f->state.chatBorwerWindowNum = winNum;
        return 1;
    }
    return 1;
}

int FriendSetSingleMan::findChatBrowserWindwNumByUid(int uid)
{
    FrdDetaiInfo_t *f = findFriendDetaiInfoByUid(uid);

    if (f){
        return f->state.chatBorwerWindowNum;
    }
    return -1;
}

void FriendSetSingleMan::eraseOneFriendFromMap(int uid)
{
    FriendsMap::iterator it = frdsMap.find(uid);
    FrdDetaiInfo_t *f;

    if (it != frdsMap.end()){
        f = &it->second;
        //if (f->state.chatBorwerWidgetPtr){
        //    delete f->state.chatBorwerWidgetPtr;
        //}
        frdsMap.erase(it);
    }
}
