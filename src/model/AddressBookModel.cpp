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

#include "AddressBookModel.h"
#include "AddressBook.h"
#include "MessageListModel.h"
#include "MessageList.h"
#include <QDebug>
#include <QHash>
#include <wallet/api/wallet2_api.h>

UnsortedAddressBookModel::UnsortedAddressBookModel(QObject *parent, AddressBook *addressBook, MessageList *messageList)
    : QAbstractListModel(parent) , m_addressBook(addressBook), m_messageList(messageList)
{
    connect(m_addressBook,SIGNAL(refreshStarted()),this,SLOT(startReset()));
    connect(m_addressBook,SIGNAL(refreshFinished()),this,SLOT(endReset()));
}

void UnsortedAddressBookModel::startReset(){
    beginResetModel();
}
void UnsortedAddressBookModel::endReset(){
    endResetModel();
}

int UnsortedAddressBookModel::rowCount(const QModelIndex &) const
{
    return m_addressBook->count();
}

QVariant UnsortedAddressBookModel::data(const QModelIndex &index, int role) const
{
    QVariant result; auto messageList = m_messageList;

    if(role == AddressBookIsMultiUserRole)
        return  m_addressBook->isMultiUser(index.row());

    if(role == AddressBookTagsRole)
        return m_addressBook->getTags(index.row());

    if(role == AddressBookIsDeletedRole)
        return m_addressBook->isDeletedRow(index.row());

    if(role == AddressBookIsBlockedRole)
        return m_addressBook->isBlockedRow(index.row());

    if(role == AddressBookIsAnonRole)
        return m_addressBook->isAnonRow(index.row());

    bool found = m_addressBook->getRow(index.row(), [&messageList, &result, &role, &index](const Monero::AddressBookRow &row) {
        switch (role) {
        case AddressBookAddressRole:
            result = QString::fromStdString(row.getAddress());
            break;
        case AddressBookDescriptionRole:
            result = QString::fromStdString(row.getDescription());
            break;
        case AddressBookMyDescriptionRole:
            result = QString::fromStdString(row.getMyDescription());
            break;
        case AddressBookPaymentIdRole:
            result = QString::fromStdString(row.getPaymentId());
            break;
        case AddressBookRowIdRole:
            // Qt doesnt support size_t overload type casting
            result.setValue(index.row());
            break;
        case AddressBookUnreadCountRole:
            if(messageList)
                result = quint64(messageList->getUnread(row.getAddress().c_str()));
            break;
        case AddressBookLastTimeRole:
            if(messageList)
                result = quint64(messageList->getLastTime(row.getAddress().c_str()));
            break;
        case AddressBookLastTimeStrRole:
            if(messageList)
                result = QDateTime().fromSecsSinceEpoch(quint64(messageList->getLastTime(row.getAddress().c_str()))).toString("dd.MM.yyyy hh:mm");
            break;
        case AddressBookAB:
            result = QString::fromStdString(row.getShortName());
            break;
        case AddressBookMyAB:
            result = QString::fromStdString(row.getMyShortName());
            break;
        case AddressBookABColor:
            result = QString::fromStdString(row.getShortNameColor());
            break;
        case AddressBookABBackground:
            result = QString::fromStdString(row.getShortNameBackground());
            break;
        default:
            qCritical() << "Unimplemented role " << role;
        }
    });
    if (!found) {
        qCritical("UnsortedAddressBookModel::%s: internal error: invalid index %d", __FUNCTION__, index.row());
    }

    return result;
}

bool UnsortedAddressBookModel::deleteRow(int row)
{
    return m_addressBook->deleteRow(row);
}

bool UnsortedAddressBookModel::undeleteRow(int row)
{
    return m_addressBook->undeleteRow(row);
}

QHash<int, QByteArray> UnsortedAddressBookModel::roleNames() const
{
    QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
    roleNames.insert(AddressBookAddressRole, "address");
    roleNames.insert(AddressBookPaymentIdRole, "paymentId");
    roleNames.insert(AddressBookDescriptionRole, "description");
    roleNames.insert(AddressBookIsMultiUserRole, "isMultiUser");
    roleNames.insert(AddressBookIsDeletedRole, "isDeleted");
    roleNames.insert(AddressBookIsBlockedRole, "isBlocked");
    roleNames.insert(AddressBookIsAnonRole, "isAnon");
    roleNames.insert(AddressBookRowIdRole, "rowId");
    roleNames.insert(AddressBookUnreadCountRole, "unreadCount");
    roleNames.insert(AddressBookLastTimeRole, "lastTimeSecs");
    roleNames.insert(AddressBookLastTimeStrRole, "lastTime");
    roleNames.insert(AddressBookAB, "Ab");
    roleNames.insert(AddressBookABColor, "AbColor");
    roleNames.insert(AddressBookABBackground, "AbBackground");
    roleNames.insert(AddressBookTagsRole, "tags");
    roleNames.insert(AddressBookMyDescriptionRole, "myDescription");
    roleNames.insert(AddressBookMyAB, "myAb");
    return roleNames;
}

AddressBookModel::AddressBookModel(QObject *parent, AddressBook *addressBook, MessageList *messageList, bool is_chat)
  : QSortFilterProxyModel(parent)
  , src_(parent, addressBook, messageList), is_chat_(is_chat)
{
  setSourceModel(&src_);

  if(is_chat_)
  {
    setSortRole(UnsortedAddressBookModel::AddressBookLastTimeRole);
    setSortOrder(true);
  }
  else
  {
    setSortRole(UnsortedAddressBookModel::AddressBookDescriptionRole);
    setSortOrder(false);
  }

  //setFilterRole(UnsortedAddressBookModel::AddressBookDescriptionRole);
}

AddressBookModel::~AddressBookModel()
{
}

void AddressBookModel::setFilterString(const QString& str)
{
  filter_ = str;
  this->setFilterCaseSensitivity(Qt::CaseInsensitive);
  this->setFilterFixedString(filter_);
}

QString AddressBookModel::getFilterString()
{
  return filter_;
}

void AddressBookModel::setSortOrder(bool checked)
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

bool AddressBookModel::deleteRow(int row)
{
    return src_.deleteRow(row);
}

bool AddressBookModel::undeleteRow(int row)
{
    return src_.undeleteRow(row);
}

void AddressBookModel::invalidate()
{
    QSortFilterProxyModel::invalidate();
}

bool AddressBookModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QAbstractItemModel *model = sourceModel();
    QModelIndex sourceIndex = model->index(sourceRow, 0, sourceParent);

    if (!sourceIndex.isValid())
        return true;

    QRegExp rx = filterRegExp();
    if (!rx.isEmpty())
    {
        QString key = model->data(sourceIndex, UnsortedAddressBookModel::AddressBookDescriptionRole).toString();
        if(!key.contains(rx))
            return false;
    }

    if(!is_chat_)
        return true;

    QString tags = model->data(sourceIndex, UnsortedAddressBookModel::AddressBookTagsRole).toString();

    if(tags.contains(Monero::TAG_DEL))
        return false;

    if(tags.contains(Monero::TAG_BLOCK))
        return false;

    if(tags.contains(Monero::TAG_ANON))
        return false;

    return true;
}
