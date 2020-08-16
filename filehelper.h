#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QObject>
#include <QJsonValue>

class FileHelper : public QObject
{
    Q_OBJECT
public:
    explicit FileHelper(QObject *parent = nullptr);

signals:

public slots:
    QJsonValue slotReadIni(const QString file_name);
    QJsonValue slotReadUrl(const QString file_name);
};

#endif // FILEHELPER_H
