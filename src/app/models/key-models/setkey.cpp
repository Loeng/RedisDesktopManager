#include "setkey.h"
#include "modules/redisclient/command.h"
#include "modules/redisclient/commandexecutor.h"

SetKeyModel::SetKeyModel(QSharedPointer<RedisClient::Connection> connection, QString fullPath, int dbIndex, int ttl)
       : ListLikeKeyModel(connection, fullPath, dbIndex, ttl)
{
    loadRowCount();
}

QString SetKeyModel::getType()
{
    return "set";
}

void SetKeyModel::updateRow(int rowIndex, const QVariantMap &)
{

}

void SetKeyModel::addRow(const QVariantMap &)
{

}

void SetKeyModel::loadRows(unsigned long rowStart, unsigned long count, std::function<void ()> callback)
{
    if (isPartialLoadingSupported()) {
        //TBD
    } else {

        if (!m_rowsCache.isEmpty())
            return;

        QStringList rows = getRowsRange("SMEMBERS").toStringList();
        unsigned int rowIndex = rowStart;

        foreach (QString row, rows) {
            m_rowsCache[rowIndex] = row.toUtf8();
            rowIndex++;
        }
    }

    callback();
}

void SetKeyModel::removeRow(int i)
{
    if (!m_rowsCache.contains(i))
        return;

    QByteArray value = m_rowsCache.value(i);

    using namespace RedisClient;

    Command deleteValues(QStringList() << "SREM" << m_keyFullPath << value, m_dbIndex);
    Response result = CommandExecutor::execute(m_connection, deleteValues);

    m_rowCount--;
    m_rowsCache.remove(i);
    Q_UNUSED(result);

    setRemovedIfEmpty();
}

void SetKeyModel::loadRowCount()
{
    m_rowCount = getRowCount("SCARD");
}

//void SetKeyModel::updateValue(const QString& value, const QModelIndex *cellIndex)
//{
//    QStandardItem * item = itemFromIndex(*cellIndex);
//    QString oldValue = item->text();
//    item->setText(value);

//    QString deleteOld = QString("SREM %1 %2")
//        .arg(keyName)
//        .arg(oldValue);

//    db->runCommand(RedisClient::Command(deleteOld, this, dbIndex));

//    QString addNew = QString("SADD %1 %2")
//        .arg(keyName)
//        .arg(value);

//    db->runCommand(RedisClient::Command(addNew, this, "loadedUpdateStatus", dbIndex));
//}
