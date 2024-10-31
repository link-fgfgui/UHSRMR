#include "widget.h"
#include "ui_widget.h"
Q_LOGGING_CATEGORY(FsMsR, "FsMsR")
Widget::Widget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    qCDebug(FsMsR) << "loading configs";
    network = new QStringList();
    file_choose = new QFile("选择.txt");
    file_path2 = new QFile("路径2.txt");
    file_path = new QFile("路径.txt");
    loadConfig(file_choose, CHOOSE);
    loadConfig(file_path, PATH1);
    loadConfig(file_path2, PATH2);
    config = new QSettings("conf.ini", QSettings::IniFormat);
    t11 = config->value("FsMsR/t11", 8000).toInt();
    t12 = config->value("FsMsR/t12", 9000).toInt();
    t13 = config->value("FsMsR/t13", 18000).toInt();
    t21 = config->value("FsMsR/t21", 7000).toInt();
    t22 = config->value("FsMsR/t22", 7500).toInt();
    t23 = config->value("FsMsR/t23", 11500).toInt();
    t24 = config->value("FsMsR/t24", 13000).toInt();
    if (config->contains("FsMsR/choose")) choose = config->value("FsMsR/choose").toInt();

    if (config->contains("FsMsR/path1")) path1 = config->value("FsMsR/path1").toString();

    if (config->contains("FsMsR/path2")) path2 = config->value("FsMsR/path2").toString();

    if (dllUrlVersion > config->value("FsMsR/dllUrlVersion").toInt())
    {
        config->setValue("FsMsR/dllUrlVersion", dllUrlVersion);
        config->setValue("FsMsR/dllUrl", dllUrl);
    }
    else
    {
        dllUrl = config->value("FsMsR/dllUrl").toString();
    }

    if (path1.isEmpty())
    {
        QSettings reg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\HYP_1_1_cn", QSettings::NativeFormat);
        QString tmp = reg.value("InstallPath").toString() + QString("\\games\\Star Rail Game\\StarRail.exe");
        if (!QFile::exists(tmp))tmp = "D:\\collapse\\SRCN\\Games\\StarRail.exe";
        if (QFile::exists(tmp))
        {
            qCDebug(FsMsR) << "get mihoyo launcher: " << tmp;
            QMessageBox::information(nullptr, tr("信息"), tr("检测到米哈游启动器,将自动为您解除限制并添加至第一区服"));
            path1 = tmp;
            unlock();
        }
    }

    getNI();

    ui->comboBox_choose->setCurrentIndex(choose);
    connect(ui->comboBox_choose, &QComboBox::currentIndexChanged, [ = ](int i)
    {
        choose = i;
        qCDebug(FsMsR) << "choose changed:" << choose;
    });
    connect(ui->pushButton_exec, &QPushButton::clicked, this, run);
    connect(ui->pushButton_fuck, &QPushButton::clicked, this, unlock);
    refreshTip();
}
void Widget::save()
{
    config->setValue("FsMsR/choose", choose);
    config->setValue("FsMsR/path1", path1);
    config->setValue("FsMsR/path2", path2);
    config->setValue("FsMsR/t11", t11);
    config->setValue("FsMsR/t12", t12);
    config->setValue("FsMsR/t13", t13);
    config->setValue("FsMsR/t21", t21);
    config->setValue("FsMsR/t22", t22);
    config->setValue("FsMsR/t23", t23);
    config->setValue("FsMsR/t24", t24);
    config->sync();
    qCDebug(FsMsR) << "saved!";
}


void Widget::closeEvent(QCloseEvent* event)
{
    save();
    event->accept();
}




void Widget::loadConfig(QFile* ptr, int type)
{

    if (ptr->exists())
    {
        ptr->open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream ts(ptr);
        ts.setEncoding(QStringConverter::System);
        QString tmp = ts.readAll();
        qCDebug(FsMsR) << "type:" << type << "|" << tmp << ";";
        if (tmp.isEmpty())
        {
            if (type == CHOOSE)
            {
                choose = 0;
            }
        }
        else
        {
            if (type == CHOOSE)
            {
                choose = tmp.trimmed().toInt();
            }
            if (type == PATH1)
            {
                path1 = tmp.trimmed();
            }
            if (type == PATH2)
            {
                path2 = tmp.trimmed();
            }
        }
        ptr->close();
    }

}

void Widget::refreshTip()
{
    QString outText;
    qCDebug(FsMsR) << "refreshTip";
    if (path1.isEmpty())
    {
        outText.append(tr("第一区服未设置,将无法使用一键双开功能\n"));
    }
    if (path2.isEmpty())
    {
        outText.append(tr("第二区服未设置,将无法使用一键双开双服功能\n"));
    }
    if (outText.isEmpty())
    {
        outText = tr("一切正常\n");
    }
    outText.append(tr("本程序完全免费  谨防倒卖!\n"));
    outText.append("Fu*k StarRail Multi-Start Restrict");
    ui->label_info->setText(outText);
}

void Widget::getNI()
{
    network->clear();
    QProcess process(this);
    process.setProgram("netsh");
    QStringList argument;
    argument << "interface" << "show" << "interface";
    process.setArguments(argument);
    process.start();
    process.waitForFinished();
    QString temp = QString::fromLocal8Bit(process.readAllStandardOutput());
    // qCDebug(FsMsR) << temp;
    QStringList rows = temp.split("\r\n");
    // qCDebug(FsMsR) << rows;
    for (int i = 3 ; i < rows.length(); i++)
    {
        if (rows[i].isEmpty())break;
        QStringList col = rows[i].split("  ");
        network->append(col.last().trimmed());
    }
    qCDebug(FsMsR) << *network;
};

void Widget::run()
{

    ui->pushButton_exec->setDisabled(true);
    int type = ui->comboBox_choose->currentIndex();
    qCDebug(FsMsR) << "run! type:" << type;
    if (type == 6)
    {
        refreshTip();
        QMessageBox::information(NULL, tr("提示"), tr("成功"));
    }
    if (type == 7)
    {
        addClient();
    }
    if (type == 8)
    {
        offInternet();
    }
    if (type == 9)
    {
        onInternet();
    }
    if (type == 10)
    {
        if (QFile::exists(path1)) blockByFirewall(path1);
    }
    if (type == 11)
    {
        allowByFirewall();
    }
    if (type == 12)
    {
        if (path1.isEmpty())
        {
            QMessageBox::critical(nullptr, tr("错误"), tr("未添加客户端"));
            ui->pushButton_exec->setEnabled(true);
            return;
        }
        start(getStartCommand(path1));
    }
    if (type == 13)
    {
        if (path2.isEmpty())
        {
            QMessageBox::critical(nullptr, tr("错误"), tr("未添加客户端"));
            ui->pushButton_exec->setEnabled(true);
            return;
        }
        start(getStartCommand(path2));
    }
    if (type == 14)
    {
        if (path2.isEmpty())
        {
            QMessageBox::critical(nullptr, tr("错误"), tr("未添加客户端"));
            ui->pushButton_exec->setEnabled(true);
            return;
        }
        startTwoClient(path2);
    }
    if (type == 15)
    {
        if (path2.isEmpty())
        {
            QMessageBox::critical(nullptr, tr("错误"), tr("未添加客户端"));
            ui->pushButton_exec->setEnabled(true);
            return;
        }
        startTwoClientByFirewall(path2);
    }
    if (type == 16)
    {
        if (QFile::exists(path2)) blockByFirewall(path2);
    }
    if (type == 0)
    {

        qCDebug(FsMsR) << "lock!";

        if (path1.isEmpty())
        {
            QMessageBox::critical(nullptr, tr("错误"), tr("未添加客户端"));
            ui->pushButton_exec->setEnabled(true);
            return;
        }
        auto mhypbasepath = QFileInfo(path1).dir().filePath("mhypbase.dll");
        QFile(mhypbasepath).moveToTrash();
        system(QString("cmd /C curl -# --retry 3 -o %1 %2").arg(mhypbasepath, dllUrl).toStdString().c_str());
        if (!path2.isEmpty())
        {
            mhypbasepath = QFileInfo(path2).dir().filePath("mhypbase.dll");
            QFile(mhypbasepath).moveToTrash();
            system(QString("cmd /C curl -# --retry 3 -o %1 %2").arg(mhypbasepath, dllUrl).toStdString().c_str());
        }
        QMessageBox::information(nullptr, tr("信息"), tr("完成"));
    }
    if (type == 1)
    {
        start("ncpa.cpl");
    }
    if (type == 2)
    {
        if (path1.isEmpty())
        {
            QMessageBox::critical(nullptr, tr("错误"), tr("未添加客户端"));
            ui->pushButton_exec->setEnabled(true);
            return;
        }
        startTwoClient(path1);
    }

    if (type == 3)
    {
        if (path1.isEmpty() || path2.isEmpty())
        {
            QMessageBox::critical(nullptr, tr("错误"), tr("未添加客户端"));
            ui->pushButton_exec->setEnabled(true);
            return;
        }
        startTwoServerClient();
    }
    if (type == 4)
    {
        if (path1.isEmpty())
        {
            QMessageBox::critical(nullptr, tr("错误"), tr("未添加客户端"));
            ui->pushButton_exec->setEnabled(true);
            return;
        }
        startTwoClientByFirewall(path1);
    }
    if (type == 5)
    {
        if (path1.isEmpty() || path2.isEmpty())
        {
            QMessageBox::critical(nullptr, tr("错误"), tr("未添加客户端"));
            ui->pushButton_exec->setEnabled(true);
            return;
        }
        startTwoServerClientByFirewall();
    }
    ui->pushButton_exec->setEnabled(true);
}

QString Widget::getStartCommand(QString path_)
{
    return QString("start \"\" /D \"%1\"  \"%2\"").arg(QFileInfo(path_).dir().path().replace("/", "\\"), path_);
}


void Widget::startTwoClient(QString path_)
{
    pathtmp = path_;
    start(getStartCommand(pathtmp));
    QTimer::singleShot(200, this, [&]() {ui->pushButton_exec->setDisabled(true);});
    QTimer::singleShot(t11, this, [&]() {offInternet();});
    QTimer::singleShot(t12, this, [&]() {start(getStartCommand(pathtmp));});
    QTimer::singleShot(t13, this, [&]() {onInternet();});
    QTimer::singleShot(t13 + 200, this, [&]() {ui->pushButton_exec->setEnabled(true);});
};

void Widget::startTwoServerClient()
{
    start(getStartCommand(path1));
    QTimer::singleShot(200, this, [&]() {ui->pushButton_exec->setDisabled(true);});
    QTimer::singleShot(t11, this, [&]() {offInternet();});
    QTimer::singleShot(t12, this, [&]() {start(getStartCommand(path2));});
    QTimer::singleShot(t13, this, [&]() {onInternet();});
    QTimer::singleShot(t13 + 200, this, [&]() {ui->pushButton_exec->setEnabled(true);});
};
void Widget::startTwoClientByFirewall(QString path_)
{
    pathtmp = path_;
    start(getStartCommand(pathtmp));
    QTimer::singleShot(200, this, [&]() {ui->pushButton_exec->setDisabled(true);});
    QTimer::singleShot(t21, this, [&]() {blockByFirewall(pathtmp);});
    QTimer::singleShot(t22, this, [&]() {start(getStartCommand(pathtmp));});
    QTimer::singleShot(t23, this, [&]() {allowByFirewall();});
    // QTimer::singleShot(t24, this, [&]() {start(getStartCommand(pathtmp));});
    QTimer::singleShot(t24 + 200, this, [&]() {ui->pushButton_exec->setEnabled(true);});
};

void Widget::startTwoServerClientByFirewall()
{
    start(getStartCommand(path1));
    blockByFirewall(path2);
    QTimer::singleShot(200, this, [&]() {ui->pushButton_exec->setDisabled(true);});
    QTimer::singleShot(t21, this, [&]() {start(getStartCommand(path2));});
    QTimer::singleShot(t23, this, [&]() {allowByFirewall();});
    QTimer::singleShot(t23 + 200, this, [&]() {ui->pushButton_exec->setEnabled(true);});
};

void Widget::offInternet()
{
    for (int i = 0 ; i < network->length(); i++)
    {
        start(QString("netsh interface set interface \"%1\" admin=disable").arg(network->at(i)));
    }
}
void Widget::onInternet()
{
    for (int i = 0 ; i < network->length(); i++)
    {
        start(QString("netsh interface set interface \"%1\" admin=enable").arg(network->at(i)));
    }
}
void Widget::blockByFirewall(QString ProgramPath)
{
    if (ProgramPath.contains(' '))ProgramPath = '"' + ProgramPath + '"';
    start(QString("netsh advfirewall firewall add rule name=\"BanSR\" dir=out action=block enable=yes program=") + ProgramPath);
}
void Widget::allowByFirewall()
{
    start("netsh advfirewall firewall delete rule name=\"BanSR\"");
}

void Widget::unlock()
{
    qCDebug(FsMsR) << "unlock!";
    ui->pushButton_fuck->setDisabled(true);
    auto path_now = getRuningClientPath(true);
    addClient(path_now);
    if (path_now.isEmpty())
    {
        if (path1.isEmpty())
        {
            QMessageBox::critical(nullptr, tr("错误"), tr("请先开启崩铁"));
            ui->pushButton_fuck->setEnabled(true);
            return;
        }
        else
        {
            path_now = path1;
        }
    }
    unlockcore(path1);
    if (!path2.isEmpty())unlockcore(path2);

    QMessageBox::information(nullptr, tr("信息"), tr("完成"));
    refreshTip();

    ui->pushButton_fuck->setEnabled(true);
}
void Widget::unlockcore(QString path_)
{
    auto mhypbasepath = QFileInfo(path_).dir().filePath("mhypbase.dll");
    QFile(mhypbasepath).moveToTrash();
    QFile::copy(":/dll/mhypbase.dll", mhypbasepath);
}


void Widget::addClient(QString ProgramPath)
{
    qCDebug(FsMsR) << "addClient!";
    QString pathtemp;
    if (ProgramPath.isEmpty())
    {
        pathtemp = getRuningClientPath();
    }
    else
    {
        pathtemp = ProgramPath;
    }

    if (pathtemp.isEmpty())
    {
        if (ui->pushButton_fuck->isEnabled())
        {
            QMessageBox::critical(nullptr, tr("错误"), tr("未找到崩铁客户端,请先启动"));
        }
    }
    else
    {
        if (path1.isEmpty())
        {
            path1 = pathtemp;
            QMessageBox::information(nullptr, tr("成功"), tr("成功添加游戏"));
        }
        else if (path1 == pathtemp)
        {
            QMessageBox::information(nullptr, tr("信息"), tr("您已添加此客户端"));
        }
        else
        {
            path2 = pathtemp;
            QMessageBox::information(nullptr, tr("成功"), tr("成功添加游戏"));
        }
    }
    refreshTip();
}

void Widget::start(QString ProgramPath)
{
    // qCDebug(FsMsR) << ProgramPath[0];
    // if (ProgramPath[0] != '"' && ProgramPath.contains(" ") && ProgramPath[0] != 'n')ProgramPath = QChar('"') + ProgramPath + QChar('"');
    qCDebug(FsMsR) << "start:" << (QString("cmd /C ") + ProgramPath).toStdString().c_str();
    WinExec((QString("cmd /C ") + ProgramPath).toLocal8Bit().toStdString().c_str(), SW_HIDE);
}
QString Widget::getRuningClientPath(bool isKill)
{
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    unsigned long pid = 0;
    QString path_now;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hProcessSnap, &pe32))
    {
        do
        {
            QString filename = QString::fromStdWString(std::wstring(pe32.szExeFile));
            if (filename == QString("StarRail.exe"))
            {
                pid = pe32.th32ProcessID;
                qCDebug(FsMsR) << "get pid:" << pid;
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE, FALSE, pid);
                TCHAR ProcessPath[MAX_PATH];
                DWORD pathLength = sizeof(ProcessPath) / sizeof(TCHAR);
                QueryFullProcessImageName(hProcess, 0, ProcessPath, &pathLength);
                path_now = QString::fromStdWString(std::wstring(ProcessPath));
                qCDebug(FsMsR) << "get path:" << path_now;
                if (isKill)
                {
                    TerminateProcess(hProcess, 0);
                }
                CloseHandle(hProcess);
                CloseHandle(hProcessSnap);
                return path_now;
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }
    CloseHandle(hProcessSnap);
    return path_now;
}


Widget::~Widget()
{
    delete ui;
}

