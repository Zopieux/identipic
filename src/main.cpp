#include "MainWindow.h"
#include <Magick++.h>
#include <QApplication>
#include <QIcon>
#include <QLibraryInfo>
#include <QTranslator>

#define APP_NAME "identipic"

int main(int argc, char *argv[]) {
  Magick::InitializeMagick(*argv);

  QCoreApplication::setOrganizationName("zopisoft");
  QCoreApplication::setOrganizationDomain("zopieux.com");
  QCoreApplication::setApplicationName(APP_NAME);

  QApplication app(argc, argv);
  app.setAttribute(Qt::AA_UseHighDpiPixmaps);

  QTranslator qtTranslator;
  qtTranslator.load("qt_" + QLocale::system().name(),
                    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app.installTranslator(&qtTranslator);

  QTranslator appTranslator;
  appTranslator.load(APP_NAME "_" + QLocale::system().name());
  app.installTranslator(&appTranslator);

  // FIXME: figure out why this is needed
  QIcon::setThemeName(QStringLiteral("default"));

  MainWindow *main_window = new MainWindow();
  main_window->show();

  return app.exec();
}
