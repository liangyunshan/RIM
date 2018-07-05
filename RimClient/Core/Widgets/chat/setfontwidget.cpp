#include "setfontwidget.h"

#include <QLabel>
#include <QFont>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFontComboBox>
#include <QColorDialog>
#include <QStackedWidget>

#include <QDebug>

#include "head.h"
#include "../../protocol/datastruct.h"

#include "customfontcombobox.h"

#define CHAT_Font_MAX_HEIGHT 30

class SetFontWidgetPrivate : public GlobalData<SetFontWidget>
{
    Q_DECLARE_PUBLIC(SetFontWidget)

private:
    explicit SetFontWidgetPrivate(SetFontWidget *q):q_ptr(q)
    {
        fontColorDialog = NULL;
        m_curMode = 0;
        m_curType = 0;
        initWidget();
    }

    void initWidget();

    SetFontWidget * q_ptr;

    QWidget * contentWidget;

    QPushButton * bubbleMode;               //气泡模式
    QPushButton * textMode;                 //文本模式

    QStackedWidget * fontModeWidget;        //字体设置框
    CustomFontComboBox * bubbleFont;        //气泡模式字体选择框
    QLabel * sizeLabel;                     //气泡模式字体大小label
    QComboBox * bubbleSize;                 //气泡模式字号选择框
    QPushButton * chooseBubble;             //气泡模式字选择气泡按钮

    QPushButton * systemMode;               //系统字体
    QPushButton * personalMode;             //个性字体
    QFontComboBox * systemFont;             //系统字体选择框
    CustomFontComboBox * personalFont;      //个性字体选择框
    QComboBox * textFontSize;               //文本模式字号选择框
    QPushButton * bold;                     //加粗按钮
    QPushButton * italic;                   //斜体按钮
    QPushButton * underLine;                //下划线按钮
    QPushButton * color;                    //字体颜色按钮

    QColorDialog * fontColorDialog;         //字体颜色设置对话窗

    int m_curMode;                          //当前的显示模式
    int m_curType;                          //当前字体类型
};

void SetFontWidgetPrivate::initWidget()
{
    contentWidget = new QWidget;
    contentWidget->setObjectName("Widget_FontContent");

    fontColorDialog = new QColorDialog(contentWidget);
    fontColorDialog->setObjectName("Font_Color");
    QObject::connect(fontColorDialog,SIGNAL(colorSelected(const QColor&)),q_ptr,SLOT(setTextColorTure(const QColor&)));

    //显示模式（气泡/文本）
    bubbleMode = new QPushButton(contentWidget);
    bubbleMode->setText(QObject::tr("bubble mode"));
    bubbleMode->setObjectName("Font_Bubble");
    bubbleMode->setCheckable(true);
    bubbleMode->setStyleSheet("QPushButton{"
                                  "height:15px;"
                                  "border-top: 1px solid #8f8f91;"
                                  "border-left: 1px solid #8f8f91;"
                                  "border-bottom: 1px solid #8f8f91;"
                                  "border-top-left-radius: 2px;"
                                  "border-bottom-left-radius: 2px;"
                                  "border-top-right-radius: 0px;"
                                  "border-bottom-right-radius: 0px;"
                                  "padding:2px;}"
                              "QPushButton:checked {"
                                  "background-color: rgb(150,160,166);"
                                  "color: rgb(255, 255, 255);}"
                              "QPushButton:!Checked{"
                                  "background-color: rgb(255, 255, 255);"
                                  "color: rgb(0, 0, 0);}"
                              );
    QObject::connect(bubbleMode,SIGNAL(clicked(bool)),q_ptr,SLOT(chooseMode(bool)));

    textMode = new QPushButton(contentWidget);
    textMode->setText(QObject::tr("text mode"));
    textMode->setObjectName("Font_Text");
    textMode->setCheckable(true);
    textMode->setStyleSheet("QPushButton{"
                                "height:15px;"
                                "border-top: 1px solid #8f8f91;"
                                "border-right: 1px solid #8f8f91;"
                                "border-bottom: 1px solid #8f8f91;"
                                "border-top-right-radius: 2px;"
                                "border-bottom-right-radius: 2px;"
                                "border-top-left-radius: 0px;"
                                "border-bottom-left-radius: 0px;"
                                "padding:2px;}"
                            "QPushButton:checked {"
                                "background-color: rgb(150,160,166);"
                                "color: rgb(255, 255, 255);}"
                            "QPushButton:!Checked{"
                                "background-color: rgb(255, 255, 255);"
                                "color: rgb(0, 0, 0);}"
                              );
    QObject::connect(textMode,SIGNAL(clicked(bool)),q_ptr,SLOT(chooseMode(bool)));
    //【1】
    QHBoxLayout * modeLayout = new QHBoxLayout;
    modeLayout->setSpacing(0);
    modeLayout->setContentsMargins(0,0,0,0);
    modeLayout->addWidget(bubbleMode);
    modeLayout->addWidget(textMode);

    //【2】
    fontModeWidget = new QStackedWidget(contentWidget);
    fontModeWidget->setStyleSheet("QPushButton{"
                                      "width:20px;"
                                      "height:20px;"
                                      "background-color: none;}"
                                  "QPushButton:hover {"
                                      "background-color: rgb(224, 229, 232);}"
                                  "QPushButton:checked {"
                                      "background-color: rgb(224, 229, 232);}"
                                  "QPushButton:!Checked{"
                                      "background-color: none;;"
                                      "color: rgb(0, 0, 0);}"
                                  "QComboBox{"
                                       "height: 15px;"
                                       "max-width:60px;"
                                       "font-size:11px;"
                                       "border: 1px solid #8f8f91;"
                                       "border-radius: 2px;}"
                                    "QComboBox::drop-down{"
                                       "border: 0px;"
                                       "border-radius: 2px;}"
                                    "QComboBox::down-arrow{"
                                       "image: url(:/icon/resource/icon/arrow-down.png);}"
                                    "QComboBox::down-arrow:on{"
                                       "image: url(:/icon/resource/icon/arrow-up.png);}"
                                    "QLabel{"
                                       "color:gray;}");
    //【2-1】气泡模式
    QWidget * bubbleWidget = new QWidget(fontModeWidget);
    QHBoxLayout * bubbleLayout = new QHBoxLayout;
    bubbleLayout->setContentsMargins(0,0,0,0);
    bubbleLayout->setSpacing(2);

    bubbleFont = new CustomFontComboBox(bubbleWidget);
    QObject::connect(bubbleFont,SIGNAL(currentFontChanged(QFont)),q_ptr,SLOT(chooseBubbleFont(QFont)));
    sizeLabel = new QLabel(bubbleWidget);
    sizeLabel->setText(QObject::tr("font size"));
    bubbleSize = new QComboBox(bubbleWidget);
    for(int t_bsize=9;t_bsize<15;t_bsize++)
    {
        bubbleSize->addItem(QString::number(t_bsize));
    }
    QObject::connect(bubbleSize,SIGNAL(currentIndexChanged(int)),q_ptr,SLOT(chooseBubbleFontSize(int)));
    chooseBubble = new QPushButton(bubbleWidget);
    chooseBubble->setText(QObject::tr("choose bubble"));
    chooseBubble->setStyleSheet("QPushButton{"
                                    "width:70px;"
                                    "height:15px;"
                                    "border: 1px solid #8f8f91;"
                                    "border-radius: 1px;"
                                    "background-color: rgb(244,244,244);"
                                    "color: rgb(0, 0, 0);"
                                    "padding:2px;}"
                            );
    bubbleLayout->addStretch(1);
    bubbleLayout->addWidget(bubbleFont);
    bubbleLayout->addWidget(sizeLabel);
    bubbleLayout->addWidget(bubbleSize);
    bubbleLayout->addWidget(chooseBubble);

    bubbleWidget->setLayout(bubbleLayout);

    //【2-2】文本模式
    QWidget * textWidget = new QWidget(fontModeWidget);
    QHBoxLayout * textLayout = new QHBoxLayout;
    textLayout->setContentsMargins(0,0,0,0);
    textLayout->setSpacing(2);

    systemMode = new QPushButton(textWidget);
    systemMode->setObjectName("System_Font");
    systemMode->setCheckable(true);
    systemMode->setStyleSheet(bubbleMode->styleSheet());
    systemMode->setStyleSheet("QPushButton:checked{"
                                  "border-top-right-radius: 0px;"
                                  "border-bottom-right-radius: 0px;"
                                  "background-color: rgb(150,160,166);"
                                  "border-image: url(:/icon/resource/icon/Font-system-white.png);}"
                              "QPushButton:!checked{"
                                  "border-top-right-radius: 0px;"
                                  "border-bottom-right-radius: 0px;"
                                  "background-color: rgb(255, 255, 255);"
                                  "border-image: url(:/icon/resource/icon/Font-system-gray.png);}"
                              );
    QObject::connect(systemMode,SIGNAL(clicked(bool)),q_ptr,SLOT(chooseTextFont(bool)));
    personalMode = new QPushButton(textWidget);
    personalMode->setObjectName("Personal_Font");
    personalMode->setCheckable(true);
    personalMode->setStyleSheet(textMode->styleSheet());
    personalMode->setStyleSheet("QPushButton:checked{"
                                    "border-top-left-radius: 0px;"
                                    "border-bottom-left-radius: 0px;"
                                    "background-color: rgb(150,160,166);"
                                    "border-image: url(:/icon/resource/icon/Font-personal.png);}"
                                "QPushButton:!checked{"
                                    "border-top-left-radius: 0px;"
                                    "border-bottom-left-radius: 0px;"
                                    "background-color: rgb(255, 255, 255);"
                                    "border-image: url(:/icon/resource/icon/Font-personal.png);}"
                                );
    QObject::connect(personalMode,SIGNAL(clicked(bool)),q_ptr,SLOT(chooseTextFont(bool)));
    QHBoxLayout *fontModeLayout = new QHBoxLayout;
    fontModeLayout->setContentsMargins(0,0,0,0);
    fontModeLayout->setSpacing(0);
    fontModeLayout->addWidget(systemMode);
    fontModeLayout->addWidget(personalMode);
    systemFont = new QFontComboBox(textWidget);
    personalFont = new CustomFontComboBox(textWidget);
    textFontSize = new QComboBox(textWidget);
    for(int t_tsize=9;t_tsize<23;t_tsize++)
    {
        textFontSize->addItem(QString::number(t_tsize));
    }
    bold = new QPushButton(textWidget);
    bold->setCheckable(true);
    bold->setStyleSheet("border-image: url(:/icon/resource/icon/Font-bold.png);");
    italic = new QPushButton(textWidget);
    italic->setCheckable(true);
    italic->setStyleSheet("border-image: url(:/icon/resource/icon/Font-italic.png);");
    underLine = new QPushButton(textWidget);
    underLine->setCheckable(true);
    underLine->setStyleSheet("border-image: url(:/icon/resource/icon/Font-underline.png); ");
    color = new QPushButton(textWidget);
    color->setCheckable(true);
    color->setStyleSheet("border-image: url(:/icon/resource/icon/Font-color.png);");
    QObject::connect(color,SIGNAL(clicked(bool)),q_ptr,SLOT(chooseTextColor(bool)));
    textLayout->addStretch(1);
    textLayout->addLayout(fontModeLayout);
    textLayout->addWidget(systemFont);
    textLayout->addWidget(personalFont);
    textLayout->addWidget(textFontSize);
    textLayout->addWidget(bold);
    textLayout->addWidget(italic);
    textLayout->addWidget(underLine);
    textLayout->addWidget(color);
    textWidget->setLayout(textLayout);
    fontModeWidget->addWidget(bubbleWidget);
    fontModeWidget->addWidget(textWidget);

    QHBoxLayout * contentLayout = new QHBoxLayout;
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->addLayout(modeLayout);
    contentLayout->addStretch(1);
    contentLayout->addWidget(fontModeWidget);

    contentWidget->setLayout(contentLayout);

    QHBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(contentWidget);
    q_ptr->setLayout(mainLayout);
}

SetFontWidget::SetFontWidget(QWidget *parent) : QWidget(parent),
    d_ptr(new SetFontWidgetPrivate(this))
{
    setFixedHeight(CHAT_Font_MAX_HEIGHT);
    setDefault();
}

SetFontWidget::~SetFontWidget()
{

}

/*!
 * @brief SetFontWidget::currentFontMode 获取当前显示模式（气泡/文本）
 * @return 当前显示模式（气泡/文本）
 */
int SetFontWidget::currentFontMode() const
{
    MQ_D(SetFontWidget);

    return d->m_curMode;
}

/*!
 * @brief SetFontWidget::setDefault 加载默认字体设置
 */
void SetFontWidget::setDefault()
{
    MQ_D(SetFontWidget);

    d->bubbleMode->setChecked(true);
    d->textMode->setChecked(false);
    d->fontModeWidget->setCurrentIndex(0);
    d->systemMode->setChecked(true);
    d->personalMode->setChecked(false);
    d->systemFont->setVisible(true);
    d->personalFont->setVisible(false);
    QFont defaultFont = font();
    defaultFont.setPointSize(9);
    emit fontChanged(defaultFont);

}

/*!
 * @brief SetFontWidget::setCurFontMode 根据设置模式切换窗口并保存设置结果
 */
void SetFontWidget::setCurFontMode(FontMode mode)
{
    MQ_D(SetFontWidget);

    switch (mode) {
    case BUBBLEMODE:

        break;
    case TEXTMODE:

        break;
    default:
        break;
    }
    d->fontModeWidget->setCurrentIndex(mode);
    d->m_curMode = mode;
    emit fontModeChanged(mode);
}

/*!
 * @brief SetFontWidget::chooseMode 选择显示模式
 */
void SetFontWidget::chooseMode(bool)
{
    MQ_D(SetFontWidget);

    FontMode t_mode = BUBBLEMODE;
    QString t_modeName = QObject::sender()->objectName();
    if(t_modeName == d->bubbleMode->objectName())
    {
        if(d->m_curMode == BUBBLEMODE)
        {
            d->bubbleMode->setChecked(true);
        }
        else
        {
            //气泡模式显示
            d->textMode->setChecked(false);
            t_mode = BUBBLEMODE;
        }
    }
    else
    {
        if(d->m_curMode == TEXTMODE)
        {
            d->textMode->setChecked(true);
        }
        else
        {
            //文本模式显示
            d->bubbleMode->setChecked(false);
            t_mode = TEXTMODE;
        }
    }
    setCurFontMode(t_mode);
}

/*!
 * @brief SetFontWidget::chooseTextFont 文本模式下选择系统/个性化字体
 */
void SetFontWidget::chooseTextFont(bool)
{
    MQ_D(SetFontWidget);

    QString m_typeName = QObject::sender()->objectName();
    if(m_typeName == d->systemMode->objectName())
    {
        d->systemFont->setVisible(true);
        d->personalFont->setVisible(false);
        if(d->m_curType == SYSTEMFONT)
        {
            d->systemMode->setChecked(true);
            return;
        }
        else
        {
            d->m_curType = SYSTEMFONT;
            d->personalMode->setChecked(false);
        }
    }
    else
    {
        d->systemFont->setVisible(false);
        d->personalFont->setVisible(true);
        if(d->m_curType == PERSONALFONT)
        {
            d->personalMode->setChecked(true);
            return;
        }
        else
        {
            d->m_curType = PERSONALFONT;
            d->systemMode->setChecked(false);
        }
    }
}

/*!
 * @brief SetFontWidget::chooseTextColor 文本模式下选择字体颜色
 */
void SetFontWidget::chooseTextColor(bool)
{
    MQ_D(SetFontWidget);

    d->fontColorDialog->show();
}

/*!
 * @brief SetFontWidget::setTextColorTure 设置文本模式下字体颜色
 * @param result 颜色设置结果
 */
void SetFontWidget::setTextColorTure(const QColor &result)
{
    if(result.isValid())
    {
        qDebug()<<result.red()<<result.green()<<result.blue();
    }
}

/*!
 * @brief SetFontWidget::chooseBubbleFont 设置气泡字体类型、字体大小
 * @param index
 */
void SetFontWidget::chooseBubbleFont(const QFont &font)
{
    MQ_D(SetFontWidget);

    switch (d->m_curMode) {
    case BUBBLEMODE:
    {
        int curbubbleFontSize = d->bubbleSize->currentText().toInt();
        QFont bubbleFont = font;
        bubbleFont.setPointSize(curbubbleFontSize);
        emit fontChanged(bubbleFont);
    }
        break;
    case TEXTMODE:

        break;
    default:
        break;
    }
}

/*!
 * @brief SetFontWidget::chooseBubbleFontSize 切换字体大小
 */
void SetFontWidget::chooseBubbleFontSize(int)
{
    MQ_D(SetFontWidget);

    switch (d->m_curMode) {
    case BUBBLEMODE:
    {
        int curbubbleFontSize = d->bubbleSize->currentText().toInt();
        QFont bubbleFont = d->bubbleFont->currentFont();
        bubbleFont.setPointSize(curbubbleFontSize);
        emit fontChanged(bubbleFont);
    }
        break;
    case TEXTMODE:

        break;
    default:
        break;
    }
}
