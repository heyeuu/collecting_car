#include <pgmspace.h>

namespace web {
const char index_html[] PROGMEM
    = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
    <title>ESP Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
    <style>
        html {
            font-family: Arial, Helvetica, sans-serif;
            text-align: center;
        }

        h1 {
            font-size: 1.8rem;
            color: white;
        }

        h2 {
            font-size: 1.5rem;
            font-weight: bold;
            color: #143642;
        }

        .topnav {
            overflow: hidden;
            background-color: #143642;
        }

        body {
            margin: 0;
        }

        .content {
            padding: 30px;
            max-width: 600px;
            margin: 0 auto;
        }

        .card {
            background-color: #F8F7F9;
            ;
            box-shadow: 2px 2px 12px 1px rgba(140, 140, 140, .5);
            padding-top: 10px;
            padding-bottom: 20px;
        }

        .button {
            padding: 15px 50px;
            font-size: 24px;
            text-align: center;
            outline: none;
            color: #fff;
            background-color: #0f8b8d;
            border: none;
            border-radius: 5px;
            -webkit-touch-callout: none;
            -webkit-user-select: none;
            -khtml-user-select: none;
            -moz-user-select: none;
            -ms-user-select: none;
            user-select: none;
            -webkit-tap-highlight-color: rgba(0, 0, 0, 0);
        }

        /*.button:hover {background-color: #0f8b8d}*/
        .button:active {
            background-color: #0f8b8d;
            box-shadow: 2 2px #CDCDCD;
            transform: translateY(2px);
        }

        .state {
            font-size: 1.5rem;
            color: #8c8c8c;
            font-weight: bold;
        }


        .direction {
            font-size: 1.5rem;
            color: #8c8c8c;
            font-weight: bold;
        }

        #joystick-container {
            position: relative;
            width: 150px;
            height: 150px;
            background-color: #f0f0f0;
            border-radius: 50%;
            border: 2px solid #ccc;
            margin: 50px 30px;
        }

        #joystick {
            position: absolute;
            width: 50px;
            height: 50px;
            background-color: #143642;
            border-radius: 50%;
            cursor: pointer;
            left: 50%;
            top: 50%;
            transform: translate(-50%, -50%);
        }
    </style>
    <title>ESP Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
</head>

<body>
    <div class="topnav">
        <h1>ESP WebSocket Server</h1>
    </div>
    <div class="content">
        <div class="card">
            <h2>Output - GPIO 2</h2>
            <p class="state">state: <span id="state">%STATE%</span></p>
            <p><button id="button" class="button">Toggle</button></p>
            <div id="joystick-container">
                <div id="joystick"></div>
            </div>
            <p class="direction">direction:<br><span id="direction">%DIRECTION%</span><br>
            </p>

        </div>
    </div>
    <script>
        let joystick = document.getElementById('joystick');
        let container = document.getElementById('joystick-container');
        let maxDistance = container.offsetWidth / 2 - joystick.offsetWidth / 2;

        var gateway = `ws://${window.location.hostname}/ws`;
        var websocket;
        window.addEventListener('load', onLoad);
        function initWebSocket() {
            console.log('Trying to open a WebSocket connection...');
            websocket = new WebSocket(gateway);
            websocket.onopen = onOpen;
            websocket.onclose = onClose;
            websocket.onmessage = onMessage;
        }
        function onOpen(event) {
            console.log('Connection opened');
        }
        function onClose(event) {
            console.log('Connection closed');
            setTimeout(initWebSocket, 2000);
        }
        function onMessage(event) {
            var message = event.data;
            if (message === "1" || message === "0") {
                var state = message === "1" ? "ON" : "OFF";
                document.getElementById('state').innerHTML = state;
            }

            try {
                var data = JSON.parse(event.data);
                if (data && typeof data === "object" && "X" in data && "Y" in data) {
                    var direction = "X: " + data.X.toFixed(2) + " Y: " + data.Y.toFixed(2);
                    document.getElementById('direction').innerHTML = direction;
                } else {
                    console.warn("Invalid direction data:", event.data);
                }
            } catch (e) {
                console.warn("Invalid JSON data:", event.data, e);
            }

        }

        function onLoad(event) {
            initWebSocket();
            initButton();
            initJoystick();
        }

        function initButton() {
            document.getElementById('button').addEventListener('click', toggle);
        }
        function initJoystick() {
            container.addEventListener('mousedown', startDrag);
            container.addEventListener('touchstart', startDrag);
        }

        function startDrag(event) {
            event.preventDefault();

            let offsetX = event.clientX || event.touches[0].clientX;
            let offsetY = event.clientY || event.touches[0].clientY;

            let containerRect = container.getBoundingClientRect();
            let centerX = containerRect.left + containerRect.width / 2;
            let centerY = containerRect.top + containerRect.height / 2;

            document.addEventListener('mouseup', stopDrag);
            document.addEventListener('touchend', stopDrag);

            document.addEventListener('mousemove', Drag);
            document.addEventListener('touchmove', Drag);

            function Drag(e) {
                let x = (e.clientX || e.touches[0].clientX) - centerX;
                let y = (e.clientY || e.touches[0].clientY) - centerY;

                let distance = Math.min(Math.sqrt(x * x + y * y), maxDistance);
                let angle = Math.atan2(y, x);

                let newX = distance * Math.cos(angle);
                let newY = distance * Math.sin(angle);

                joystick.style.left = `${newX + containerRect.width / 2 - joystick.offsetWidth / 2}px`;
                joystick.style.top = `${newY + containerRect.height / 2 - joystick.offsetHeight / 2}px`;

                let direction = { x: newX / maxDistance, y: newY / maxDistance };
                sendControlSignal(direction);

            }

            function stopDrag() {
                document.removeEventListener('mousemove', Drag);
                document.removeEventListener('touchmove', Drag);
                document.removeEventListener('mouseup', stopDrag);
                document.removeEventListener('touchend', stopDrag);

                joystick.style.left = '50%';
                joystick.style.top = '50%';

                let direction = { x: 0.0, y: 0.0 };
                sendControlSignal(direction);
            }

            function sendControlSignal(direction) {
                if (websocket.readyState === WebSocket.OPEN) {
                    websocket.send(JSON.stringify(direction));
                }
            }
        }

        function toggle() {
            websocket.send('toggle');
        }


    </script>
</body>

</html>
)rawliteral";

}