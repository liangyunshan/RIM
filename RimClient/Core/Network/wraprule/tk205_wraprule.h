/*!
 *  @brief     信息封装规则
 *  @details   抽象了不同协议规则，支持不同规则之间的嵌套，最终返回封装后的结果数据
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.06
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef TK205_WRAPRULE_H_2018_06_07
#define TK205_WRAPRULE_H_2018_06_07

#ifdef __LOCAL_CONTACT__

#include "network/wraprule/wraprule.h"
using namespace ClientNetwork;

class TK205_WrapRule : public WrapRule
{
public:
    TK205_WrapRule();

    QByteArray wrap(const QByteArray &data);
    QByteArray unwrap(const QByteArray &data);


};

#endif //__LOCAL_CONTACT__


#endif // WRAPRULE_H
