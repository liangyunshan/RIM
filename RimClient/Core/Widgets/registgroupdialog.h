/*!
 *  @brief     群注册页面
 *  @details   提供群的信息注册
 *  @author    wey
 *  @version   1.0
 *  @date      2018.04.25
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef REGISTGROUPDIALOG_H_04_25
#define REGISTGROUPDIALOG_H_04_25

#include <QSharedPointer>

#include "widget.h"

class RegistGroupDialogPrivate;

class RegistGroupDialog : public Widget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RegistGroupDialog)
public:
    explicit RegistGroupDialog(QWidget * parent = nullptr);

private slots:
    void prepareRegist();
    void validateInput(QString);
    void respValidateChanged(bool flag);
    void recvRegistResult(bool flag);

private:
    enum SearchVisible{
        S_VISIBLE,
        S_INVISIBLE
    };

    enum ValidateAble{
        V_VALIDATE,
        V_NOVALIDATE
    };

    void clearInput();

private:
    RegistGroupDialogPrivate * d_ptr;
};

#endif // REGISTGROUPDIALOG_H
