
#ifndef MSFSCONNECT_H
#define MSFSCONNECT_H


#include <QObject>

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#include <QThread>

#include <SimConnect.h>


class MSFSConnect : public QObject
{
    Q_OBJECT

public:
    explicit MSFSConnect(QObject *parent = nullptr);
    bool connect();
    void disconnect();
    void loop();

private:
    void CALLBACK dispatchProc();

signals:
    Q_SIGNAL void appendText(QString text);
    Q_SIGNAL void clearText();

};

#endif // MSFSCONNECT_H
