/*!
 *  @brief     用户界面
 *  @details   显示用户的联系人信息
 *  @file      panelpersonpage.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.15
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef PANELPERSONPAGE_H
#define PANELPERSONPAGE_H

#include <QWidget>

class PanelPersonPagePrivate;
class ToolItem;

class PanelPersonPage : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PanelPersonPage)
public:
    explicit PanelPersonPage(QWidget * parent =0);
    ~PanelPersonPage();

private slots:
    void refreshList();
    void addGroup();
    void renameGroup();
    void delGroup();

    void createChatWindow(ToolItem * item);

    void sendInstantMessage();
    void showUserDetail();
    void modifyUserInfo();
    void deleteUser();

private:
    void createAction();

private:
    PanelPersonPagePrivate * d_ptr;
};

#endif // PANELPERSONPAGE_H
