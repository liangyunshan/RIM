#include "ripwidget.h"
#include <QKeyEvent>
#include <QPainter>
#include <QHBoxLayout>
#include <QRegExpValidator>
#include <QDebug>

const int pHBoxSpacing = 10;
const int pHBoxMargins = 2;

RIPWidget::RIPWidget(QWidget *parent) :
    QLineEdit(parent)
{
    this->setFrame(true);
    this->setMinimumWidth(160);
    QRegExp rx("(2[0-5]{2}|2[0-4][0-9]|1?[0-9]{1,2})");
    QHBoxLayout *pHBox = new QHBoxLayout(this);
    pHBox->setSpacing(pHBoxSpacing);
    pHBox->setContentsMargins(pHBoxMargins, pHBoxMargins, pHBoxMargins, pHBoxMargins);

    QLineEdit *leIP = NULL;
    for (int i = 0; i < 4; i++)
    {
        leIP = new QLineEdit(this);
        leIP->setFrame(false);
        leIP->setMaxLength(3);
        leIP->setAlignment(Qt::AlignCenter);
        leIP->installEventFilter(this);
        leIP->setValidator(new QRegExpValidator(rx, this));
        leIP->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

        pHBox->addWidget(leIP);
        m_lineEidt.append(leIP);
    }
    this->setFocusProxy(m_lineEidt.first());
    m_lineEidt.first()->setFocus();
    this->setReadOnly(true);
}

RIPWidget::~RIPWidget()
{

}

/*!
 * @brief 设置界面需要显示的IP地址
 * @param IP 需要设置的IP地址
 * @return 返回设置结果，设置成功返回true，否则为false
 */
bool RIPWidget::setIP(QString IP)
{
    QStringList iplist = IP.split(".");
    if(iplist.count() != 4)
    {
        return false;
    }
    int ipUnit = 0;
    for (int i=0; i<4;i++)
    {
        QString str = iplist.at(i);
        ipUnit = str.toInt();
        if(str>=0 && ipUnit<=255)
        {
            m_lineEidt.at(i)->setText(str);
        }
        else
        {
            return false;
        }
    }

    return true;
}

/*!
 * @brief 设置界面需要显示的IP地址
 * @param IP IP地址
 * @param size IP地址字符串的长度
 * @return 返回设置结果，设置成功返回true，否则为false
 */
bool RIPWidget::setIP(const char *IP, int size)
{
    QByteArray addr(IP,size);
    return setIP(QString::fromLocal8Bit(addr));
}

/*!
 * @brief 设置界面需要显示的IP地址
 * @param IP IP地址
 * @return 返回设置结果，设置成功返回true，否则为false
 */
bool RIPWidget::setIP(unsigned long IP)
{
    QString str_IP;
    str_IP = QString("%1.%2.%3.%4")
            .arg((IP&0xFF000000)>>(8*3))
            .arg((IP&0x00FF0000)>>(8*2))
            .arg((IP&0x0000FF00)>>(8*1))
            .arg((IP&0x000000FF)>>(8*0));
    return setIP(str_IP);
}

/*!
 * @brief 设置界面需要显示的IP地址
 * @param IP IP地址
 * @return 返回设置结果，设置成功返回true，否则为false
 */
bool RIPWidget::setIP(RIPStruct IP)
{
    QString str_IP;
    str_IP = QString("%1.%2.%3.%4")
            .arg(IP.ip1)
            .arg(IP.ip2)
            .arg(IP.ip3)
            .arg(IP.ip4);
    return setIP(str_IP);
}

/*!
 * @brief 获取界面中输入的IP地址字符串
 * @return  界面中输入的IP地址字符串
 */
QString RIPWidget::getIPString()
{
    QString IP;
    IP = QString("%1.%2.%3.%4")
            .arg(m_lineEidt.at(0)->text())
            .arg(m_lineEidt.at(1)->text())
            .arg(m_lineEidt.at(2)->text())
            .arg(m_lineEidt.at(3)->text());
    return IP;
}

/*!
 * @brief 获取界面中输入的IP地址结构体
 * @return IP地址结构体
 */
RIPStruct RIPWidget::getIPStruct()
{
    RIPStruct IP;
    IP.ip1 = (unsigned char)m_lineEidt.at(0)->text().toUShort();
    IP.ip2 = (unsigned char)m_lineEidt.at(1)->text().toUShort();
    IP.ip3 = (unsigned char)m_lineEidt.at(2)->text().toUShort();
    IP.ip4 = (unsigned char)m_lineEidt.at(3)->text().toUShort();
    return IP;
}

/*!
 * @brief 获取界面中输入的IP地址整型数值
 * @return IP地址整型数值
 */
unsigned long RIPWidget::getIPULong()
{
    RIPStruct IP = getIPStruct();
    unsigned long ul_IP = 0x00000000;
    ul_IP += IP.ip1<<(8*3);
    ul_IP += IP.ip2<<(8*2);
    ul_IP += IP.ip3<<(8*1);
    ul_IP += IP.ip4<<(8*0);
    return ul_IP;
}

/*!
 * @brief 控件绘制事件，绘制IP之间的圆点
 * @param event 界面绘制事件
 */
void RIPWidget::paintEvent(QPaintEvent *event)
{
    QLineEdit::paintEvent(event);

    QPainter painter(this);
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(3);
    pen.setBrush(Qt::black);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);

    int width = 0;
    for (int i = 0; i < 3; i++)
    {
        width += m_lineEidt[i]->width() + (i == 0 ? 3 : pHBoxSpacing);//布局的间隔
        painter.drawPoint(width, height() - 8);
    }
    return ;
}

/*!
 * @brief 处理界面的数字输入和左右键切换
 * @param obj 事件发出对象
 * @param ev  事件类型
 * @return 事件处理结果
 */
bool RIPWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if (children().contains(obj) && QEvent::KeyPress == ev->type())
    {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(ev);
        QLineEdit *pEdit = qobject_cast<QLineEdit *>(obj);
        switch (keyEvent->key())
        {
        case Qt::Key_0:
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        case Qt::Key_6:
        case Qt::Key_7:
        case Qt::Key_8:
        case Qt::Key_9:
        {
            QString strText = pEdit->text();
            if (pEdit->selectedText().length())
            {
                pEdit->text().replace(pEdit->selectedText(), QChar(keyEvent->key()));
            }
            else if (strText.length() == 3 || strText.length() < 3 && strText.toInt() * 10 > 255)
            {
                int index = getIndex(pEdit);
                if (index != -1 && index != 3)
                {
                    m_lineEidt[index + 1]->setFocus();
                    m_lineEidt[index + 1]->selectAll();
                }
            }
            else if (strText.length() == 2 && strText.toInt() * 10 < 255)
            {
                if (Qt::Key_0 == keyEvent->key() && strText.toInt())
                {
                    pEdit->setText(strText.insert(pEdit->cursorPosition(), QChar(Qt::Key_0)));
                }
            }
            else if(strText.toInt() == 0)
            {
                pEdit->clear();
            }
            return QLineEdit::eventFilter(obj, ev);
        }
        break;
        case Qt::Key_Backspace:
        {
            QString strText = pEdit->text();
            if (!strText.length() || strText.length() && !pEdit->cursorPosition())
            {
                int index = getIndex(pEdit);
                if (index != -1 && index != 0)
                {
                    m_lineEidt[index - 1]->setFocus();
                    int length = m_lineEidt[index - 1]->text().length();
                    m_lineEidt[index - 1]->setCursorPosition(length ? length : 0);
                }
            }
            return QLineEdit::eventFilter(obj, ev);
        }
        case Qt::Key_Left:
        {
            if (!pEdit->cursorPosition())
            {
                int index = getIndex(pEdit);
                if (index != -1 && index != 0)
                {
                    m_lineEidt[index - 1]->setFocus();
                    int length = m_lineEidt[index - 1]->text().length();
                    m_lineEidt[index - 1]->setCursorPosition(length ? length : 0);
                }
            }
            return QLineEdit::eventFilter(obj, ev);
        }
        case Qt::Key_Right:
        case Qt::Key_Period :
        {
            if (pEdit->cursorPosition() == pEdit->text().length())
            {
                int index = getIndex(pEdit);
                if (index != -1 && index != 3)
                {
                    m_lineEidt[index + 1]->setFocus();
                    m_lineEidt[index + 1]->setCursorPosition(0);
                }
            }
            return QLineEdit::eventFilter(obj, ev);
        }
        default:
            break;
        }
    }
    return QLineEdit::eventFilter(obj,ev);
}

int RIPWidget::getIndex(QLineEdit *pEdit)
{
    int index = -1;
    for (int i = 0; i < 4; i++)
    {
        if (pEdit == m_lineEidt[i])
            index = i;
    }
    return index;
}
