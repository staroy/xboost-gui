#pragma once

#include <QObject>
#include <QString>
#include <QFileSystemWatcher>
#include <memory>

class LuaSync : public QObject
{
    Q_OBJECT

public:
    LuaSync(QObject *parent = 0);

    void init();
    void reinit();
    void recursiveDirWatcher(const QString& path);

    Q_SLOT void fileEventModified(const QString& str);
    Q_SLOT void dirEventModified(const QString& str);

signals:
    void luaFileModified(const QString& str);

private:
    QFileSystemWatcher m_watcher;
    time_t m_time;
};

extern std::unique_ptr<LuaSync> g_luaSync;
