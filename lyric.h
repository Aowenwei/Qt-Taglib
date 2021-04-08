
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
  void setAlbumImage(QString &ImagePath);
  void setMessage(QString &Title, QString &Album, QString &Artist);
  void parse_details_Json(QString &json);
  void getLyric(const QString &rhs = nullptr, const int ID = 0);
  void parse_Artist(QString &json);
signals:
  void SIGN_Lyric_Message(QString &, QString &);
  void SIGN_SetImage();
private slots:
  void on_ReplydetailsFinish(QNetworkReply *reply);
  void on_btn_close_clicked();

private:
  QString Title;
  QString Artist;
  QString parse_Art;
  QNetworkAccessManager *Net_Manage_details;
  QNetworkRequest *Net_Request_details;
  Ui::Lyric *ui;
};

#endif // LYRIC_H
