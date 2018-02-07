/*!
 *  @brief     自定义下拉框
 *  @details
 *  @file      rcombobox.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.02.06
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef RCOMBOBOX_H
#define RCOMBOBOX_H

#include <QComboBox>

class RComboBox : public QComboBox
{
    Q_OBJECT
public:
    RComboBox(QWidget * parent = 0);
    ~RComboBox();
};

#endif // RCOMBOBOX_H
