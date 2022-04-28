#include <Arduino.h>

#define FS(x) (__FlashStringHelper*)(x)

const char getResponseHeaders[] PROGMEM =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n"
    "\r\n";

const char getResponseBody[] PROGMEM =
    "<!DOCTYPE HTML>"
    "<html>"
    "<head>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "<meta name=\"apple-mobile-web-app-status-bar-style\" content=\"black\">"
    "<meta name=\"apple-mobile-web-app-capable\" content=\"yes\">"
    "<script>setInterval(() => {fetch('/status').then(r => r.json()).then(r => {document.getElementById('status').innerText = r.status})}, 500)</script>"
    "</head><body style=\"background-color: #000; color: #f00;\">"
    "<h1>Dragon Lair Roof</h1>"
    "<dl><dt>Status:</dt><dd id=\"status\">"
    "</dd></dl>"
    "<form style=\"margin: 1em;\" action=\"/open\" method=\"POST\"><button style=\"padding: 1em;\">Open</button></form>"
    "<form style=\"margin: 1em;\" action=\"/close\" method=\"POST\"><button style=\"padding: 1em;\">Close</button></form>"
    "<form style=\"margin: 1em;\" action=\"/stop\" method=\"POST\"><button style=\"padding: 1em;\">Stop</button></form>"
    "</body></html>\r\n";


const char postResponse[] PROGMEM = "HTTP/1.1 303 See Other\r\nLocation: /\r\n\r\n";
