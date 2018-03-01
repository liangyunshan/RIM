/*!
 *  @brief     修改备注名称编辑页面
 *  @details   输入新的备注名，点击确定这应用新备注名到服务器请求中
 *  @file      modifyremarkwindow.h
 *  @author    LYS
 *  @version   1.0
 *  @date      2017.02.26
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *
 */
#ifndef MODIFYREMARKWINDOW_H
#define MODIFYREMARKWINDOW_H

#include "widget.h"
#include "observer.h"

class ModifyRemarkWindowPrivate;

class ModifyRemarkWindow : public Widget,public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ModifyRemarkWindow)
public:
    ModifyRemarkWindow(QWidget * parent = 0);
    ~ModifyRemarkWindow();

    void onMessage(MessageType type);

    void setOldRemarkName(QString remarkName);

signals:
    void setNewRemark(QString remark);

private slots:
    void applyNewRemark();

private:
    ModifyRemarkWindowPrivate * d_ptr;
};

#endif // MODIFYREMARKWINDOW_H
