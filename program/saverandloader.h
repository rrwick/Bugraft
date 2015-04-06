#ifndef SAVERANDLOADER_H
#define SAVERANDLOADER_H

#include <QObject>
#include <QString>

class Landscape;
class History;

class SaverAndLoader : public QObject
{
    Q_OBJECT

public:
    SaverAndLoader(QString fullFileName) :
        m_fullFileName(fullFileName) {}

private:
    QString m_fullFileName;

public slots:
    void saveSimulation();
    void loadSimulation();

signals:
    void finishedSaving();
    void finishedLoading();
};

#endif // SAVERANDLOADER_H
