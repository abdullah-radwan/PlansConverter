#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>

MainWindow::MainWindow(ConfigEditor::Config* config) : ui(new Ui::MainWindow), converterThread(this)
{
    ui->setupUi(this);

    this->config = config;

    setConverter();
    setUpdater();

    ui->fromFplRadio->setChecked(config->inputData.fplSelected);
    ui->fromPlnRadio->setChecked(config->inputData.plnSelected);
    ui->fromFmsRadio->setChecked(config->inputData.fmsSelected);

    ui->toFplRadio->setChecked(config->outputData.fplSelected);
    ui->toPlnRadio->setChecked(config->outputData.plnSelected);
    ui->toFmsRadio->setChecked(config->outputData.fmsSelected);

    ui->ifrRadio->setChecked(config->typeIFR);
    ui->vfrRadio->setChecked(!config->typeIFR);

    if (config->altitude)
    {
        ui->altitudeCheck->setChecked(true);
        ui->altitudeSpin->setValue(config->altitude);
    }
    else ui->altitudeCheck->setChecked(false);

    ui->deleteCheck->setChecked(config->deleteInput);

    ui->routeEdit->setMinimumHeight(ui->routeEdit->fontMetrics().lineSpacing() +
                                   (ui->routeEdit->document()->documentMargin() + ui->routeEdit->frameWidth()) * 2 +
                                    ui->routeEdit->contentsMargins().top() + ui->routeEdit->contentsMargins().bottom());

    show();

    setWidgets();

    ui->toLabel->setMinimumWidth(ui->fromLabel->width());

    if (config->mainSize != QSize(-1, -1)) resize(config->mainSize);

    if (config->mainPos != QPoint(-1, -1)) move(config->mainPos);

    if (config->checkUpdates) on_checkAction_triggered();
}

void MainWindow::setConverter()
{
    converterOp = new ConverterOp;

    converterOp->moveToThread(&converterThread);

    connect(this, &MainWindow::convert, converterOp, &ConverterOp::convert);

    connect(converterOp, &ConverterOp::convertingFile, this, &MainWindow::onConvertingFile);
    connect(converterOp, &ConverterOp::done, this, &MainWindow::onDone);
    connect(converterOp, &ConverterOp::error, this, &MainWindow::onError);

    connect(&converterThread, &QThread::finished, converterOp, &QObject::deleteLater);

    converterThread.start();
}

void MainWindow::setUpdater()
{
    QDir appDir(qApp->applicationDirPath());

#ifdef Q_OS_MACOS
    appDir.cdUp(); appDir.cdUp(); appDir.cdUp();
#endif

    updater = QtAutoUpdater::Updater::create("qtifw", {{"path", appDir.absolutePath() + "/Plans Converter Maintenance Tool"}}, qApp);

    if (updater)
    {
        connect(updater, &QtAutoUpdater::Updater::checkUpdatesDone, [&](QtAutoUpdater::Updater::State state)
        {
            switch (state)
            {
            case QtAutoUpdater::Updater::State::Error:
                qDebug() << "An error occured";
                config->checkUpdates = false;
                break;

            case QtAutoUpdater::Updater::State::NoUpdates:
                qDebug() << "No updates were found";
                break;

            case QtAutoUpdater::Updater::State::NewUpdates:
                qDebug() << "An update was found" << updater->updateInfo();
                break;

            default:
                break;
            }
        });

        updateController = new QtAutoUpdater::UpdateController(updater, this);
    }
    else qDebug() << "Couldn't create the updater";
}

void MainWindow::setWidgets(bool fromChanged)
{
    if (fromChanged) resetInput();

    if (ui->fromFplRadio->isChecked())
    {
        if (ui->toFplRadio->isChecked()) ui->toPlnRadio->setChecked(true);

        ui->toFplRadio->setEnabled(false);
    }
    else ui->toFplRadio->setEnabled(true);

    if (ui->fromPlnRadio->isChecked())
    {
        if (ui->toPlnRadio->isChecked()) ui->toFmsRadio->setChecked(true);

        ui->toPlnRadio->setEnabled(false);
    } else ui->toPlnRadio->setEnabled(true);

    if (ui->fromFmsRadio->isChecked())
    {
        if (ui->toFmsRadio->isChecked()) ui->toPlnRadio->setChecked(true);

        ui->toFmsRadio->setEnabled(false);
    } else ui->toFmsRadio->setEnabled(true);

    if (ui->toFplRadio->isChecked())
    {
        ui->flightTypeWidget->setVisible(false);

        ui->routeCheck->setVisible(false);
        ui->routeEdit->setVisible(false);
        ui->routeTypeWidget->setVisible(false);

        ui->altitudeCheck->setVisible(false);
        ui->altitudeSpin->setVisible(false);
    }
    else
    {
        ui->flightTypeWidget->setVisible(ui->toPlnRadio->isChecked());
        ui->routeCheck->setVisible(true);
        ui->routeEdit->setVisible(true);

        ui->routeTypeWidget->setVisible(ui->toPlnRadio->isChecked() && ui->routeCheck->isChecked());

        ui->altitudeCheck->setVisible(true);
        ui->altitudeSpin->setVisible(true);
    }

    if (ui->ifrRadio->isChecked())
    {
        ui->type1Radio->setText("Low-altitude airways");
        ui->type2Radio->setText("High-altitude airways");
    }
    else
    {
        ui->type1Radio->setText("Direct - GPS");
        ui->type2Radio->setText("VOR to VOR");
    }
}

void MainWindow::resetInput()
{
    ui->fileLabel->setText("Select the input file:");

    converterOp->data.inputPath = QString();
}

void MainWindow::on_fromFplRadio_clicked() { setWidgets(true); }

void MainWindow::on_fromPlnRadio_clicked() { setWidgets(true); }

void MainWindow::on_fromFmsRadio_clicked() { setWidgets(true); }

void MainWindow::on_toFplRadio_clicked() { setWidgets(); }

void MainWindow::on_toPlnRadio_clicked() { setWidgets(); }

void MainWindow::on_toFmsRadio_clicked() { setWidgets(); }

void MainWindow::on_ifrRadio_clicked() { setWidgets(); }

void MainWindow::on_vfrRadio_clicked() { setWidgets(); }

void MainWindow::on_routeCheck_stateChanged(int arg1)
{
    ui->routeEdit->setEnabled(arg1);
    setWidgets();
}

void MainWindow::on_altitudeCheck_stateChanged(int arg1)
{
    ui->altitudeSpin->setEnabled(arg1);
}

void MainWindow::on_fileButton_clicked()
{
    if (ui->fromFplRadio->isChecked())
    {
        inputFolder = &config->inputData.fplFolder;
        inputFormat = "FPL";
        converterOp->data.inputFormat = ConverterOp::FPL;
    }
    else if (ui->fromPlnRadio->isChecked())
    {
        inputFolder = &config->inputData.plnFolder;
        inputFormat = "PLN";
        converterOp->data.inputFormat = ConverterOp::PLN;
    }
    else if (ui->fromFmsRadio->isChecked())
    {
        inputFolder = &config->inputData.fmsFolder;
        inputFormat = "FMS";
        converterOp->data.inputFormat = ConverterOp::FMS;
    }

    QString path = QFileDialog::getOpenFileName(this, QString("Select %1 File").arg(inputFormat), *inputFolder,
                                                QString("%1 Files (*.%2)").arg(inputFormat, inputFormat.toLower()));

    if (path.isEmpty()) return;

    *inputFolder = QFileInfo(path).absolutePath();

    converterOp->data.inputPath = path;

    ui->fileLabel->setText("Selected file: " + path);
}

void MainWindow::on_convertButton_clicked()
{
    if (converterOp->data.inputPath.isEmpty())
    {
        statusBar()->showMessage("You must select an input file.");
        return;
    }

    if (ui->toFplRadio->isChecked())
    {
        outputFolder = &config->outputData.fplFolder;
        outputFormat = "FPL";
        converterOp->data.outputFormat = ConverterOp::FPL;
    }
    else if (ui->toPlnRadio->isChecked())
    {
        outputFolder = &config->outputData.plnFolder;
        outputFormat = "PLN";
        converterOp->data.outputFormat = ConverterOp::PLN;
    }
    else if (ui->toFmsRadio->isChecked())
    {
        outputFolder = &config->outputData.fmsFolder;
        outputFormat = "FMS";
        converterOp->data.outputFormat = ConverterOp::FMS;
    }

    QString savePath;

    if (outputFolder->isEmpty())
    {
        savePath = converterOp->data.inputPath;
        savePath.remove("." + inputFormat.toLower());
        savePath.append("." + outputFormat.toLower());
    }
    else savePath = *outputFolder + "/" + QFileInfo(converterOp->data.inputPath).baseName().append("." + outputFormat.toLower());

    QString path = QFileDialog::getSaveFileName(this, QString("Save %1 File").arg(outputFormat), savePath,
                                                QString("%1 Files (*.%2)").arg(outputFormat, outputFormat.toLower()));

    if (path.isEmpty()) return;

    if (!path.endsWith("." + outputFormat, Qt::CaseInsensitive)) path.append("." + outputFormat.toLower());

    ui->convertButton->setEnabled(false);

    *outputFolder = QFileInfo(path).absolutePath();

    converterOp->data.typeIFR = ui->ifrRadio->isVisible() && ui->ifrRadio->isChecked();

    if (ui->routeTypeWidget->isVisible())
    {
        if (converterOp->data.typeIFR)
        {
            if (ui->type1Radio->isChecked()) converterOp->data.routeType = "LowAlt";
            else converterOp->data.routeType = "HighAlt";
        }
        else
        {
            if (ui->type1Radio->isChecked()) converterOp->data.routeType = QString();
            else converterOp->data.routeType = "VOR";
        }
    }
    else converterOp->data.routeType = QString();

    converterOp->data.route = ui->routeCheck->isVisible() && ui->routeCheck->isChecked() ? ui->routeEdit->toPlainText().replace("\n", " ") : QString();

    converterOp->data.altitude = ui->altitudeCheck->isVisible() && ui->altitudeCheck->isChecked() ? ui->altitudeSpin->value() : 0;

    converterOp->data.deleteInput = ui->deleteCheck->isChecked();

    converterOp->data.outputPath = path;

    emit convert();
}

void MainWindow::onConvertingFile()
{
    statusBar()->showMessage("Converting the input file...");
}

void MainWindow::onDone()
{
    statusBar()->showMessage("The input file was converted successfully.");
    ui->convertButton->setEnabled(true);
}

void MainWindow::onError(QString error)
{
    statusBar()->showMessage("Error: " + error);
    ui->convertButton->setEnabled(true);
}

void MainWindow::on_checkAction_triggered()
{
    if (!updater)
    {
        QMessageBox::warning(this, "Updater initialization", "Couldn't initialize the updater.");
        return;
    }

    qDebug() << "Starting updater";

    updateController->start(QtAutoUpdater::UpdateController::DisplayLevel::ExtendedInfo);
}

void MainWindow::on_aboutAction_triggered()
{
    QMessageBox::about(this, "Plans Converter", "Plans Converter\n"
                             "An app to convert between major flight plan formats.\n\n"

                             "Version: 1.0.1\n"
                             "Release date: 4 March 2021\n\n"

                             "Copyright © Abdullah Radwan");
}

void MainWindow::closeEvent(QCloseEvent *bar)
{
    config->inputData.fplSelected = ui->fromFplRadio->isChecked();
    config->inputData.plnSelected = ui->fromPlnRadio->isChecked();
    config->inputData.fmsSelected = ui->fromFmsRadio->isChecked();

    config->outputData.fplSelected = ui->toFplRadio->isChecked();
    config->outputData.plnSelected = ui->toPlnRadio->isChecked();
    config->outputData.fmsSelected = ui->toFmsRadio->isChecked();

    config->typeIFR = ui->ifrRadio->isChecked();
    config->altitude = ui->altitudeCheck->isChecked() && ui->altitudeSpin->isVisible() ? ui->altitudeSpin->value() : 0;
    config->deleteInput = ui->deleteCheck->isChecked();

    config->mainSize = size();
    config->mainPos = pos();

    bar->accept();
}

MainWindow::~MainWindow()
{
    converterThread.quit();
    converterThread.wait();

    delete ui;
}
