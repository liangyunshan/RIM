#ifndef QMAINPANEL_H
#define QMAINPANEL_H

#include <QMouseEvent>

#ifdef Q_CC_MSVC
#include "qwinwidget.h"

class QMainPanel : public QWinWidget
{
    Q_OBJECT
public:
  QMainPanel( HWND hWnd );

#if QT_VERSION >= 0x050000
  bool nativeEvent(const QByteArray &, void *msg, long *result);
#else
  bool winEvent(MSG *message, long *result);
#endif
  void mousePressEvent( QMouseEvent *event );

public slots:
    void pushButtonMinimizeClicked();
    void pushButtonMaximizeClicked();
    void pushButtonCloseClicked();

private:
    HWND windowHandle;

};

#endif //Q_CC_MSVC

#endif // QMAINPANEL_H
