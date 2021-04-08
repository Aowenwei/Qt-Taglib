/*
 * @Author: your name
 * @Date: 2021-03-07 22:24:58
 * @LastEditTime: 2021-04-08 15:23:02
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /QtCode/Music/music.cpp
 */
#include "music.h"
#include "./ui_music.h"
#include "collect.h"
#include "lyric.h"
#include "player.h"
#include "search.h"
#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>
#include <QListWidgetItem>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMessageBox>
#include <QTableWidget>
#include <attachedpictureframe.h>
#include <id3v2frame.h>
#include <id3v2tag.h>
#include <mpegfile.h>
Music::Music(QWidget *parent) : QMainWindow(parent), ui(new Ui::Music) {
    ui->setupUi(this);
    setListWidgetStyle();

    MyPlay = new Player(this);
    colle = new collect(this);
    search = new Search(this);
    lyric = new Lyric(this);
    lyric->hide();

    ui->stackedWidget->insertWidget(0, MyPlay);
    ui->stackedWidget->insertWidget(1, colle);
    ui->stackedWidget->insertWidget(2, search);
    // ui->stackedWidget->insertWidget(1, collect);
    //
    connect(ui->listWidget, &QListWidget::currentRowChanged, ui->stackedWidget,
            &QStackedWidget::setCurrentIndex);

    // Set the default volume value
    ui->Sli_volum->setMaximum(100);
    ui->Sli_volum->setValue(50);
    ui->listWidget->setFrameShape(QListWidget::NoFrame);

    // ui->listWidget->setFocusPolicy(Qt::NoFocus);可以改变listwidget的焦点策略
    addlistWiwdgetItem();
    hover();

    connect(Player::Play,&QMediaPlayer::mediaStatusChanged,this,[=]{


    });

    connect(MyPlay->Play, &QMediaPlayer::durationChanged, this, [=]() {
        int curindex = MyPlay->Playlist->currentIndex();
        emit MyPlay->setImage();
        if (MyPlay->state == 0) {
            getCurrMp3Image(colle->Mp3Path.at(curindex));
        }
        if (MyPlay->state == 1) {
            getCurrMp3Image(MyPlay->getPath().at(curindex));
        }

        if (MyPlay->state == 2) {
            lyric->Details();
            int n=  search->getID().at(curindex);
            std::cout<<"*************n = "<<n<<std::endl;
            lyric->getLyric(nullptr,n);
            emit lyric->SIGN_SetImage();
        }
    });

    //设置底部信息
    connect(lyric, &Lyric::SIGN_Lyric_Message, this,
            &Music ::on_setBottomInformation);

    //获取当前歌曲时长
    connect(MyPlay->Play, &QMediaPlayer::durationChanged, this,
            &Music::curSongTime);
    connect(MyPlay, &Player::setImage, this, [=]() {
        ui->btn_stop->setStyleSheet(
            "#btn_stop{border-image:url(:/images/play.png)}");
    });

    //通过播放器发出的信号调用自定义槽函数  信号是播放器自动触发
    connect(MyPlay->Play, &QMediaPlayer::positionChanged, this,
            &Music::onPositionChanged);

    connect(MyPlay->Play, &QMediaPlayer::durationChanged, this,
            &Music::onDurationChanged);
}

Music::~Music() {}

void Music::hover() {
    ui->btn_next->setToolTip(tr("Ctrl+Right"));
    ui->btn_stop->setToolTip(tr("Ctrl+Space"));
    ui->btn_prev->setToolTip(tr("Ctrl+Left"));
}
void Music::addlistWiwdgetItem() {
    QListWidgetItem *MyMusic = new QListWidgetItem(
        QIcon(":/images/myMusic.png"), tr("我的音乐"), ui->listWidget);

    // MyMusic->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    QListWidgetItem *item2 = new QListWidgetItem(QIcon(":/images/collect.png"),
                                                 tr("收藏歌单"), ui->listWidget);

    QListWidgetItem *item3 = new QListWidgetItem(QIcon(":/images/search.png"),
                                                 tr("搜索"), ui->listWidget);
    ui->listWidget->addItem(MyMusic);
    ui->listWidget->addItem(item2);
    ui->listWidget->addItem(item3);
}

void Music::setListWidgetStyle() {
    QFile qss{":/Style/listWidget_Style.qss"};
    if (qss.open(QIODevice::ReadOnly)) {
        ui->listWidget->setStyleSheet(qss.readAll());
        qss.close();
    } else {
        puts("Failed to open file!");
        qss.close();
        exit(EXIT_SUCCESS);
    }
}

void Music::getCurrMp3Image(const QString &path) {
    if (path.isEmpty()) {
        return;
    }
    TagLib::MPEG::File mpeg(path.toStdString().c_str());
    TagLib::ID3v2::FrameList frameList = mpeg.ID3v2Tag()->frameListMap()["APIC"];
    if (!frameList.isEmpty()) {
        //这里取front就好了，back图片一般没用，有人在这里用iterator循环去取，应该就是担心front图片不存在，不过front图片不存在的话，循环也没用吧
        TagLib::ID3v2::AttachedPictureFrame *picFrame =
            static_cast<TagLib::ID3v2::AttachedPictureFrame *>(frameList.front());
        QDir dir = QDir::currentPath();
        dir.cdUp(); //上级目录
        QString jpgFileName = dir.absolutePath() + "/pictrue/cover.jpg";
        FILE *fd = fopen(jpgFileName.toStdString().c_str(), "wb");
        if (fd != NULL) {
            fwrite(picFrame->picture().data(), picFrame->picture().size(), 1, fd);
            fclose(fd);
        }
        puts(" The album picture has been written to cover.jpg");
        ui->btn_pictrue->setStyleSheet(
            QString("border-image:url(%0)").arg(jpgFileName));
        QString title = mpeg.tag()->title().toCString(true);
        QString artist = mpeg.tag()->artist().toCString(true);
        QString Album = mpeg.tag()->album().toCString(true);
        //设置歌词界面的信息
        lyric->setAlbumImage(jpgFileName);
        lyric->setMessage(title, Album, artist);

        ui->lab_meage->setText(QString("  %0\n  %1").arg(title).arg(artist));
    } else {
        puts("there seem to be no picture frames (APIC) frames in this audio file");
    }
}

void Music::Presskey(QKeyEvent *event) {
    // Ctrl+键盘右键
    switch (event->key()) {
    case Qt::Key_M:
        setWindowState(Qt::WindowMaximized);
        break;
    case Qt::Key_N:
        setWindowState(Qt::WindowMinimized);
        break;
    case Qt::Key_Q:
        close();
        break;
    case Qt::Key_Left:
        MyPlay->Playlist->previous();
        MyPlay->Play->play();
        break;
    case Qt::Key_Right:
        MyPlay->Playlist->next();
        MyPlay->Play->play();
        break;
    case Qt::Key_Space:
        on_btn_stop_clicked();
        break;
    case Qt::Key_Up:
        ui->Sli_volum->setValue(curVolume += 1);
        break;
    case Qt::Key_Down:
        ui->Sli_volum->setValue(curVolume -= 1);
        break;
    default:
        event->key();
        break;
    }
}

bool Music::eventFilter(QObject *obj, QEvent *event) {
    if (obj == MyPlay) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            Presskey(keyEvent);
            return true; //该事件已被处理
        } else {
            return false; //其他事件
        }
    }
    if (obj == ui->listWidget) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            Presskey(keyEvent);
            return true; //该事件已被处理
        } else {
            return false; //其他事件
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void Music::install_EeventFilter() {
    // install EeventFilter
    MyPlay->getTableW()->installEventFilter(this);
    ui->listWidget->installEventFilter(this);
    ui->botton_widget->installEventFilter(this);
    search->getSearchTableW()->installEventFilter(this);
    colle->getCollectTabW()->installEventFilter(this);
}

// Pause or play
void Music::on_btn_stop_clicked() {

    if (MyPlay->Playlist->isEmpty()) {
        QMessageBox::critical(this, tr("Warning"), tr("播放列表为空"),
                              QMessageBox::Close);
        return;
    }
    if (MyPlay->Play->state() == QMediaPlayer::PlayingState) {
        ui->btn_stop->setStyleSheet(
            "#btn_stop{border-image:url(:/images/pause.png)}");
        MyPlay->Play->pause();
    } else {
        ui->btn_stop->setStyleSheet(
            "#btn_stop{border-image:url(:/images/play.png)}");
        MyPlay->Play->play();
    }
}

// set silent mode
void Music::on_btn_volum_clicked() {
    if (ui->Sli_volum->value() == 0) {
        ui->Sli_volum->setValue(curVolume);
        MyPlay->setVolum(curVolume);
        ui->btn_volum->setStyleSheet(
            "#btn_volum{border-image:url(:/images/volume.png)}");
    } else {
        //先获取当前音量，保留恢复使用
        curVolume = ui->Sli_volum->value();
        ui->Sli_volum->setValue(0);
        ui->btn_volum->setStyleSheet(
            "#btn_volum{border-image:url(:/images/mute.png)}");
    }
}

// Set the volume
void Music::on_Sli_volum_valueChanged(int value) { MyPlay->setVolum(value); }

// Last song
void Music::on_btn_prev_clicked() {
    //    int curInde = MyPlay->Playlist->currentIndex();
    MyPlay->Playlist->setCurrentIndex(MyPlay->Playlist->previousIndex());
    emit MyPlay->setImage();
    MyPlay->Play->play();
}
// Next song
void Music::on_btn_next_clicked() {
    //    int curInde = MyPlay->Playlist->currentIndex();
    MyPlay->Playlist->setCurrentIndex(MyPlay->Playlist->nextIndex());
    emit MyPlay->setImage();
    MyPlay->Play->play();
}

void Music::onDurationChanged(qint64 duration) {
    ui->horizontalSlider->setMaximum(duration);
    //  int secs = duration / 1000; //所有秒数
    //  int mins = secs / 60;       //分
    //  secs = secs % 60;           //秒
    //  durationTime = QString::asprintf("%d : %d", mins, secs);
    ui->lab_time->setText("  00 : 00");
}

void Music::onPositionChanged(qint64 position) {
    if (ui->horizontalSlider->isSliderDown()) {
        return; //如果手动调整进度条，则不处理
    }
    ui->horizontalSlider->setSliderPosition(position);
    int secs = position / 1000;
    int mins = secs / 60;
    secs = secs % 60;
    ui->lab_time->setText(QString("  0%0 : %1").arg(mins).arg(secs));
}
// Get song length
void Music::curSongTime(qint64 songTime) {
    long long h{}, m{}, s{};
    songTime /= 1000; //获得的时间是以毫秒为单位的
    h = songTime / 3600;
    m = (songTime - h * 3600) / 60;
    s = songTime - h * 3600 - m * 60;
    ui->lab_songDuration->setText(
        QString("  0%1 : %2 ")
            .arg(m)
            .arg(s)); //把int型转化为string类型后再设置为label的text
}

void Music::on_horizontalSlider_sliderReleased() {
    MyPlay->Play->setPosition(ui->horizontalSlider->value());
}

// lyric GUI 不能剧中显示
void Music::on_btn_pictrue_clicked() {
    ui->listWidget->hide();
    ui->stackedWidget->hide();
    ui->horizontalLayout->addWidget(lyric);
    lyric->show();
    //歌词界面关闭信息，恢复初始界面
    connect(lyric, &Lyric::SIGN_Close, this, [this]() {
        ui->listWidget->show();
        ui->stackedWidget->show();
    });
}

void Music::on_setBottomInformation(QString &Title, QString &Artist) {
    ui->lab_meage->setText(QString("%1\n%2").arg(Title).arg(Artist));
    QDir dir = QDir::currentPath();
    dir.cdUp(); //上级目录
    QString path = dir.absolutePath() + "/pictrue/cover.jpg";
    ui->btn_pictrue->setStyleSheet(QString("border-image: url(%1)").arg(path));
}
