/*
 * @Author: your name
 * @Date: 2021-04-08 08:57:10
 * @LastEditTime: 2021-04-10 23:24:52
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /QtCode/Music/search.cpp
 */
#include "search.h"
#include "player.h"
#include "ui_search.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
QVector<int> Search::Song_ID{};

Search::Search(QWidget *parent) : QWidget(parent), ui(new Ui::Search) {
  ui->setupUi(this);
  initTableWidget();
  Net_Manage = new QNetworkAccessManager(this);
  Net_Request = new QNetworkRequest{};

  connect(Net_Manage, &QNetworkAccessManager::finished, this,
          &Search::on_ReplyFinish);
}

Search::~Search() {
  delete ui;
  delete Net_Request;
}

void Search::initTableWidget() {
  //设置触发条件：不可编辑
  ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableWidget->setSortingEnabled(false); //启动排序
  // item 水平表头自适应大小
  ui->tableWidget->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  ui->tableWidget->horizontalHeader()->setDefaultSectionSize(35);
  //
}
void Search::parseJson(QString &json) {
  Song_ID.clear();
  QString songanme{};
  QString artistsname{};
  QString albumname{};
  int duration{};

  QByteArray byte_array;
  QJsonParseError err_rpt;
  QJsonDocument docment =
      QJsonDocument::fromJson(byte_array.append(json), &err_rpt);
  if (err_rpt.error == QJsonParseError::NoError) {
    QJsonObject Rootobj = docment.object();

    //开始解析result对象
    QJsonValue result = Rootobj.value("result");
    if (result.isObject()) {
      QJsonObject resultoobj = result.toObject();
      QJsonValue songs = resultoobj.value("songs");
      if (songs.isArray()) {
        QJsonArray songArr = songs.toArray();
        //解析songs数组
        int songcount = songArr.count();
        for (int x = 0; x != songcount; x++) {
          QJsonValue songsobj = songArr.at(x);
          if (songsobj.isObject()) {
            QJsonObject sobj = songsobj.toObject();
            QJsonValue name = sobj.value("name");
            songanme = name.toString();
            qDebug() << "songname = " << name.toString();
            //获取歌曲ID
            Song_ID.push_back(sobj.value("id").toInt());
            //解析artists数组
            QJsonValue artists = sobj.value("artists");
            if (artists.isArray()) {
              QJsonArray artistsArr = artists.toArray();
              if (artistsArr.at(0).isObject()) {
                QJsonObject artistsobj = artistsArr.at(0).toObject();
                QJsonValue name = artistsobj.value("name");
                artistsname = name.toString();
                qDebug() << "artistsname = " << name.toString();
              }
            }
            //解析album对象
            QJsonValue album = sobj.value("album");
            if (album.isObject()) {
              QJsonObject albumobj = album.toObject();
              QJsonValue name = albumobj.value("name");
              albumname = name.toString();
              qDebug() << "albumname = " << name.toString();
            }
            //解析duration对象
            QJsonValue Time = sobj.value("duration");
            duration = Time.toInt();
            //                        duration =
            //                        QString("%1:%2").arg(Time.toInt()/60).arg(Time.toInt()
            //                        % 60);
            qDebug() << "Time = " << Time.toInt();
          }
          QStringList song;
          song << songanme << artistsname << albumname
               << QString("%1:%2").arg(duration / 1000 / 60).arg(duration % 60);
          //插入tablewidget
          ui->tableWidget->insertRow(x);
          for (int z = 0; z != 4; z++) {
            ui->tableWidget->setItem(x, z, new QTableWidgetItem(song.at(z)));
          }
          song.clear();
        }
      }
    }
  } else {
    qDebug() << "JSON格式错误";
    return;
  }
}

void Search::playerUrl(const int ID) {
  Player::Playlist->clear();
  QVector<int>::const_iterator it = Song_ID.cbegin();
  while (it != Song_ID.cend()) {
    Player::Playlist->addMedia(
        QUrl(QString("https://music.163.com/song/media/outer/url?id=%1.mp3")
                 .arg(*it)));
    ++it;
  }
}

QTableWidget *Search::getSearchTableW() { return ui->tableWidget; }

void Search::on_line_search_returnPressed() {
  QString url =
      QString("http://musicapi.leanapp.cn/search?keywords=%1&limit=50")
          .arg(ui->line_search->text());
  Net_Request->setUrl(url);
  Net_Manage->get(*Net_Request);
}

void Search::on_btn_enter_clicked() { on_line_search_returnPressed(); }

void Search::on_ReplyFinish(QNetworkReply *reply) {
  //获取响应的信息，状态码为200表示正常
  QVariant status_code =
      reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  //无错误返回
  if (reply->error() == QNetworkReply::NoError) {
    QByteArray bytes = reply->readAll(); //获取字节
    QString result(bytes);               //转化为字符串
    qDebug() << result;
    parseJson(result); //该函数用于解析api接口返回的json
  } else {
    //处理错误
    QMessageBox::critical(this, tr("Error"),
                          tr("搜索失败，请检查网络或搜索接口"),
                          QMessageBox::NoAll);
  }
}

void Search::on_tableWidget_cellDoubleClicked(int row, int) {
  Player::state = 2;
  int curID = Song_ID.at(row);
  playerUrl(curID);
  Player::Play->setMedia(Player::Playlist);
  Player::Playlist->setCurrentIndex(row);
  Player::Play->play(); //播放网络音乐
}
