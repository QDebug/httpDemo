#include "httpclient.h"
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QEventLoop>
#include <QDebug>
#include <QUrl>
#include <QMutexLocker>
#include <QMutex>
#include <QFile>
#include <QTextCodec>
#include <QNetworkCookie>
#include <QNetworkCookieJar>

QMutex HttpClient::s_fileMutex;

HttpClient::HttpClient(QObject *parent)
    : QObject(parent)
    , m_networkAccessManager(new QNetworkAccessManager(this))
    , m_eventLoop(new QEventLoop(this))
    , m_defaultCharset("utf-8")
{
    //设置本地代理，方便调试
   // m_networkAccessManager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, "127.0.0.1", 8888));

    //设置user-agent信息
    m_commonHeaders.insert("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)");
}

HttpClient::~HttpClient()
{
}

QNetworkAccessManager *HttpClient::networkAccessManager() const
{
    return m_networkAccessManager;
}

void HttpClient::setNetworkAccessManager(QNetworkAccessManager *networkAccessManager)
{
    m_networkAccessManager = networkAccessManager;
}


bool HttpClient::get(QNetworkRequest request)
{
    QNetworkReply *reply;
    bool success = false;
    request.setPriority(QNetworkRequest::HighPriority);

    //向networkRequest里面添加公用的头
    for (QMap<QByteArray, QByteArray>::const_iterator i = m_commonHeaders.constBegin(); i != m_commonHeaders.constEnd(); ++i)
    {
        if (!request.hasRawHeader(i.key()))
        {
            request.setRawHeader(i.key(), i.value());
        }
    }

    //输出调试信息
    qDebug() << "GET: " + request.url().toEncoded();
    qDebug() << QString("Send Headers: ");
    QList<QByteArray> headerList = request.rawHeaderList();
    foreach(QByteArray header, headerList)
        qDebug() << header + ":" + request.rawHeader(header);
    QList<QNetworkCookie> cookieList = m_networkAccessManager->cookieJar()->cookiesForUrl(request.url());
    qDebug() << "send cookies :";
    foreach (QNetworkCookie cookie, cookieList)
        qDebug() << cookie.name() << "=" << cookie.value();

    reply = m_networkAccessManager->get(request);
    processReply(reply);

    if (m_error == QNetworkReply::NoError)
        success = true;
    else if (m_error == QNetworkReply::TimeoutError)
        success = false;

    return success;
}

bool HttpClient::post(QNetworkRequest request, const QByteArray &data)
{
    QNetworkReply *reply;
    bool success = false;
    request.setPriority(QNetworkRequest::HighPriority);

    //向networkRequest里面添加公用的头
    for (QMap<QByteArray, QByteArray>::const_iterator i = m_commonHeaders.constBegin(); i != m_commonHeaders.constEnd(); ++i)
    {
        if (!request.hasRawHeader(i.key()))
        {
            request.setRawHeader(i.key(), i.value());
        }
    }

    //输出调试信息
    qDebug() << "POST " + request.url().toEncoded();
    qDebug() << "Content: " + data;
    qDebug() << QString("Send Headers:");
    QList<QByteArray> headerList = request.rawHeaderList();
    foreach (QByteArray header, headerList)
          qDebug() << header + ": " + request.rawHeader(header);
    QList<QNetworkCookie> cookieList = m_networkAccessManager->cookieJar()->cookiesForUrl(request.url());
    qDebug() << "send cookies :";
    foreach (QNetworkCookie cookie, cookieList)
        qDebug() << cookie.name() << "=" << cookie.value();

    reply = m_networkAccessManager->post(request, data);
    processReply(reply);

    if (m_error == QNetworkReply::NoError)
        success = true;
    else if (m_error == QNetworkReply::TimeoutError)
        success = false;
    else
        success = false;

    return success;
}

bool HttpClient::post(QNetworkRequest request, QHttpMultiPart *multiPart)
{
    QNetworkReply *reply;
    bool success = false;
    request.setPriority(QNetworkRequest::HighPriority);

    //向networkRequest里面添加公用的头
    for (QMap<QByteArray, QByteArray>::const_iterator i = m_commonHeaders.constBegin(); i != m_commonHeaders.constEnd(); ++i)
    {
        if (!request.hasRawHeader(i.key()))
        {
            request.setRawHeader(i.key(), i.value());
        }
    }

    //输出调试信息
    qDebug() << "POST " + request.url().toEncoded();
    qDebug() << "Content: HttpMultiPart";
    qDebug() << QString("Send Headers:");
    QList<QByteArray> headerList = request.rawHeaderList();
    foreach (QByteArray header, headerList)
          qDebug() << header + ": " + request.rawHeader(header);
    QList<QNetworkCookie> cookieList = m_networkAccessManager->cookieJar()->cookiesForUrl(request.url());
    qDebug() << "send cookies :";
    foreach (QNetworkCookie cookie, cookieList)
        qDebug() << cookie.name() << "=" << cookie.value();


    reply = m_networkAccessManager->post(request, multiPart);
    processReply(reply);

    if (m_error == QNetworkReply::NoError)
        success = true;
    else if (m_error == QNetworkReply::TimeoutError)
        success = false;
    else
        success = false;

    return success;
}

void HttpClient::processReply(QNetworkReply *reply)
{
    //下载进度显示
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(slotDownLoadProcess(qint64,qint64)));

    //启用事件循环等待reply返回数据之后再进行下一步操作
    connect(reply, SIGNAL(finished()), m_eventLoop, SLOT(quit()));
    m_eventLoop->exec();

    //所有状态重置
    m_httpStatusCode = 0;
    m_data.clear();
    m_text.clear();

    m_httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    m_error = reply->error();
    m_errorString = reply->errorString();

    m_data = reply->readAll();

    //解决读取到的中文字符乱码问题
    QTextCodec *codec = 0;
    if(!codec)
        codec = QTextCodec::codecForHtml(m_data, QTextCodec::codecForName(m_defaultCharset));
    if(codec)
        m_text = codec->toUnicode(m_data);

    //输出调试信息
    qDebug() << QString("Status Code: %1").arg(m_httpStatusCode);
    qDebug() << QString("Error Code: %1").arg(m_error);
    qDebug() << QString("Error String: %1").arg(m_errorString);
    //qDebug() << QString("text: %1").arg(m_text);

    delete reply;
}

QByteArray HttpClient::data() const
{
    return m_data;
}

QString HttpClient::text() const
{
    return m_text;
}

void HttpClient::slotDownLoadProcess(qint64 bytesSent, qint64 bytesTotal)
{
    emit signalDownLoadProcess(bytesSent, bytesTotal);

    qDebug() << QString("download process:%1%").arg(double(bytesSent) / bytesTotal * 100);
    qDebug() << "bytesSend: " << bytesSent << ":" << "bytesTotal:" << bytesTotal;
}

bool HttpClient::writeFile(const QString fileName, const QByteArray &data)
{
    //线程安全的读写
    QMutexLocker locker(&s_fileMutex);

    QFile file(fileName);

    //二进制方式打开，只写入
    if (!file.open(QIODevice::WriteOnly))
        return false;
    //判断写入文件是否成功
    if (-1 == file.write(data))
        return false;

    return true;
}

void HttpClient::insertCommonHeader(const QByteArray &name, const QByteArray &value)
{
    m_commonHeaders.insert(name, value);
}

QMap<QByteArray, QByteArray> HttpClient::commonHeaders() const
{
    return m_commonHeaders;
}

void HttpClient::setCommonHeaders(const QMap<QByteArray, QByteArray> &commonHeaders)
{
    m_commonHeaders = commonHeaders;
}

void HttpClient::setDefaultCharset(const QByteArray &defaultCharset)
{
    m_defaultCharset = defaultCharset;
}

QByteArray HttpClient::getDefaultCharset() const
{
    return m_defaultCharset;
}

QByteArray HttpClient::buildQuery(const QVariantMap &postMap, const char *charset)
{
    QByteArray query;
    QTextCodec *codec = QTextCodec::codecForName(charset);
    QVariantMap::ConstIterator iter = postMap.constBegin();
    while (iter != postMap.constEnd())
    {
        QByteArray key = codec->fromUnicode(iter.key()).toPercentEncoding();
        query += key;
        query += '=';
        QByteArray value = codec->fromUnicode(iter.value().toString()).toPercentEncoding();
        query += value;
        query += '&';
        ++iter;
    }

    if (!query.isEmpty())
        query.chop(1); //移除最后添加的'&'符号

    return query;
}
