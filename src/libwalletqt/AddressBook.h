// Copyright (c) 2014-2024, The Monero Project
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

#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H

#include <wallet/api/wallet2_api.h>
#include <QMap>
#include <QObject>
#include <QReadWriteLock>
#include <QList>
#include <QDateTime>

namespace Monero {
struct AddressBook;
}
class AddressBookRow;


extern const char *TAG_DELETE;
extern const char *TAG_BLOCKED;

class AddressBook : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE int newMultiUserRow(const QString &description, const QString &ab, const QString &bg) const;
    Q_INVOKABLE bool getRow(int index, std::function<void (Monero::AddressBookRow &)> callback) const;
    Q_INVOKABLE int addRow(const QString &address, const QString &payment_id, const QString &description, const QString &ab, const QString &bg, const QString &my_description, const QString &my_ab);
    Q_INVOKABLE bool deleteRow(int rowId);
    Q_INVOKABLE bool blockRow(int rowId);
    Q_INVOKABLE bool undeleteRow(int rowId);
    Q_INVOKABLE bool unblockRow(int rowId);
    Q_INVOKABLE bool isDeletedRow(int rowId);
    Q_INVOKABLE bool isBlockedRow(int rowId);
    Q_INVOKABLE bool isAnonRow(int rowId);
    Q_INVOKABLE void setTags(int index, const QString& tags);
    Q_INVOKABLE QString getTags(int index) const;
    Q_INVOKABLE void addAttr(int index, const QString& name, const QString& val);
    Q_INVOKABLE QString getAttr(int index, const QString& name) const;
    Q_INVOKABLE bool delAttr(int index, const QString& name);
    quint64 count() const;
    Q_INVOKABLE QString errorString() const;
    Q_INVOKABLE int errorCode() const;
    Q_INVOKABLE QString getDescription(const QString &address) const;
    Q_INVOKABLE void setDescription(int index, const QString &label);
    Q_INVOKABLE void setFields(int index, const QString &address, const QString &description, const QString &shortName, const QString &shortNameBackground);
    Q_INVOKABLE QString getShortNameTextColor(const QString &backgroundAb);
    Q_INVOKABLE QString getShortNameBackgroundColorRandomize();
    Q_INVOKABLE bool isMultiUser(int index);

    enum ErrorCode {
        Status_Ok,
        General_Error,
        Invalid_Address,
        Invalid_Payment_Id
    };

    Q_ENUM(ErrorCode);

signals:
    void refreshStarted() const;
    void refreshFinished() const;


public slots:

private:
    explicit AddressBook(Monero::AddressBook * abImpl, QObject *parent);
    friend class Wallet;
    Monero::AddressBook * m_addressBookImpl;
    mutable QReadWriteLock m_lock;
};

#endif // ADDRESSBOOK_H
