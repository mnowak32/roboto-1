/**
 * 
 */
var ongoingTouches = {}
//var sock = new WebSocket
function startup() {
	var el = document.getElementsByTagName("body")[0];
	el.addEventListener("touchstart", handleStart, false);
	el.addEventListener("touchend", handleEnd, false);
	el.addEventListener("touchcancel", handleEnd, false); //handle the same as finger p
	el.addEventListener("touchmove", handleMove, false);
}

startup();

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
			t.target.setAttribute("style", "top:" + deltaY + "px");
		}
	}
}
