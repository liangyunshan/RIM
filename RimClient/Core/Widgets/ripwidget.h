#ifndef RIPWIDGET_H
#define RIPWIDGET_H

#include <QLineEdit>

struct RIPStruct{
    unsigned char ip1;
    unsigned char ip2;
    unsigned char ip3;
    unsigned char ip4;
};

class  RIPWidget : public QLineEdit
{
    Q_OBJECT

public:
    explicit RIPWidget(QWidget *parent = 0);
    ~RIPWidget();
    bool setIP(QString IP);
    bool setIP(const char* IP,int size=4);
    bool setIP(unsigned long IP);
    bool setIP(RIPStruct IP);

    QString getIPString();
    RIPStruct getIPStruct();
    unsigned long getIPULong();

protected:
    void paintEvent(QPaintEvent *event);
    bool eventFilter(QObject *obj, QEvent *ev);

private:
    QList<QLineEdit *> m_lineEidt;
    int getIndex(QLineEdit *pEdit);
};

#endif // RIPWIDGET_H
