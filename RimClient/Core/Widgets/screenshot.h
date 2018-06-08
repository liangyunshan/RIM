/*!
 *  @brief     截屏功能
 *  @details   截屏功能
 *  @author    尚超
 *  @version   1.0
 *  @date      2018.01.28
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QObject>
#include <QProcess>
class QTimer;
class QAction;

class ScreenShot : public QObject
{
    Q_OBJECT
public:
    explicit ScreenShot(QObject *parent = nullptr);
    static ScreenShot *instance();
    int setScreenShotKey(QString);
    int setScreenShotKey(QKeySequence);
    QAction *instanceScreenShotAction();
    QAction *instanceHideWindowAction();

signals:
    void sig_ShotReady(bool);

public slots:

private slots:
    void slot_ScreenTimeout();
    void slot_ScreenShot(bool flag);
    void slot_ScreenShot_Ready(int, QProcess::ExitStatus);
    void slot_ScreenShotHide(bool flag);

private:
    void initKey();

private:
    QProcess *p_shotProcess;
    bool b_isScreeHide;
    QTimer *p_shotTimer;
};

#endif // SCREENSHOT_H
