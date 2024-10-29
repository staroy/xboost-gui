#include <QDebug>
#include "LuaSync.h"
#include <boost/filesystem.hpp>

std::unique_ptr<LuaSync> g_luaSync;

LuaSync::LuaSync(QObject *parent)
  : QObject(parent), m_time(time(NULL))

{
  QObject::connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, &LuaSync::dirEventModified);
  QObject::connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &LuaSync::fileEventModified);
}

void LuaSync::init()
{
  QString xboost_lua = qgetenv("XBOOST_LUA");
  if (!xboost_lua.isEmpty())
    recursiveDirWatcher(xboost_lua);
  else
    qWarning() << "XBOOST_LUA is empty!!!";
}

void LuaSync::reinit()
{
  m_watcher.removePaths(m_watcher.files());
  m_watcher.removePaths(m_watcher.directories());
  init();
}

void LuaSync::recursiveDirWatcher(const QString& path)
{
  m_watcher.addPath(path);
  //qWarning() << "Add directory to watcher: " << path.toStdString().c_str();
  const boost::filesystem::path dir = boost::filesystem::path{path.toStdString()};
  for(const boost::filesystem::directory_entry& f :
    boost::filesystem::recursive_directory_iterator{dir})
  {
    if(boost::filesystem::is_regular_file(f))
    {
      m_watcher.addPath(f.path().string().c_str());
      //qWarning() << "Add file to watcher: " << f.path().string().c_str();
    }
    else
    {
      recursiveDirWatcher(f.path().string().c_str());
    }
  }
}

void LuaSync::fileEventModified(const QString& str)
{
  //qWarning() << "Watcher file modified: " << str.toStdString().c_str();
  time_t tm = time(NULL);
  if(tm > m_time)
    emit luaFileModified(str);
  m_time = tm + 1;
}

void LuaSync::dirEventModified(const QString& str)
{
  //qWarning() << "Watcher directory modified: " << str.toStdString().c_str();
  reinit();
}
