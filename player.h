/*
 * @Author: your name
 * @Date: 2021-03-07 23:20:05
 * @LastEditTime: 2021-04-01 13:40:47
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /QtCode/Music/player.h
 */
#ifndef PLAYER_H
#define PLAYER_H

#include <QMediaPlayer>
#include <QMediaPlaylist>

#include <QSqlDatabase>
#include <QSqlQuery>

#include <QWidget>
#include <mpegfile.h>
class QMenu;
class QLabel;
class QTableWidget;
class QTableWidgetItem;

 void InitTabWidget(QTableWidget *tableWidget);
namespace Ui {
class Player;
}
void deleteTableWidgetRow(QTableWidget *tablewidget);
class Player : public QWidget {
  Q_OBJECT

public:
  explicit Player(QWidget *parent = nullptr);
  ~Player();
  void LoadQss();

  void setVolum(int volume);
  QStringList &getPath() { return Filepath; }
  QTableWidget *getTableW();
  void getTag(QStringList &Medialist);
  void AddToMysql(const int row);
  bool initMySQL();

  static int state;
  static int LoacSongSum;
  static QMediaPlayer *Play;
  static QMediaPlaylist *Playlist;
  static QSqlDatabase db;
  static QSqlQuery *query;

private:
  Ui::Player *ui;
  QString Artist;
  QString Album;
  QString Title;
  QMenu *TableWidget_Menu;
  QStringList Filepath;
  QStringList tagList;
  TagLib::MPEG::File *mpeg;
signals:
  void setImage();
public slots:
  void on_btn_add_Music_clicked();
  void on_tableWidget_customContextMenuRequested(const QPoint &pos);
private slots:
  void on_line_search_textChanged(const QString &lab_text);
  virtual void on_tableWidget_cellDoubleClicked(int row, int column);
  //防止删除item时自动切换歌曲
  void on_tableWidget_itemChanged(QTableWidgetItem *item);
  void on_btn_flush_clicked();
};

#endif // PLAYER_H
