#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QMessageBox>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <iostream>
#include <fstream>
#include <QFileDialog>
#include <QImage>
#include <QCoreApplication>
#include <QObject>
#include <QTextCodec>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_pb_ok_clicked();//кнопка ок для утверждения IP и порта

    void on_pb_send_clicked();//кнопка отправить

    void on_pb_nick_clicked();//кнопка ввести

    void socketReadyRead();//ф-Ция для считывания

    bool normal_ip(const QString& ipAddress);//ф-ция для проверки корректного IP

    QString getLocalIP();//ф-ция для получения IP

    void save_history_message();//ф-ция для сохранения переписки    

    void on_pb_recovery_clicked();//кнопка для восстановление переписки

    void ban();//бан юзера с IP

    void on_pb_ban_clicked();

    void unban();

    void on_pb_unban_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket *socket;//сокет
    QString nick;//ник
    QHostAddress ban2;
    QHostAddress remote2;
    QHostAddress local;

};
#endif // MAINWINDOW_H
