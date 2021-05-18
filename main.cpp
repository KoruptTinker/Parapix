#include <QApplication>
#include "nesting.h"
#include "mainwindow.h"


int main(int argc, char *argv[]) {

  QApplication app(argc, argv);

//  MainWindow window;

//  window.resize(3000, 1000);
//  window.setWindowTitle("Parapix");
//  window.show();

  Layouts w;
  w.resize(3000, 1000);
  w.setWindowTitle("Parapix");
  w.show();

  return app.exec();
}
