/*!
 *  @brief     个人信息编辑页面
 *  @details   编辑个人基础信息，保存后可同步至服务器
 *  @file      editpersoninfowindow.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.27
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note      20171228:wey:添加本地图片选择框，可选择系统自带的图像;
 *             20180206:wey:增加对输入的信息验证;
 *             20180323:wey:增加本地头像上传功能;
 */
#ifndef EDITPERSONINFOWINDOW_H
#define EDITPERSONINFOWINDOW_H

#include "widget.h"
#include "observer.h"
#include "systemuserimagewindow.h"
#include "protocoldata.h"
using namespace ProtocolType;

class EditPersonInfoWindowPrivate;

class EditPersonInfoWindow : public Widget,public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(EditPersonInfoWindow)
public:
    EditPersonInfoWindow(QWidget * parent = 0);
    ~EditPersonInfoWindow();

    void onMessage(MessageType type);

private slots:
    void openSystemImage();
    void openLocalImage();
    void updateUserBaseInfo();
    void updateSystemIconInfo(QString filename);
    void recvUpdateBaseInfoResponse(ResponseUpdateUser status, UpdateBaseInfoResponse response);
    void singleBaseInfoFinished();

private:
    void loadCustomUserImage();

private:
    EditPersonInfoWindowPrivate * d_ptr;
    QSharedPointer<Observer> share_pointer;
};

#endif // EDITPERSONINFOWINDOW_H
