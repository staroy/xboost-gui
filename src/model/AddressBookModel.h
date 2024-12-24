// Copyright (c) 2014-2019, The Monero Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef ADDRESSBOOKMODEL_H
#define ADDRESSBOOKMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

class AddressBook;
class MessageList;

class UnsortedAddressBookModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum AddressBookRowRole {
        AddressBookRole = Qt::UserRole + 1, // for the AddressBookRow object;
        AddressBookAddressRole,
        AddressBookDescriptionRole,
        AddressBookIsMultiUserRole,
        AddressBookIsDeletedRole,
        AddressBookIsBlockedRole,
        AddressBookIsAnonRole,
        AddressBookPaymentIdRole,
        AddressBookRowIdRole,
        AddressBookUnreadCountRole,
        AddressBookLastTimeRole,
        AddressBookLastTimeStrRole,
        AddressBookAB,
        AddressBookABColor,
        AddressBookABBackground,
        AddressBookTagsRole,
        AddressBookMyDescriptionRole,
        AddressBookMyAB
    };
    //Q_ENUM(AddressBookRowRole)

    UnsortedAddressBookModel(QObject *parent, AddressBook *addressBook, MessageList *messageList);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE bool deleteRow(int row);
    Q_INVOKABLE bool undeleteRow(int row);
    virtual QHash<int, QByteArray> roleNames() const  override;

public slots:
    void startReset();
    void endReset();

private:
    AddressBook *m_addressBook;
    MessageList *m_messageList;
};

class AddressBookModel : public QSortFilterProxyModel
{
  Q_OBJECT
  UnsortedAddressBookModel src_;
  QString filter_;
  bool is_chat_;

public:
  Q_ENUM(UnsortedAddressBookModel::AddressBookRowRole)

  AddressBookModel(QObject *parent, AddressBook *AddressBook, MessageList *messageList, bool is_chat = false);
  ~AddressBookModel();

  Q_INVOKABLE void setFilterString(const QString& filter);
  Q_INVOKABLE void setSortOrder(bool checked);

  Q_INVOKABLE bool deleteRow(int row);
  Q_INVOKABLE bool undeleteRow(int row);

  Q_INVOKABLE QString getFilterString();
  Q_INVOKABLE void invalidate();

protected:
  bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};

#endif // ADDRESSBOOKMODEL_H
