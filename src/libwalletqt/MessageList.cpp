#include "MessageList.h"
#include <QDebug>

MessageList::MessageList(Monero::MessageList * abImpl, QObject *parent)
  : QObject(parent), m_messageListImpl(abImpl)
{
}

quint64 MessageList::send(
          const QString& chat,
          const QString& text,
          const QString& description,
          const QString& ab,
          bool enable_comments,
          bool is_anon,
          quint64 amount,
          bool unprunable,
          const QString& parent)
{
    uint64_t n = 0;
    if(m_messageListImpl->send(chat.toStdString(),
                               text.toStdString(),
                               description.toStdString(),
                               ab.toStdString(),
                               enable_comments,
                               is_anon,
                               amount,
                               unprunable,
                               n,
                               parent.toStdString()))
    {
         return n;
    }
    return quint64(-1);
}

quint64 MessageList::put(
          const QString& chat,
          const QString& text,
          const QString& description,
          const QString& ab,
          bool enable_comments,
          const QString& txid,
          quint64 height,
          quint64 timestamp,
          const QString& parent)
{
    uint64_t n = 0;
    if(m_messageListImpl->put(n,
                              chat.toStdString(),
                              text.toStdString(),
                              description.toStdString(),
                              ab.toStdString(),
                              enable_comments,
                              txid.toStdString(),
                              height,
                              timestamp,
                              parent.toStdString()))
    {
        return n;
    }
    return quint64(-1);
}

bool MessageList::get(
          const QString& chat,
          quint64 n,
          QString& sender,
          quint64& sender_rowid,
          QString& text,
          bool& enable_comments,
          quint64& height,
          quint64& ts,
          QString& txid,
          QString& tags)
{
    std::string s, t, desc, ab, id, tg; uint64_t s_id, H, TS;
    if(m_messageListImpl->get(chat.toStdString(), n, s, s_id, t, desc, ab, enable_comments, H, TS, id, tg))
    {
        sender = s.c_str(); sender_rowid = s_id; text = t.c_str(); txid = id.c_str(); height = H; ts = TS; tags = tg.c_str();
        return true;
    }
    return false;
}

bool MessageList::get(
          const QString& txid,
          QString& sender,
          quint64& sender_rowid,
          QString& text,
          bool& enable_comments,
          quint64& height,
          quint64& ts,
          QString& tags)
{
    std::string s, t, desc, ab, tg; uint64_t s_id, H, TS;
    if(m_messageListImpl->get(txid.toStdString(), s, s_id, t, desc, ab, enable_comments, H, TS, tg))
    {
        sender = s.c_str(); sender_rowid = s_id; text = t.c_str(); height = H; ts = TS; tags = tg.c_str();
        return true;
    }
    return false;
}

bool MessageList::del(const QString& chat_addr, quint64 n)
{
    return m_messageListImpl->del(chat_addr.toStdString(), n);
}

bool MessageList::undel(const QString& chat_addr, quint64 n)
{
    return m_messageListImpl->undel(chat_addr.toStdString(), n);
}

void MessageList::setTags(const QString& chat_addr, quint64 n, const QString& tags)
{
    m_messageListImpl->setTags(chat_addr.toStdString(), n, tags.toStdString());
}

void MessageList::setTags(const QString& txid, const QString& tags)
{
    m_messageListImpl->setTags(txid.toStdString(), tags.toStdString());
}

bool MessageList::getTags(const QString& chat_addr, quint64 n, QString& tags)
{
    std::string tg;
    if(m_messageListImpl->getTags(chat_addr.toStdString(), n, tg))
    {
        tags = tg.c_str();
        return true;
    }
    return false;
}

bool MessageList::getTags(const QString& txid, QString& tags)
{
    std::string tg;
    if(m_messageListImpl->getTags(txid.toStdString(), tg))
    {
        tags = tg.c_str();
        return true;
    }
    return false;
}

bool MessageList::addTag(const QString& chat_addr, quint64 n, const QString& tag)
{
    return m_messageListImpl->addTag(chat_addr.toStdString(), n, tag.toStdString());
}

bool MessageList::delTag(const QString& chat_addr, quint64 n, const QString& tag)
{
    return m_messageListImpl->delTag(chat_addr.toStdString(), n, tag.toStdString());
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
