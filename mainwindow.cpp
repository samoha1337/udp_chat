#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{//конструктор
    ui->setupUi(this);
    ui->pb_ok->setEnabled(false);
    ui->pb_send->setEnabled(false);
    ui->pb_photo->setEnabled(false);
    setWindowTitle("UDP-Chat");
    //ui->lineEdit_local->setText(getLocalIP());
    ui->Chat_edit->setReadOnly(true);
    setFixedSize(size());
    ui->pb_photo->hide();
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(save_history_message()));

}
//деструктор
MainWindow::~MainWindow()
{
    delete ui;
}

//кнопка ок
void MainWindow::on_pb_ok_clicked()
{

    if (!normal_ip(ui->lineEdit_local->text()))
    {
        QMessageBox::warning(this,"Ошибка!","Ваш IP некорректный!");
    }
    if (!normal_ip(ui->lineEdit_remote->text()))
    {
        QMessageBox::warning(this,"Ошибка!","Некорректный IP удаленного пользователя!");
    }
    socket = new QUdpSocket(this);
    if (socket!=nullptr)
    {
        ui->pb_send->setEnabled(true);
        ui->pb_photo->setEnabled(true);
    }
    else
    {
        QMessageBox::warning(this,"Ошибка!","Соединение не установлено!");
    }
    connect(socket,SIGNAL(readyRead()),this,SLOT(socketReadyRead()));
    QHostAddress localAddress(ui->lineEdit_local->text());
    local=localAddress;
    socket->bind(localAddress,((ushort)ui->spinBox_client_local->value()));
}
//кнопка отправить
void MainWindow::on_pb_send_clicked()
{
    if (!nick.isEmpty())
    {        
        QString text = ui->input_edit->toPlainText();
        if (ui->lineEdit_ban->text()!=nick)
        {
            ui->Chat_edit->appendHtml("<p style=\"color:blue\">" + nick + ": </b>"+ text + "</p>");
        }

        QString nick_2 = nick + ": " + text;        
        QByteArray buffer = nick_2.toUtf8();
        QHostAddress remoteAddress(ui->lineEdit_remote->text());
        ushort remotePort = (ushort)ui->spinBox_client_remote->value();
        //Отправка данных
        socket->writeDatagram(buffer,remoteAddress,remotePort);

        ui->Chat_edit->document()->toPlainText();       
    }
    else
    {
        QMessageBox::information(this,"Внимание!","Проверьте и подтвердите ваш nickname");
    }
    ui->input_edit->clear();
}
//кнопка утвердить ник
void MainWindow::on_pb_nick_clicked()
{    
    if (!ui -> lineEdit_nick->text().isEmpty())
    {

        nick = ui -> lineEdit_nick ->text();        
        ui->pb_ok->setEnabled(true);
    }
    else
    {
        QMessageBox::information(this,"Внимание","Введите ник");
    }
}
//ф-ция для считывания сообщения
void MainWindow::socketReadyRead()
{    
    while(socket->hasPendingDatagrams())
    {
        QByteArray buffer;
        buffer.resize(socket->pendingDatagramSize());
        QHostAddress remoteAdress;
        remote2=remoteAdress;        
        ushort remotePort;
        //чтение датаграммы        
        socket->readDatagram(buffer.data(), buffer.size(), &remoteAdress,&remotePort);
        QString text = QString::fromUtf8(buffer);
        ui->Chat_edit->appendHtml("<p style=\"color:red\">"+ text + "</p>");
    }

}
//ф-ция для проверки IP
bool MainWindow::normal_ip(const QString &ipAddress)
{
    // Проверяем, соответствует ли IP-адрес шаблону X.X.X.X, где X - число от 0 до 255
    QRegExp ipPattern("^([0-9]{1,3}\\.){3}[0-9]{1,3}$");
    if (ipPattern.exactMatch(ipAddress))
    {
        // Разделяем IP-адрес на компоненты
        QStringList ipComponents = ipAddress.split(".");
        foreach (QString ipComponent, ipComponents)
        {
            // Проверяем, что каждая компонента является числом от 0 до 255
            int num = ipComponent.toInt();
            if (num < 0 || num > 255)
            {
                return false;
            }
        }
        return true;
    }
    return false;
}
//получения локального IP
QString MainWindow::getLocalIP()
{
    QString localIp;

    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

    foreach (const QNetworkInterface &interface, interfaces)
    {
        // Проверяем, что сетевой интерфейс имеет активное подключение и не является петлевым интерфейсом
        if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
                !interface.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            QList<QNetworkAddressEntry> entries = interface.addressEntries();

            foreach (const QNetworkAddressEntry &entry, entries)
            {
                // Проверяем, что IP-адрес является IPv4-адресом и не является недействительным адресом (localhost, многоадресная рассылка и т.п.)
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol && entry.ip() != QHostAddress::Null && !entry.ip().isInSubnet(QHostAddress("127.0.0.0"), 8))
                {
                    localIp = entry.ip().toString();
                    break;
                }
            }
        }
        // Если найден локальный IP-адрес, прерываем цикл
        if (!localIp.isEmpty())
            break;
    }
    return localIp;
}
//ф-ция сохранения переписки
void MainWindow::save_history_message()
{    
    QString filename = "C:\\Qt\\project\\project_2_4_1\\Log.txt";  // имя файла
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        file.write(ui->Chat_edit->toPlainText().toUtf8()); //пишем в файл, используя цепочки преобразований)
    }
    file.close();
}

//кнопка восстановить
void MainWindow::on_pb_recovery_clicked()
{
    QFile file("C:\\Qt\\project\\project_2_4_1\\Log.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        in.setCodec("UTF-8");
        QString text = in.readAll();
        QString text_2 = text;
        if (!text_2.isEmpty())
        {
            QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
            ui->Chat_edit->setPlainText(text);
            file.close();
        }
        else
        {
            QMessageBox::information(this,"Ошибка!","Предыдущих переписок не обнаружено. Будьте первым!");
            ui->pb_recovery->hide();
        }
    }
    else
    {
        QMessageBox::warning(this,"Ошибка!","Не удается открыть файл.");
    }
}
//ф-ция для бана
void MainWindow::ban()
{
    if (!ui->lineEdit_ban->text().isEmpty())
    {
        QHostAddress ban(ui->lineEdit_ban->text());
        ban2=ban;
        QString ban3 = ui->lineEdit_ban->text();
        if (!normal_ip(ban3))
        {
           QMessageBox::warning(this,"ВНИМАНИЕ!","IP введен не правильно, проверьте!");
        }
        else
        {
            if (!ban2.isNull())
            {
                socket->close();
            }
            else
            {
                QMessageBox::warning(this,"ВНИМАНИЕ!","ОН НЕ В БАНЕ!");
            }
        }

   }
   else
   {
        QMessageBox::warning(this,"ВНИМАНИЕ!","ВЫ ХОТИТЕ ЗАБАНИТЬ ПУСТОТУ! ВВЕДИТЕ IP!");
        return;
   }

}

void MainWindow::on_pb_ban_clicked()
{
    ban();
}

void MainWindow::unban()
{
    if (!ui->lineEdit_unban->text().isEmpty())
    {
        QHostAddress ban(ui->lineEdit_unban->text());
        ban2=ban;
        QString ban3 = ui->lineEdit_unban->text();
        if (!normal_ip(ban3))
        {
           QMessageBox::warning(this,"ВНИМАНИЕ!","IP введен не правильно, проверьте!");
        }
        else
        {
            if (!ban2.isNull())
            {
               ui->pb_ok->click();
            }
            else
            {
                QMessageBox::warning(this,"ВНИМАНИЕ!","ОН НЕ В БАНЕ!");
            }
        }

   }
   else
   {
        QMessageBox::warning(this,"ВНИМАНИЕ!","ВЫ ХОТИТЕ ЗАБАНИТЬ ПУСТОТУ! ВВЕДИТЕ IP!");
        return;
   }

}

void MainWindow::on_pb_unban_clicked()
{
    unban();
}

