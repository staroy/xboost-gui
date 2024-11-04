#include "MessageListModel.h"
#include <QDebug>
#include <QHash>

#include <wallet/api/wallet2_api.h>

UnsortedMessageListModel::UnsortedMessageListModel(QObject *parent, AddressBook *address, MessageList *message)
    : QAbstractListModel(parent)
    , m_addressBook(address)
    , m_messageList(message)
    , m_isMultiUser(false)
{
}

int UnsortedMessageListModel::rowCount(const QModelIndex &) const
{
    return m_messageList->getCnt( getCurrentChat() ) + (isSubChat() ? 1 : 0);
}

QVariant UnsortedMessageListModel::data(const QModelIndex &index, int role) const
{
    QString text, sender, txid;
    std::string ab, ab_color, ab_background;
    quint64 height; quint64 ts;
    bool enable_comments;

    bool issc = isSubChat();
    bool r = false;
    bool isTitle = issc && index.row()==0;
  

    if(isTitle)
    {
        txid = getCurrentChat();
        r = m_messageList->get(txid, sender, text, enable_comments, height, ts);
    }
    else
    {
        r = m_messageList->get(getCurrentChat(), index.row() - (issc ? 1 : 0), sender, text, enable_comments, height, ts, txid);
    }

    if(r)
    {
      auto getAddressRow = [&](const QString& addr) {
         for(size_t n=0; n<m_addressBook->count() && ab.empty(); n++) {
             m_addressBook->getRow(n, [&](const Monero::AddressBookRow &row) {
                 if(row.getAddress() == addr.toStdString()) {
                     ab = row.getAb(); ab_color = row.getAbColor(); ab_background = row.getAbBackground();
                 }
             });
         }
      };

      switch(role)
      {
      case MessageFromAddressRole: return sender;
      case MessageFromLabelRole: return m_addressBook->getDescription(sender);
      case MessageTextRole: return text;
      case MessageEnableCommentsRole: return enable_comments;
      case MessageTimeRole: return ts;
      case MessageTimeStrRole: return QDateTime().fromSecsSinceEpoch(ts).toString("dd.MM.yyyy hh:mm");
      case MessageTxidRole: return txid;
      case MessageIndexRole: return index.row();
      case MessageSendByMeRole: return sender == m_selfAddress;
      case MessageIsMultiUserRole: return m_isMultiUser || m_currentSubChat.length() > 0;
      case MessageIsTitleRole: return isTitle;
      case MessageABRole:
      case MessageABColorRole:
      case MessageABBackgroundRole:
          getAddressRow(sender);
          switch(role)
          {
          case MessageABRole: return ab.c_str();
          case MessageABColorRole: return ab_color.c_str();
          case MessageABBackgroundRole: return ab_background.c_str();
          }
      }
    }
    return QVariant();
}

void UnsortedMessageListModel::append(const QString& text, bool enable_comments, quint64 amount, bool unprunable)
{
    quint64 n = m_messageList->getCnt(getCurrentChat()) + (isSubChat() ? 1 : 0);
    beginInsertRows(QModelIndex(), int(n), int(n));
    quint64 new_n = m_messageList->send(getCurrentChat(), text, enable_comments && !isSubChat(), amount, unprunable, getParentChat());
    if(new_n != quint64(-1))
    {
        endInsertRows();
        QString sender_addr; QString atext; bool _enable_comments; quint64 height; quint64 ts; QString txid;
        m_messageList->get(getCurrentChat(), new_n, sender_addr, atext, _enable_comments, height, ts, txid);
    }
    else
    {
        endInsertRows();
    }
}

void UnsortedMessageListModel::received(const QString &chat, quint64 n, const QString &txid)
{
}

void UnsortedMessageListModel::removed(const QString &chat, quint64 n, const QString &txid)
{
    if(getCurrentChat() == chat)
    {
        beginInsertRows(QModelIndex(), int(n), int(n));
        endInsertRows();
    }
}

bool UnsortedMessageListModel::isSubChat() const
{
    return m_currentSubChat.length() > 0;
}

void UnsortedMessageListModel::setSubChat(const QString& txid)
{
    beginResetModel();
    m_currentSubChat = txid;
    endResetModel();
}

void UnsortedMessageListModel::setToParent()
{
    beginResetModel();
    m_currentSubChat = "";
    endResetModel();
}

void UnsortedMessageListModel::setCurrent(const QString& address, bool isMultiUser)
{
    beginResetModel();
    m_isMultiUser = isMultiUser;
    m_currentAddress = address;
    endResetModel();
}

void UnsortedMessageListModel::setSelf(const QString& address)
{
    m_selfAddress = address;
}

QHash<int, QByteArray> UnsortedMessageListModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
    roleNames.insert(MessageFromAddressRole, "address");
    roleNames.insert(MessageFromLabelRole, "desc");
    roleNames.insert(MessageTextRole, "text");
    roleNames.insert(MessageEnableCommentsRole, "isEnableComments");
    roleNames.insert(MessageTimeRole, "timeSecs");
    roleNames.insert(MessageTimeStrRole, "time");
    roleNames.insert(MessageTxidRole, "txid");
    roleNames.insert(MessageIndexRole, "n_index");
    roleNames.insert(MessageSendByMeRole, "sendByMe");
    roleNames.insert(MessageIsMultiUserRole, "isMultiUser");
    roleNames.insert(MessageIsTitleRole, "isTitle");
    roleNames.insert(MessageABRole, "Ab");
    roleNames.insert(MessageABColorRole, "AbColor");
    roleNames.insert(MessageABBackgroundRole, "AbBackground");
    return roleNames;
}

const QString& UnsortedMessageListModel::getCurrentChat() const
{
    return m_currentSubChat.length() == 0 ? m_currentAddress : m_currentSubChat;
}

const QString& UnsortedMessageListModel::getParentChat() const
{
    static QString null_str;
    return m_currentSubChat.length() > 0 ? m_currentAddress : null_str;
}

MessageListModel::MessageListModel(QObject *parent, AddressBook *address, MessageList *message)
  : QSortFilterProxyModel(parent)
  , src_(parent, address, message)
{
  setSortOrder(false);
  setSourceModel(&src_);
  setSortRole(UnsortedMessageListModel::MessageIndexRole);
  setFilterRole(UnsortedMessageListModel::MessageTextRole);
}

MessageListModel::~MessageListModel()
{
}

void MessageListModel::msgReceived(const QString &chat, quint64 n, const QString &txid)
{
  src_.received(chat, n, txid);
  if(src_.m_currentAddress == chat)
  {
    beginResetModel();
    dataChanged(index(n, 0), index(n, 0));
    endResetModel();
  }
  emit addressBookInvalidate();
}

void MessageListModel::msgRemoved(const QString &chat, quint64 n, const QString &txid)
{
  src_.removed(chat, n, txid);
  emit addressBookInvalidate();
}

void MessageListModel::setFilterString(QString string)
{
  this->setFilterCaseSensitivity(Qt::CaseInsensitive);
  this->setFilterFixedString(string);
}

void MessageListModel::setSortOrder(bool checked)
{
  if(checked)
  {
    this->sort(0, Qt::DescendingOrder);
  }
  else
  {
    this->sort(0, Qt::AscendingOrder);
  }
}

void MessageListModel::append(const QString& sms, bool enable_comments, quint64 amount, bool unprunable)
{
  src_.append(sms, enable_comments, amount, unprunable);
  emit addressBookInvalidate();
}

void MessageListModel::setCurrent(const QString& Address, bool isMultiUser)
{
  src_.setCurrent(Address, isMultiUser);
  emit addressBookInvalidate();
}

bool MessageListModel::isSubChat()
{
  return src_.isSubChat();
}

void MessageListModel::setSubChat(const QString& txid)
{
  src_.setSubChat(txid);
  invalidate();
}

void MessageListModel::setToParent()
{
  src_.setToParent();
  invalidate();
}

UnsortedMessageListModel& MessageListModel::source()
{
  return src_;
}

