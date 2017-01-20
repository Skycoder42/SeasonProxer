import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3
import "../../qtandroidstuff/qml"

AlertDialog {
	title: qsTr("Add Anime")
	property var control: null
	width: 350

	GridLayout {
		columns: 3
		columnSpacing: 20
		width: parent.width

		Label {
			text: qsTr("Proxer-ID:")
		}

		TextField {
			Layout.fillWidth: true
			placeholderText: "894"
			focus: true
			selectByMouse: true
			validator: IntValidator {
				bottom: 1
				top: 2147483647
			}

			text: control && control.id > 0 ? control.id : ""
			onEditingFinished: control.id = text
		}

		Item {
			id: previewItem
			Layout.rowSpan: 2
			Layout.preferredHeight: 100
			Layout.preferredWidth: 66

			readonly property bool showLoading: (control && control.loading) || previewImage.status == Image.Loading

			Image {
				id: previewImage
				anchors.fill: parent
				asynchronous: true
				fillMode: Image.PreserveAspectFit
				horizontalAlignment: Image.AlignHCenter
				verticalAlignment: Image.AlignVCenter
				source: control && control.id > 0 ? "https://cdn.proxer.me/cover/%1.jpg".arg(control.id) : ""
				visible: !previewItem.showLoading
			}

			BusyIndicator {
				anchors.fill: parent
				visible: previewItem.showLoading
			}
		}

		Label {
			text: qsTr("Title:")
		}

		TextField {
			Layout.fillWidth: true
			readOnly: true
			selectByMouse: true
			text: control ? control.title : ""
		}
	}

	footer: DialogButtonBox {
		id: btnBox
		standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel

		Component.onCompleted: standardButton(DialogButtonBox.Ok).enabled = Qt.binding(function() { return control && control.loading })
	}

	onAccepted: control.accept(false);
}
