#include "MessageList.h"
#include <QDebug>

MessageList::MessageList(Monero::MessageList * abImpl, QObject *parent)
  : QObject(parent), m_messageListImpl(abImpl)
{
}

quint64 MessageList::send(const QString& chat, const QString& text, bool enable_comments, quint64 amount, bool unprunable, const QString& parent)
{
    uint64_t n = 0;
    if(m_messageListImpl->send(chat.toStdString(), text.toStdString(), enable_comments, amount, unprunable, n, parent.toStdString()))
         return n;
    return quint64(-1);
}

quint64 MessageList::put(const QString& chat, const QString& text, bool enable_comments, const QString& txid, quint64 height, quint64 timestamp, const QString& parent)
{
    uint64_t n = 0;
    if(m_messageListImpl->put(n, chat.toStdString(), text.toStdString(), enable_comments, txid.toStdString(), height, timestamp, parent.toStdString()))
        return n;
    return quint64(-1);
}

bool MessageList::get(
          const QString& chat,
          quint64 n,
          QString& sender,
          QString& text,
          bool& enable_comments,
          quint64& height,
          quint64& ts,
          QString& txid)
{
    std::string s, t, id; uint64_t H, TS;
    if(m_messageListImpl->get(chat.toStdString(), n, s, t, enable_comments, H, TS, id))
    {
        sender = s.c_str(); text = t.c_str(); txid = id.c_str(); height = H; ts = TS;
        return true;
    }
    return false;
}

bool MessageList::get(
          const QString& txid,
          QString& sender,
          QString& text,
          bool& enable_comments,
          quint64& height,
          quint64& ts)
{
    std::string s, t; uint64_t H, TS;
    if(m_messageListImpl->get(txid.toStdString(), s, t, enable_comments, H, TS))
    {
        sender = s.c_str(); text = t.c_str(); height = H; ts = TS;
        return true;
    }
    return false;
}

bool MessageList::getParent(const QString& chat, QString& parent)
{
    std::string res;
    if(m_messageListImpl->getParent(chat.toStdString(), res))
    {
        parent = res.c_str();
        return true;
    }
    return false;
}

quint64 MessageList::getCnt(const QString& chat_addr)
{
    return m_messageListImpl->getCnt(chat_addr.toStdString());
}

quint64 MessageList::getUnread(const QString& chat_addr)
{
    return m_messageListImpl->getUnread(chat_addr.toStdString());
}

quint64 MessageList::getLastTime(const QString& chat_addr)
{
    return m_messageListImpl->getLastTime(chat_addr.toStdString());
}
