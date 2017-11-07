#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include "ui_mainwindow.h"
#include <QFileSystemModel>
#include <QSettings>
#include <slabel.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_selectDirectoryButton_clicked();

    void on_selectedDirectoryLine_returnPressed();

    void message(QString s);

    void on_treeView_clicked(const QModelIndex &index);

    void on_eraseButton_clicked();

    void on_thresholdSpin_editingFinished();

    void on_brushradiusSpin_editingFinished();

private:
    void setTreeViewPath(QString path);
    void readSettings();
    void writeSettings();
    void start();


private:
    QSettings settings;
    Ui::MainWindow *ui;

    //------ File Selection
    QToolButton* browseButton;
    QLineEdit* selectedDirectoryLine;
    QTreeView* treeView;
    QFileSystemModel* fileSystemModel;

    QLineEdit* snr;
    QLineEdit* noise;
    QLineEdit* signal;
    QLineEdit* psnr;

    QSpinBox* threshold;
    QSpinBox* brushradius;

//    QGraphicsView* graphicsView;
    SLabel* label;

};

#endif // MAINWINDOW_H
