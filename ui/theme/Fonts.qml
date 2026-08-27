pragma Singleton

import QtQuick 2.15

QtObject {
    property string family: "Press Start 2P"
    property string fallbackFamily: "Monospace"

    property bool mobile: false

    property int bigTitleSize: mobile ? 56 : 128
    property int titleSize: mobile ? 20 : 32
    property int textSize: mobile ? 11 : 16
    property int secondaryTextSize: mobile ? 9 : 12

    readonly property font bigTitle8bit: Qt.font({
                                                     family: family || fallbackFamily,
                                                     pixelSize: bigTitleSize
                                                 })

    readonly property font title8bit: Qt.font({
                                                  family: family || fallbackFamily,
                                                  pixelSize: titleSize
                                              })

    readonly property font text8bit: Qt.font({
                                                 family: family || fallbackFamily,
                                                 pixelSize: textSize
                                             })

    readonly property font underlinedText8bit: Qt.font({
                                                           family: family || fallbackFamily,
                                                           pixelSize: textSize,
                                                           underline: true
                                                       })

    readonly property font secondaryText8bit: Qt.font({
                                                          family: family || fallbackFamily,
                                                          pixelSize: secondaryTextSize
                                                      })
}
