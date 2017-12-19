/*!
 *  @brief     自定义工具栏
 *  @details   1.提供统一最大化、最小化等工具的设置；2.支持添加、插入工具按钮
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.11
 *  @warning
 *  @copyright GNU Public License.
 *  @note      工具栏的组成:[T][T]XXXXX[T][T][T]    [T]表示工具按钮；XXX表示弹簧
 */
#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QWidget>

namespace Ui {
class ToolBar;
}

class QHBoxLayout;
class RToolButton;

class ToolBar : public QWidget
{
    Q_OBJECT

public:
    explicit ToolBar(QWidget *parent = 0);
    ~ToolBar();

    /*!
     * @brief 向窗口的工具栏中插入工具按钮，默认是自左向右排列
     *
     * @param[in] toolButton 待插入的工具按钮
     *
     * @return 是否插入成功
     *
     */
    bool appendToolButton(RToolButton * toolButton);


    /*!
     * @brief 插入工具按钮
     *
     * @param[in] toolButton 待插入的工具按钮
     * @param[in] ID 若为NULL，则从工具栏的最前面插入；不为NULL，则从指定的控件前面插入
     *
     * @return 是否插入成功
     */
    bool insertToolButton(RToolButton * toolButton,const char * ID = NULL);

    bool containButton(RToolButton * button);

signals:
    void closeWindow();
    void minimumWindow();

private:
    void initDialog();

private:
    QWidget * toolBar;
    QWidget * content;

    QHBoxLayout * toolBarLayout;

    QList<RToolButton *> toolButts;
};

#endif // TOOLBAR_H
