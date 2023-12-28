#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>


class Message
{
//"yyyy-MM-dd HH:mm:ss"

public:
    typedef enum MessageType
    {
       Heartbeat = 0,UserList = 1,Information = 2
    }MessageType;

    Message();
    Message(MessageType type, QString from,QString to,QString time,QString content);
    Message(MessageType type, int from,int to,QString time,QString content);

    QString toString();
    QString toMessage();
    static Message* valueOf(QString message);

    MessageType getType();
    void setType(MessageType type);
    QString getFrom();
    void setFrom(QString from);
    QString getTo();
    void setTo(QString to);
    QString getTime();
    void setTime(QString time);
    QString getContent();
    void setContent(QString content);

private:
    MessageType type;
    QString from;
    QString to;
    QString time;
    QString content;

};

#endif // MESSAGE_H
