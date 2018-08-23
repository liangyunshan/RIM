#ifndef FORMAT495FUNCTION_H
#define FORMAT495FUNCTION_H

#include <QByteArray>

class Format495Function
{
public:
    Format495Function();
    static void sendRegistPack();
    static void sendUnRegistPack();
    static void sendTestSelfOnlinePack();
    static void sendTestPack(QString destId);
    static void sendTestResultPack(QString destId);

    static void addRegistNode(QByteArray & data, unsigned short nodeNums...);
    static bool checkFileCanbeSend(QString filePath);
    static bool checkFileCanbeSend(int fileSize);
};

#endif // FORMAT495FUNCTION_H
