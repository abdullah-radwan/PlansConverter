#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "configeditor.h"
#include "converterop.h"

#include <QMainWindow>
#include <QThread>
#include <QtAutoUpdaterWidgets/UpdateController>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(ConfigEditor::Config* config);
    ~MainWindow();

signals:
    void convert();

public slots:
    void onConvertingFile();
    void onDone();
    void onError(QString error);

private slots:
    void on_fromFplRadio_clicked();
    void on_fromPlnRadio_clicked();
    void on_fromFmsRadio_clicked();
    void on_toFplRadio_clicked();
    void on_toPlnRadio_clicked();
    void on_toFmsRadio_clicked();
    void on_ifrRadio_clicked();
    void on_vfrRadio_clicked();

    void on_routeCheck_stateChanged(int arg1);
    void on_altitudeCheck_stateChanged(int arg1);

    void on_fileButton_clicked();
    void on_convertButton_clicked();

    void on_aboutAction_triggered();
    void on_checkAction_triggered();

private:
    Ui::MainWindow* ui;
    ConfigEditor::Config* config;
    QtAutoUpdater::Updater* updater;

    QString inputFormat;
    QString* inputFolder;
    QString outputFormat;
    QString* outputFolder;

    ConverterOp* converterOp;
    QThread converterThread;

    void setConverter();
    void setUpdater();
    void setWidgets(bool fromChanged = false);
    void resetInput();

    void closeEvent(QCloseEvent *bar) override;
};
#endif // MAINWINDOW_H
