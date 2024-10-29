#include "MessageList.h"
#include <QDebug>

MessageList::MessageList(Monero::MessageList * abImpl, QObject *parent)
  : QObject(parent), m_messageListImpl(abImpl)
{
}

quint64 MessageList::send(const QString& chat_addr, const QString& text, quint64 amount, bool unprunable)
{
    uint64_t n = 0;
    if(m_messageListImpl->send(chat_addr.toStdString(), text.toStdString(), amount, unprunable, n))
         return n;
    return quint64(-1);
}

quint64 MessageList::put(const QString& chat, const QString& text, const QString& txid, quint64 height, quint64 timestamp)
{
    uint64_t n = 0;
    if(m_messageListImpl->put(n, chat.toStdString(), text.toStdString(), txid.toStdString(), height, timestamp))
        return n;
    return quint64(-1);
}

bool MessageList::get(
          const QString& chat_addr,
          quint64 n,
          QString& sender_addr,
          QString& text,
          quint64& height,
          quint64& ts,
          QString& txid)
{
    std::string s, t, id; uint64_t H, TS;
    if(m_messageListImpl->get(chat_addr.toStdString(), n, s, t, H, TS, id))
    {
        sender_addr = s.c_str(); text = t.c_str(); txid = id.c_str(); height = H; ts = TS;
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
