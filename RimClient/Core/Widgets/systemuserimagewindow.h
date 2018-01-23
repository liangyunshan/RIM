/*!
 *  @brief     系统图片选择框
 *  @details   加载系统默认的图片资源，支持选定查
 *  @file      systemuserimagewindow.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.28
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef SYSTEMUSERIMAGEWINDOW_H
#define SYSTEMUSERIMAGEWINDOW_H

#include "widget.h"
#include "observer.h"

class SystemUserImageWindowPrivate;

class SystemUserImageWindow : public Widget , public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SystemUserImageWindow)
public:
    SystemUserImageWindow(QWidget * parent = 0);
    ~SystemUserImageWindow();

    void onMessage(MessageType type);

signals:
    void selectedFileBaseName(QString filename);

private slots:
    void processImagePresses();

private:
    SystemUserImageWindowPrivate * d_ptr;

};

#endif // SYSTEMUSERIMAGEWINDOW_H
