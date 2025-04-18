const char *HTML_CONTENT = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>RC Car Command Center</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css">
  <style>
    :root {
      --primary: #4361ee;
      --primary-light: #4cc9f0;
      --primary-dark: #3a0ca3;
      --secondary: #f72585;
      --secondary-light: #ff9e00;
      --danger: #e63946;
      --success: #2ecc71;
      --dark: #2b2d42;
      --light: #f8f9fa;
      --gray: #8d99ae;
      --card-bg: rgba(255, 255, 255, 0.95);
      --border-radius: 16px;
      --shadow-sm: 0 4px 6px rgba(0, 0, 0, 0.05);
      --shadow-md: 0 10px 15px rgba(0, 0, 0, 0.1);
      --shadow-lg: 0 20px 25px rgba(0, 0, 0, 0.15);
      --transition: all 0.3s cubic-bezier(0.25, 0.8, 0.25, 1);
    }

    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
    }

    body {
      font-family: 'Segoe UI', 'Roboto', system-ui, -apple-system, sans-serif;
      background: linear-gradient(135deg, #4cc9f0 0%, #3a0ca3 100%);
      color: var(--dark);
      line-height: 1.6;
      min-height: 100vh;
      overflow-x: hidden;
      padding: 0;
      margin: 0;
      display: flex;
      flex-direction: column;
    }

    .particles-container {
      position: fixed;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      z-index: -1;
      overflow: hidden;
    }

    .particle {
      position: absolute;
      border-radius: 50%;
      opacity: 0.6;
      animation: float 60s infinite linear;
    }

    @keyframes float {
      0% {
        transform: translateY(0) rotate(0deg);
      }
      100% {
        transform: translateY(-2000px) rotate(360deg);
      }
    }

    .app-container {
      display: flex;
      flex-direction: column;
      flex-grow: 1;
      max-width: 1000px;
      width: 100%;
      margin: 0 auto;
      padding: 20px;
    }

    header {
      text-align: center;
      padding: 20px 0;
      margin-bottom: 30px;
      position: relative;
    }

    .logo-container {
      display: flex;
      align-items: center;
      justify-content: center;
      margin-bottom: 15px;
    }

    .logo {
      width: 60px;
      height: 60px;
      background-color: var(--primary);
      border-radius: 50%;
      display: flex;
      align-items: center;
      justify-content: center;
      color: white;
      font-size: 24px;
      box-shadow: var(--shadow-md);
      margin-right: 15px;
      animation: pulse 2s infinite;
    }

    @keyframes pulse {
      0% {
        box-shadow: 0 0 0 0 rgba(67, 97, 238, 0.7);
      }
      70% {
        box-shadow: 0 0 0 10px rgba(67, 97, 238, 0);
      }
      100% {
        box-shadow: 0 0 0 0 rgba(67, 97, 238, 0);
      }
    }

    h1 {
      font-size: 2.5rem;
      color: white;
      text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.2);
      margin: 0;
      font-weight: 600;
    }

    .tagline {
      color: rgba(255, 255, 255, 0.9);
      font-size: 1.1rem;
      margin-top: 5px;
    }

    .card {
      background-color: var(--card-bg);
      border-radius: var(--border-radius);
      box-shadow: var(--shadow-md);
      padding: 25px;
      transition: var(--transition);
      display: flex;
      flex-direction: column;
    }

    .card:hover {
      box-shadow: var(--shadow-lg);
      transform: translateY(-5px);
    }

    .card-header {
      display: flex;
      align-items: center;
      justify-content: space-between;
      margin-bottom: 20px;
      padding-bottom: 15px;
      border-bottom: 1px solid rgba(0, 0, 0, 0.1);
      flex-shrink: 0;
    }

    .card-title {
      font-size: 1.3rem;
      font-weight: 600;
      color: var(--primary-dark);
      display: flex;
      align-items: center;
      gap: 10px;
    }

    .card-title i {
      font-size: 1.2rem;
      color: var(--primary);
    }

    .status-card {
      display: flex;
      align-items: center;
      gap: 15px;
      background-color: rgba(0, 0, 0, 0.05);
      padding: 15px;
      border-radius: 12px;
      margin-bottom: 15px;
    }

    .status-indicator {
      position: relative;
      width: 50px;
      height: 50px;
      border-radius: 50%;
      background-color: var(--light);
      display: flex;
      align-items: center;
      justify-content: center;
      box-shadow: inset 0 0 5px rgba(0, 0, 0, 0.1);
      flex-shrink: 0;
    }

    .status-dot {
      width: 30px;
      height: 30px;
      border-radius: 50%;
      background-color: var(--danger);
      position: relative;
      transition: var(--transition);
    }

    .status-dot::after {
      content: '';
      position: absolute;
      top: -5px;
      left: -5px;
      right: -5px;
      bottom: -5px;
      border-radius: 50%;
      border: 2px solid var(--danger);
      opacity: 0.5;
      animation: ripple 1.5s linear infinite;
    }

    .connected .status-dot {
      background-color: var(--success);
    }

    .connected .status-dot::after {
      border-color: var(--success);
    }

    @keyframes ripple {
      0% {
        transform: scale(0.8);
        opacity: 1;
      }
      100% {
        transform: scale(1.5);
        opacity: 0;
      }
    }

    .status-info {
      flex: 1;
      min-width: 0;
    }

    .status-label {
      font-weight: 600;
      font-size: 1rem;
    }

    .status-details {
      font-size: 0.9rem;
      color: var(--gray);
      margin-top: 3px;
      word-break: break-word;
    }

    .connect-btn {
      background-color: var(--primary);
      color: white;
      border: none;
      border-radius: 50px;
      padding: 12px 25px;
      font-size: 1rem;
      font-weight: 600;
      cursor: pointer;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 10px;
      transition: var(--transition);
      box-shadow: var(--shadow-sm);
      width: 100%;
      margin-top: auto;
      padding-top: 20px;
      margin-top: 20px;
    }

    .connect-btn:hover {
      background-color: var(--primary-dark);
      transform: translateY(-2px);
      box-shadow: var(--shadow-md);
    }

    .disconnect-btn {
      background-color: var(--danger);
    }

    .disconnect-btn:hover {
      background-color: #c42c34;
    }

    .settings-panel {
      margin-top: 20px;
      margin-bottom: 20px;
    }

    .settings-group {
      margin-bottom: 20px;
    }

    .settings-title {
      font-weight: 600;
      margin-bottom: 10px;
      font-size: 1rem;
      color: var(--primary-dark);
    }

    .settings-option {
      display: flex;
      align-items: center;
      margin-bottom: 10px;
    }

    .toggle-switch {
      position: relative;
      display: inline-block;
      width: 50px;
      height: 26px;
      margin-right: 10px;
      flex-shrink: 0;
    }

    .toggle-switch input {
      opacity: 0;
      width: 0;
      height: 0;
    }

    .toggle-slider {
      position: absolute;
      cursor: pointer;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
      background-color: #ccc;
      transition: .4s;
      border-radius: 34px;
    }

    .toggle-slider:before {
      position: absolute;
      content: "";
      height: 18px;
      width: 18px;
      left: 4px;
      bottom: 4px;
      background-color: white;
      transition: .4s;
      border-radius: 50%;
    }

    input:checked + .toggle-slider {
      background-color: var(--primary);
    }

    input:focus + .toggle-slider {
      box-shadow: 0 0 1px var(--primary);
    }

    input:checked + .toggle-slider:before {
      transform: translateX(24px);
    }

    .option-label {
      flex: 1;
    }

    .toast-container {
      position: fixed;
      bottom: 20px;
      left: 20px;
      z-index: 1050;
      max-width: 350px;
    }

    .toast {
      background-color: white;
      border-radius: 10px;
      box-shadow: var(--shadow-md);
      padding: 15px 20px;
      margin-top: 10px;
      display: flex;
      align-items: center;
      gap: 10px;
      transform: translateX(-120%);
      transition: transform 0.5s cubic-bezier(0.68, -0.55, 0.27, 1.55);
      opacity: 0.95;
    }

    .toast.show {
      transform: translateX(0);
    }

    .toast-icon {
      font-size: 20px;
      width: 24px;
      height: 24px;
      display: flex;
      align-items: center;
      justify-content: center;
      flex-shrink: 0;
    }

    .toast-success .toast-icon {
      color: var(--success);
    }

    .toast-error .toast-icon {
      color: var(--danger);
    }

    .toast-message {
      font-size: 0.9rem;
    }

    .connecting-loader {
      display: inline-block;
      width: 20px;
      height: 20px;
      border: 3px solid rgba(255, 255, 255, 0.3);
      border-radius: 50%;
      border-top-color: white;
      animation: spin 1s ease-in-out infinite;
      margin-left: 10px;
    }

    @keyframes spin {
      to { transform: rotate(360deg); }
    }

    .floating-controls {
      position: fixed;
      bottom: 20px;
      right: 10px;
      z-index: 1000;
      display: flex;
      flex-direction: column;
      align-items: flex-end;
      gap: 10px;
    }

    .toggle-floating-controls {
      background-color: var(--primary);
      color: white;
      border-radius: 50%;
      width: 50px;
      height: 50px;
      display: flex;
      align-items: center;
      justify-content: center;
      box-shadow: var(--shadow-md);
      cursor: pointer;
      transition: background-color 0.2s;
    }

    .toggle-floating-controls:hover {
      background-color: var(--primary-dark);
    }

    .mini-control-panel {
      display: none;
      background: var(--card-bg);
      border-radius: var(--border-radius);
      padding: 15px;
      box-shadow: var(--shadow-lg);
      opacity: 0.95;
    }

    .mini-control-panel.active {
      display: block;
    }

    .mini-control-grid {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      grid-template-rows: repeat(3, 1fr);
      gap: 8px;
      width: 160px;
      height: auto;
    }

    .mini-control {
      background: var(--primary);
      color: white;
      border: none;
      border-radius: 8px;
      display: flex;
      align-items: center;
      justify-content: center;
      cursor: pointer;
      font-size: 1.2rem;
      transition: background-color 0.2s, transform 0.1s;
      width: 45px;
      height: 45px;
    }

    .mini-control:hover {
      background-color: var(--primary-dark);
    }

    .mini-control.up    { grid-row: 1; grid-column: 2; }
    .mini-control.left  { grid-row: 2; grid-column: 1; }
    .mini-control.stop  { grid-row: 2; grid-column: 2; background: var(--danger); }
    .mini-control.right { grid-row: 2; grid-column: 3; }
    .mini-control.down  { grid-row: 3; grid-column: 2; }

    .mini-control.stop:hover {
      background: #c42c34;
    }

    .mini-control.active {
      transform: scale(0.9);
      box-shadow: inset 0 2px 4px rgba(0,0,0,0.2);
    }

    @media (max-width: 767px) {
      .app-container {
        padding: 15px;
      }
      h1 {
        font-size: 2rem;
      }
      .logo {
        width: 50px;
        height: 50px;
        font-size: 20px;
      }
      .card {
        padding: 20px;
      }
      .floating-controls {
        bottom: 15px;
        right: 15px;
      }
      .toast-container {
         left: 15px;
         bottom: 15px;
         right: auto;
         width: calc(100% - 30px);
       }
    }

    @media (max-width: 480px) {
      h1 {
        font-size: 1.8rem;
      }
      .card-title {
        font-size: 1.1rem;
      }
      .mini-control-grid {
          width: 140px;
          gap: 6px;
      }
      .mini-control {
          font-size: 1.1rem;
          flex-basis: 40px;
          height: 40px;
      }
       .toast-container {
         bottom: 10px;
         left: 10px;
         width: calc(100% - 20px);
       }
    }
  </style>
</head>
<body>
  <div class="particles-container" id="particles"></div>

  <div class="app-container">
    <header>
      <div class="logo-container">
        <div class="logo">
          <i class="fas fa-car"></i>
        </div>
        <h1>RC Command Center</h1>
      </div>
      <p class="tagline">Advanced Wireless Control Interface</p>
    </header>

    <div class="card">
      <div class="card-header">
        <div class="card-title">
          <i class="fas fa-wifi"></i>
          Connection & Status
        </div>
      </div>

      <div class="status-card" id="statusPanel">
        <div class="status-indicator">
          <div class="status-dot"></div>
        </div>
        <div class="status-info">
          <div class="status-label">WebSocket: <span id="ws_state">DISCONNECTED</span></div>
          <div class="status-details" id="connection_details">Not connected to device</div>
        </div>
      </div>

      <div class="settings-panel">
        <div class="settings-group">
          <div class="settings-title">Interface Settings</div>
          <div class="settings-option">
            <label class="toggle-switch">
              <input type="checkbox" id="particle_toggle" checked>
              <span class="toggle-slider"></span>
            </label>
            <div class="option-label">Background Effects</div>
          </div>
          <div class="settings-option">
            <label class="toggle-switch">
              <input type="checkbox" id="keyboard_toggle" checked>
              <span class="toggle-slider"></span>
            </label>
            <div class="option-label">Keyboard Controls</div>
          </div>
        </div>
      </div>

      <button id="wc_conn" class="connect-btn">
        <i class="fas fa-plug"></i>
        <span>Connect</span>
      </button>
    </div>
  </div>

  <!-- Floating Controls -->
  <div class="floating-controls">
    <div class="toggle-floating-controls" title="Toggle Controls">
      <i class="fas fa-gamepad"></i>
    </div>
    <div class="mini-control-panel">
      <div class="mini-control-grid">
        <button data-command="1" class="mini-control up" title="Forward (W/↑)"><i class="fas fa-chevron-up"></i></button>
        <button data-command="4" class="mini-control left" title="Left (A/←)"><i class="fas fa-chevron-left"></i></button>
        <button data-command="0" class="mini-control stop" title="Stop (Space)"><i class="fas fa-stop"></i></button>
        <button data-command="8" class="mini-control right" title="Right (D/→)"><i class="fas fa-chevron-right"></i></button>
        <button data-command="2" class="mini-control down" title="Backward (S/↓)"><i class="fas fa-chevron-down"></i></button>
      </div>
    </div>
  </div>

  <div class="toast-container" id="toast-container"></div>

  <script>
    // Constants
    const CMD_STOP     = 0;
    const CMD_FORWARD  = 1;
    const CMD_BACKWARD = 2;
    const CMD_LEFT     = 4;
    const CMD_RIGHT    = 8;

    // --- State Variables ---
    let ws = null;
    let keyboardEnabled = true;
    let latestPing = 0;
    let pingInterval = null;
    let reconnectAttempts = 0;
    const maxReconnectAttempts = 3;

    // --- DOM Element References (Cache for performance) ---
    const particlesContainer = document.getElementById('particles');
    const toggleFloatingControlsBtn = document.querySelector('.toggle-floating-controls');
    const miniControlPanel = document.querySelector('.mini-control-panel');
    const miniControls = document.querySelectorAll('.mini-control');
    const statusPanel = document.getElementById('statusPanel');
    const wsStateEl = document.getElementById('ws_state');
    const connectionDetailsEl = document.getElementById('connection_details');
    const connectBtn = document.getElementById('wc_conn');
    const particleToggle = document.getElementById('particle_toggle');
    const keyboardToggle = document.getElementById('keyboard_toggle');
    const toastContainer = document.getElementById('toast-container');

    // --- Utility Functions ---
    function showToast(message, type = 'info', duration = 3000) {
      if (!toastContainer) return;
      const toast = document.createElement('div');
      toast.classList.add('toast', `toast-${type}`);

      let icon = 'info-circle';
      if (type === 'success') icon = 'check-circle';
      if (type === 'error') icon = 'exclamation-circle';
      toast.innerHTML = `
        <div class="toast-icon">
          <i class="fas fa-${icon}"></i>
        </div>
        <div class="toast-message">${message}</div>
      `;
      toastContainer.appendChild(toast);

      // Trigger reflow for transition
      toast.offsetHeight;

      // Show the toast
      toast.classList.add('show');

      // Remove after duration
      setTimeout(() => {
        toast.classList.remove('show');
        // Remove from DOM after transition finishes
        toast.addEventListener('transitionend', () => toast.remove(), { once: true });
      }, duration);
    }

    // --- Initialization Functions ---
    function initParticles() {
      if (!particlesContainer || !particleToggle.checked || particlesContainer.childElementCount > 0) return; // Don't re-init
      const particleCount = 20;
      const fragment = document.createDocumentFragment();
      for (let i = 0; i < particleCount; i++) {
        const particle = document.createElement('div');
        particle.classList.add('particle');
        const size = Math.floor(Math.random() * 30) + 10;
        particle.style.width = `${size}px`; particle.style.height = `${size}px`;
        particle.style.left = `${Math.random() * 100}%`;
        particle.style.top = `${Math.random() * 100 + 100}%`; // Start below view
        const hue = Math.floor(Math.random() * 60) + 200; // Blueish
        particle.style.backgroundColor = `hsla(${hue}, 70%, 70%, 0.3)`;
        particle.style.animationDelay = `${Math.random() * 30}s`;
        fragment.appendChild(particle);
      }
      particlesContainer.appendChild(fragment);
    }

    function initFloatingControls() {
      toggleFloatingControlsBtn.addEventListener('click', () => {
        miniControlPanel.classList.toggle('active');
      });

      miniControls.forEach(control => {
        const command = parseInt(control.getAttribute('data-command'));
        
        control.addEventListener('pointerdown', (e) => {
          e.preventDefault();
          control.classList.add('active');
          sendCommand(command);
        });
        
        control.addEventListener('pointerup', (e) => {
          e.preventDefault();
          control.classList.remove('active');
          sendCommand(CMD_STOP);
        });
        
        control.addEventListener('pointerleave', (e) => {
          if (control.classList.contains('active')) {
            control.classList.remove('active');
            sendCommand(CMD_STOP);
          }
        });
      });
    }

    function setupKeyboardControls() {
      document.addEventListener('keydown', handleKeyDown);
      document.addEventListener('keyup', handleKeyUp);
    }

    // --- Event Handlers ---
    function handleKeyDown(event) {
      if (!keyboardEnabled || event.target.matches('input, select, textarea')) return;
      
      let command = -1;
      switch(event.key) {
        case 'ArrowUp': case 'w': case 'W':
          command = CMD_FORWARD; break;
        case 'ArrowDown': case 's': case 'S':
          command = CMD_BACKWARD; break;
        case 'ArrowLeft': case 'a': case 'A':
          command = CMD_LEFT; break;
        case 'ArrowRight': case 'd': case 'D':
          command = CMD_RIGHT; break;
        case ' ': // Spacebar for stop
          command = CMD_STOP; break;
        default: return;
      }
      
      event.preventDefault();
      if (command >= 0) sendCommand(command);
    }

    function handleKeyUp(event) {
      if (['ArrowUp', 'w', 'W', 'ArrowDown', 's', 'S', 'ArrowLeft', 'a', 'A', 'ArrowRight', 'd', 'D'].includes(event.key)) {
        event.preventDefault();
        sendCommand(CMD_STOP);
      }
    }

    function toggleParticles(event) {
      if (!particlesContainer) return;
      const enabled = event.target.checked;
      particlesContainer.style.display = enabled ? 'block' : 'none';
      if (enabled && particlesContainer.childElementCount === 0) {
        initParticles();
      }
    }

    function toggleKeyboardControls(event) {
      keyboardEnabled = event.target.checked;
      showToast(`Keyboard controls ${keyboardEnabled ? 'enabled' : 'disabled'}`, 'info');
    }

    function handleConnectionToggle() {
      if (!ws || ws.readyState === WebSocket.CLOSED) {
        connectWebSocket();
      } else if (ws.readyState === WebSocket.OPEN){
        disconnectWebSocket();
      }
    }

    // --- WebSocket Logic ---
    function connectWebSocket() {
      if (ws && ws.readyState !== WebSocket.CLOSED) return;

      try {
        const host = window.location.hostname || '192.168.4.1';
        const wsUrl = `ws://${host}:81`;
        ws = new WebSocket(wsUrl);
        console.log(`Attempting to connect to ${wsUrl}`);

        updateConnectionStatus('CONNECTING');
        connectBtn.innerHTML = '<i class="fas fa-spinner fa-spin"></i><span>Connecting</span>';
        connectBtn.disabled = true;

        ws.onopen = handleWebSocketOpen;
        ws.onclose = handleWebSocketClose;
        ws.onerror = handleWebSocketError;
      } catch (error) {
        console.error('WebSocket instantiation error:', error);
        showToast('Failed to initiate connection', 'error');
        updateConnectionStatus('ERROR');
        connectBtn.innerHTML = '<i class="fas fa-plug"></i><span>Connect</span>';
        connectBtn.disabled = false;
        ws = null;
      }
    }

    function disconnectWebSocket() {
      if (ws && (ws.readyState === WebSocket.OPEN || ws.readyState === WebSocket.CONNECTING)) {
        ws.close(1000, "User disconnected");
      }
    }

    function handleWebSocketOpen() {
      console.log('WebSocket Connected');
      updateConnectionStatus('CONNECTED');
      statusPanel.classList.add('connected');
      connectBtn.innerHTML = '<i class="fas fa-unlink"></i><span>Disconnect</span>';
      connectBtn.classList.add('disconnect-btn');
      connectBtn.disabled = false;
      connectionDetailsEl.textContent = `Connected to ${ws.url}`;
      showToast('Connected successfully', 'success');
      reconnectAttempts = 0;
      clearInterval(pingInterval);
      pingInterval = setInterval(sendPing, 3000);
      sendPing();
    }

    function handleWebSocketClose(event) {
      console.log(`WebSocket Closed: Code=${event.code}, Reason='${event.reason}', WasClean=${event.wasClean}`);
      updateConnectionStatus('DISCONNECTED');
      statusPanel.classList.remove('connected');
      connectBtn.innerHTML = '<i class="fas fa-plug"></i><span>Connect</span>';
      connectBtn.classList.remove('disconnect-btn');
      connectBtn.disabled = false;
      connectionDetailsEl.textContent = 'Not connected to device';
      clearInterval(pingInterval);
      pingInterval = null;

      if (event.wasClean) {
        showToast('Disconnected', 'info');
      } else {
        showToast(`Connection lost (Code: ${event.code})`, 'error');
      }
      ws = null;
    }

    function handleWebSocketError(error) {
      console.error('WebSocket Error:', error);
      showToast('WebSocket connection error', 'error');
      updateConnectionStatus('ERROR');
      if (ws && ws.readyState !== WebSocket.OPEN) {
          connectBtn.innerHTML = '<i class="fas fa-plug"></i><span>Connect</span>';
          connectBtn.classList.remove('disconnect-btn');
          connectBtn.disabled = false;
      }
    }

    function sendPing() {
      if (ws && ws.readyState === WebSocket.OPEN) {
        latestPing = Date.now();
        ws.send('PING\r\n');
      } else {
        clearInterval(pingInterval);
        pingInterval = null;
      }
    }

    // --- Command & Control ---
    function sendCommand(command) {
      if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(command + "\r\n");
        console.log(`Sent command: ${command}`);
      }
    }

    // --- Status Updates ---
    function updateConnectionStatus(status) {
      if (wsStateEl) wsStateEl.textContent = status;
    }

    // --- Initial Page Load ---
    function initializeApp() {
      particleToggle.addEventListener('change', toggleParticles);
      keyboardToggle.addEventListener('change', toggleKeyboardControls);
      connectBtn.addEventListener('click', handleConnectionToggle);

      if (particleToggle.checked) initParticles();
      initFloatingControls();
      setupKeyboardControls();

      updateConnectionStatus('DISCONNECTED');
      showToast('Welcome to RC Command Center!', 'success');
    }

    document.addEventListener('DOMContentLoaded', initializeApp);
  </script>
</body>
</html>
)=====";
