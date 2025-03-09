class JoystickController {
    private stick: HTMLElement;
    private maxDistance: number;
    private deadzone: number;
    private isDragging: boolean;
    private startX: number;
    private startY: number;
    private touchID: number | null;
    private value: { x: number, y: number };
    private socket: WebSocket;

    constructor(stickID: string, maxDistance: number, deadzone: number) {
        this.stick = document.getElementById(stickID)!;
        this.maxDistance = maxDistance;
        this.deadzone = deadzone;
        this.isDragging = false;
        this.startX = 0;
        this.startY = 0;
        this.touchID = null;
        this.value = { x: 0, y: 0 };

        this.socket = new WebSocket(`ws://${window.location.hostname}/cmd`);
        this.socket.onopen = () => console.log('WebSocket connection established');
        this.socket.onerror = (error) => console.error('WebSocket error:', error);
        this.socket.onmessage = function (event) {
            const data = JSON.parse(event.data);
            console.log('Received data:', data);
            const leftVelocityElement = document.getElementById("left_velocity");
            const rightVelocityElement = document.getElementById("right_velocity");

            if (leftVelocityElement && rightVelocityElement) {
                leftVelocityElement.innerText = data.leftVelocity.toFixed(2);
                rightVelocityElement.innerText = data.rightVelocity.toFixed(2);
            }

            // document.getElementById("x_point")!.innerText = data.xPoint.toFixed(2);
            // document.getElementById("y_point")!.innerText = data.yPoint.toFixed(2);
        };

        this.initEvents();
    }


    private initEvents() {
        this.stick.addEventListener("mousedown", this.handleDown.bind(this));
        this.stick.addEventListener("touchstart", this.handleDown.bind(this));

        document.addEventListener("mousemove", this.handleMove.bind(this), { passive: false });
        document.addEventListener("touchmove", this.handleMove.bind(this), { passive: false });

        document.addEventListener("mouseup", this.handleUp.bind(this));
        document.addEventListener("touchend", this.handleUp.bind(this));
    }


    private handleDown(event: MouseEvent | TouchEvent) {
        event.preventDefault();
        this.isDragging = true;

        if (event instanceof MouseEvent) {
            this.startX = event.clientX;
            this.startY = event.clientY;
        } else if (event instanceof TouchEvent) {
            this.startX = event.changedTouches[0].clientX;
            this.startY = event.changedTouches[0].clientY;
            this.touchID = event.changedTouches[0].identifier;
        }

        this.stick.style.transition = "0s";
    }


    private handleMove(event: MouseEvent | TouchEvent) {
        if (!this.isDragging) return;

        let dx, dy;
        if (event instanceof MouseEvent) {
            dx = event.clientX - this.startX;
            dy = event.clientY - this.startY;
        } else if (event instanceof TouchEvent) {
            // const touch = Array.from(event.changedTouches).find(t => t.identifier === this.touchID);
            const touch = (Array as any).from(event.changedTouches).find((t: any) => t.identifier === this.touchID);
            if (touch) {
                dx = touch.clientX - this.startX;
                dy = touch.clientY - this.startY;
            }
        }

        if (dx === undefined || dy === undefined) return;

        const distance = Math.min(Math.sqrt(dx * dx + dy * dy), this.maxDistance);
        const angle = Math.atan2(dy, dx);

        const xPosition = distance * Math.cos(angle);
        const yPosition = distance * Math.sin(angle);

        this.stick.style.transform = `translate(-50%, -50%) translate(${xPosition}px, ${yPosition}px)`;


        const distance2 = distance < this.deadzone ? 0 : (this.maxDistance / (this.maxDistance - this.deadzone)) * (distance - this.deadzone);
        const xPosition2 = distance2 * Math.cos(angle);
        const yPosition2 = distance2 * Math.sin(angle);
        this.value = { x: xPosition2 / this.maxDistance, y: yPosition2 / this.maxDistance };

        this.updateUI();

        if (this.socket.readyState === WebSocket.OPEN) {
            this.socket.send(JSON.stringify({ xPoint: this.value.x, yPoint: this.value.y }));
        }
    }


    private handleUp(event: MouseEvent | TouchEvent) {
        if (!this.isDragging) return;

        this.isDragging = false;
        this.stick.style.transition = ".2s";
        this.stick.style.transform = "translate(-50%, -50%)";

        this.value = { x: 0, y: 0 };

        this.updateUI();
    }

    private updateUI() {
        document.getElementById("x_point")!.innerText = this.value.x.toFixed(2);
        document.getElementById("y_point")!.innerText = this.value.y.toFixed(2);
    }
}
window.addEventListener("load", () => {
    const joystick = new JoystickController("joystick", 50, 7);
});
