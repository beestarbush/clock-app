#include "Driver.h"
#include <QSettings>
using namespace Drivers::Storage;

#ifdef PLATFORM_IS_TARGET
const QString SETTINGS_PATH = QStringLiteral(".");
#else
const QString SETTINGS_PATH = QStringLiteral("/workdir/build");
#endif

Driver::Driver(QObject* parent)
    : QObject(parent)
{
    QSettings::setDefaultFormat(QSettings::NativeFormat);
    QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, SETTINGS_PATH);
}
