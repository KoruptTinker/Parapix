#pragma once

#include <QWidget>

class Layouts : public QWidget {
  Q_OBJECT

  public:
    Layouts(QWidget *parent = nullptr);


   public slots:
     void log_button_clicked();
     void contrast_button_clicked();
     void blur_button_clicked();
     void edge_button_clicked();
};
