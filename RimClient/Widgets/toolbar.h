/*!
 *  @brief     自定义工具栏
 *  @details   1.提供统一最大化、最小化等工具的设置；2.支持添加、插入工具按钮
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.11
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QWidget>

namespace Ui {
class ToolBar;
}

class QHBoxLayout;
class QToolButton;

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
    bool insertToolButton(QToolButton * toolButton);


    /*!
     * @brief 在beforeButton按钮之前插入toolButton，若beforeButton为NULL，则默认插入最后
     *
     * @param[in] toolButton 待插入的工具按钮
     * @param[in] beforeButton 在此按钮之前插入
     *
     * @return 是否插入成功
     */
    bool insertToolButton(QToolButton * toolButton,const char * ID);

    bool containButton(QToolButton * button);

signals:
    void closeWindow();
    void minimumWindow();

private:
    void initDialog();

private:
    QWidget * toolBar;
    QWidget * content;

    QHBoxLayout * toolBarLayout;

    QToolButton * minSize;
    QToolButton * closeButt;
    QToolButton * systemSetting;

    QList<QToolButton *> toolButts;

};

#endif // TOOLBAR_H
