/**
 * 
 */
var ongoingTouches = {}
var sock = new WebSocket("ws://"+ location.hostname + ":81");

function startup() {
	var el = document.getElementsByTagName("body")[0];
	el.addEventListener("touchstart", handleStart, false);
	el.addEventListener("touchend", handleEnd, false);
	el.addEventListener("touchcancel", handleEnd, false); //handle the same as finger up
	el.addEventListener("touchmove", handleMove, false);
	sock.onopen = sockOpen; 
	sock.onmessage = sockMessage; 
}

startup();

function sockOpen(e) {
	console.log("Socket connected");
	sock.send("HI");
}

function sockMessage(e) {
	console.log("Socket message: ");
	console.log(e.data);
}

function handleStart(e) {
	e.preventDefault();
	var touches = e.changedTouches;
	for (var i = 0; i < touches.length; i++) {
		var t = touches[i];
		if (t.target.nodeName == 'BUTTON') {
			ongoingTouches[t.identifier] = t.pageY;
		}
	}
}

function handleEnd(e) {
	e.preventDefault();
	var touches = e.changedTouches;
	for (var i = 0; i < touches.length; i++) {
		var t = touches[i];
		if (ongoingTouches[t.identifier] != undefined) {
			delete(ongoingTouches[t.identifier]);
			t.target.setAttribute("style", "top:0");
			var dir = t.target.getAttribute("id");
			sock.send(dir + "0");
		}
	}
}

function handleMove(e) {
	e.preventDefault();
	var touches = e.changedTouches;
	for (var i = 0; i < touches.length; i++) {
		var t = touches[i];
		if (ongoingTouches[t.identifier] != undefined) {
			var deltaY = t.pageY - ongoingTouches[t.identifier];
			if (deltaY > 100) { deltaY = 100; }
			if (deltaY < -100) { deltaY = -100; }
			t.target.setAttribute("style", "top:" + deltaY + "px");
			var dir = t.target.getAttribute("id");
			sock.send(dir + deltaY);
		}
	}
}
