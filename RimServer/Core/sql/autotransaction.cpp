#include "autotransaction.h"

AutoTransaction::AutoTransaction(QSqlDatabase &db):
    database(db),rollBack(false)
{
    transacting = database.transaction();
}

AutoTransaction::~AutoTransaction()
{
    if(transacting){
        if(rollBack)
            database.rollback();
        else
            database.commit();
    }
}

/*!
 * @brief 设置提交/回滚
 * @param[in] flag true为回滚事务、false为提交事务
 * @return 是否插入成功
 */
void AutoTransaction::operator()(bool flag)
{
    rollBack = flag;
}
