#ifndef RAES_H
#define RAES_H

#include <QObject>
class AES;

class RAES
{
public:
    RAES();
    ~RAES();

    QByteArray aesData(QByteArray & data);

    QByteArray unAesData(QByteArray & data);

private:
    AES *m_aes;
};

#endif // RAES_H
