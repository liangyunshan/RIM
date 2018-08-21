/*!
 *  @brief     基础文本编辑
 *  @details
 *  @file      basetextedit.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.22
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef BASETEXTEDIT_H
#define BASETEXTEDIT_H

#include <QTextEdit>
#include "../../protocol/datastruct.h"

class QTextStream;
class QXmlStreamReader;
class BaseTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    BaseTextEdit(QWidget * parent = 0 );

    QString insertCopyImage(QImage &image);
    void setInputTextColor(QColor Color);
    void getInputedImgs(QStringList &imgDirs);
    void clearInputImg();
    bool isVaiablePlaintext();
    int imageCount();

protected:
    bool eventFilter(QObject *, QEvent *);
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
    void contextMenuEvent(QContextMenuEvent *e);

Q_SIGNALS:
    void sigDropFile(QString);

private slots:
    void copySelectData(bool);
    void pasteData(bool);
    void selectAllData(bool);

private:
    QStringList m_inputImgs;
    QTextEdit m_tempEdit;
    QAction *m_pCopyAction;
    QAction *m_pPasteAction;
    QAction *m_pSelectAllAction;
};

#endif // BASETEXTEDIT_H
