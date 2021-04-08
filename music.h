/*
 * @Author: your name
 * @Date: 2021-03-12 22:33:09
 * @LastEditTime: 2021-03-30 22:38:10
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /QtCode/Music/music.h
 */
#ifndef MUSIC_H
#define MUSIC_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class Music;
}
QT_END_NAMESPACE
class Search;
class collect;
class Player;
class Lyric;
class QListWidgetItem;

class Music : public QMainWindow {
  Q_OBJECT

public:
  Music(QWidget *parent = nullptr);
  ~Music();
  void hover();
  void addlistWiwdgetItem();
  void setListWidgetStyle();
  void getCurrMp3Image(const QString &path = " ");
  void Presskey(QKeyEvent *even);
  bool eventFilter(QObject *obj, QEvent *event) override;
  void install_EeventFilter();
private slots:
  void on_btn_stop_clicked();
  void on_Sli_volum_valueChanged(int value);
  void on_btn_volum_clicked();
  void on_btn_prev_clicked();
  void on_btn_next_clicked();
  //文件时长变化，更新当前播放文件名显示
  //播放文件数据总大小的信号， 它可以获得文件时间长度。
  void onDurationChanged(qint64 duration);
  //当前文件播放位置变化，更新进度显示
  //播放到什么位置的信号， 参数是以毫秒来计算的。
  void onPositionChanged(qint64 position);
  void curSongTime(qint64 songTime);
  // void on_horizontalSlider_valueChanged(int value);
  //释放鼠标后触发
  void on_horizontalSlider_sliderReleased();
  void on_btn_pictrue_clicked();
  void on_setBottomInformation(QString &,QString &);

private:
  int curVolume{50};
  QString durationTime{};
  QString positionTime{};
  Ui::Music *ui;
  Player *MyPlay;
  Search *search;
  collect * colle;
  Lyric *lyric;
};
#endif // MUSIC_H
