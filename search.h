#ifndef SEARCH_H
#define SEARCH_H

#include <QWidget>
class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;
class QTableWidget;
namespace Ui {
class Search;
}

class Search : public QWidget {
  Q_OBJECT

public:
  explicit Search(QWidget *parent = nullptr);
  ~Search();
  void parseJson(QString &json);
  void playerUrl(const int ID);
  void setMessage(QString &Title, QString &Album, QString &Artist);
  QTableWidget *getSearchTableW();
  static QVector<int> getID() { return Search::Song_ID; };

private slots:
  void on_line_search_returnPressed();
  void on_btn_enter_clicked();
  void on_ReplyFinish(QNetworkReply *reply);
  void on_tableWidget_cellDoubleClicked(int row, int column);

private:
  static QVector<int> Song_ID;
  QNetworkAccessManager *Net_Manage;
  QNetworkRequest *Net_Request;

  Ui::Search *ui;
};

#endif // SEARCH_H
