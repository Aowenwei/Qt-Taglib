/*
 * @Author: your name
 * @Date: 2021-03-30 23:57:07
 * @LastEditTime: 2021-04-01 13:05:31
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /QtCode/Music/collect.cpp
 */
#include "collect.h"
#include "ui_collect.h"
#include <QTableWidgetItem>
#include <iostream>
collect::collect(QWidget *parent) : QWidget(parent), ui(new Ui::collect) {
  ui->setupUi(this);
  InitTabWidget(ui->tableWidget);
  LocalMysqlSong();

}
collect::~collect() { delete ui; }

void collect::LocalMysqlSong() {
  Player::query->exec("select * from Music");
  while (Player::query->next()) {
    //插入标题以便删除数据库中的数据
    MySQLMp3List << Player::query->value(0).toString();
    Mp3Path.append(Player::query->value(5).toString());
    ui->tableWidget->insertRow(Mp3Sum);
    for (int index = 0; index != 5; index++) {
      ui->tableWidget->setItem(
          Mp3Sum, index,
          new QTableWidgetItem(Player::query->value(index).toString()));
    }
    ++Mp3Sum;
  }
  ui->lab_songsum->setText(QString("共 %1 首歌曲").arg(Mp3Sum));
}

void collect::CancelCollect() {
  int curRow = ui->tableWidget->currentRow();
  Player::query->exec(QString("DELETE FROM Music WHERE Title = '%1'")
                    .arg(MySQLMp3List.at(curRow)));
  deleteTableWidgetRow(ui->tableWidget);
  --Mp3Sum;
  Mp3Path.clear();
  MySQLMp3List.clear();
  LocalMysqlSong();
}

QTableWidget *collect::getCollectTabW() { return ui->tableWidget; }

void collect::on_tableWidget_cellDoubleClicked(int row, int) {

    if ( Player::LoacSongSum != 0) {
   Player::Playlist->clear();
   Player::LoacSongSum = 0;
  }

  for (int i = 0; i != Mp3Sum; i++) {
   Player::Playlist->addMedia(QUrl::fromLocalFile(Mp3Path.at(i)));
    ++Player::LoacSongSum;
  }
  Player::state = 0;
  Player::Play->setMedia(Player::Playlist);
  Player::Playlist->setCurrentIndex(row);
  Player::Play->play();
}

void collect::on_flush_clicked() {
  deleteTableWidgetRow(ui->tableWidget);
  Mp3Sum = 0;
  Mp3Path.clear();
  MySQLMp3List.clear();
  LocalMysqlSong();
}
