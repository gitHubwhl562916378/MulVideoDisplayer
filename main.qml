import QtQuick 2.12
import QtQuick.Controls 2.12
import SceneGraphRendering 1.0

ApplicationWindow{
    visible: true
    visibility: ApplicationWindow.FullScreen
//    flags: Qt.Window | Qt.FramelessWindowHint
    title: qsTr('多路视频播放器')

    FileHelper{
        id: fileHelper
    }

    PlayerScreen{
        id: playerScreen
        property var screens_ini: null
        property var urls_config: null
        property int screenIndex: 0
        property int playIndex: 0
        anchors.fill: parent

        Timer{
            id: playerTimer
            triggeredOnStart: true
            repeat: true
            onTriggered: {
                if(triggeredOnStart)
                {
                    for(var i = 0; i < parent.grid.children.length; i++)
                    {
                        if(parent.playIndex == parent.urls_config.length){
                            parent.playIndex = 0;
                            break;
                        }

                        var player = parent.grid.children[i];
                        if(player != undefined)
                        {
                            player.play(parent.urls_config[parent.playIndex]['url'], parent.urls_config[parent.playIndex]['device']);
                            parent.playIndex++;
                        }
                    }
                    triggeredOnStart = false
                }

                var sIndex = parent.screenIndex++ % parent.grid.children.length;
                if(parent.playIndex == parent.urls_config.length){
                    parent.playIndex = 0;
                }

                var player = parent.grid.children[sIndex];
                if(player != undefined)
                {
                    player.play(parent.urls_config[parent.playIndex]['url'], parent.urls_config[parent.playIndex]['device']);
                    parent.playIndex++;
                }
            }
        }
    }

    Component.onCompleted: {
        playerScreen.screens_ini = fileHelper.slotReadIni('config.ini')
        playerScreen.urls_config = fileHelper.slotReadUrl('urls.txt')
        playerTimer.interval = playerScreen.screens_ini['timeout']
        playerScreen.setScreen(playerScreen.screens_ini['default_screen'])
        playerTimer.start()
    }
}
