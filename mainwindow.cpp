#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QSettings>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    loadStysheet();

    m_httpClient = new HttpClient();
    connect(m_httpClient, SIGNAL(signalDownLoadProcess(qint64,qint64)), this, SLOT(slotProgressBar(qint64,qint64)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::testHttpClient()
{
    QNetworkRequest req;


    /*
    //腾讯QQ下载
    {
        //https://qd.myapp.com/myapp/qqteam/tim/down/tim_pc.exe
        //https://netix.dl.sourceforge.net/project/qtav/release/1.11.0/QtAV1.11.0-MinGW32.exe
        req.setUrl(QUrl("https://qd.myapp.com/myapp/qqteam/tim/down/tim_pc.exe"));

        m_httpClient->get(req);
        QByteArray recvData = m_httpClient->data();

        if (!HttpClient::writeFile("QQ.exe", recvData))
            return;
    }
    */



    /*
    //巧虎网获取信息
    {
        req.setUrl(QUrl("https://www.qiaohu.com/jsonDate/switchData.json?ran=0.9337392607168618"));
        req.setRawHeader("Referer", "https://www.qiaohu.com/Login/GoLogin");
        req.setRawHeader("X-Requested-With", "XMLHttpRequest");

        m_httpClient->get(req);
        QByteArray recvData = m_httpClient->data();

        if (!HttpClient::writeFile("qiaohuwang.txt", recvData))
            return;
    }
    */


    /*
    //巧虎网post信息
    {
        req.setUrl(QUrl("http://i.ytsg.com/api/reader/getReaderPhoto"));
        //req.setRawHeader("Upgrade-Insecure-Requests", "1");
        //req.setRawHeader("Referer", "https://www.qiaohu.com/Login/GoLogin");
        //req.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
        QVariantMap postMap;
        postMap.insert("username", "03");
        postMap.insert("password", "03");
        postMap.insert("telephone", "");
        postMap.insert("verifyCode", "");
        postMap.insert("loginType", "");
        postMap.insert("subactivecode", "");
       // QByteArray postData = HttpClient::buildQuery(postMap);
        QByteArray postData = "142326198412061213";
        if (!m_httpClient->post(req, postData))
            return;

        QByteArray recvData = m_httpClient->data();

        if (!HttpClient::writeFile("login.txt", recvData))
            return;
    }
    */


    /*
    //验证buildQuery正确性
    {
        QVariantMap postMap;
        postMap.insert("username", "03");
        postMap.insert("password", "03");
        postMap.insert("telephone", "110");
        postMap.insert("verifyCode", "12580");
        postMap.insert("loginType", "y");
        postMap.insert("subactivecode", "qw");
        QByteArray postData = HttpClient::buildQuery(postMap);
    }
    */

    /*
    //测试JSON解析
    {
        //遍历以下格式的json字符串内容
        //{"data":[{"name":"7~12月龄","url":"baobao1"},{"name2":"7~12月龄","url2":"baobao1"}]}
        QFile file("qiaohuwang.txt");
        if (!file.open(QIODevice::ReadOnly))
            return;

        QByteArray fileData = file.readAll();
        QString fileStr = QString::fromUtf8(fileData);

        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(fileStr.toUtf8(), &jsonError);

        if(jsonError.error == QJsonParseError::NoError)
        {
            QVariantMap tempMap = jsonDoc.toVariant().toMap();
            QVariantList dataList = tempMap.value("data").toList();
            foreach (QVariant data, dataList)
            {
                QVariantMap detailMap = data.toMap();
                qDebug() << "name: " << detailMap.value("name").toString();
                qDebug() << "url: " << detailMap.value("url").toString();
                qDebug() << "text: " << detailMap.value("text").toString();
            }
        }


        //重新组装成以下格式
        //{"data":[{"name":"7~12月龄","url":"baobao1"},{"name2":"7~12月龄","url2":"baobao1"}]}
        QVariantMap tempMap;
        tempMap.insert("name", "7~12月龄");
        tempMap.insert("url", "baobao1");

        QVariantMap tempMap2;
        tempMap2.insert("name2", "7~12月龄");
        tempMap2.insert("url2", "baobao1");

        QVariantList dataList;
        dataList.append(tempMap);
        dataList.append(tempMap2);

        QVariantMap dataMap;
        dataMap.insert("data", dataList);
        QJsonDocument jsonDocument = QJsonDocument::fromVariant(dataMap);
        qDebug() << jsonDocument.toJson(QJsonDocument::Compact);
    }
    */

    //测试QSettings
    {
        //保存配置文件
        QSettings saveSettings("saveSettings.ini", QSettings::IniFormat);
        saveSettings.setValue("mainwindow/size", "123");
        saveSettings.setValue("mainwindow/long", "123654");
        saveSettings.setValue("mainwindow/kuan", "0089");
        saveSettings.setValue("family/father", "8y28");
        saveSettings.setValue("family/mother", "17821");

        saveSettings.beginGroup("school");
        saveSettings.setValue("teacher", "mr.wang");
        saveSettings.setValue("teacher", "mr.zhang");
        saveSettings.endGroup();

        saveSettings.beginGroup("class");
        saveSettings.setValue("student", "stu.li");
        saveSettings.setValue("student", "stu.xiao");
        saveSettings.endGroup();
    }
    return;
}

void MainWindow::slotProgressBar(qint64 bytesSent, qint64 bytesTotal)
{
    ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(bytesSent);
}

void MainWindow::on_pushButton_clicked()
{
    testHttpClient();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    //:/image/image/desktop.jpeg
//    QPixmap pixmap = QPixmap(":/image/image/test2.jpg").scaled(this->size());
//    QPainter painter(this);
//    painter.drawPixmap(this->rect(), pixmap);
}

void MainWindow::loadStysheet()
{
    //:/image/image/my_coffice.qss
    QFile file(":/theme/Theme/coffee.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QString::fromLatin1(file.readAll());
    qApp->setStyleSheet(styleSheet);

    ui->progressBar->setValue(49);
}

void MainWindow::on_comboBox_currentIndexChanged()
{
    QString fileName = ui->comboBox->currentText();
    //QString fileName = "coffee";
    QFile file(QString(":/theme/Theme/%1.qss").arg(fileName));
    file.open(QFile::ReadOnly);
    QString styleSheet = QString::fromLatin1(file.readAll());
    qApp->setStyleSheet(styleSheet);
}
