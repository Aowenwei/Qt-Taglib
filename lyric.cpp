#include "lyric.h"
#include "player.h"
#include "search.h"
#include "ui_lyric.h"
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
//正则库
#include <QRegExp>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
Lyric::Lyric(QWidget *parent) : QWidget(parent), ui(new Ui::Lyric) {
  ui->setupUi(this);
  Net_Manage_lyric = new QNetworkAccessManager(this);
  Net_request_lyric = new QNetworkRequest{};

  Net_Manage_details = new QNetworkAccessManager(this);
  Net_Request_details = new QNetworkRequest{};

  connect(Net_Manage_details, &QNetworkAccessManager::finished, this,
          &Lyric::on_ReplydetailsFinish);
}

Lyric::~Lyric() {
  delete ui;
  delete Net_Request_details;
}

void Lyric::setAlbumImage(QString &ImagePath) {
  ui->album_image->setStyleSheet(
      QString("border-image: url(%1);").arg(ImagePath));
}

void Lyric::setMessage(QString &Title, QString &Album, QString &Artist) {
  ui->labe_Title->setText(Title);
  ui->labe_album->setText(Album);
  ui->labe_Aritst->setText(Artist);
}

void Lyric::on_btn_close_clicked() {
  close();
  emit SIGN_Close(); //关闭歌词界面信号
}

void Lyric::on_setLablePlayTime() {
  if (!getListLyricsTime().empty() &&
      Player::Play->position() >= getListLyricsTime().at(lyricsID)) {
    ui->lab_lyric->setText(getListLyricsText().at(lyricsID));
    ++lyricsID;
  }
}

//获取歌曲ID
void Lyric::get_song_ID(QString &song) {
  //搜索歌曲，之后解析获得音乐ID
  QString URL = QString("http://musicapi.leanapp.cn/search?keywords=%1&limit=1")
                    .arg(song);
  Net_request_lyric->setUrl(QUrl(URL));
  Net_Manage_lyric->get(*Net_request_lyric);

  connect(Net_Manage_lyric, &QNetworkAccessManager::finished, this,
          [=](QNetworkReply *reply) {
            if (reply->error() == QNetworkReply::NoError) {
              QByteArray byte = reply->readAll();
              QString json(byte);
              parse_Artist(json);
            } else {
              std::cout << "网络连接不稳定，无法 获取音乐ID\n\a";
            }
          });
}

void Lyric::Details() {
  int index = Player::Playlist->currentIndex();
  QString URL = QString("http://musicapi.leanapp.cn/song/detail?ids=%1")
                    .arg(Search::getID().at(index));

  Net_Request_details->setUrl(URL);
  Net_Manage_details->get(*Net_Request_details);
}

void Lyric::on_ReplydetailsFinish(
    QNetworkReply *reply) { //获取响应的信息，状态码为200表示正常
  QVariant status_code =
      reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  //无错误返回
  if (reply->error() == QNetworkReply::NoError) {
    QByteArray bytes = reply->readAll(); //获取字节
    QString result(bytes);               //转化为字符串
    parse_details_Json(result); //该函数用于解析api接口返回的json
  } else {
    //处理错误
    QMessageBox::information(this, tr("Error"),
                             tr("歌曲详情搜索失败，请检查网络或搜索接口"));
  }
}

void Lyric::parse_details_Json(QString &json) {
  QNetworkAccessManager *Net_pic = new QNetworkAccessManager(this);
  QNetworkRequest *requst = new QNetworkRequest;

  QString pictrue{};
  QByteArray byte_array;
  QJsonParseError err_rpt;
  QJsonDocument docment =
      QJsonDocument::fromJson(byte_array.append(json), &err_rpt);
  if (err_rpt.error == QJsonParseError::NoError) {
    QJsonObject RootObj = docment.object();
    //解析songs数组
    QJsonValue songs = RootObj.value("songs");
    if (songs.isArray()) {
      QJsonArray songsArr = songs.toArray();
      int cunt = songsArr.count();
      for (int x = 0; x != cunt; x++) {
        QJsonValue songsVal = songsArr.at(x);
        if (songsVal.isObject()) {
          QJsonObject songsObj = songsVal.toObject();
          QJsonValue name = songsObj.value("name");
          Title = songsObj.value("name").toString();
          ui->labe_Title->setText(Title);
          //解析ar数组
          name = songsObj.value("ar");
          if (name.isArray()) {
            QJsonArray arArr = name.toArray();
            if (arArr.at(0).isObject()) {
              QJsonObject arrobj = arArr.at(0).toObject();
              Artist = arrobj.value("name").toString();
              ui->labe_Aritst->setText(Artist);
            }
          }
          //解析Album 和 专辑图片
          name = songsObj.value("al");
          if (name.isObject()) {
            QJsonObject albumobj = name.toObject();
            ui->labe_album->setText(albumobj.value("name").toString());
            //**********获取图片*********//
            pictrue = albumobj.value("picUrl").toString();
            requst->setUrl(QUrl(pictrue));
            Net_pic->get(*requst);
            connect(Net_pic, &QNetworkAccessManager::finished, this,
                    [=](QNetworkReply *reply) {
                      if (reply->error() == QNetworkReply::NoError) {
                        QByteArray byts = reply->readAll();
                        write_File("/pictrue/cover.jpg", byts); //将图片写入文件
                        QDir dir = QDir::currentPath();
                        dir.cdUp(); //上级目录
                        QString path =
                            dir.absolutePath() + "/pictrue/cover.jpg";
                        ui->album_image->setStyleSheet(
                            QString("border-image: url(%1)").arg(path));

                      } else {
                        QMessageBox::information(this, tr("Error"),
                                                 tr("获取专辑图片失败"));
                      }
                      //修改底部显示信息
                      emit SIGN_Lyric_Message(Title, Artist);
                    });
          }
        }
      }
    }
  } else {
    QMessageBox::information(this, tr("error"), tr("获取歌曲详情失败"));
  }

  emit SIGN_SetImage();
  delete requst;
}

void Lyric::parse_Artist(QString &json) {
  QByteArray byte_array;
  QJsonParseError err_rpt;
  QJsonDocument docment =
      QJsonDocument::fromJson(byte_array.append(json), &err_rpt);
  if (err_rpt.error == QJsonParseError::NoError) {
    QJsonObject RootObj = docment.object();
    QJsonValue resultobj = RootObj.value("result");
    if (resultobj.isObject()) {
      QJsonObject resuobj = resultobj.toObject();
      QJsonValue resVal = resuobj.value("songs");
      if (resVal.isArray()) {
        QJsonArray resultArr = resVal.toArray();
        QJsonValue name = resultArr.at(0);
        if (name.isObject()) {
          QJsonObject nameobj = name.toObject();
          QString SongName = nameobj.value("name").toString();
          int ID = nameobj.value("id").toInt();
          //获取歌词
          QString lyrURL =
              QString("http://music.163.com/api/song/media?id=%1").arg(ID);
          QNetworkAccessManager *net_manage = new QNetworkAccessManager(this);
          QNetworkRequest *net_request = new QNetworkRequest(lyrURL);
          net_manage->get(*net_request);

          connect(net_manage, &QNetworkAccessManager::finished, this,
                  [=](QNetworkReply *reply) {
                    QString name = SongName;
                    QByteArray byte = reply->readAll();
                    write_File(QString("/Lyric/%1.lrc").arg(SongName), byte);
                    std::cout << "文件写入完毕\n";
                    readLyricFile(name); //读取歌词
                  });
        }
      }
    }
  }
}

//读取lrc文件
void Lyric::readLyricFile(QString &lrcName) {
  QDir dir = QDir::currentPath();
  dir.cdUp(); //上级目录
  QString lyricpath =
      dir.absolutePath() + QString("/Lyric/%1.lrc").arg(lrcName);
  QFile file(lyricpath);

  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString str = file.readAll();
    QRegularExpression regularExpression(
        "\\[(\\d+)?:(\\d+)?(\\.\\d+)?\\](.*)?");
    int index = 0;
    QRegularExpressionMatch match;
    match = regularExpression.match(str, index);
    if (match.hasMatch()) {
      int totalTime;
      totalTime = match.captured(1).toInt() * 60000 +
                  match.captured(2).toInt() * 1000; /*  计算该时间点毫秒数 */
      QString currentText = QString::fromStdString(
          match.captured(4).toStdString()); /*   获取歌词文本*/
      listLyricsText.push_back(currentText);
      listLyricsTime.push_back(totalTime);
    }

    ui->lab_lyric->clear();
    for (auto x : listLyricsText) {
      if (x.toStdString() == "\n") {
      }
      ui->lab_lyric->setText(x);
    }

    //
    file.close();
  } else {
    std::cout << "歌词文件打开失败\a\n";
    file.close();
  }
}

QList<QString> Lyric::getListLyricsText() const { return listLyricsText; }

QList<int> Lyric::getListLyricsTime() const { return listLyricsTime; }

void write_File(const QString &filename, QByteArray &lyric) {
  QDir dir = QDir::currentPath();
  dir.cdUp(); //上级目录
  QString curPath = dir.absolutePath() + filename;
  FILE *fd = fopen(curPath.toStdString().c_str(), "wb");
  if (fd != nullptr) {
    fwrite(lyric.data(), lyric.size(), 1, fd);
    fclose(fd);
  } else {
    fclose(fd);
  }
}
