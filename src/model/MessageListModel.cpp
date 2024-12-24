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
    return m_messageList->getCnt( getCurrentChat() );
}

QVariant UnsortedMessageListModel::data(const QModelIndex &index, int role) const
{
    QString text, sender, txid, tags; quint64 sender_id;
    std::string ab, ab_color, ab_background;
    quint64 height; quint64 ts;
    bool enable_comments;

    if(role == MessageTagsRole)
    {
        if(!m_messageList->getTags(getCurrentChat(), index.row(), tags))
            return QString();
        return tags;
    }

    if(role == MessageIsDeletedRole)
    {
        if(!m_messageList->getTags(getCurrentChat(), index.row(), tags))
            return false;
        return tags.contains(Monero::TAG_DEL);
    }

    if(m_messageList->get(getCurrentChat(), index.row(), sender, sender_id, text, enable_comments, height, ts, txid, tags))
    {
      auto getAddressRow = [&](const QString& addr) {
         for(size_t n=0; n<m_addressBook->count() && ab.empty(); n++) {
             m_addressBook->getRow(n, [&](const Monero::AddressBookRow &row) {
                 if(row.getAddress() == addr.toStdString()) {
                     ab = row.getShortName(); ab_color = row.getShortNameColor(); ab_background = row.getShortNameBackground();
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
      case MessageIsTitleRole: return isSubChat() && 0 == index.row();
      }
    }
    return QVariant();
}

void UnsortedMessageListModel::append(const QString& text, bool enable_comments, bool is_anon, quint64 amount, bool unprunable)
{
    quint64 n = m_messageList->getCnt(getCurrentChat());
    beginInsertRows(QModelIndex(), int(n), int(n));
    quint64 new_n = m_messageList->send(getCurrentChat(), text, m_myDescription, m_myAb, enable_comments && !isSubChat(), is_anon, amount, unprunable, getParentChat());
    endInsertRows();

    if(new_n != quint64(-1))
    {
        QString sender_addr; QString atext; bool _enable_comments; quint64 sender_id; quint64 height; quint64 ts; QString txid, tags;
        m_messageList->get(getCurrentChat(), new_n, sender_addr, sender_id, atext, _enable_comments, height, ts, txid, tags);
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

void UnsortedMessageListModel::setCurrent(const QString& address, bool isMultiUser, const QString& myDescription, const QString& myAb)
{
    beginResetModel();
    m_isMultiUser = isMultiUser;
    m_currentAddress = address;
    m_currentSubChat = "";
    endResetModel();
    m_myDescription = myDescription;
    m_myAb = myAb;
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
    roleNames.insert(MessageABRole, "Ab");
    roleNames.insert(MessageABColorRole, "AbColor");
    roleNames.insert(MessageABBackgroundRole, "AbBackground");
    roleNames.insert(MessageTagsRole, "tags");
    roleNames.insert(MessageIsDeletedRole, "isDeleted");
    roleNames.insert(MessageIsTitleRole, "isTitle");
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
  , src_(parent, address, message), view_deleted_(false)
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
  if(src_.getCurrentChat() == chat)
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

void MessageListModel::setViewDeleted(bool f)
{   bool r = (view_deleted_ != f);
    view_deleted_ = f;
    if(r) invalidate();
}

bool MessageListModel::getViewDeleted()
{
    return view_deleted_;
}

void MessageListModel::append(const QString& sms, bool enable_comments, bool is_anon, quint64 amount, bool unprunable)
{
  src_.append(sms, enable_comments, is_anon, amount, unprunable);
  emit addressBookInvalidate();
}

void MessageListModel::del(quint64 index)
{
    src_.m_messageList->del(src_.getCurrentChat(), index);
    invalidate();
}

void MessageListModel::undel(quint64 index)
{
    src_.m_messageList->undel(src_.getCurrentChat(), index);
    invalidate();
}

void MessageListModel::setCurrent(const QString& Address, bool isMultiUser, const QString& myDescription, const QString& myAb)
{
  src_.setCurrent(Address, isMultiUser, myDescription, myAb);
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

bool MessageListModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QAbstractItemModel *model = sourceModel();
    QModelIndex sourceIndex = model->index(sourceRow, 0, sourceParent);

    if (!sourceIndex.isValid())
        return true;

    QString sender_addr; QString text; bool enable_comments; quint64 sender_id; quint64 height; quint64 ts; QString txid, tags;
    if(!src_.m_messageList->get(src_.getCurrentChat(), sourceIndex.row(), sender_addr, sender_id, text, enable_comments, height, ts, txid, tags))
        return false;

    QRegExp rx = filterRegExp();
    if (!rx.isEmpty() && !text.contains(rx))
        return false;

    if(view_deleted_)
        return true;

    if(tags.contains(Monero::TAG_DEL))
        return false;

    tags = src_.m_addressBook->getTags(sender_id);
    if(tags.contains(Monero::TAG_DEL) || tags.contains(Monero::TAG_BLOCK))
        return false;

    return true;
}
