
#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

#include "AddressBook.h"
#include "MessageList.h"

class UnsortedMessageListModel : public QAbstractListModel
{
    Q_OBJECT

    friend class MessageListModel;

public:
    enum MessageRole {
        MessageFromAddressRole = Qt::UserRole + 1,
        MessageFromLabelRole,
        MessageTextRole,
        MessageEnableCommentsRole,
        MessageTimeRole,
        MessageTimeStrRole,
        MessageIndexRole,
        MessageTxidRole,
        MessageSendByMeRole,
        MessageIsMultiUserRole,
        MessageABRole,
        MessageABColorRole,
        MessageABBackgroundRole,
        MessageTagsRole,
        MessageIsDeletedRole,
        MessageIsTitleRole
    };
    //Q_ENUM(MessageRole)

    UnsortedMessageListModel(QObject *parent, AddressBook *address, MessageList *message);

    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE void append(const QString& message, bool enable_comments, bool is_anon, quint64 amount, bool unprunable);
    Q_INVOKABLE void setCurrent(const QString& address, bool isMultiUser = false, const QString& myDescription = QString(), const QString& myAb = QString());
    Q_INVOKABLE bool isSubChat() const;
    Q_INVOKABLE void setSubChat(const QString& txid);
    Q_INVOKABLE void setToParent();
    Q_INVOKABLE void setSelf(const QString& address);

    QHash<int, QByteArray> roleNames() const  override;
    const QString& getCurrentAddress() { return m_currentAddress; }

protected:
    void received(const QString &chat, quint64 n, const QString &txid);
    void removed(const QString &chat, quint64 n, const QString &txid);
    const QString& getCurrentChat() const;
    const QString& getParentChat() const;

    AddressBook *m_addressBook;
    MessageList *m_messageList;
    QString m_currentAddress;
    bool m_isMultiUser;
    QString m_selfAddress;
    QString m_currentSubChat;
    QString m_myDescription;
    QString m_myAb;
};

class MessageListModel : public QSortFilterProxyModel
{
  Q_OBJECT

  UnsortedMessageListModel src_;
  bool view_deleted_;

public:
  Q_ENUM(UnsortedMessageListModel::MessageRole)

  MessageListModel(QObject *parent, AddressBook *Address, MessageList *message);
  ~MessageListModel();

  Q_INVOKABLE void setFilterString(QString string);
  Q_INVOKABLE void setSortOrder(bool checked);
  Q_INVOKABLE void setViewDeleted(bool f);
  Q_INVOKABLE bool getViewDeleted();

  Q_INVOKABLE void append(const QString& text, bool enable_comments, bool is_anon, quint64 amount, bool unprunable);
  Q_INVOKABLE void del(quint64 index);
  Q_INVOKABLE void undel(quint64 index);

  Q_INVOKABLE void setCurrent(const QString& Address, bool isMultiUser = false, const QString& myDescription = QString(), const QString& myAb = QString());
  Q_INVOKABLE void setSubChat(const QString& txid);
  Q_INVOKABLE bool isSubChat();
  Q_INVOKABLE void setToParent();

  UnsortedMessageListModel& source();

public slots:
  void msgReceived(const QString &chat, quint64 n, const QString &txid);
  void msgRemoved(const QString &chat, quint64 n, const QString &txid);

signals:
  void addressBookInvalidate();

protected:
  bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

#endif // MESSAGEMODEL_H
