# How to Run the Camera WebSocket Stream Code

> Note: This code is designed for use with the OV2640 camera module (2MP, 1/4" CMOS, UXGA 1600x1200, supports JPEG, YUV, RGB output formats).

## Steps

1. Open Your Web Browser  
   Use a modern browser like Chrome, Firefox, or Edge.

2. Access the Target Web Page  
   Navigate to the web page where you want to display the camera stream.

3. Open Developer Tools  
   - Press F12 on your keyboard, or  
   - Right-click anywhere on the page and select "Inspect", then go to the Console tab.

4. Paste the Code  
   - Copy the code below.
   - Paste it into the Console and press Enter.

5. Replace the IP Address  
   - In the code, replace `ws://<YOUR_CAMERA_IP>:82` with the actual IP address and port of your camera's WebSocket server.


---

**Requirements:**  
- Your HTML must include an element with the ID `camera-stream`.
- The camera must support WebSocket streaming and be accessible from your network.
- If you encounter errors, verify the IP address, port, and network connectivity.

---


## Copy the code below, paste it into the console, and press Enter:

// Create a WebSocket connection to the camera

const cameraWS = new WebSocket("ws://<YOUR_CAMERA_IP>:82"); // <-- Replace <YOUR_CAMERA_IP> with your camera's IP address
console.log("Connecting to camera WebSocket...");

// Set up the image element 
const img = document.getElementById('camera-stream');

// Process incoming binary frames
cameraWS.onmessage = function(event) {
  if (event.data instanceof Blob) {
    const url = URL.createObjectURL(event.data);
    img.src = url;
    // Clean up previous blob URL to prevent memory leaks
    requestAnimationFrame(() => URL.revokeObjectURL(img.src));
  }
};

// Handle connection events
cameraWS.onopen = () => console.log("Camera WebSocket connected!");
cameraWS.onerror = (e) => console.error("Camera WebSocket error:", e);
cameraWS.onclose = () => console.log("Camera WebSocket disconnected");

// Modify the interface's camera control functions to use our WS connection
window.startVideoStreamOriginal = window.startVideoStream;
window.startVideoStream = function() {
  if (cameraWS.readyState !== WebSocket.OPEN) {
    showToast('Camera not connected', 'error');
    return;
  }
  document.getElementById('stream-overlay').classList.add('hidden');
  document.getElementById('camera-toggle').innerHTML = '<i class="fas fa-pause"></i><span>Pause Feed</span>';
  document.getElementById('camera-toggle').disabled = false;
};
