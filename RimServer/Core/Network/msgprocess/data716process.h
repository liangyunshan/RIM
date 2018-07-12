/*!
 *  @brief     处理716数据请求
 *  @details
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.28
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef DATA716PROCESS_H
#define DATA716PROCESS_H

#ifdef __LOCAL_CONTACT__
class Database;

#include "Network/head.h"
#include "../../protocol/datastruct.h"

#include <vector>
#include <mutex>
#include <map>

class Data716Process
{
public:
    Data716Process();

    void processText(Database * db,int sockId,ProtocolPackage & data);
    void processUserRegist(Database * db,int sockId,ProtocolPackage & data);

    void processFileData(Database * db,int sockId,ProtocolPackage & data);

private:
    /*!
     *  @brief  建立连接过程状态
     *  @note 未建立-建立过程中-建立成功/失败
     */
    enum ServerConnStats{
        SCS_NONE,       /*!< 未建立 */
        SCS_OK,         /*!< 建立成功 */
        SCS_ERR,        /*!< 建立失败 */
        SCS_IN          /*!< 建立过程中 */
    };

    struct TextServerCacheInfo{
        TextServerCacheInfo():connStats(SCS_NONE){

        }
        ServerConnStats connStats;
        ParameterSettings::NodeServer serverInfo;
        std::vector<ProtocolPackage> msgCache;
    };

    struct FileServerCacheInfo{
        FileServerCacheInfo():connStats(SCS_NONE){

        }
        ServerConnStats connStats;
        ParameterSettings::NodeServer serverInfo;
        std::vector<ParameterSettings::Simple716FileInfo> fileCache;
    };

private:
    void cacheMsgProtocol(ParameterSettings::NodeServer serverInfo, ProtocolPackage & package);
    void cacheFileProtocol(Database * db,ParameterSettings::NodeServer serverInfo, ParameterSettings::Simple716FileInfo & fileIndo);

    void respTextNetConnectResult(QString nodeId, bool connected, int socketId);
    void respFileNetConnectResult(QString nodeId, bool connected, int socketId);

    void saveFile2Database(std::vector<ParameterSettings::Simple716FileInfo> &recvFileCache);

private:
    std::mutex textCacheMutex;
    std::map<QString,TextServerCacheInfo> textServerCache;

    std::mutex fileCacheMutex;
    std::map<QString,FileServerCacheInfo> fileServerCache;
};

#endif

#endif // DATA716PROCESS_H
