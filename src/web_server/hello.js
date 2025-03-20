var JoystickController = /** @class */ (function () {
    function JoystickController(stickID, maxDistance, deadzone) {
        this.stick = document.getElementById(stickID);
        this.maxDistance = maxDistance;
        this.deadzone = deadzone;
        this.isDragging = false;
        this.startX = 0;
        this.startY = 0;
        this.touchID = null;
        this.value = { x: 0, y: 0, distance: 0 };
        this.socket = new WebSocket("wss://".concat(window.location.hostname, "/cmd"));
        this.socket.onopen = function () {
            console.log('WebSocket connection established');
            document.getElementById("x_point").innerText = "0.00";
            document.getElementById("y_point").innerText = "0.00";
            // console.log(document.getElementById("x_point")!.innerText);
            // console.log(document.getElementById("y_point")!.innerText);
        };
        this.socket.onerror = function (error) { return console.error('WebSocket error:', error); };
        this.socket.onmessage = function (event) {
            var data = JSON.parse(event.data);
            console.log('Received data:', data);
            var leftVelocityElement = document.getElementById("left_velocity");
            var rightVelocityElement = document.getElementById("right_velocity");
            var xPointElement = document.getElementById("x_point");
            var yPointElement = document.getElementById("y_point");
            if (leftVelocityElement && rightVelocityElement) {
                leftVelocityElement.innerText = data.leftVelocity.toFixed(2);
                rightVelocityElement.innerText = data.rightVelocity.toFixed(2);
            }
            if (xPointElement && yPointElement) {
                xPointElement.innerText = data.xPoint.toFixed(2);
                yPointElement.innerText = data.yPoint.toFixed(2);
            }
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
        this.value = { x: xPosition2 / this.maxDistance, y: yPosition2 / this.maxDistance, distance: distance2 };
        this.updateUI();
        if (this.socket.readyState === WebSocket.OPEN) {
            this.socket.send(JSON.stringify({ xPoint: this.value.x, yPoint: this.value.y, distance: this.value.distance }));
        }
    };
    JoystickController.prototype.handleUp = function (event) {
        if (!this.isDragging)
            return;
        this.isDragging = false;
        this.stick.style.transition = ".2s";
        this.stick.style.transform = "translate(-50%, -50%)";
        this.value = { x: 0, y: 0, distance: 0 };
        this.updateUI();
        if (this.socket.readyState === WebSocket.OPEN) {
            this.socket.send(JSON.stringify({ xPoint: this.value.x, yPoint: this.value.y, distance: this.value.distance }));
        }
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
