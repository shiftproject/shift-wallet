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
/** @file main.cpp
 * @author Ales Katona <almindor@gmail.com>
 * @date 2015
 *
 * Main entry point
 */

#include <QApplication>
#include <QTranslator>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml/qqml.h>
#include <QIcon>
#include <QPixmap>
#include <QDebug>
#include "etherlog.h"
#include "settings.h"
#include "clipboard.h"
#include "accountmodel.h"
#include "accountproxymodel.h"
#include "transactionmodel.h"
#include "currencymodel.h"
#include "gshiftlog.h"

using namespace ShiftWallet;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<AccountProxyModel>("AccountProxyModel", 0, 1, "AccountProxyModel");

    QCoreApplication::setOrganizationName("Etherdyne");
    QCoreApplication::setOrganizationDomain("shiftwallet.com");
    QCoreApplication::setApplicationName("ShiftWallet");
    QCoreApplication::setApplicationVersion("1.5.1");
    app.setWindowIcon(QIcon(QPixmap(":/images/icon")));

    QTranslator translator;
    translator.load("i18n/shiftwallet_" + QLocale::system().name());
    app.installTranslator(&translator);

    Settings settings;

    bool testnet = settings.value("gshift/testnet", false).toBool();
    const QString gshiftPath = settings.value("gshift/path", DefaultGshiftPath()).toString();
    const QString dataPath = settings.value("gshift/datadir", DefaultDataDir).toString();
    const QString ipcPath = DefaultIPCPath(testnet);

    // set defaults
    if ( !settings.contains("ipc/path") ) {
        settings.setValue("ipc/path", ipcPath);
    }
    if ( !settings.contains("gshift/path") ) {
        settings.setValue("gshift/path", gshiftPath);
    }
    if ( !settings.contains("gshift/datadir") ) {
        settings.setValue("gshift/datadir", dataPath);
    }

    ClipboardAdapter clipboard;
    ShiftLog log;
    GshiftLog gshiftLog;
    EtherIPC ipc(ipcPath, gshiftLog);

    CurrencyModel currencyModel;
    AccountModel accountModel(ipc, currencyModel);
    TransactionModel transactionModel(ipc, accountModel);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("settings", &settings);
    engine.rootContext()->setContextProperty("ipc", &ipc);
    engine.rootContext()->setContextProperty("accountModel", &accountModel);
    engine.rootContext()->setContextProperty("transactionModel", &transactionModel);
    engine.rootContext()->setContextProperty("currencyModel", &currencyModel);
    engine.rootContext()->setContextProperty("clipboard", &clipboard);
    engine.rootContext()->setContextProperty("log", &log);
    engine.rootContext()->setContextProperty("gshift", &gshiftLog);

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    if ( settings.contains("program/firstrun") ) {
        ipc.init();
    }

    return app.exec();
}