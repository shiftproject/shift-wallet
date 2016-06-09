/*
    This file is part of shiftwallet.
    shiftwallet is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    shiftwallet is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with shiftwallet. If not, see <http://www.gnu.org/licenses/>.
*/
/** @file accountmodel.h
 * @author Ales Katona <almindor@gmail.com>
 * @date 2015
 *
 * Account model header
 */

#ifndef ACCOUNTMODEL_H
#define ACCOUNTMODEL_H

#include <QAbstractListModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "types.h"
#include "currencymodel.h"
#include "etheripc.h"
#include "etherlog.h"

namespace ShiftWallet {

    class AccountModel : public QAbstractListModel
    {
        Q_OBJECT
        Q_PROPERTY(int selectedAccountRow READ getSelectedAccountRow WRITE setSelectedAccountRow NOTIFY accountSelectionChanged)
        Q_PROPERTY(QString selectedAccount READ getSelectedAccount NOTIFY accountSelectionChanged)
        Q_PROPERTY(QString total READ getTotal NOTIFY totalChanged)
    public:
        AccountModel(EtherIPC& ipc, const CurrencyModel& currencyModel);
        QString getError() const;
        QHash<int, QByteArray> roleNames() const;
        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
        bool containsAccount(const QString& from, const QString& to, int& i1, int& i2) const;
        const QJsonArray getAccountsJsonArray() const;
        const QString getTotal() const;

        Q_INVOKABLE void newAccount(const QString& pw);
        Q_INVOKABLE void renameAccount(const QString& name, int index);
        Q_INVOKABLE void deleteAccount(const QString& pw, int index);
        Q_INVOKABLE const QString getAccountHash(int index) const;
    public slots:
        void connectToServerDone();
        void getAccountsDone(const AccountList& list);
        void newAccountDone(const QString& hash, int index);
        void deleteAccountDone(bool result, int index);
        void accountChanged(const AccountInfo& info);
        void newBlock(const QJsonObject& block);
        void currencyChanged();
    signals:
        void accountSelectionChanged(int);
        void totalChanged();
    private:
        EtherIPC& fIpc;
        AccountList fAccountList;
        int fSelectedAccountRow;
        QString fSelectedAccount;
        const CurrencyModel& fCurrencyModel;

        int getSelectedAccountRow() const;
        void setSelectedAccountRow(int row);
        const QString getSelectedAccount() const;
    };

}

#endif // ACCOUNTMODEL_H