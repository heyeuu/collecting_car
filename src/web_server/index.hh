#include <pgmspace.h>

namespace web {
const char index_html[] PROGMEM
    = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Graceful name Website</title>
    <style>
        #joystick-container {
            position: absolute;
            width: 10vw;
            height: 10vw;
            bottom: clamp(20px, 0.5vh, 24px);
            left: 3vw;
            background-color: #f0f0f0;
            border-radius: 50%;
            border: 2px solid #ccc;
            margin: 50px 30px;
            padding: auto;
        }

        #joystick {
            position: absolute;
            width: 70%;
            height: 70%;
            background-color: #143642;
            border-radius: 50%;
            cursor: pointer;
            left: 50%;
            top: 50%;
            transform: translate(-50%, -50%);
        }

        .comment_text {
            color: #1d586c;
            font-size: 1rem;
        }

        #point_comment {
            position: absolute;
            top: 100%;
            left: 50%;
            transform: translateX(-50%);
            white-space: nowrap;
        }

        #velocity_comment {
            right: 2vw;
            top: 20%;
            font-size: clamp(12px, 2vw, 24px);
            position: absolute;
        }

        .navigation_bar {
            color: #b8eaf4;
            font-size: 2.5rem;
            left: 50%;
            position: relative;
            transform: translateX(-50%);
            text-align: center;
            background-color: #081c47;
            padding: 0.5vw 0.5vw;
            border-radius: 0.5vw;
        }

        .avatar {
            display: flex;
            left: 2%;
            position: relative;
        }

        .avatar img {
            width: 9vw;
            height: vw;
            border-radius: 50%;
            overflow: hidden;
            object-fit: cover;
            margin-right: 1%;
        }

        .avatar p {
            height: fit-content;
            width: auto;
            color: #1d586c;
            font-size: clamp(12px, 2vw, 24px);
            background-color: #80e8f0;
            padding: 0.5vw 0.5vh;
            border-radius: 0.5vw;
            display: inline-block;
        }

        body {
            overflow: hidden;
        }

        .live_image {
            height: 100%;
            bottom: 0vh;
        }
    </style>
</head>

<body>
    <h1 class="navigation_bar">Hello, world!</h1>
    <div class="live_image">
        <div class="avatar">
        <img src="https://pic1.imgdb.cn/item/67d53bc288c538a9b5beb898.jpg" alt="xxx">
        <p>hello dog!</p>
    </div>
        <div class="comment_text">
            <p id="velocity_comment">leftVelocity:<span id="left_velocity">%LEFT_VELOCITY%</span><br>rightVelocity:<span
                    id="right_velocity">%RIGHT_VELOCITY%</span>
            </p>
        </div>

        <div id="joystick-container">
            <div id="joystick"></div>

            <div class="comment_text">
                <p id="point_comment">(x:<span id="x_point">%0.00%</span>,y:<span id="y_point">%0.00%</span>)</p>
            </div>
        </div>
    </div>

    <script>
        var JoystickController = /** @class */ (function () {
    function JoystickController(stickID, maxDistance, deadzone) {
        this.stick = document.getElementById(stickID);
        this.maxDistance = maxDistance;
        this.deadzone = deadzone;
        this.isDragging = false;
        this.startX = 0;
        this.startY = 0;
        this.touchID = null;
        this.value = { x: 0, y: 0 };
        this.socket = new WebSocket("ws://".concat(window.location.hostname, "/cmd"));
        this.socket.onopen = function () { return console.log('WebSocket connection established'); };
        this.socket.onerror = function (error) { return console.error('WebSocket error:', error); };
        this.socket.onmessage = function (event) {
            var data = JSON.parse(event.data);
            console.log('Received data:', data);
            var leftVelocityElement = document.getElementById("left_velocity");
            var rightVelocityElement = document.getElementById("right_velocity");
            if (leftVelocityElement && rightVelocityElement) {
                leftVelocityElement.innerText = data.leftVelocity.toFixed(2);
                rightVelocityElement.innerText = data.rightVelocity.toFixed(2);
            }
            // document.getElementById("x_point")!.innerText = data.xPoint.toFixed(2);
            // document.getElementById("y_point")!.innerText = data.yPoint.toFixed(2);
        };
        this.initEvents();
    }
    JoystickController.prototype.initEvents = function () {
        this.stick.addEventListener("mousedown", this.handleDown.bind(this));
        this.stick.addEventListener("touchstart", this.handleDown.bind(this));
        document.addEventListener("mousemove", this.handleMove.bind(this), { passive: false });
        document.addEventListener("touchmove", this.handleMove.bind(this), { passive: false });
        document.addEventListener("mouseup", this.handleUp.bind(this));
        document.addEventListener("touchend", this.handleUp.bind(this));
    };
    JoystickController.prototype.handleDown = function (event) {
        event.preventDefault();
        this.isDragging = true;
        if (event instanceof MouseEvent) {
            this.startX = event.clientX;
            this.startY = event.clientY;
        }
        else if (event instanceof TouchEvent) {
            this.startX = event.changedTouches[0].clientX;
            this.startY = event.changedTouches[0].clientY;
            this.touchID = event.changedTouches[0].identifier;
        }
        this.stick.style.transition = "0s";
    };
    JoystickController.prototype.handleMove = function (event) {
        var _this = this;
        if (!this.isDragging)
            return;
        var dx, dy;
        if (event instanceof MouseEvent) {
            dx = event.clientX - this.startX;
            dy = event.clientY - this.startY;
        }
        else if (event instanceof TouchEvent) {
            // const touch = Array.from(event.changedTouches).find(t => t.identifier === this.touchID);
            var touch = Array.from(event.changedTouches).find(function (t) { return t.identifier === _this.touchID; });
            if (touch) {
                dx = touch.clientX - this.startX;
                dy = touch.clientY - this.startY;
            }
        }
        if (dx === undefined || dy === undefined)
            return;
        var distance = Math.min(Math.sqrt(dx * dx + dy * dy), this.maxDistance);
        var angle = Math.atan2(dy, dx);
        var xPosition = distance * Math.cos(angle);
        var yPosition = distance * Math.sin(angle);
        this.stick.style.transform = "translate(-50%, -50%) translate(".concat(xPosition, "px, ").concat(yPosition, "px)");
        var distance2 = distance < this.deadzone ? 0 : (this.maxDistance / (this.maxDistance - this.deadzone)) * (distance - this.deadzone);
        var xPosition2 = distance2 * Math.cos(angle);
        var yPosition2 = distance2 * Math.sin(angle);
        this.value = { x: xPosition2 / this.maxDistance, y: yPosition2 / this.maxDistance };
        this.updateUI();
        if (this.socket.readyState === WebSocket.OPEN) {
            this.socket.send(JSON.stringify({ xPoint: this.value.x, yPoint: this.value.y }));
        }
    };
    JoystickController.prototype.handleUp = function (event) {
        if (!this.isDragging)
            return;
        this.isDragging = false;
        this.stick.style.transition = ".2s";
        this.stick.style.transform = "translate(-50%, -50%)";
        this.value = { x: 0, y: 0 };
        this.updateUI();
    };
    JoystickController.prototype.updateUI = function () {
        document.getElementById("x_point").innerText = this.value.x.toFixed(2);
        document.getElementById("y_point").innerText = this.value.y.toFixed(2);
    };
    return JoystickController;
}());
window.addEventListener("load", function () {
    var joystick = new JoystickController("joystick", 50, 7);
});

    </script>
</body>

</html>
)rawliteral";

}