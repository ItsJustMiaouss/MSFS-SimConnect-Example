
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    addTextData("No data.");
    setStatusBarText("Not connected.");

    QObject::connect(&msfsConnect, &MSFSConnect::appendText, this, &MainWindow::addTextData);
    QObject::connect(&msfsConnect, &MSFSConnect::clearText, this, &MainWindow::clearTextData);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setStatusBarText(QString text)
{
    ui->statusBar->showMessage(text);
}

/**
 * Receive signal from MSFSConnect (via the connect function)
 * and add a line to the text box.
 */
void MainWindow::addTextData(QString text)
{
    ui->plainText->appendPlainText(text);
}

/**
 * Receive signal from MSFSConnect (via the connect function)
 * and clear the text box.
 */
void MainWindow::clearTextData()
{
    ui->plainText->clear();
}

/**
 * Toggle the connection with SimConnect.
 */
void MainWindow::on_connectBtn_clicked()
{
    ui->connectBtn->setDisabled(true);

    if(ui->connectBtn->text() == "Connect")
    {
        if(msfsConnect.connect())
        {
            ui->connectBtn->setDisabled(false);
            ui->connectBtn->setText("Disconnect");
            setStatusBarText("Connected.");
            msfsConnect.loop();
        }
        else
        {
            ui->connectBtn->setDisabled(false);
            QMessageBox::critical(this, "Error", "Connection failed with MSFS! Make sure the game is launched.");
            setStatusBarText("Connection failed. Please retry.");
        }
    }
    else
    {
        ui->connectBtn->setDisabled(false);
        ui->connectBtn->setText("Connect");
        setStatusBarText("Not connected.");
        msfsConnect.disconnect();
    }
}
