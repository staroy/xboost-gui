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

#include "string_tools.h"
#include "hex.h"

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

int AddressBook::newMultiUserRow(const QString &description, const QString &ab, const QString &bg) const
{
    size_t index;

    bool result;
    emit refreshStarted();

    {
        QReadLocker locker(&m_lock);
        result = m_addressBookImpl->newMultiUserRow(description.toStdString(), ab.toStdString(), bg.toStdString(), [&index](Monero::AddressBookRow& row, std::size_t rowId)
        {
            index = rowId;
        });
    }

    emit refreshFinished();

    if(!result)
      return -1;

    return index;
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

int AddressBook::addRow(const QString &address, const QString &payment_id, const QString &description, const QString &ab, const QString &bg, const QString &my_description, const QString &my_ab)
{
    size_t rowId;

    bool result;
    emit refreshStarted();

    {
        QWriteLocker locker(&m_lock);
        result = m_addressBookImpl->addRow({
            address.toStdString(),
            payment_id.toStdString(),
            description.toStdString(),
            ab.toStdString(),
            getShortNameTextColor(bg).toStdString(),
            bg.toStdString(),
            false,
            false,
            my_description.toStdString(),
            my_ab.toStdString()
        }, rowId);
    }

    emit refreshFinished();

    if(!result)
      return -1;

    return rowId;
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

bool AddressBook::undeleteRow(int rowId)
{
    bool result;
    emit refreshStarted();

    {
        QWriteLocker locker(&m_lock);
        result = m_addressBookImpl->undeleteRow(rowId);
    }

    emit refreshFinished();
    return result;
}

bool AddressBook::blockRow(int rowId)
{
    bool result;
    emit refreshStarted();

    {
        QWriteLocker locker(&m_lock);
        result = m_addressBookImpl->blockRow(rowId);
    }

    emit refreshFinished();
    return result;
}

bool AddressBook::unblockRow(int rowId)
{
    bool result;
    emit refreshStarted();

    {
        QWriteLocker locker(&m_lock);
        result = m_addressBookImpl->unblockRow(rowId);
    }

    emit refreshFinished();
    return result;
}

bool AddressBook::isDeletedRow(int rowId)
{
    return m_addressBookImpl->isTaged(rowId, Monero::TAG_DEL);
}

bool AddressBook::isBlockedRow(int rowId)
{
    return m_addressBookImpl->isTaged(rowId, Monero::TAG_BLOCK);
}

bool AddressBook::isAnonRow(int rowId)
{
    return m_addressBookImpl->isTaged(rowId, Monero::TAG_ANON);
}

void AddressBook::setTags(int index, const QString& tags)
{
    QReadLocker locker(&m_lock);
    m_addressBookImpl->setTags(index, tags.toStdString());
}

QString AddressBook::getTags(int index) const
{
    QReadLocker locker(&m_lock);
    std::string val;
    if(!m_addressBookImpl->getTags(index, val))
        return QString();
    return QString::fromStdString(val);
}

void AddressBook::addAttr(int index, const QString& name, const QString& val)
{
    QReadLocker locker(&m_lock);
    m_addressBookImpl->addAttr(index, name.toStdString(), val.toStdString());
}

QString AddressBook::getAttr(int index, const QString& name) const
{
    QReadLocker locker(&m_lock);
    std::string out;
    if(!m_addressBookImpl->getAttr(index, name.toStdString(), out))
        return QString();
    return QString::fromStdString(out);
}

bool AddressBook::delAttr(int index, const QString& name)
{
    QReadLocker locker(&m_lock);
    return m_addressBookImpl->delAttr(index, name.toStdString());
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

void AddressBook::setFields(int index, const QString &address, const QString &description, const QString &shortAb, const QString &backgroundAb)
{
    emit refreshStarted();

    {
        QWriteLocker locker(&m_lock);
        m_addressBookImpl->setFields(index, address.toStdString(), description.toStdString(), shortAb.toStdString(), backgroundAb.toStdString());
    }

    emit refreshFinished();
}

QString AddressBook::getShortNameTextColor(const QString& color)
{
    uint8_t c[3];
    if(epee::from_hex::to_buffer(c, color.toStdString().substr(1)) && 0.3*c[0]+0.59*c[1]+0.11*c[2] > 128.0)
      return "#303030";
    return "#f0f0f0";
}

QString AddressBook::getShortNameBackgroundColorRandomize()
{
    std::string bc;
    m_addressBookImpl->getShortNameBackgroundColorRandomize(bc);
    return QString::fromStdString(bc);
}

bool AddressBook::isMultiUser(int index)
{
    QWriteLocker locker(&m_lock);
    return m_addressBookImpl->isMultiUser(index);
}

