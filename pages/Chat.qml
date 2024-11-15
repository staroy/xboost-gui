// Copyright (c) 2014-2024, The Monero Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import QtQuick 2.9
import QtQuick.Controls 2.0
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import FontAwesome 1.0

import "../components" as MoneroComponents
import "../components/effects/" as MoneroEffects

import moneroComponents.Clipboard 1.0
import moneroComponents.Wallet 1.0
import moneroComponents.WalletManager 1.0
import moneroComponents.TransactionHistory 1.0
import moneroComponents.TransactionHistoryModel 1.0
import moneroComponents.AddressBook 1.0
import moneroComponents.AddressBookModel 1.0
import moneroComponents.MessageList 1.0
import moneroComponents.MessageListModel 1.0
import "../js/TxUtils.js" as TxUtils

Rectangle {
    id: pageChat
    color: "transparent"

    property int chatHeight: appWindow.height - 80
    property string currentAddress: ""
    readonly property string vertexColor: MoneroComponents.Style.blackTheme ? "#303030" : "#C9C9C9"
    readonly property string itemBorderColor: MoneroComponents.Style.blackTheme ? "#404040" : "#B9B9B9"
    readonly property string dimmedColor: MoneroComponents.Style.blackTheme ? "#c9c9c9" : "#474747"
    readonly property string dimmed2Color: MoneroComponents.Style.blackTheme ? "#777777" : "#999999"

    function setSubChat(txid)
    {
        currentWallet.messageListModel.setSubChat(txid);
        btnToParent.visible = true
        enableCommentsCheckBox.visible = false
    }

    function setCurrent(address, isMultiUser)
    {
        currentAddress = address
        currentWallet.messageListModel.setCurrent(address, isMultiUser)
        btnToParent.visible = false
        enableCommentsCheckBox.visible = true
    }

    SplitView {
        id: splitView
        orientation: Qt.Horizontal
        implicitWidth: 20
        anchors.fill: parent
        anchors.leftMargin: 0
        anchors.rightMargin: 0
        anchors.topMargin: 0
        anchors.bottomMargin: 0

        handle: Rectangle {
            id: splitViewHandleDelegate
            implicitWidth: 31
            implicitHeight: 31
            border.width: 0
            color: "transparent"
            Rectangle {
                id: splitViewHandleDelegateColored
                x: 15
                y: 0
                width: 1
                height: splitViewHandleDelegate.height
                //color: SplitHandle.pressed ? MoneroComponents.Style.inputBorderColorActive
                //                           : (SplitHandle.hovered ? MoneroComponents.Style.inputBorderColorActive
                //                                                  : MoneroComponents.Style.inputBorderColorInActive)
                color: vertexColor
            }
        }

        ColumnLayout {

            SplitView.preferredWidth: 300
            SplitView.minimumWidth: 200
            SplitView.maximumWidth: 600

            RowLayout {
                Layout.alignment: Qt.AlignTop
                Layout.rightMargin: 4

                MoneroComponents.TextPlain {
                    id: btnLeftPanelVisble
                    color: "transparent"

                    Layout.topMargin: 28
                    Layout.preferredWidth: 28
                    Layout.preferredHeight: 20

                    Text {
                        text: FontAwesome.layerGroup
                        font.family: FontAwesome.fontFamilySolid
                        font.pixelSize: 18
                        color: dimmed2Color
                        font.styleName: "Solid"
                        opacity: 0.75
                    }
                
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: leftPanel.visible = !leftPanel.visible
                    }
                }

                MoneroComponents.LineEdit {
                    id: addressFilter
                    Layout.fillWidth: true
                    fontSize: 16
                    labelFontSize: 14
                    labelText: " "
                    placeholderFontSize: 16
                    placeholderText: qsTr("Address filter") + "..." + translationManager.emptyString
                    readOnly: false
                    clip: true
                    onTextChanged: {
                        currentWallet.chatAddressModel.setFilterString(text);
                    }
                }
            }

            RowLayout {
                id: addressListRow
                property int addressListItemHeight: 50
                Layout.leftMargin: 4

                ListView {
                    id: addressListView
                    Layout.topMargin: 12
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    boundsBehavior: ListView.StopAtBounds
                    interactive: false
                    delegate: Rectangle {
                        id: addressListRect
                        height: addressListRow.addressListItemHeight
                        width: addressListView.width
                        Layout.fillWidth: true
                        color: selected ? (MoneroComponents.Style.blackTheme ? "#505050" : "#b0b0b0") : "transparent"

                        readonly property bool selected: address == currentAddress

                        Rectangle {
                            id: "avatar"
                            radius: 19
                            y:6
                            x:6
                            width: 38
                            height: 38
                            color: AbBackground
                            Label {
                                anchors.centerIn: parent
                                text: Ab
                                font.pixelSize: 16
                                color: AbColor
                            }
                        }

                        Rectangle {
                            color: MoneroComponents.Style.appWindowBorderColor
                            anchors.right: parent.right
                            anchors.left: parent.right
                            anchors.top: parent.top
                            height: 1
                    
                            MoneroEffects.ColorTransition {
                                targetObj: parent
                                blackColor: MoneroComponents.Style._b_appWindowBorderColor
                                whiteColor: MoneroComponents.Style._w_appWindowBorderColor
                            }
                        }
                    
                        Rectangle {
                            anchors.right: parent.right
                            anchors.left: avatar.right
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.topMargin: 5
                            color: "transparent"
                            radius: 5

                            MoneroComponents.Label {
                                id: descriptionLabel
                                color: MoneroComponents.Style.defaultFontColor
                                anchors.left: parent.left
                                anchors.leftMargin: 6
                                fontSize: 16
                                text: description
                                elide: Text.ElideRight
                            }
                    
                            Label {
                                id: unreadCntLabel
                                y: 0
                                x: addressListRect.width - 60 - dummyUnreadCnt.paintedWidth
                                height: 22
                                width: 12 + dummyUnreadCnt.paintedWidth
                                padding: 3
                                color: MoneroComponents.Style.defaultFontColor
                                font.pointSize: 8
                                horizontalAlignment: Qt.AlignCenter
                                text: unreadCount;
                                visible: unreadCount > 0
                                background: Rectangle {
                                    radius: 10
                                    color: selected ? (MoneroComponents.Style.blackTheme ? "#909090" : "White") : (MoneroComponents.Style.blackTheme ? "#505050" : "#b0b0b0")
                                }
                            }

                            Text {
                                id: dummyUnreadCnt
                                visible: false
                                text: unreadCount
                            }

                            Label {
                                id: addressLabel
                                y: 22
                                anchors.left: parent.left
                                anchors.leftMargin: 6
                                font.pixelSize: 14
                                font.family: MoneroComponents.Style.fontMonoRegular.name
                                color: dimmedColor
                                text: TxUtils.addressTruncatePretty(address, addressListView.width < 220 ? 1 : (addressListView.width < 330 ? 2 : 3));
                            }
                        }
                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton
                            onClicked: pageChat.setCurrent(address, isMultiUser)
                        }
                    }
                    ScrollBar.vertical: ScrollBar {}
                }
            }

        }

        ColumnLayout {
            SplitView.minimumWidth: 200
            SplitView.fillWidth: true

            RowLayout {
                Layout.alignment: Qt.AlignTop
                Layout.leftMargin: 4

                Rectangle {
                    id: btnToParent
                    color: "transparent"
                    visible: false

                    width: 20
                    height: 20

                    Layout.topMargin: 28

                    Text {
                        text: FontAwesome.arrowLeft
                        font.family: FontAwesome.fontFamilySolid
                        font.pixelSize: 18
                        color: dimmed2Color
                        font.styleName: "Solid"
                        opacity: 0.75
                    }
                
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            currentWallet.messageListModel.setToParent()
                            btnToParent.visible = false
                            enableCommentsCheckBox.visible = true
                        }
                    }
                }

                MoneroComponents.LineEdit {
                    id: messageFilter
                    Layout.fillWidth: true
                    fontSize: 16
                    labelFontSize: 14
                    labelText: " "
                    placeholderFontSize: 16
                    placeholderText: qsTr("Message filter") + "..." + translationManager.emptyString
                    readOnly: false
                    clip: true
                    onTextChanged: {
                        currentWallet.messageListModel.setFilterString(text);
                    }
                }
            }

            RowLayout {
                id: messageListRow
                property int messageListItemHeight: 22
                Layout.leftMargin: 4

                ListView {
                    id: messageListView
                    Layout.topMargin: 12
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    width: messageListRow.width
                    clip: true
                    boundsBehavior: ListView.StopAtBounds
                    interactive: true
                    delegate: Rectangle {
                        Layout.fillWidth: true
                        readonly property int fieldTextY: 4
                        readonly property int fieldTextX: 8
                        readonly property int lessed: 100
                        readonly property int textItemMinWidth: 260
                        readonly property bool textItemWidthLess: dummyText.paintedWidth < (messageFilter.width - lessed)
                        readonly property int textItemHeight: fieldTextY + (fieldText.paintedHeight < 25 ? 25 : fieldText.paintedHeight + 4)
                        readonly property int textItemWidthCalc: textItemWidthLess ? dummyText.paintedWidth + 18 : messageFilter.width - lessed + 16
                        readonly property int textItemWidth: textItemWidthCalc < textItemMinWidth ? textItemMinWidth : textItemWidthCalc
                        readonly property string backgroundColor: sendByMe ? (MoneroComponents.Style.blackTheme ? "#333333" : "#cccccc") : (MoneroComponents.Style.blackTheme ? "#333333" : "#cccccc")
                        readonly property string timeColor: sendByMe ? (MoneroComponents.Style.blackTheme ? "#777777" : "#999999") : MoneroComponents.Style.dimmedFontColor
                        readonly property string textColor: sendByMe ? dimmedColor : MoneroComponents.Style.defaultFontColor
                        height: 36 + (isEnableComments ? 22 : 0) + (isTitle ? 5 : 0) + textItemHeight

                        Rectangle {
                            visible: isMultiUser && !sendByMe
                            id: "avatar2"
                            radius: 14
                            y: 10
                            x: isTitle ? (messageListRow.width - textItemWidth)/2 - 34 : 6
                            width: 28
                            height: 28
                            color: AbBackground
                            Label {
                                anchors.centerIn: parent
                                text: Ab
                                font.pixelSize: 13
                                color: AbColor
                            }
                        }

                        Rectangle {
                            id: textArea
                            x: isTitle ? (messageListRow.width - textItemWidth)/2 : (sendByMe ? messageListRow.width - textItemWidth : (isMultiUser ? 40 : 10))
                            y: 6
                            height: textItemHeight + 25 + (isEnableComments ? 22 : 0)
                            width: textItemWidth
                            radius: 10
                            color: "transparent"
                            border.color: itemBorderColor
                            border.width: 1
                            property string fullText: text

                            TextEdit {
                                id: fieldText
                                y: fieldTextY
                                x: fieldTextX
                                width: textItemWidth - 16
                                height: textItemHeight - 10
                                text: textArea.fullText
                                readOnly: true
                                wrapMode: Text.WordWrap
                                font.pixelSize: 16
                                color: textColor
                                selectByMouse: true
                                selectedTextColor: MoneroComponents.Style.blackTheme ? "#333333" : "#cccccc"
                                selectionColor: MoneroComponents.Style.blackTheme ? "#cccccc" : "#333333"
                            }

                            Text {
                                id: dummyText
                                visible: false
                                font.pixelSize: 16
                                text: textArea.fullText
                            }

                            Label {
                                id: txidLabel
                                y: textItemHeight
                                x: fieldTextX + 4
                                font.pixelSize: 14
                                font.family: MoneroComponents.Style.fontMonoRegular.name
                                color: dimmed2Color
                                text: textItemWidth < 710 ? TxUtils.txidTruncatePretty(txid, textItemWidth < 325 ? 1 : (textItemWidth < 400 ? 2 : (textItemWidth < 475 ? 3 : (textItemWidth < 550 ? 4 : (textItemWidth < 625 ? 5 : 6))))) : txid;
                            }

                            Label {
                                id: timeLabel
                                y: textItemHeight
                                x: textItemWidth - 142
                                font.pixelSize: 14
                                font.family: MoneroComponents.Style.fontMonoRegular.name
                                color: dimmed2Color
                                text: time
                            }

                            Label {
                                id: textComments
                                y: textItemHeight + 17
                                x: textItemWidth / 2 - 12
                                font.pixelSize: 16
                                font.family: MoneroComponents.Style.fontMonoRegular.name
                                color: textColor
                                text: "..."
                                visible: isEnableComments
                            }

                            Rectangle {
                                id: btnComments
                                color: "transparent"

                                y: textItemHeight + 22
                                x: textItemWidth - 24
                            
                                width: 20
                                height: 20

                                visible: isEnableComments && !isTitle

                                Text {
                                    text: FontAwesome.arrowRight
                                    font.family: FontAwesome.fontFamilySolid
                                    font.pixelSize: 16
                                    color: dimmed2Color
                                    font.styleName: "Solid"
                                    opacity: 0.75
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: pageChat.setSubChat(txid);
                                }
                            }
                        }
                    }
                    ScrollBar.vertical: ScrollBar {}
                    onCountChanged:  {
                        currentIndex = count-1
                    }
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignBottom
                Layout.leftMargin: 4

                MoneroComponents.CheckBox {
                    id: enableCommentsCheckBox
                    width: 30
                    visible: true
                    checked: false
                    tooltip: qsTr("Enable comments") + translationManager.emptyString
                }

                MoneroComponents.LineEditMulti {
                    id: textToSend
                    Layout.bottomMargin: 25
                    Layout.leftMargin: 2
                    fontSize: 16
                    labelFontSize: 14
                    labelText: " "
                    placeholderFontSize: 16
                    placeholderText: qsTr("Message") + "..." + translationManager.emptyString
                    readOnly: false
                    clip: true
                    wrapMode: Text.WrapAnywhere
                    enabled: currentAddress.length > 0
                    focus: true
                    function onEnterPressed() {
                      if(textToSend.text.length > 0) {
                        currentWallet.messageListModel.append(
                            textToSend.text.substring(0, textToSend.text.length),
                            enableCommentsCheckBox.checked,
                            0, false)
                        textToSend.text = ""
                      }
                    }
                }
                MoneroComponents.TextPlain {
                    id: btnMessageSend
                    color: "transparent"

                    Layout.topMargin: 5
                    Layout.leftMargin: 5
                    Layout.preferredWidth: 20
                    Layout.preferredHeight: 20

                    enabled: currentAddress.length > 0 && textToSend.text.length > 0

                    Text {
                        text: FontAwesome.locationArrow
                        font.family: FontAwesome.fontFamilySolid
                        font.pixelSize: 18
                        color: dimmed2Color
                        font.styleName: "Solid"
                        opacity: 0.75
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            currentWallet.messageListModel.append(
                                textToSend.text.substring(0, textToSend.text.length),
                                enableCommentsCheckBox.checked,
                                0, false)
                            textToSend.text = ""
                        }
                    }
                }
            }
        }
    }

    function onPageCompleted() {
        console.log("chat");
        leftPanel.visible = false
        addressListView.model = currentWallet.chatAddressModel;
        addressFilter.text = currentWallet.chatAddressModel.getFilterString();
        messageListView.model = currentWallet.messageListModel
        textToSend.enterPressed.connect(textToSend.onEnterPressed)
        textToSend.returnPressed.connect(textToSend.onEnterPressed)
    }

    function onPageClosed() {
    }
}
