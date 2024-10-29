
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
        MessageTimeRole,
        MessageTimeStrRole,
        MessageIndexRole,
        MessageTxidRole,
        MessageSendByMeRole,
        MessageIsChannelRole,
        MessageABRole,
        MessageABColorRole,
        MessageABBackgroundRole
    };
    //Q_ENUM(MessageRole)

    UnsortedMessageListModel(QObject *parent, AddressBook *address, MessageList *message);

    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE void append(const QString& message, quint64 amount, bool unprunable);
    Q_INVOKABLE void setCurrent(const QString& address, bool isChannel = false);
    Q_INVOKABLE void setSelf(const QString& address);

    QHash<int, QByteArray> roleNames() const  override;
    const QString& getCurrentAddress() { return m_currentAddress; }

protected:
    void received(const QString &chat, quint64 n, const QString &txid);
    void removed(const QString &chat, quint64 n, const QString &txid);

    AddressBook *m_addressBook;
    MessageList *m_messageList;
    QString m_currentAddress;
    bool m_isChannel;
    QString m_selfAddress;
};

class MessageListModel : public QSortFilterProxyModel
{
  Q_OBJECT

  UnsortedMessageListModel src_;

public:
  Q_ENUM(UnsortedMessageListModel::MessageRole)

  MessageListModel(QObject *parent, AddressBook *Address, MessageList *message);
  ~MessageListModel();

  Q_INVOKABLE void setFilterString(QString string);
  Q_INVOKABLE void setSortOrder(bool checked);

  Q_INVOKABLE void append(const QString& text, quint64 amount, bool unprunable);
  Q_INVOKABLE void setCurrent(const QString& Address, bool isChannel = false);

  UnsortedMessageListModel& source();

public slots:
    void msgReceived(const QString &chat, quint64 n, const QString &txid);
    void msgRemoved(const QString &chat, quint64 n, const QString &txid);

signals:
  void addressBookInvalidate();
};

#endif // MESSAGEMODEL_H
