#include "message.h"

Message::Message()
{
    this->content="";
    this->time="";
    this->to="";
    this->from="";
    this->type=Message::Heartbeat;

}

Message::Message(MessageType type, int from, int to, QString time, QString content)
{
    this->type = type;
    this->from = QString::number(from);
    this->to = QString::number(to);
    this->time = time;
    this->content = content;

}

Message::Message(MessageType type, QString from, QString to, QString time, QString content)
{
    this->type = type;
    this->from = from;
    this->to = to;
    this->time = time;
    this->content = content;
}

QString Message::toString()
{
    QString t ="TYPE: %1\nFROM: %2\nTO: %3\nTIME: %4\nCONTENT: %5";
    t = t.arg(this->type).arg(this->from).arg(this->to).arg(time).arg(this->content);
    return t;
}

QString Message::toMessage()
{
    QString t = "%1\n%2\n%3\n%4\n%5";
    t = t.arg("Information").arg(this->from).arg(this->to).arg(time).arg(this->content);
    return t;
}

Message* Message::valueOf(QString message)
{
    QStringList result = message.split("\n");
    MessageType type;
    if(result.at(0) == "UserList")
        type = MessageType::UserList;
    else if(result.at(0) == "Information")
        type = MessageType::Information;
    else if(result.at(0) == "Heartbeat")
        type = MessageType::Heartbeat;

    QString content;
    for(int i=4;i<result.size();++i)
    {
        content += result.at(i);
        if (i != result.size()-1){
            content += '\n';
        }
    }

    return new Message(type,result.at(1),result.at(2),result.at(3),content);
}


Message::MessageType Message::getType() {return this->type;}
void Message::setType(MessageType type) {this->type = type;}

QString Message::getFrom() {return this->from;}
void Message::setFrom(QString from) {this->from =from;}

QString Message::getTo() {return this->to;}
void Message::setTo(QString to){this->to =to;}

QString Message::getTime(){return this->time;}
void Message::setTime(QString time){this->to =time;}

QString Message::getContent(){return this->content;}
void Message::setContent(QString content){this->content = content;}





