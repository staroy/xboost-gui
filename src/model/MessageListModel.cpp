#include "MessageListModel.h"
#include <QDebug>
#include <QHash>

#include <wallet/api/wallet2_api.h>

UnsortedMessageListModel::UnsortedMessageListModel(QObject *parent, AddressBook *address, MessageList *message)
    : QAbstractListModel(parent)
    , m_addressBook(address)
    , m_messageList(message)
    , m_isChannel(false)
{
}

int UnsortedMessageListModel::rowCount(const QModelIndex &) const
{
    return m_messageList->getCnt(m_currentAddress);
}

QVariant UnsortedMessageListModel::data(const QModelIndex &index, int role) const
{
    QString text, sender, txid;
    std::string ab, ab_color, ab_background;
    quint64 height; quint64 ts;

    if(m_messageList->get(m_currentAddress, index.row(), sender, text, height, ts, txid))
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
      case MessageFromAddressRole: return sender == m_currentAddress.length() > 0 ? "self" : sender;
      case MessageFromLabelRole: return m_addressBook->getDescription(sender);
      case MessageTextRole: return text;
      case MessageTimeRole: return ts;
      case MessageTimeStrRole: return QDateTime().fromSecsSinceEpoch(ts).toString("dd.MM.yyyy hh:mm");
      case MessageTxidRole: return txid;
      case MessageIndexRole: return index.row();
      case MessageSendByMeRole: return sender == m_selfAddress;
      case MessageIsChannelRole: return m_isChannel;
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

void UnsortedMessageListModel::append(const QString& text, quint64 amount, bool unprunable)
{
    quint64 n = m_messageList->getCnt(m_currentAddress);
    beginInsertRows(QModelIndex(), int(n), int(n));
    quint64 new_n = m_messageList->send(m_currentAddress, text, amount, unprunable);
    if(new_n != quint64(-1))
    {
        endInsertRows();
        QString sender_addr; QString atext; quint64 height; quint64 ts; QString txid;
        m_messageList->get(m_currentAddress, new_n, sender_addr, atext, height, ts, txid);
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
    if(m_currentAddress == chat)
    {
        beginInsertRows(QModelIndex(), int(n), int(n));
        endInsertRows();
    }
}

void UnsortedMessageListModel::setCurrent(const QString& address, bool isChannel)
{
    beginResetModel();
    m_isChannel = isChannel;
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
    roleNames.insert(MessageTimeRole, "timeSecs");
    roleNames.insert(MessageTimeStrRole, "time");
    roleNames.insert(MessageTxidRole, "txid");
    roleNames.insert(MessageIndexRole, "n_index");
    roleNames.insert(MessageSendByMeRole, "sendByMe");
    roleNames.insert(MessageIsChannelRole, "isChannel");
    roleNames.insert(MessageABRole, "Ab");
    roleNames.insert(MessageABColorRole, "AbColor");
    roleNames.insert(MessageABBackgroundRole, "AbBackground");
    return roleNames;
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

void MessageListModel::append(const QString& sms, quint64 amount, bool unprunable)
{
  src_.append(sms, amount, unprunable);
  emit addressBookInvalidate();
}

void MessageListModel::setCurrent(const QString& Address, bool isChannel)
{
  src_.setCurrent(Address, isChannel);
  emit addressBookInvalidate();
}

UnsortedMessageListModel& MessageListModel::source()
{
  return src_;
}

