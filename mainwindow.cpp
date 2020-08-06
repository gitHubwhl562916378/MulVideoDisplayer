#include <QHBoxLayout>
#include <QSettings>
#include <QFile>
#include <QMenu>
#include "realplaymanager.h"
#include "player.h"
#include "mainwindow.h"

#pragma execution_character_set("utf-8")
MainWindow::MainWindow(QWidget *parent)
    : WidgetI(parent)
{
    QSettings configSettings("config.ini",QSettings::IniFormat);
    playManager_ = new RealPlayManager;

    menu_ = new QMenu(playManager_);
    QSettings setting("config.ini",QSettings::IniFormat);
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
        if(i >= urlList_.count()){
            break;
        }
        Player* player = playManager_->indexWidget(i);
        if(player)
        {
            player->startPlay(urlList_.at(i).first, urlList_.at(i).second);
        }
    }
    isFirst_ = true;

    return WidgetI::event(event);
}
