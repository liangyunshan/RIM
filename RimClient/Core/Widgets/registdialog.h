/*!
 *  @brief     账号注册界面
 *  @details   提供账号注册
 *  @file      registdialog.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.16
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef REGISTDIALOG_H
#define REGISTDIALOG_H

#include "widget.h"

#include "protocoldata.h"
using namespace ProtocolType;

class RegistDialogPrivate;

class RegistDialog : public Widget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RegistDialog)
public:
    explicit RegistDialog(QWidget * parent = 0);
    ~RegistDialog();

private slots:
    void respValidInfo(QString info);
    void connectToServer();
    void recvResponse(ResponseRegister status,RegistResponse resp);
    void respConnect(bool flag);

protected:
    void closeEvent(QCloseEvent *event);

private:
    RegistDialogPrivate * d_ptr;
};

#endif // REGISTDIALOG_H
