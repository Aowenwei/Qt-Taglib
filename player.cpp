/*
 * @Author: your name
 * @Date: 2021-03-07 23:20:05
 * @LastEditTime: 2021-04-10 23:15:22
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /QtCode/Music/player.cpp
 */
#include "player.h"
#include "ui_player.h"
#include <QAction>
#include <QContextMenuEvent>
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSqlQuery>

int Player::state{1};
int Player::LoacSongSum{};
QSqlDatabase Player::db{};
QSqlQuery *Player::query{};
QMediaPlayer *Player::Play{};
QMediaPlaylist *Player::Playlist{};

void deleteTableWidgetRow(QTableWidget *tablewidget) {
  for (int index = tablewidget->rowCount() - 1; index >= 0; --index) {
    tablewidget->removeRow(index);
  }
}

Player::Player(QWidget *parent) : QWidget(parent), ui(new Ui::Player) {
  ui->setupUi(this);
  initMySQL();
  query = new QSqlQuery(db);
  Play = new QMediaPlayer(this);
  Playlist = new QMediaPlaylist(this), LoadQss();
  ui->btn_add_Music->setStyleSheet("border-image:url(:/images/add.png)");
  ui->btn_add_Music->setToolTip(tr("添加本地音乐"));

  ui->btn_flush->setStyleSheet("border-image:url(:/images/flush.png)");
  ui->btn_flush->setToolTip(tr("刷新列表"));

  //允许QTableWidget接收QWidget::customContextMenuRequested()信号。
  ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  InitTabWidget(ui->tableWidget);
  // Play load failed
  connect(Playlist, &QMediaPlaylist::loadFailed, this, [this]() {
    QMessageBox::critical(this, tr("Error"), tr("Play load failed!"),
                          QMessageBox::No);
  });
}

Player::~Player() { delete ui; }

bool Player::initMySQL() {
  db = QSqlDatabase::addDatabase("QMYSQL");
  db.setHostName("127.0.0.1");
  db.setDatabaseName("Collection_list");
  db.setUserName("root");
  db.setPassword("hg521ewq");
  if (!db.open()) {
    QMessageBox::critical(this, "Can't open database",
                          "Unable to establish a database connection.",
                          QMessageBox::Cancel);
    return false;
  }
  return true;
}

void Player::setVolum(int volume) { Play->setVolume(volume); }

QTableWidget *Player::getTableW() { return ui->tableWidget; }

void Player::getTag(QStringList &Filepaths) {
  tagList.clear();
  QStringList::Iterator it = Filepaths.begin();
  while (it != Filepaths.end()) {
    mpeg = new TagLib::MPEG::File(it->toStdString().c_str());
    TagLib::AudioProperties *aproperties = mpeg->audioProperties();
    Artist = mpeg->tag()->artist().toCString(true);
    Album = mpeg->tag()->album().toCString(true);
    Title = mpeg->tag()->title().toCString(true);
    QString times =
        QDateTime::fromSecsSinceEpoch(aproperties->length()).toString("mm:ss");
    it->toStdString().length();

    QString Size{};
    QFile f(it->toStdString().c_str());
    if (f.open(QIODevice::ReadOnly)) {
      Size = (QString(" %0 M").number((double)f.size() / 1024 / 1024, 'f', 2) +
              " M");
      f.close();
    } else {
      f.close();
    }
    //存放mpeg获取的tag信息
    tagList << Title << Artist << Album << times << Size;
    ++it;
  }
}

void Player::AddToMysql(const int row) {
  QStringList cursong{Filepath.at(row)};
  getTag(cursong);
  query->exec(QString("INSERT INTO Music(Title,Artist,Album,M_time,M_size,"
                      "path)VALUES('%1','%2','%3','%4','%5','%6')")
                  .arg(tagList.at(0))
                  .arg(tagList.at(1))
                  .arg(tagList.at(2))
                  .arg(tagList.at(3))
                  .arg(tagList.at(4))
                  .arg(cursong.at(0)));
}
void Player::LoadQss() {
  QFile qss{":/Style/tabWidget_style.qss"};

  if (qss.open(QIODevice::ReadOnly)) {
    ui->tableWidget->setStyleSheet(qss.readAll());
    qss.close();
  } else {
    puts("Failed to open file!");
    qss.close();
    exit(EXIT_SUCCESS);
  }
}

void Player::on_btn_add_Music_clicked() {
  if (!Filepath.isEmpty()) {
    Filepath.clear();
    LoacSongSum = 0;
    deleteTableWidgetRow(ui->tableWidget);
    Playlist->clear();
  }
  Filepath = QFileDialog::getOpenFileNames(
      this, tr("select all mp3"), tr("/mnt/hgfs/Lo_Music"), tr("mp3(*.mp3)"));
  if (Filepath.isEmpty()) {
    puts("Error");
    exit(EXIT_SUCCESS);
  };
  int len = Filepath.length();
  printf("LNE = %d\n", len);

  // get Song Tag
  getTag(Filepath);
  int taglen = 0;
  for (int index = 0; index != len; index++) {
    Playlist->addMedia(
        QUrl::fromLocalFile(Filepath.at(index).toStdString().c_str()));
    ui->tableWidget->insertRow(index);
    for (int x = 0; x != 5; x++) {
      ui->tableWidget->setItem(index, x,
                               new QTableWidgetItem(tagList.at(taglen)));
      ++taglen;
    }
    ++LoacSongSum; //统计加入的歌曲数量
  }

  ui->lab_LoacSongSum->setText(QString("共 %0 首歌曲").arg(LoacSongSum));
  Play->setPlaylist(Playlist);
  Playlist->setCurrentIndex(0);
}

// right clicked
void Player::on_tableWidget_customContextMenuRequested(const QPoint &pos) {
  if (ui->tableWidget->itemAt(pos) == nullptr) {
    puts("Nullptr ");
    return;
  }
  TableWidget_Menu = new QMenu(ui->tableWidget);
  QAction *play =
      new QAction(QIcon(":/images/pause.png"), tr("播放"), ui->tableWidget);
  play->setShortcut(Qt::CTRL + Qt::Key_0);

  QAction *del =
      new QAction(QIcon(":/images/delete.png"), tr("delete"), ui->tableWidget);
  del->setShortcut(QKeySequence::Delete);

  QAction *Collect = new QAction(QIcon(":/images/collect.png"),
                                 tr("收藏到歌单"), ui->tableWidget);

  TableWidget_Menu->addAction(play);
  TableWidget_Menu->addAction(del);
  TableWidget_Menu->addSeparator(); //设置间隔期
  TableWidget_Menu->addAction(Collect);

  QTableWidgetItem *item = ui->tableWidget->itemAt(pos);

  // play song
  connect(play, &QAction::triggered, this, [=]() {
    Playlist->setCurrentIndex(item->row());
    emit setImage();
    Play->play();
  });

  // delete song
  connect(del, &QAction::triggered, this, [=]() {
    --LoacSongSum;
    int index = item->row();
    Playlist->removeMedia(index);
    ui->tableWidget->removeRow(index);
    Filepath.removeAt(index);
    tagList.removeAt(index);
    ui->lab_LoacSongSum->setText(QString("共 %0 首歌曲").arg(LoacSongSum));
  });

  // Add to MySQL
  connect(Collect, &QAction::triggered, this,
          [this]() { AddToMysql(ui->tableWidget->currentRow()); });
  puts(QString("item = %0").arg(item->row()).toStdString().c_str());
  TableWidget_Menu->exec(QCursor::pos());
}

/*
 * 查询播放列表的歌曲
 * 判断输入是否为空？全部显示（结束）：进一步判断
 * 获取符合条件的行号
 * 隐藏所有行
 * 判断行号是否为空？（是，结束操作）：否，显示相关行（结束）
 */
void Player::on_line_search_textChanged(const QString &lab_text) {
  //  QString searchText = ui->line_search->text();  get lable text
  const int curRow = ui->tableWidget->rowCount();
  if ("" == lab_text) {
    for (int i = 0; i != curRow; i++) {
      ui->tableWidget->setRowHidden(i, false);
    }
  } else {
    //列出所有的条件的cell索引
    QList<QTableWidgetItem *> item =
        ui->tableWidget->findItems(lab_text, Qt::MatchContains);
    for (int i = 0; i != curRow; i++) {
      //隐藏所有行
      ui->tableWidget->setRowHidden(i, true);
    }
    if (!item.isEmpty()) {
      //打印查询到的结果
      for (int i = 0; i != item.count(); i++) {
        ui->tableWidget->setRowHidden(item.at(i)->row(), false);
      }
    }
  }
}

// Double clicked to play music
void Player::on_tableWidget_cellDoubleClicked(int row, int) {
  if (LoacSongSum != 0) { //如果播放列不是空的，那么清空
    Playlist->clear();
    LoacSongSum = 0;
  }
  //    state = 1 设置状态好获取mp3的图片等信息
  state = 1;
  for (int index = 0; index != Filepath.length(); index++) {
    Playlist->addMedia(QUrl::fromLocalFile(Filepath.at(index)));
    ++LoacSongSum; //统计加入的歌曲数量
  }
  Play->setMedia(Playlist);
  Playlist->setCurrentIndex(row);
  Play->play();
  // emit setImage(); // changed image
}

//防止删除item时自动切换歌曲
void Player::on_tableWidget_itemChanged(QTableWidgetItem *) {
  //    puts("Items changed\n");
}

//问题未解决，逆序插入问题gicd
void Player::on_btn_flush_clicked() {
  deleteTableWidgetRow(ui->tableWidget);
  Playlist->clear();
  //插入数据到tableWidget
  int index = 0;
  int TagLen = 0;
  getTag(Filepath);
  QStringList::iterator it = Filepath.begin();
  while (it != Filepath.end()) {
    Playlist->addMedia(QUrl::fromLocalFile(*it));
    ui->tableWidget->insertRow(index);
    for (int row = 0; row != 5; row++) {
      ui->tableWidget->setItem(index, row,
                               new QTableWidgetItem(tagList.at(TagLen)));
      ++TagLen;
    }
    ++it;
  }
  Play->setPlaylist(Playlist);
  Playlist->setCurrentIndex(0);
}

void InitTabWidget(QTableWidget *tableWidget) {
  //设置无边框
  // ui->tableWidget->setFrameShape(QFrame::NoFrame);
  //设置触发条件：不可编辑
  tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tableWidget->setSortingEnabled(false); //启动排序
  // item 水平表头自适应大小
  tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  tableWidget->horizontalHeader()->setDefaultSectionSize(35);
  // item 垂直表头自适应大小
  // ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  QStringList HorizontalHeaderItem{QObject::tr("音乐标题"), QObject::tr("歌手"),
                                   QObject::tr("专辑"), QObject::tr("时长"),
                                   QObject::tr("大小")};
  //获取列数
  int column = HorizontalHeaderItem.count();
  tableWidget->setColumnCount(column);
  //设置行高
  tableWidget->setLineWidth(10);
  for (int x = 0; x != column; ++x) {
    tableWidget->setHorizontalHeaderItem(
        x, new QTableWidgetItem(HorizontalHeaderItem.at(x)));
  }
  tableWidget->setAlternatingRowColors(true);
  tableWidget->setSelectionBehavior(
      QAbstractItemView::SelectRows); //设置选择行为时每次选择一行
}
