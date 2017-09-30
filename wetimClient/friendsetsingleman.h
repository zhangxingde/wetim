#ifndef FRIENDSETSINGLEMAN_H
#define FRIENDSETSINGLEMAN_H
#include <map>
#include "include_h/singletontemp.h"

class UsrChatBrowserWidget;

class FriendSetSingleMan : public SingletonTempBase<FriendSetSingleMan>
{
public:
    friend class SingletonTempBase<FriendSetSingleMan>;
    typedef struct{
        int uid;
        char name[32];
        int avicon;
    }FrdBaseInfo_t;

    typedef struct {
        int chatBorwerWindowNum;
    }FrdState_t;

    typedef struct {
        FrdBaseInfo_t baseinfo;
        FrdState_t state;
    }FrdDetaiInfo_t;

    bool instOneFriend2Map (int uid, const char *name, int avicon);
    void removeOneFriend (int uid);
    bool setChatBrowserWindwNum (int uid, int winNum);
    int findChatBrowserWindwNumByUid (int uid);
    const FrdBaseInfo_t* findFriendBaseInfo (int uid)
    {
        FrdDetaiInfo_t *f = findFriendDetaiInfoByUid(uid);
        return f? &f->baseinfo:0;
    }

    void setMineUid (int uid) {mineBaseinfo.uid = uid;}
    int  getMineUid () {return mineBaseinfo.uid;}

private:
    typedef std::map<int, FrdDetaiInfo_t> FriendsMap;

    FriendSetSingleMan();
    ~FriendSetSingleMan();
    FrdBaseInfo_t mineBaseinfo;

    FriendsMap frdsMap;

    FrdDetaiInfo_t* findFriendDetaiInfoByUid (int uid)
    {
        FriendsMap::iterator it = frdsMap.find(uid);

        if (it != frdsMap.end()){
            return &it->second;
        }
        return 0;
    }


    void eraseOneFriendFromMap (int uid);

};

#endif // FRIENDSETSINGLEMAN_H
