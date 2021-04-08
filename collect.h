/*
 * @Author: your name
 * @Date: 2021-03-30 23:44:03
 * @LastEditTime: 2021-03-31 21:52:18
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /QtCode/Music/collect.h
 */
#ifndef COLLECT_H
#define COLLECT_H

#include "player.h"
#include <QWidget>
namespace Ui {
class collect;
}

class collect : public QWidget {
  Q_OBJECT

public:
  explicit collect(QWidget *parent = nullptr);
  ~collect();
  void LocalMysqlSong();
  void CancelCollect();
  QTableWidget *getCollectTabW();
  QStringList Mp3Path;
private slots:
  void on_tableWidget_cellDoubleClicked(int row, int column);
  void on_flush_clicked();

private:
  int Mp3Sum{};
  QStringList MySQLMp3List;


  Ui::collect *ui;
};

#endif // COLLECT_H
