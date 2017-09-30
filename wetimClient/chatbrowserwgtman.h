#ifndef CHATBROWSERWGTMAN_H
#define CHATBROWSERWGTMAN_H

class QWidget;

class UsrChatBrowserWidget;

class ChatBrowserWgtMan
{
public:
    typedef struct {
        UsrChatBrowserWidget *chatWgtSet;
        int uid;
    }FrdWgtBind_t;

    ChatBrowserWgtMan(QWidget *parent);
    ~ChatBrowserWgtMan();

    int openChatBrowserWgtByUid (int uid); //返回值 >=0 表示成功新开窗口号，否则表示前次窗口已经打开或者已经达到窗口最高数量
    void closeChatBrowserWgtByUid (int uid);
private:
    QWidget *parentWgtPtr;
    FrdWgtBind_t frdChatWgtSet[128];

    void closeAllChatWgt ();
};

#endif // CHATBROWSERWGTMAN_H
