/**
 * \file src/application_info.cpp
 * \brief Implementation of application info application module.
 */

#include "application_info.hpp"

#include <QCoreApplication>
#include <QDir>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

#include "icsograf_info.hpp"

QString Application_Info::name() { return tr("icsograf"); }

QString Application_Info::version() { return QString(VERSION); }

QString Application_Info::trimmed_version()
{
    static const QRegExp version_re("^[0-9]+\\.[0-9]+\\.[0-9]+");
    QString version_str = version();
    version_re.indexIn(version_str);
    return version_str.left(version_re.matchedLength());
}

bool Application_Info::has_least_version(int maj, int min)
{
    return check_least_version(version(), maj, min);
}

bool Application_Info::has_least_version(QString checkversion)
{
    QStringList v_in = checkversion.split(".");
    QStringList v_k = version().split(".");
    if (v_in.size() < 2) return false;  // weird format
    for (int i = 0; i < v_in.size(); i++) {
        if (i >= v_k.size()) return false;  // input version has one extra sub-number
        int vk = v_k[i].remove(QRegExp("[^0-9]")).toInt();
        int vin = v_in[i].remove(QRegExp("[^0-9]")).toInt();
        if (vk < vin)
            return false;  // input version is greater
        else if (vk > vin)
            return true;  // local version is greater
    }
    return true;  // versions are the same
}

bool Application_Info::check_least_version(QString version, int maj, int min)
{
    QStringList v = version.split(".");
    // int i = v[0].toInt(), j = v[1].toInt();
    if (v.size() < 2)
        return false;
    else
        return v[0].toInt() >= maj && v[1].toInt() >= min;
}

static QStringList data_roots()
{
    QStringList search;

    // executable dir
    QDir path(QCoreApplication::applicationDirPath());
    path.cdUp();
    search << path.filePath("data");
    search << path.filePath("share/" TARGET "/" TARGET);
    path.cdUp();
    search << path.filePath("data");

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    search << QStandardPaths::standardLocations(QStandardPaths::DataLocation);
#else
    search << QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif

    search << QDir::current().filePath("data");  // current dir

    return search;
}

QString Application_Info::data(QString name)
{
    // executable dir
    for (QDir path : data_roots()) {
        if (path.exists(name)) return QDir::cleanPath(path.absoluteFilePath(name));
    }
    return {};
}

QStringList Application_Info::data_directories(QString name)
{
    QStringList found;

    QStringList search = data_directories_unckecked(name);

    for (QString d : search) {
        if (QFileInfo(d).exists()) found << d;
    }
    found.removeDuplicates();

    return found;
}

QStringList Application_Info::data_directories_unckecked(QString name)
{
    QStringList filter;
    for (QString d : data_roots()) {
        filter << QDir::cleanPath(QDir(d).absoluteFilePath(name));
    }
    filter.removeDuplicates();

    return filter;
}

QString Application_Info::writable_data_directory(QString name)
{
    QStringList search;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

    search << QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    search << QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif

    if (!search.empty()) {
        return QDir::cleanPath(QDir(search[0]).absoluteFilePath(name));
    }

    return QString();
}
