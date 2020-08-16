import QtQuick 2.12
import QtQuick.Layouts 1.12
import SceneGraphRendering 1.0

Rectangle{
    id: playerScrennRoot
    property Item grid: grid

    GridLayout{
        id: grid
        property int screenCount: 0
        anchors.fill: parent
        rowSpacing: 5
        columnSpacing: 5
    }

    function initItem(itemObj, args)
    {
        itemObj.Layout.fillWidth = true
        itemObj.Layout.fillHeight = true
        itemObj.Layout.row = args[0]
        itemObj.Layout.column = args[1]
        itemObj.Layout.rowSpan = args[2]
        itemObj.Layout.columnSpan = args[3]
        itemObj.color = 'black'
//        itemObj.children[0].slotPlay('F:/qt_project/VideoWidget/test.mp4', 'cuda_plugin')
    }

    function setScreen(screen_info)
    {
        var rows = screen_info['rows']
        var cols = screen_info['cols']
        var bigFrameRow = screen_info['bigFrameRow']
        var bigFrameCol = screen_info['bigFrameCol']
        var bigFrameRowSpan = screen_info['bigFrameRowSpan']
        var bigFrameColSpan = screen_info['bigFrameColSpan']

        grid.rows = rows
        grid.columns = cols
        var newCount = rows * cols - bigFrameRowSpan * bigFrameColSpan + 1
        var oldCount = grid.screenCount;
        if(newCount == oldCount)return

        var realFrameLayStack = new Array()
        for(var i = oldCount - 1; i >= 0; --i){
            realFrameLayStack.push(grid.children[i]);
        }

        var qml_str =  'import QtQuick 2.12
                        import SceneGraphRendering 1.0
                        Rectangle {
                            VideoItem{
                                id: player
                                anchors.fill: parent
                                mirrorVertically: true
                                onSigVideoStarted: {
                                    errorText.visible = false
                                    videoTm.stop()
                                }
                                onSigError: {
                                    errorText.text = msg
                                    errorText.visible = true
                                    videoTm.start()
                                }
                            }

                            Timer{
                                id: videoTm
                                triggeredOnStart: false
                                repeat: true
                                interval: 5000
                                onTriggered: {
                                    player.slotPlay(player.url(), player.deviceName())
                                }
                            }

                            Text{
                                id: errorText
                                visible: false
                                anchors.centerIn: parent
                                color: \'red\'
                            }

                            function play(url, device){
                                videoTm.stop()
                                errorText.visible = false
                                player.slotPlay(url, device)
                            }
                        }'
        var loopCount = bigFrameRow * cols;                                   //bigFrameRow 启示位置0， cols 总列， bigFrameCol 0，rows 总行，bigFrameRowSpan 大窗占有行
        var _row = -1, _col = -1, _rowSpan = -1 , _colSpan = -1;
        for(var i = 0 ; i < loopCount ; i ++){
            _row = i / bigFrameCol; _col = i % bigFrameCol; _rowSpan = 1; _colSpan = 1;
            if(realFrameLayStack.length){
                var itemObj = realFrameLayStack.pop();
                initItem(itemObj, [_row, _col, _rowSpan, _colSpan])
            }else{
                var itemObj = Qt.createQmlObject(qml_str,
                                                 grid,
                                                 "dynamicSnippet1")
                initItem(itemObj, [_row, _col, _rowSpan, _colSpan])
            }
        }

        loopCount = bigFrameCol * bigFrameRowSpan;
        for(var i = 0; i < loopCount; i ++){
            _row = bigFrameRow + i / bigFrameCol; _col = i % bigFrameCol; _rowSpan = 1; _colSpan = 1;
            if(realFrameLayStack.length){
                var itemObj = realFrameLayStack.pop();
                initItem(itemObj, [_row, _col, _rowSpan, _colSpan])
            }else{
                var itemObj = Qt.createQmlObject(qml_str,
                                                 grid,
                                                 "dynamicSnippet1")
                initItem(itemObj, [_row, _col, _rowSpan, _colSpan])
            }
        }

        _row = bigFrameRow; _col = bigFrameCol; _rowSpan = bigFrameRowSpan; _colSpan = bigFrameColSpan;
        if(realFrameLayStack.length){
            var itemObj = realFrameLayStack.pop();
            initItem(itemObj, [_row, _col, _rowSpan, _colSpan])
        }else{
            var itemObj = Qt.createQmlObject(qml_str,
                                                 grid,
                                                 "dynamicSnippet1")
            initItem(itemObj, [_row, _col, _rowSpan, _colSpan])
        }

        var colStart = bigFrameCol + bigFrameColSpan;
        var colCount = cols - colStart;
        loopCount = colCount * bigFrameRowSpan;
        for(var i = 0; i < loopCount; i++){
            _row = bigFrameRow + i / colCount; _col = colStart + i % colCount; _rowSpan = 1; _colSpan = 1;
            if(realFrameLayStack.length){
                var itemObj = realFrameLayStack.pop();
                initItem(itemObj, [_row, _col, _rowSpan, _colSpan])
            }else{
                var itemObj = Qt.createQmlObject(qml_str,
                                                 grid,
                                                 "dynamicSnippet1")
                initItem(itemObj, [_row, _col, _rowSpan, _colSpan])
            }
        }

        var rowStart = bigFrameRow + bigFrameRowSpan;
        var rowCount = rows - rowStart;
        loopCount = rowCount * cols;
        for(var i = 0; i < loopCount; i++){
            _row = rowStart + i / cols; _col = i % cols; _rowSpan = 1; _colSpan = 1;
            if(realFrameLayStack.length){
                var itemObj = realFrameLayStack.pop();
                initItem(itemObj, [_row, _col, _rowSpan, _colSpan])
            }else{
                var itemObj = Qt.createQmlObject(qml_str,
                                                 grid,
                                                 "dynamicSnippet1")
                initItem(itemObj, [_row, _col, _rowSpan, _colSpan])
            }
        }

        if(realFrameLayStack.length){
            while (realFrameLayStack.length) {
                var itemObj = realFrameLayStack.pop();
                itemObj.destroy();
            }
        }

        grid.screenCount = newCount
    }
}
