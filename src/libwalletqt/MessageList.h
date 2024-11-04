#ifndef MESSAGELIST_H
#define MESSAGELIST_H

#include <wallet/api/wallet2_api.h>
#include <QMap>
#include <QObject>
#include <QReadWriteLock>
#include <QList>
#include <QDateTime>

class MessageList : public QObject
{
    Q_OBJECT
public:

    Q_INVOKABLE quint64 send(const QString& chat, const QString& text, bool enable_comments, quint64 amount, bool unprunable, const QString& parent);
    Q_INVOKABLE quint64 put(const QString& chat, const QString& text, bool enable_comments, const QString& txid, quint64 height, quint64 timestamp, const QString& parent);
    Q_INVOKABLE bool get(
          const QString& chat_addr,
          quint64 n,
          QString& sender_addr,
          QString& text,
          bool& enable_comments,
          quint64& height,
          quint64& ts,
          QString& txid);
    Q_INVOKABLE bool get(
          const QString& txid,
          QString& sender_addr,
          QString& text,
          bool& enable_comments,
          quint64& height,
          quint64& ts);
    Q_INVOKABLE bool getParent(const QString& chat, QString& parent);
    Q_INVOKABLE quint64 getCnt(const QString& chat);
    Q_INVOKABLE quint64 getUnread(const QString& chat);
    Q_INVOKABLE quint64 getLastTime(const QString& chat);
private:

public slots:

private:
    explicit MessageList(Monero::MessageList * abImpl, QObject *parent);
    friend class Wallet;
    Monero::MessageList * m_messageListImpl;
};

#endif // MESSAGELIST_H
