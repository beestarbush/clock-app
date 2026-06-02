import QtQuick

Item {
	id: pendulum

	readonly property int pendulumAngle: 10
	property alias rodColor: rod.color
	property alias bobColor: bob.color
	property bool active: false
	property bool swingPositive: true

	// Call this once per second from the clock app's ticked() signal
	function tick(tickPhase) {
		if (!active) {
			return;
		}
		swingPositive = tickPhase
		swingAnimation.stop()
		swingAnimation.start()
	}

	Rectangle {
		id: rod
		width: pendulum.width / 10
		height: pendulum.height / 2
		radius: 25
		color: Color.black
		anchors.top: parent.top
		anchors.horizontalCenter: parent.horizontalCenter

		transform: Rotation {
			id: swing
			origin.x: rod.width / 2
			origin.y: 0
			angle: pendulumAngle
		}

		Circle {
			id: bob
			width: pendulum.height / 1.25
			height: width
			color: Color.green1
			anchors.top: rod.bottom
			anchors.topMargin: -(height / 2)
			anchors.horizontalCenter: parent.horizontalCenter
		}

		NumberAnimation {
			id: swingAnimation
			target: swing
			property: "angle"
			to: swingPositive ? pendulumAngle : -pendulumAngle
			duration: 1000
			easing.type: Easing.InOutSine
		}
	}
}
