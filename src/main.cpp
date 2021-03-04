#include "mainwindow.h"

#include <QApplication>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>

QFile* logFile;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context) Q_UNUSED(type)

    QTextStream out(logFile);

    out << QDateTime::currentDateTime().toString("hh:mm:ss.zzz ");

    out << msg << Qt::endl;

    out.flush();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Plans Converter");
    app.setApplicationVersion("1.0.1");

    QString dirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir(dirPath).mkpath(".");

    logFile = new QFile(dirPath + "/log.txt");
    logFile->open(QFile::WriteOnly | QFile::Text);

    qInstallMessageHandler(messageHandler);

    qDebug() << "Plans Converter 1.0.1, 4 March 2021";
    qDebug() << "Current date:" << QDateTime::currentDateTime().toString("yyyy-MM-dd");

    ConfigEditor configEditor(dirPath);
    configEditor.readConfig();

    MainWindow mainWindow(&configEditor.config);

    int result = app.exec();

    configEditor.writeConfig();

    logFile->close();
    delete logFile;

    return result;
}
