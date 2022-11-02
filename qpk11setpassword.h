#ifndef QPK11SETPASSWORD_H
#define QPK11SETPASSWORD_H

#include "qpk11setpassword_global.h"
#include <qt5keychain/keychain.h>

#include <QCoreApplication>
#include <QThread>

class Q_DECL_EXPORT SharedLibrary : public QObject
{
Q_OBJECT
public:
    SharedLibrary();

private slots:

    void onStarted();

private:
    QCoreApplication *app;
    QThread *thread;
};

#endif // QPK11SETPASSWORD_H
