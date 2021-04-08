/*
 * @Author: your name
 * @Date: 2021-03-07 22:24:58
 * @LastEditTime: 2021-03-20 23:08:11
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /QtCode/Music/main.cpp
 */
#include "music.h"
#include "collect.h"
#include <QApplication>
#include <QFile>
#include <QSqlDatabase>
#include <QStyleFactory>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  // add qss
  QFile botn_widget_style{":/Style/botn_style.qss"};
  if (botn_widget_style.open(QIODevice::ReadOnly)) {
    QString qss = botn_widget_style.readAll();
    a.setStyleSheet(qss);
    botn_widget_style.close();
  } else {
    puts("Failed to open file!");
    botn_widget_style.close();
    exit(EXIT_SUCCESS);
  }
  a.setWindowIcon(QIcon(":/images/Music.png"));   // add icon
  a.setStyle(QStyleFactory::create("macintosh")); //设置窗口风格
  Music w;
  w.show();
  return a.exec();
}
//
