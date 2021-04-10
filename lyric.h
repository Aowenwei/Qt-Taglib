#ifndef LYRIC_H
#define LYRIC_H

#include <QWidget>
class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;
void write_File(const QString &filename, QByteArray &lyric);
namespace Ui {
class Lyric;
}

class Lyric : public QWidget {
  Q_OBJECT

signals:
  void SIGN_Close();

public:
  explicit Lyric(QWidget *parent = nullptr);
  ~Lyric();
  void Details(); //歌曲详情
  void get_song_ID(QString &songNmae);
  void setAlbumImage(QString &ImagePath);
  void setMessage(QString &Title, QString &Album, QString &Artist);
  void parse_details_Json(QString &json);
  void parse_Artist(QString &json);
  void readLyricFile(QString &lrcName);
  QList<QString> getListLyricsText() const;
  QList<int> getListLyricsTime() const;
  void on_setLablePlayTime();

signals:
  void SIGN_Lyric_Message(QString &, QString &);
  void SIGN_SetImage();
private slots:
  void on_ReplydetailsFinish(QNetworkReply *reply);
  void on_btn_close_clicked();

private:
  QString Title;
  QString Artist;

  QNetworkAccessManager *Net_Manage_details;
  QNetworkRequest *Net_Request_details;

  QNetworkAccessManager *Net_Manage_lyric;
  QNetworkRequest *Net_request_lyric;

  int lyricsID{0};
  QList<QString> listLyricsText;
  QList<int> listLyricsTime;
  Ui::Lyric *ui;
};

#endif // LYRIC_H
