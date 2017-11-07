#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Image SNR Calculator");

    // File Selection
    browseButton = this->findChild<QToolButton*>("toolButton");
    treeView = this->findChild<QTreeView*>("treeView");
    selectedDirectoryLine = this->findChild<QLineEdit*>("selectedDirectoryLine");

    noise = this->findChild<QLineEdit*>("noise");
    signal = this->findChild<QLineEdit*>("signal");
    snr = this->findChild<QLineEdit*>("snr");
    psnr = this->findChild<QLineEdit*>("psnr");
    threshold = this->findChild<QSpinBox*>("thresholdSpin");
    brushradius = this->findChild<QSpinBox*>("brushradiusSpin");

    QLabel* signalLabel = this->findChild<QLabel*>("signalLabel");
    QLabel* noiseLabel = this->findChild<QLabel*>("noiseLabel");


    label = this->findChild<SLabel*>("labelView");

    label->noise = noise;
    label->signal = signal;
    label->snr = snr;
    label->psnr = psnr;
    label->signalLabel = signalLabel;
    label->noiseLabel = noiseLabel;

    // Setup widgets
    fileSystemModel = new QFileSystemModel;
    fileSystemModel->setRootPath(QDir::currentPath());
    treeView->setModel(fileSystemModel);
    treeView->setHeaderHidden(true);
    treeView->header()->hideSection(1);
    treeView->header()->hideSection(2);
    treeView->header()->hideSection(3);

    readSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::message(QString s) {
    QMessageBox msgBox;
    msgBox.setText(s);
    msgBox.exec();
}

void MainWindow::on_selectDirectoryButton_clicked()
{
    QDir dir = QFileDialog::getExistingDirectory(this, tr("Open"), QDir::currentPath());
    selectedDirectoryLine->setText(dir.absolutePath());
    setTreeViewPath(dir.absolutePath());
    this->update();
    writeSettings();
}

void MainWindow::setTreeViewPath(QString path)
{
    fileSystemModel->setRootPath(path);
    treeView->setModel(fileSystemModel);
    treeView->setRootIndex(fileSystemModel->index(path));
}

void MainWindow::on_selectedDirectoryLine_returnPressed()
{
    setTreeViewPath(selectedDirectoryLine->text());
}



void MainWindow::writeSettings()
{
    QSettings settings("nicola", "SNR");

    settings.beginGroup("MainWindow");

    settings.setValue("selectedDirectory", selectedDirectoryLine->text());
    settings.setValue("threshold", threshold->value());
    settings.setValue("brushradius", brushradius->value());

    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings("nicola", "SNR");

    settings.beginGroup("MainWindow");

    selectedDirectoryLine->setText(settings.value("selectedDirectory").toString());
    setTreeViewPath(selectedDirectoryLine->text());

    threshold->setValue(settings.value("threshold").toInt());
    label->setThreshold(settings.value("threshold").toInt());

    brushradius->setValue(settings.value("brushradius").toInt());
    label->setBrushRadius(settings.value("brushradius").toInt());

    settings.endGroup();
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    QImage image(fileSystemModel->filePath(index));

    if (!image.isNull()) {
        label->setImage(image);
    }
}

void MainWindow::on_eraseButton_clicked()
{
    label->erase();
}

void MainWindow::on_thresholdSpin_editingFinished()
{
    label->setThreshold(threshold->value());
    writeSettings();
}

void MainWindow::on_brushradiusSpin_editingFinished()
{
    label->setBrushRadius(brushradius->value());
    writeSettings();
}
