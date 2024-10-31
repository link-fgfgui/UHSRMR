#include "widget.h"
#include <QTranslator>
#include <QLocale>
#include <QApplication>


QString version = "0.7";


void customMessageHandler(QtMsgType type, const QMessageLogContext&, const QString& str)
{
    QString txt = str;
    QString app_run_addr;
    QString current_PathName = QCoreApplication::applicationDirPath();
    if (QFile::exists(current_PathName) == false)
    {
        app_run_addr = "debug.log";
    }
    else
    {
        app_run_addr = current_PathName + "/" + "debug.log";
    }
    QFile outFile(app_run_addr);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << Qt::endl;
}


int main(int argc, char* argv[])
{
    // qCDebug(FsMsR) << Widget::getRuningClientPath(false);
    // exit(0);
    QTranslator translator;
    translator.load(QLocale(), "FsMsR", "_", ":/trans", ".qm");
    QApplication a(argc, argv);
    QCoreApplication::installTranslator(&translator);
    // anti multi-boxing
    if (argc == 1)
    {
        HANDLE Mutex  =  CreateMutex(NULL, FALSE,  L"FsMsR_anti_multi");
        if (GetLastError()  ==  ERROR_ALREADY_EXISTS)
        {
            CloseHandle(Mutex);
            Mutex  =  NULL;
            // QMessageBox::critical(NULL, "错误", "不建议普通用户开启多个实例,如果您有需求,请加命令行参数");
            exit(1);
        }
    }
    qInstallMessageHandler(customMessageHandler);
    QMessageBox::information(NULL, "FsMsR", "By:\nKarami\nflatied@foxmail.com\nlink-fgfgui\nlink.fgfgui@gmail.com");
    Widget w;
    w.setWindowTitle(QObject::tr("崩铁解除双开限制 v") + version);
    w.show();
    return a.exec();
}
