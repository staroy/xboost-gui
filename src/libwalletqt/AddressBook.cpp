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

#include "AddressBook.h"
#include <QDebug>

AddressBook::AddressBook(Monero::AddressBook *abImpl,QObject *parent)
  : QObject(parent), m_addressBookImpl(abImpl)
{
}

QString AddressBook::errorString() const
{
    return QString::fromStdString(m_addressBookImpl->errorString());
}

int AddressBook::errorCode() const
{
    return m_addressBookImpl->errorCode();
}

bool AddressBook::newMultiUserRow(const QString &description) const
{
    bool result;
    emit refreshStarted();

    {
        QReadLocker locker(&m_lock);
        result = m_addressBookImpl->newMultiUserRow(description.toStdString(), [](Monero::AddressBookRow& row, std::size_t rowId){});
    }

    emit refreshFinished();
    return result;
}

bool AddressBook::getRow(int index, std::function<void (Monero::AddressBookRow &)> callback) const
{
    QReadLocker locker(&m_lock);

    //if (index < 0 || index >= m_addressBookImpl->count())
    //{
    //    return false;
    //}

    return m_addressBookImpl->getRow(index, callback);
}

bool AddressBook::addRow(const QString &address, const QString &payment_id, const QString &description)
{
    bool result;
    emit refreshStarted();

    {
        size_t rowId;
        QWriteLocker locker(&m_lock);
        result = m_addressBookImpl->addRow({
            address.toStdString(),
            payment_id.toStdString(),
            description.toStdString(),
            "", "", ""
        }, rowId);
    }

    emit refreshFinished();
    return result;
}

bool AddressBook::deleteRow(int rowId)
{
    bool result;
    emit refreshStarted();

    {
        QWriteLocker locker(&m_lock);
        result = m_addressBookImpl->deleteRow(rowId);
    }

    emit refreshFinished();
    return result;
}

quint64 AddressBook::count() const
{
    QReadLocker locker(&m_lock);
    return m_addressBookImpl->count();
}

QString AddressBook::getDescription(const QString &address) const
{
    QReadLocker locker(&m_lock);

    int idx = m_addressBookImpl->lookupAddress(address.toStdString());
    if(idx < 0)
         return QString();

    QString description;
    if(m_addressBookImpl->getRow(idx, [&description](const Monero::AddressBookRow& row)
        {
            description = QString::fromStdString(row.getDescription());
        }))
    {
        return description;
    };
    return QString();
}

void AddressBook::setDescription(int index, const QString &description)
{
    emit refreshStarted();

    {
        QWriteLocker locker(&m_lock);
        m_addressBookImpl->setDescription(index, description.toStdString());
    }

    emit refreshFinished();
}

bool AddressBook::isMultiUser(int index)
{
    QWriteLocker locker(&m_lock);

    return m_addressBookImpl->isMultiUser(index);
}
