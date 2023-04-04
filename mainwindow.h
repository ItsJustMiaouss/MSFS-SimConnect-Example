
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "msfs_connect.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void addTextData(QString text);
    void clearTextData();

private slots:
    void on_connectBtn_clicked();

private:
    Ui::MainWindow *ui;
    void setStatusBarText(QString text);
    MSFSConnect msfsConnect;

};

#endif // MAINWINDOW_H
