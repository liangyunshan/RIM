/*!
 *  @brief     应用程序
 *  @details   对特定的事件进行捕获
 *  @file      application.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.26
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int &argc, char **argv);

protected:
    bool notify(QObject *obj, QEvent *event);

};

#endif // APPLICATION_H
