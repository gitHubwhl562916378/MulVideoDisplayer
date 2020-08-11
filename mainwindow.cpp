#include <QHBoxLayout>
#include <QSettings>
#include <QFile>
#include <QMenu>
#include <QTimer>
#include "realplaymanager.h"
#include "player.h"
#include "mainwindow.h"

#pragma execution_character_set("utf-8")
MainWindow::MainWindow(QWidget *parent)
    : WidgetI(parent)
{
    playTimer_ = new QTimer(this);
    connect(playTimer_, SIGNAL(timeout()), this, SLOT(slotVideoTimeout()));
    playManager_ = new RealPlayManager;

    menu_ = new QMenu(playManager_);
    QSettings setting("config.ini",QSettings::IniFormat);
    playTimer_->setInterval(setting.value("VideoScreen/timeout").toInt() * 1000);
    int default_screen = setting.value("VideoScreen/default").toInt();
    int count = setting.beginReadArray("VideoScreen");
    for(int i = 0; i < count; i++){
        setting.setArrayIndex(i);
        QStringList scrennInfoList = setting.value("screen").toString().split(' ');

        if(scrennInfoList.size() != 6){
            continue;
        }
        int rows = scrennInfoList.first().toInt();
        int cols = scrennInfoList.at(1).toInt();
        int bigFrameRow = scrennInfoList.at(2).toInt();
        int bigFrameCol = scrennInfoList.at(3).toInt();
        int bigFrameRowSpan = scrennInfoList.at(4).toInt();
        int bigFrameColSpan = scrennInfoList.at(5).toInt();
        int showCount = rows * cols - bigFrameRowSpan * bigFrameColSpan + 1;
        menu_->addAction(tr("%1屏").arg(showCount), this, [=]{
            playManager_->splitScreen(rows, cols, bigFrameRow, bigFrameCol, bigFrameRowSpan, bigFrameColSpan);
        });
        if(default_screen == i)
        {
            playManager_->splitScreen(rows, cols, bigFrameRow, bigFrameCol, bigFrameRowSpan, bigFrameColSpan);
        }
    }
    setting.endArray();
    connect(playManager_, &RealPlayManager::customContextMenuRequested, playManager_, [=](const QPoint p){
        menu_->move(playManager_->mapToGlobal(p));
        menu_->show();
    });
    playManager_->setContextMenuPolicy(Qt::CustomContextMenu);

    QHBoxLayout *mainLay = new QHBoxLayout;
    mainLay->addWidget(playManager_);
    mainLay->setMargin(0);
    setLayout(mainLay);

    QFile urlFile("urls.txt");
    if(!urlFile.open(QIODevice::ReadOnly))
        return;

    while (!urlFile.atEnd()) {
        QByteArray line = urlFile.readLine().simplified();
        QList<QByteArray> values = line.split(' ');
        if(values.count() != 2){
            continue;
        }
        urlList_ << qMakePair(values.at(0),values.at(1));
    }
    urlFile.close();
}

MainWindow::~MainWindow()
{

}

void MainWindow::setUserStyle(int s)
{

}

bool MainWindow::event(QEvent *event)
{
    if(event->type() != QEvent::Show || !isFirst_){
        return WidgetI::event(event);
    }

    for(int i = 0; i < playManager_->screenCount(); i++)
    {
        if(playIndex_ == urlList_.count()){
            playIndex_ = 0;
            break;
        }

        Player* player = playManager_->indexWidget(i);
        if(player)
        {
            player->startPlay(urlList_.at(playIndex_).first, urlList_.at(playIndex_).second);
            playIndex_++;
        }
    }
    playTimer_->start();
    isFirst_ = false;

    return WidgetI::event(event);
}

void MainWindow::slotVideoTimeout()
{
    int sIndex = screenIndex_++ % playManager_->screenCount();
    if(playIndex_ == urlList_.count()){
        playIndex_ = 0;
    }

    Player* player = playManager_->indexWidget(sIndex);
    if(player)
    {
        player->startPlay(urlList_.at(playIndex_).first, urlList_.at(playIndex_).second);
        playIndex_++;
    }
}
