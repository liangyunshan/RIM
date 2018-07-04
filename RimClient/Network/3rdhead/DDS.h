/*********************************************************************
构件名称： DDS			
构件版本： 4.10.0
构件描述:  订阅分发中间件
应用范围:  局域网、广域网的数据传输 
构件版权:  版权所有(C)江苏省自动化研究所(JARI,P.R.C.)
文件名称:  DDS.h 
功能描述:  DDS应用接口文件
建立日期:  2010/06/08
修改日期:  2016/01/08
版    权:  版权所有(C)江苏省自动化研究所(JARI,P.R.C.)
程序拟制:  刘庆生
备    注:  系统集成平台下的数据传输服务
----------------------------------------------------------------------
修改记录：
日    期       修改者   描述
-------------  -------  ---------------------------------------------
2010年06月23日 刘庆生   统一不同操作系统下的接口函数
2014年01月04日 刘庆生   增加发布接口，修改其他接口的参数V4.5
2014年07月15日 刘庆生   增加Qos可靠性控制V4.7

*********************************************************************/

#ifndef	_PLATFORM_DDS_H_
#define	_PLATFORM_DDS_H_


#define DTS_SUCCEED					1			//成功
#define DTS_ERROR					0			//失败
#define DTS_ERROR_PARAMETER			-1			//参数错误
#define DTS_ERROR_NODEPROXYERR		-2			//节点代理错误
#define DTS_ERROR_COMINFOERR 		-3			//初始化信息(模块信息)错误
#define DTS_ERROR_CRETEMEMERY		-10			//创建缓存错误
#define DTS_ERROR_LICENSE			-20			//授权错误
#define DTS_ERROR_NETMSGERR			-51			//网络报文错误
#define DTS_ERROR_LEN2SHORT			-50			//缓存太小
#define DTS_ERROR_SEND				-40			//发送报文失败
#define DTS_ERROR_QUIT				-41			//进程退出
#define DTS_ERROR_MSSTATUS			-60			//备机状态

#ifdef WIN32
#ifdef DDS_EXPORTS
#define DDS_API __declspec(dllexport)
#else
#define DDS_API __declspec(dllimport)
#endif
#endif


#ifdef LINUX
#include <QtCore/qglobal.h>
//#define LINUX
#if defined(DDS_LIBRARY)
#  define DDS_API Q_DECL_EXPORT
#else
#  define DDS_API Q_DECL_IMPORT
#endif
#endif


#ifdef SOLARIS
#define DDS_API
#endif

#define QOS_NONE 0
#define QOS_RELIABLE 1			//可靠性QoS 

//////////////////////////////////////////////////////////////////////////
//功    能： 初始化数据传输服务
//参    数： 
//			szComponentName	--模块名称
//			iField			--域
//返 回 值：
//			1  --注册成功
//			<0  --获取失败
extern  "C" DDS_API	int  DDSInit(char*  szComponentName,int iField=0);


//////////////////////////////////////////////////////////////////////////
//功    能： 创建主题信息
//参    数： 
//			szTopic		    --主题（系统内唯一）
//			dwTopicID		--主题对应的构件内部ID
//返 回 值：
//			1  --成功
//			<0  --失败
extern "C" DDS_API int DDSCreateTopic(char *szTopic,unsigned int dwTopicID);

//////////////////////////////////////////////////////////////////////////
//功    能： 订阅信息
//参    数： 
//			iTopic :		主题
//返 回 值： 1  --订阅成功
//		    <=0  --订阅失败
extern "C" DDS_API int  DDSSubscribe(unsigned int dwTopicID,unsigned int QoS,unsigned int QoSValue);

//////////////////////////////////////////////////////////////////////////
//功    能： 发布信息
//参    数： 
//			dwTopicID :		主题
//返 回 值： 1  --成功
//		    <=0 --失败
extern "C" DDS_API int  DDSPublish(unsigned int dwTopicID,unsigned int QoS,unsigned int QoSValue);

//////////////////////////////////////////////////////////////////////////
//功    能： 取消订阅报文 
//参    数： iTopic - 主题
//	        iComponent: 本模块号
//返 回 值： 1     --取消订阅成功
//		    <=0   --取消订阅失败
extern "C" DDS_API int  DDSUnSubscribe(unsigned int dwTopicID,int iComponent=0);

//////////////////////////////////////////////////////////////////////////
//功    能： 发布报文
//参    数： 
//      pBuf				--待发送的报文
//      iLen				--待发送的报文长度
//      iTopic		    	--信息主题
//返 回 值：1     --发送成功
//         <=0   --发送失败
//－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－
extern "C" DDS_API int  DDSSend(char *pBuf, int iLen, unsigned int dwTopicID,unsigned int iTargetNode=0);

//////////////////////////////////////////////////////////////////////////
//功    能： 获取报文
//参    数： 
//      pBuf				--缓存
//		iLen				--输入缓存的长度，输出报文的长度		
//      iTopic				--信息主题
//	    iComponent:			--本模块号
//		iSourceNodeID		--源主机ID
//返 回 值：  1   --成功
//          <=0  --获取失败
extern "C" DDS_API int  DDSFetch(char *pBuf,int &iLen,unsigned int &dwTopicID,int iComponent=0,unsigned int *iSourceNodeID=0);

//////////////////////////////////////////////////////////////////////////
//功    能： 构件退出
//参    数： 
//	    iComponent:			--本模块号
//返 回 值：  1   --成功
//          <=0  --获取失败
extern "C" DDS_API int  DDSQuit();

//////////////////////////////////////////////////////////////////////////
//功    能：  获取节点标识
//参    数：
//
//返 回 值：  >0   --成功
//          =0   --获取失败
extern "C" DDS_API unsigned int  DDSGetNodeID();



#endif