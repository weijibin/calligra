/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

import QtQuick 2.0
import org.calligra 1.0

Rectangle {
    id: base;

    property bool keyboardVisible: false;
    onKeyboardVisibleChanged: if (!keyboardVisible) keys.mode = KeyboardModel.NormalMode;

    anchors.left: parent.left;
    anchors.right: parent.right;

    y: parent.height;
    height: parent.height * 0.45;
    color: "#e8e9ea";

    MouseArea {
        anchors.fill: parent;
        onClicked: { }
    }
    SimpleTouchArea {
        anchors.fill: parent;
        onTouched: { }
    }

   Flow {
       visible: keys.useBuiltIn;
       anchors.fill: parent;
       //anchors.margins: 4;

       Repeater {
           model: keys;
           delegate: keyDelegate;
       }
   }

    states: State {
        name: "visible";
        PropertyChanges { target: base; y: base.parent.height * 0.55; keyboardVisible: true; }
    }

    transitions: Transition {
        reversible: true;
        SequentialAnimation {
            NumberAnimation { properties: "y"; }
            PropertyAction { property: "keyboardVisible"; }
        }
    }

    Connections {
        target: Settings;

        onFocusItemChanged: {
            if (Settings.focusItem != null && Settings.focusItem != undefined) {
                if (Settings.focusItem.text == "") {
                    keys.mode = KeyboardModel.CapitalMode;
                }
                if (Settings.focusItem.numeric != undefined && Settings.focusItem.numeric === true) {
                    keys.mode = KeyboardModel.NumericMode;
                }
            }
        }
    }

    KeyboardModel {
        id: keys;
    }

    Component {
        id: keyDelegate;

        Item {
            width: (base.width / 12) * model.width; //Settings.theme.adjustedPixel(40) * model.width;
            height: (base.height - 8) / 4;

            Button {
                anchors {
                    left: parent.left;
                    right: parent.right;
                    top: parent.top;
                    margins: 4;
                }

                height: model.keyType == KeyboardModel.EnterKey && keys.mode == KeyboardModel.NumericMode ? parent.height * 2 - 4: parent.height - 4;

                border.width: model.keyType == KeyboardModel.SpacerKey ? 0 : 2;
                border.color: "white";
                //radius: 8;

                color: {
                    if (model.keyType == KeyboardModel.ShiftKey && keys.mode == KeyboardModel.CapitalMode) {
                        return "#666666";
                    } else if (model.keyType == KeyboardModel.NumericModeKey && keys.mode == KeyboardModel.NumericMode) {
                        return "#666666";
                    } else if(model.keyType == KeyboardModel.SpacerKey) {
                        return "transparent";
                    } else {
                        return "white";
                    }
                }

                text: model.text;
                textColor: "black";//model.keyType != KeyboardModel.SpacerKey ? "white" : "#333333";

                highlight: model.keyType != KeyboardModel.SpacerKey ? true : false;
                highlightColor: "#666666";

                onClicked: {
                    switch(model.keyType) {
                        case KeyboardModel.BackspaceKey:
                            Settings.focusItem.text = Settings.focusItem.text.substring(0, Settings.focusItem.text.length - 1);
                        case KeyboardModel.EnterKey:
                            base.state = "";
                        case KeyboardModel.ShiftKey:
                            keys.mode = keys.mode != KeyboardModel.CapitalMode ? KeyboardModel.CapitalMode : KeyboardModel.NormalMode;
                        case KeyboardModel.LeftArrowKey:
                            Settings.focusItem.cursorPosition -= 1;
                        case KeyboardModel.RightArrowKey:
                            Settings.focusItem.cursorPosition += 1;
                        case KeyboardModel.NumericModeKey:
                            keys.mode = keys.mode != KeyboardModel.NumericMode ? KeyboardModel.NumericMode : KeyboardModel.NormalMode;
                        case KeyboardModel.NormalKey: {
                            Settings.focusItem.text += model.text;
                            if (keys.mode == KeyboardModel.CapitalMode) {
                                keys.mode = KeyboardModel.NormalMode;
                            }
                            return;
                        }
                        case KeyboardModel.CloseKey:
                            base.state = "";
                        default:
                            return;
                    }
                }
            }
        }
    }
}
