var s=function(d) {
    var r = new XMLHttpRequest();
    r.open("GET", "/"+d, true);
    r.onreadystatechange = function () {
    };
    r.send();
};
var dq=function(s,ev,c) { return document.querySelector(s).addEventListener(ev, c); };
window.addEventListener('DOMContentLoaded', function() {
    console.log("ready");
    dq("#panel .on", 'touchstart', function(e) { s("01"); });
    dq("#panel .off", 'touchstart', function(e) { s("00"); });
});