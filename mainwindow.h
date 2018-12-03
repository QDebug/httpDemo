#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "httpclient.h"
#include <QNetworkProxy>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void testHttpClient();
    void paintEvent(QPaintEvent *e);

    void loadStysheet();

public slots:
    void slotProgressBar(qint64 bytesSent, qint64 bytesTotal);
    void on_pushButton_clicked();
    void on_comboBox_currentIndexChanged();

private:
    Ui::MainWindow *ui;
    HttpClient *m_httpClient;
};

#endif // MAINWINDOW_H
