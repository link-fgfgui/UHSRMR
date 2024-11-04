#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QObject>
#include <QMessageBox>
#include <windows.h>
#include <tlhelp32.h>
#include <QProcess>
#include <QTimer>
#include <QLoggingCategory>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QCloseEvent>

Q_DECLARE_LOGGING_CATEGORY(UHSRMR)
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget* parent = nullptr);
    ~Widget();
    const int CHOOSE = 0;
    const int PATH2 = 2;
    const int PATH1 = 3;
    int t11, t12, t13, t21, t22, t23, t24;
    QSettings* config;
    QFile* file_choose;
    QFile* file_path2;
    QFile* file_path;
    int choose = 0;
    QString path2;
    QString path1;
    QString pathtmp;
    QString dllUrl = "https://autopatchcn.bhsr.com/client/cn/20241010190206_tkloMB0w9r83S2Nz/PC/unzip/mhypbase.dll";
    int dllUrlVersion = 1;
    void loadConfig(QFile* ptr, int type);
    void refreshTip();
    void run();
    void unlock();
    void unlockcore(QString path_);
    void openNI();
    void addClient(QString ProgramPath = QString());
    void startTwoClient(QString path_);
    void startTwoServerClient();
    void startTwoClientByFirewall(QString path_);
    void startTwoServerClientByFirewall();
    void start(QString ProgramPath);
    void offInternet();
    void onInternet();
    void blockByFirewall(QString ProgramPath);
    void allowByFirewall();
    void save();
    void getNI();
    virtual void closeEvent(QCloseEvent* event) override;
    QString getStartCommand(QString path_);
    QStringList* network;
    static QString getRuningClientPath(bool isKill = false);

private:
    Ui::Widget* ui;
};
#endif // WIDGET_H
