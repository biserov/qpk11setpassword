#include "qpk11setpassword.h"

#include<QDebug>

#include <dlfcn.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

SharedLibrary::SharedLibrary() : app(nullptr), thread(nullptr)
{
    if (thread == NULL)
    {
        thread = new QThread();
        connect(thread, SIGNAL(started()), this, SLOT(onStarted()), Qt::DirectConnection);
        thread->start();
    }
}

void SharedLibrary::onStarted()
{
    static int argc = 0;
    //static char *argv[] = { "SharedLibrary", NULL };

    if (QCoreApplication::instance() == NULL) {
       app = new QCoreApplication(argc, NULL);
       app->exec();
   }
}

static SharedLibrary *_library = nullptr;

extern "C"
{

typedef char *(*PK11PasswordFunc)(void *slot, int retry, void *arg);
typedef char *(*_PL_strdupFunc)(const char *s);
typedef void (*_PK11_SetPasswordFunc)(PK11PasswordFunc func);

/* Function pointers to hold the value of the glibc functions */
static _PL_strdupFunc real_PL_strdup = NULL;
static _PK11_SetPasswordFunc real_PK11_SetPasswordFunc = NULL;
static PK11PasswordFunc real_PK11PasswordFunc = NULL;

QLatin1String storage_name("qpk11setpassword");
const char *storage_key = "PK11Password";

static char * MyPK11PasswordFunc(void *slot, int retry, void *arg)
{
    qInfo()  << "[" << getpid() << "] " << "qpk11setpassword: Called MyPK11PasswordFunc(retry=" << retry << ")";

    char *password = nullptr;
    if (retry == 0) {
        QKeychain::ReadPasswordJob job(storage_name);
        job.setKey(storage_key);
        job.setAutoDelete(false);

        QEventLoop loop;
        job.connect( &job, SIGNAL(finished(QKeychain::Job*)), &loop, SLOT(quit()) );
        job.start();
        loop.exec();

        if ( job.error() ) {
            qCritical()  << "[" << getpid() << "] " << "qpk11setpassword: Restoring password failed: " << qPrintable(job.errorString());
        } else {
            qInfo()  << "[" << getpid() << "] " << "qpk11setpassword: Restored OK";
            const QString pw = job.textData();
            return real_PL_strdup(pw.toUtf8().constData());
        }
    } else {
        QKeychain::DeletePasswordJob job(storage_name);
        job.setAutoDelete(false);
        job.setKey(storage_key);
        QEventLoop loop;
        job.connect( &job, SIGNAL(finished(QKeychain::Job*)), &loop, SLOT(quit()) );
        job.start();
        loop.exec();
        if ( job.error() ) {
            qCritical()  << "[" << getpid() << "] " << "qpk11setpassword: Delete password failed: " << qPrintable(job.errorString());
        }

    }

    if (real_PK11PasswordFunc != NULL) {
        password = real_PK11PasswordFunc(slot, retry, arg);

        if (password && strlen(password) > 0) {
            QKeychain::WritePasswordJob job(storage_name);
            job.setAutoDelete( false );
            job.setKey(storage_key);
            job.setTextData(password);
            QEventLoop loop;
            job.connect( &job, SIGNAL(finished(QKeychain::Job*)), &loop, SLOT(quit()) );
            job.start();
            loop.exec();
            if ( job.error() ) {
                qCritical()  << "[" << getpid() << "] " << "qpk11setpassword: Saving password failed: " << qPrintable(job.errorString());
            }
        }

        return password;
    }

    return NULL;
}

/* wrapping function call */
void PK11_SetPasswordFunc(PK11PasswordFunc func)
{
    qInfo() << "[" << getpid() << "] " << "qpk11setpassword: Called PK11_SetPasswordFunc(...)";

    if (_library == nullptr) {
        _library = new SharedLibrary();
    }

    if (real_PL_strdup == NULL)
        real_PL_strdup = (_PL_strdupFunc)dlsym(RTLD_NEXT, "PL_strdup");

    if (real_PK11_SetPasswordFunc == NULL)
        real_PK11_SetPasswordFunc = (_PK11_SetPasswordFunc)dlsym(RTLD_NEXT, "PK11_SetPasswordFunc");

    if (real_PK11PasswordFunc != NULL) {
        qCritical()  << "[" << getpid() << "] " << "qpk11setpassword: ERRROR! real_PK11PasswordFunc already registered!";
    }

    real_PK11PasswordFunc = func;
    real_PK11_SetPasswordFunc(MyPK11PasswordFunc);
}


} // extern "C"
