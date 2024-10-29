import QtQuick 2.9
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2

import "../js/Utils.js" as Utils
import "../components" as MoneroComponents

Rectangle {
    id: root
    property alias consoleArea: consoleArea
    color: "transparent"
    Layout.fillWidth: true
    Layout.fillHeight: true
    property int atomicSwapLogHeight: appWindow.height - 90

    ColumnLayout {
        id: atomicSwapLog
        property int itemHeight: 60
        Layout.fillWidth: true
        Layout.fillHeight: true
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        spacing: 10

        MoneroComponents.CheckBox {
            id: slaveAtomicSwapCheck
            visible: true
            enabled: true
            checked: false
            onClicked: appWindow.currentWallet.atomicSwapSecondEnable(checked)
            text: qsTr("Atomic swap process ...") + translationManager.emptyString
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.color: MoneroComponents.Style.inputBorderColorInActive
                border.width: 1
                radius: 4
            }

            Flickable {
                id: flickable
                anchors.fill: parent
                boundsBehavior: isMac ? Flickable.DragAndOvershootBounds : Flickable.StopAtBounds

                TextArea.flickable: TextArea {
                    id : consoleArea
                    color: MoneroComponents.Style.defaultFontColor
                    selectionColor: MoneroComponents.Style.textSelectionColor
                    textFormat: TextEdit.RichText
                    selectByMouse: true
                    selectByKeyboard: true
                    font.family: MoneroComponents.Style.fontRegular.name
                    font.pixelSize: 14
                    wrapMode: TextEdit.Wrap
                    readOnly: true
                    function logCommand(msg){
                        msg = log_color(msg, MoneroComponents.Style.blackTheme ? "lime" : "green");
                        consoleArea.append(msg);
                    }
                    function logMessage(msg){
                        msg = msg.trim();

                        if(msg.toLowerCase()=="/clear"){
                          consoleArea.text = ""
                          return
                        }

                        var color = MoneroComponents.Style.defaultFontColor;
                        if(msg.toLowerCase().indexOf('error') >= 0){
                            color = MoneroComponents.Style.errorColor;
                        } else if (msg.toLowerCase().indexOf('warning') >= 0){
                            color = "#fa6800"
                        }

                        // format multi-lines
                        if(msg.split("\n").length >= 2){
                            msg = msg.split("\n").join('<br>');
                        }

                        log(msg, color);
                    }
                    function log_color(msg, color){
                        return "<span style='color: " + color +  ";' >" + msg + "</span>";
                    }
                    function log(msg, color){
                        var timestamp = Utils.formatDate(new Date(), {
                            weekday: undefined,
                            month: "numeric",
                            timeZoneName: undefined
                        });

                        var _timestamp = log_color("[" + timestamp + "]", MoneroComponents.Style.defaultFontColor);
                        var _msg = log_color(msg, color);
                        consoleArea.append(_timestamp + " " + _msg);

                        // scroll to bottom
                        //if(flickable.contentHeight > content.height){
                        //    flickable.contentY = flickable.contentHeight;
                        //}
                    }
                }

                ScrollBar.vertical: ScrollBar {
                    onActiveChanged: if (!active && !isMac) active = true
                    policy: isMac ? ScrollBar.AsNeeded : ScrollBar.AlwaysOn
                }
            }
        }
    }

    function updateStatus() {
       var msg = appWindow.currentWallet.atomicSwapGetNotify()
       while(msg.length > 0) {
         consoleArea.logMessage(msg)
         msg = appWindow.currentWallet.atomicSwapGetNotify()
       }
    }

    Component.onCompleted: {
    }
}
