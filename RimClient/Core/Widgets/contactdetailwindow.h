/*!
 *  @brief     显示联系人资料页面
 *  @details   将待查看的联系人资料显示在界面中，部分字段允许修改
 *  @file      contactdetailwindow.h
 *  @author    LYS
 *  @version   1.0
 *  @date      2017.02.27
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *      20180302:LYS:修复连续查看联系人资料时显示分组问题
 *
 */
#ifndef CONTACTDETAILWINDOW_H
#define CONTACTDETAILWINDOW_H

#include "widget.h"
#include "observer.h"
#include "protocoldata.h"
using namespace ProtocolType;

class ContactDetailWindowPrivate;

class ContactDetailWindow : public Widget,public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ContactDetailWindow)
public:
    ContactDetailWindow(QWidget * parent = 0);
    ~ContactDetailWindow();

    void onMessage(MessageType type);
    void setContactDetail(const SimpleUserInfo &);
    void setGroups(QStringList groups,int current);

private slots:
    void modifyRemark();
    void modifyGroup(int);
    void showHead();
    void recvContactDetailResponse(ResponseUpdateUser status,UpdateBaseInfoResponse response);
    void remarkEditFinished();

private:
    ContactDetailWindowPrivate * d_ptr;
};

#endif // CONTACTDETAILWINDOW_H
