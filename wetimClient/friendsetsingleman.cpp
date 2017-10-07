#include <string.h>
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

    ret = frdsMap.insert(std::pair<int, FrdDetaiInfo_t>(uid, frd));
    return ret.second;
}

void FriendSetSingleMan::removeOneFriend(int uid)
{
    eraseOneFriendFromMap(uid);
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
