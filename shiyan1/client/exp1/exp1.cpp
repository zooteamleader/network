#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include "exp1/message.h"
#include <QProcess>
#include <QDir>

void MainWindow::initExp1()
{
    exp1_socket = new QTcpSocket();
    exp1_ip = "127.0.0.1";
    exp1_post = 60001;

    //槽函数
    connect(this->exp1_socket, &QTcpSocket::readyRead, this, &MainWindow::exp1_Socket_Read_Data);
    connect(this->exp1_socket, &QTcpSocket::disconnected, this, &MainWindow::exp1_Socket_Disconnected);

    //更新系统时间
    QTimer* timer_System = new QTimer(this);
    timer_System->start(500);    //0.5s更新一下系统时间
    connect(timer_System, &QTimer::timeout, [=](){
        ui->label_exp1_time->setText(QDateTime::currentDateTime().toString("yyyy年MM月dd日 hh:mm:ss"));
    });

}

void MainWindow::on_pushButton_exp1_login_clicked()
{
    if(ui->lineEdit_exp1_username->text().isEmpty())
    {
        QMessageBox::critical(this,"登录错误","请输入用户名");
        return ;
    }

    if(!exp1_login)
    {
        //登陆状态
        ui->pushButton_exp1_login->setText("正在登陆中......");
        exp1_socket->abort(); //清楚已有的连接
        exp1_socket->connectToHost(exp1_ip,exp1_post);

        //等待连接成功
        if(!exp1_socket->waitForConnected(2000))
        {
            ui->pushButton_exp1_login->setText("登录");
            qDebug() << "Connection failed!";
            QMessageBox::critical(this,"连接错误","请开启服务器");
            delete exp1_socket;
            return;
        }
        qDebug() << "Connect successfully!";
        qDebug() << exp1_socket->localPort();
        ui->lineEdit_exp1_username->setText(QString::number(exp1_socket->localPort()));
        exp1_login = true;
        exp1_person = exp1_socket->localPort();
        ui->pushButton_exp1_login->setText("注销");
        //        ui->lineEdit_exp1_username->setEnabled(false);
    }
    else
    {
        //注销状态
        exp1_socket->disconnectFromHost();
        ui->lineEdit_exp1_username->setText("请点击登录");


        exp1_login = false;
        ui->pushButton_exp1_login->setText("登录");
        //        ui->lineEdit_exp1_username->setEnabled(true);
    }
}

void MainWindow::on_pushButton_exp1_reset_clicked()
{
    this->exp1_count_accept = 0;
    this->exp1_count_send = 0;
    ui->label_exp1_count->setText(QString("%1/%2").arg(exp1_count_accept).arg(exp1_count_send));
}

void MainWindow::on_pushButton_exp1_clear_clicked()
{
    ui->textBrowser_exp1_chat->setText("");
    ui->textEdit_exp1_send->setText("");
}

void MainWindow::on_pushButton_exp1_send_clicked()
{
    if(!exp1_login)
    {
        QMessageBox::critical(this,"错误","请先登录");
        return;
    }
    if(exp1_person == 0)
    {
        QMessageBox::critical(this,"错误","请先选择一个用户");
        return;
    }


    Message* t= new Message(Message::Information,exp1_socket->localPort(),exp1_person,QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"),ui->textEdit_exp1_send->toPlainText());
    ui->textEdit_exp1_send->setText("");
    exp1_socket->write(t->toMessage().toUtf8());
    exp1_socket->flush();

    this->exp1_count_send += t->getContent().length();
    ui->label_exp1_count->setText(QString("%1/%2").arg(exp1_count_accept).arg(exp1_count_send));

    QString show ="%1 [SEND:%2] :\n";
    show = show.arg(t->getTime()).arg(t->getTo());
    show += t->getContent();
    ui->textBrowser_exp1_chat->append("\n"+show);

    delete t;

}

void MainWindow::exp1_Socket_Read_Data()
{
    QByteArray buffer;
    buffer = exp1_socket->readAll();
    if(buffer.isEmpty())
    {
        return ;
    }

    QString message = QString(buffer);
    Message* t= Message::valueOf(message);

    if(t->getType()==Message::Heartbeat)
    {
        delete t;
        return;
    }
    else if(t->getType()==Message::UserList)
    {
        ui->listWidget_exp1_userList->clear();
        ui->listWidget_exp1_userList->addItems(t->getContent().split("\n"));
    }
    else if(t->getType()==Message::Information)
    {
        this->exp1_count_accept += t->getContent().length();
        ui->label_exp1_count->setText(QString("%1/%2").arg(exp1_count_accept).arg(exp1_count_send));

        QString show ="%1 [FROM:%2] :\n";
        show = show.arg(t->getTime()).arg(t->getFrom());
        show += t->getContent();
        ui->textBrowser_exp1_chat->append("\n"+show);
        ui->textBrowser_exp1_chat->moveCursor(QTextCursor::End);
    }

    delete t;

    //    ui->textBrowser_exp1_chat->append("\n"+buffer);
    //    ui->textBrowser_exp1_chat->moveCursor(QTextCursor::End);
}

void MainWindow::exp1_Socket_Disconnected()
{
    ui->lineEdit_exp1_username->setText("请点击登录");
    exp1_login = false;
    ui->pushButton_exp1_login->setText("登录");
    exp1_count_accept = 0;
    exp1_count_send = 0;

    exp1_person = 0;
    exp1_person_index = 0;
    ui->label_exp1_person->setText("ronglin");
}

void MainWindow::on_listWidget_exp1_userList_clicked(const QModelIndex &index)
{
    this->exp1_person = ui->listWidget_exp1_userList->item(index.row())->text().toInt();
    this->exp1_person_index = index.row();
    ui->label_exp1_person->setText(QString::number(this->exp1_person));
}

void MainWindow::on_pushButton_exp1_startExe_clicked()
{
    QProcess* process = new QProcess(this);
    process->startDetached("F:\\Qt\\Release\\Network\\Network.exe");
}

