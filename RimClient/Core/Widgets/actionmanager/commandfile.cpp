#include "commandfile.h"

#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QKeySequence>
#include <QXmlStreamReader>
#include <QApplication>
#include <QDateTime>

#include "shortcutsettings.h"
#include "Util/fileutils.h"
#include "head.h"

//XML文件中各个字含义
struct Context
{
    Context();
    const QString mappingElement;
    const QString shortCutElement;
    const QString idAttribute;
    const QString keyElement;
    const QString valueAttribute;
};

Context::Context() :
    mappingElement(QLatin1String("mapping")),
    shortCutElement(QLatin1String("shortcut")),
    idAttribute(QLatin1String("id")),
    keyElement(QLatin1String("key")),
    valueAttribute(QLatin1String("value"))
{
}

CommandFile::CommandFile(const QString &filename):
    m_filename(filename)
{

}

bool CommandFile::importCommands(QMap<QString, QKeySequence> & result) const
{
    QFile file(m_filename);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return false;
    }

    QXmlStreamReader r(&file);

    QString currentId;

    const Context ctx;

    while (!r.atEnd())
    {
        switch (r.readNext())
        {
            case QXmlStreamReader::StartElement:
            {
                const QStringRef name = r.name();
                if (name == ctx.shortCutElement)
                {
                    currentId = r.attributes().value(ctx.idAttribute).toString();
                }
                else if (name == ctx.keyElement)
                {
                    const QXmlStreamAttributes attributes = r.attributes();
                    if(attributes.hasAttribute(ctx.valueAttribute))
                    {
                        const QString keyString = attributes.value(ctx.valueAttribute).toString();
                        result.insert(currentId, QKeySequence(keyString));
                    }
                    else
                    {
                        result.insert(currentId, QKeySequence());
                    }
                    currentId.clear();
                 }
            } // case QXmlStreamReader::StartElement
            default:
                break;
        }
    }
    file.close();
    return true;
}

bool CommandFile::exportCommands(const QList<ShortcutItem *> &items)
{
    FileUtils::FileSaver saveFile(m_filename,QIODevice::Text);
    if(!saveFile.hasError())
    {
        const Context ctx;
        QXmlStreamWriter writer(saveFile.file());
        writer.setAutoFormatting(true);
        writer.setAutoFormattingIndent(1);

        writer.writeStartDocument();
        writer.writeDTD(QLatin1String("<!DOCTYPE KeyboardMappingScheme>"));
        writer.writeComment(QString::fromLatin1(" Written by %1 , %2. ").
                       arg(qApp->applicationName(),
                           QDateTime::currentDateTime().toString(Qt::ISODate)));

        writer.writeStartElement(ctx.mappingElement);
        foreach (const ShortcutItem *item, items)
        {
            Id id = item->m_id;
            if (item->m_key.isEmpty())
            {
                writer.writeEmptyElement(ctx.shortCutElement);
                writer.writeAttribute(ctx.idAttribute, id.toString());
            }
            else
            {
                writer.writeStartElement(ctx.shortCutElement);
                writer.writeAttribute(ctx.idAttribute, id.toString());
                writer.writeEmptyElement(ctx.keyElement);
                writer.writeAttribute(ctx.valueAttribute, item->m_key.toString());
                writer.writeEndElement(); // Shortcut
            }
        }
        writer.writeEndElement();
        writer.writeEndDocument();
    }

    return saveFile.finalize();
}

QString CommandFile::fileName() const
{
    QFileInfo fileInfo(m_filename);
    return fileInfo.absoluteFilePath();
}
