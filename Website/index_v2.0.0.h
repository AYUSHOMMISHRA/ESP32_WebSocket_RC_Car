const char *HTML_CONTENT = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>RC Car Command Center</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css">
  <style>
    /* --- Base Variables and Resets --- */
    :root {
        /* Light Theme Palette (F6F5F2, F0EBE3, F3D0D7, FFEFEF) + Dark Blue Accent */
        --bg-main: #F6F5F2;
        --bg-card: #FFFFFF; /* White cards for contrast */
        --bg-navbar: rgba(240, 235, 227, 0.9); /* F0EBE3 with transparency */
        --bg-panel: #F0EBE3; /* Panel background */
        --text-primary: #333333; /* Dark text */
        --text-secondary: #666666; /* Gray text */
        --border-color: #E0E0E0; /* Light border */
        --accent-primary: #19376D; /* Dark blue from dark palette for buttons/accents */
        --accent-primary-light: #576CBC; /* Lighter blue */
        --accent-primary-dark: #0B2447; /* Darkest blue */
        --accent-secondary: #F3D0D7; /* Light Pink */
        --accent-secondary-light: #FFEFEF; /* Very Light Pink/Red */
        --danger: #e63946;
        --success: #2ecc71;
        --dark: #2b2d42; /* Kept for reference, use theme colors */
        --light: #f8f9fa;/* Kept for reference, use theme colors */
        --gray: #888888; /* Adjusted gray */

        --border-radius: 12px;
        --shadow-sm: 0 2px 4px rgba(0, 0, 0, 0.05);
        --shadow-md: 0 5px 10px rgba(0, 0, 0, 0.08);
        --shadow-lg: 0 10px 20px rgba(0, 0, 0, 0.1);
        --transition: all 0.3s cubic-bezier(0.25, 0.8, 0.25, 1);
        --navbar-height: 60px;
    }

    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
    }

    html {
      height: 100%;
    }

    body {
      font-family: 'Segoe UI', 'Roboto', system-ui, -apple-system, sans-serif;
      background-color: var(--bg-main);
      color: var(--text-primary);
      line-height: 1.6;
      height: 100vh;
      overflow: hidden;
      display: flex;
      flex-direction: column;
      transition: background-color 0.3s ease, color 0.3s ease;
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

    /* Particle styles */
    .particle {
      position: absolute;
      border-radius: 50%;
      opacity: 0.6;
      animation: float 60s infinite linear;
    }
    @keyframes float {
      0% { transform: translateY(0) rotate(0deg); }
      100% { transform: translateY(-2000px) rotate(360deg); }
    }

    /* --- Top Navigation Bar --- */
    .top-navbar {
      height: var(--navbar-height);
      background-color: var(--bg-navbar);
      backdrop-filter: blur(5px);
      -webkit-backdrop-filter: blur(5px);
      padding: 0 2%; /* Reduced padding */
      display: flex;
      align-items: center;
      justify-content: space-between;
      color: var(--text-primary);
      box-shadow: var(--shadow-md);
      flex-shrink: 0;
      z-index: 100;
      transition: background-color 0.3s ease, color 0.3s ease;
    }

    .top-navbar .navbar-left,
    .top-navbar .navbar-right {
      display: flex;
      align-items: center;
      gap: 15px;
    }
    .top-navbar .navbar-center { /* For potential future use */
        flex-grow: 1;
        text-align: center;
    }

    .menu-button {
        background: none;
        border: none;
        color: var(--text-primary);
        font-size: 1.5rem;
        cursor: pointer;
        padding: 5px;
        transition: color 0.2s;
    }
    .menu-button:hover {
        color: var(--accent-primary);
    }

    .top-navbar .logo-container {
      display: flex;
      align-items: center;
      gap: 10px; /* Reduced gap */
    }

    .top-navbar .logo {
      width: 38px; /* Slightly smaller */
      height: 38px;
      background-color: var(--accent-primary);
      border-radius: 50%;
      display: flex;
      align-items: center;
      justify-content: center;
      font-size: 17px;
      box-shadow: var(--shadow-sm);
      animation: pulse 2s infinite;
      color: white;
    }

    .top-navbar h1 {
      font-size: 1.4rem; /* Slightly smaller */
      font-weight: 600;
      margin: 0;
      color: var(--text-primary);
      text-shadow: none; /* Remove shadow for light theme */
    }

    @keyframes pulse {
      0% { box-shadow: 0 0 0 0 rgba(25, 55, 109, 0.7); } /* Use accent color */
      70% { box-shadow: 0 0 0 10px rgba(25, 55, 109, 0); }
      100% { box-shadow: 0 0 0 0 rgba(25, 55, 109, 0); }
    }

    /* Centered Navbar Status */
    .navbar-connection-status {
        position: absolute;
        right: 5px;
        transform: translateX(-25%);
        font-size: 0.95rem;
        font-weight: 600;
        color: var(--text-secondary);
        background-color: rgba(255, 255, 255, 0.7);
        padding: 5px 15px;
        border-radius: 15px;
        box-shadow: var(--shadow-sm);
        display: block; /* Hidden by default */
        white-space: nowrap;
        transition: color 0.3s ease;
    }
    .navbar-connection-status.connected {
        color: var(--success);
    }
    .navbar-connection-status.error {
        color: var(--danger);
    }
    .navbar-connection-status.connecting .fa-spinner {
        margin-right: 5px;
    }


    /* Dark Mode Toggle in Nav */
    .dark-mode-nav-toggle {
        margin-right: 10px; /* Spacing */
    }
    .dark-mode-nav-toggle .option-label {
        color: var(--text-primary);
        font-size: 0.9rem;
    }

    /* --- Left Menu Popup --- */
    .left-menu {
        position: fixed;
        top: 0;
        left: 0;
        width: 280px;
        height: 100%;
        background-color: var(--bg-panel);
        box-shadow: var(--shadow-lg);
        z-index: 1050;
        transform: translateX(-100%);
        transition: transform 0.3s ease-out;
        display: flex;
        flex-direction: column;
        padding: 20px;
        border-right: 1px solid var(--border-color);
    }
    .left-menu.visible {
        transform: translateX(0);
    }
    .left-menu-header {
        display: flex;
        justify-content: space-between;
        align-items: center;
        padding-bottom: 15px;
        margin-bottom: 15px;
        border-bottom: 1px solid var(--border-color);
    }
    .left-menu-header h3 {
        margin: 0;
        font-size: 1.2rem;
        color: var(--text-primary);
    }
    .close-menu-button {
        background: none;
        border: none;
        font-size: 1.8rem;
        font-weight: bold;
        color: var(--text-secondary);
        cursor: pointer;
        padding: 0 5px;
        line-height: 1;
    }
    .close-menu-button:hover {
        color: var(--accent-primary);
    }
    .menu-overlay {
        position: fixed;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        background-color: rgba(0, 0, 0, 0.4);
        z-index: 1040; /* Below menu, above content */
        opacity: 0;
        visibility: hidden;
        transition: opacity 0.3s ease-out, visibility 0.3s ease-out;
    }
    .menu-overlay.visible {
        opacity: 1;
        visibility: visible;
    }
    .left-menu .settings-group { margin-bottom: 0; } /* Remove extra margin */
    .left-menu .settings-option { margin-bottom: 12px; }
    .left-menu .option-label { font-size: 0.95rem; color: var(--text-primary); }


    /* --- Main Content Area --- */
    .main-content {
      display: flex;
      flex-grow: 1;
      padding: 20px;
      gap: 20px;
      height: calc(100vh - var(--navbar-height) - 40px); /* Adjust for footer */
      overflow: hidden;
      margin-left: 2%; /* Match navbar padding */
      margin-right: 2%;
    }

    /* --- Left Panel (25%) --- */
    .left-panel {
      flex: 0 0 25%;
      min-width: 280px;
      display: flex;
      flex-direction: column;
      gap: 20px;
      overflow-y: auto;
      padding-bottom: 5px;
    }

    /* --- Center Panel (Expanded to fill remaining space) --- */
    .center-panel {
      flex: 1;
      min-width: 400px;
      display: flex;
      flex-direction: column;
      overflow: hidden;
    }

    /* --- Card Styles (General) --- */
    .card {
      background-color: var(--bg-card);
      border-radius: var(--border-radius);
      box-shadow: var(--shadow-md);
      padding: 20px;
      transition: var(--transition);
      display: flex;
      flex-direction: column;
      border: 1px solid var(--border-color);
      transition: background-color 0.3s ease, color 0.3s ease, border-color 0.3s ease;
    }

    .card-header {
      display: flex;
      align-items: center;
      justify-content: space-between;
      margin-bottom: 15px;
      padding-bottom: 10px;
      border-bottom: 1px solid var(--border-color);
      flex-shrink: 0;
    }

    .card-title {
      font-size: 1.1rem;
      font-weight: 600;
      color: var(--accent-primary-dark); /* Darker blue title */
      display: flex;
      align-items: center;
      gap: 8px;
    }

    .card-title i {
      font-size: 1rem;
      color: var(--accent-primary);
    }

    /* --- Connection Panel Specific Styles --- */
    .connection-panel { flex-shrink: 0; }
    .status-card { display: flex; align-items: center; gap: 12px; background-color: rgba(0, 0, 0, 0.03); padding: 12px; border-radius: 8px; margin-bottom: 12px; }
    .status-indicator { width: 40px; height: 40px; flex-shrink: 0; display: flex; align-items: center; justify-content: center; } /* Centered dot */
    .status-dot { width: 20px; height: 20px; } /* Slightly smaller dot */
    .status-dot { border-radius: 50%; background-color: var(--danger); position: relative; transition: var(--transition); }
    .status-dot::after { content: ''; position: absolute; top: -4px; left: -4px; right: -4px; bottom: -4px; border-radius: 50%; border: 2px solid var(--danger); opacity: 0.5; animation: ripple 1.5s linear infinite; }
    .connected .status-dot { background-color: var(--success); }
    .connected .status-dot::after { border-color: var(--success); }
    .auth-dot { background-color: var(--gray); }
    .auth-dot::after { border-color: var(--gray); }
    .authorized .auth-dot { background-color: var(--success); }
    .authorized .auth-dot::after { border-color: var(--success); }
    .unauthorized .auth-dot { background-color: var(--danger); }
    .unauthorized .auth-dot::after { border-color: var(--danger); }
    #auth_time { font-size: 0.8rem; margin-top: 3px; font-style: italic; color: var(--text-secondary); }
    @keyframes ripple { 0% { transform: scale(0.8); opacity: 1; } 100% { transform: scale(1.5); opacity: 0; } }
    .status-info { flex: 1; min-width: 0; }
    .status-label { font-weight: 600; font-size: 0.95rem; color: var(--text-primary); }
    .status-details { font-size: 0.85rem; color: var(--text-secondary); margin-top: 2px; word-break: break-word; }
    .status-timestamp {
      font-size: 0.8rem;
      color: var(--gray-500);
      font-style: italic;
    }
    /* Connect Button Styling */
    .connect-btn { margin-top: auto; padding-top: 15px; border: none; } /* Remove padding-top */
    .connect-btn { background-color: var(--accent-primary); color: white; border-radius: 50px; padding: 10px 20px; font-size: 0.95rem; font-weight: 600; cursor: pointer; display: flex; align-items: center; justify-content: center; gap: 8px; transition: var(--transition); box-shadow: var(--shadow-sm); width: 100%; }
    .connect-btn:hover { background-color: var(--accent-primary-dark); transform: translateY(-2px); box-shadow: var(--shadow-md); }
    .connect-btn:disabled { background-color: var(--gray); cursor: not-allowed; transform: none; box-shadow: none; }
    .disconnect-btn { background-color: var(--danger); }
    .disconnect-btn:hover { background-color: #c42c34; }

    /* Settings Panel (Now only used inside left-menu) */
    .settings-panel { margin-top: 15px; margin-bottom: 15px; }
    .settings-group { margin-bottom: 15px; }
    .settings-title { font-weight: 600; margin-bottom: 8px; font-size: 0.9rem; color: var(--accent-primary-dark); }
    .settings-option { display: flex; align-items: center; margin-bottom: 8px; }
    .toggle-switch { position: relative; display: inline-block; width: 44px; height: 22px; margin-right: 8px; flex-shrink: 0; }
    .toggle-switch input { opacity: 0; width: 0; height: 0; }
    .toggle-slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; transition: .4s; border-radius: 22px; }
    .toggle-slider:before { position: absolute; content: ""; height: 16px; width: 16px; left: 3px; bottom: 3px; background-color: white; transition: .4s; border-radius: 50%; }
    input:checked + .toggle-slider { background-color: var(--accent-primary); }
    input:focus + .toggle-slider { box-shadow: 0 0 1px var(--accent-primary); }
    input:checked + .toggle-slider:before { transform: translateX(22px); }
    .option-label { flex: 1; font-size: 0.9rem; color: var(--text-primary); }

    /* --- Telemetry Section (Left Panel) --- */
    .telemetry-section { flex-shrink: 0; }
    .telemetry-bar { display: flex; flex-direction: row; flex-wrap: wrap; gap: 10px; justify-content: space-around; }
    .telemetry-item { display: flex; flex-direction: column; align-items: center; justify-content: center; background-color: rgba(0, 0, 0, 0.03); padding: 10px; border-radius: 8px; color: var(--text-primary); text-align: center; transition: transform 0.2s ease-out; min-width: 70px; flex: 1; }
    .telemetry-item:hover { transform: scale(1.05); }
    .telemetry-icon { font-size: 1.3rem; margin-bottom: 5px; color: var(--accent-primary); }
    .telemetry-item:nth-child(1) .telemetry-icon { color: var(--success); }
    .telemetry-item:nth-child(2) .telemetry-icon { color: var(--accent-secondary-light); } /* Use theme color */
    .telemetry-item:nth-child(3) .telemetry-icon { color: var(--accent-secondary); } /* Use theme color */
    .telemetry-label { font-size: 0.7rem; font-weight: 600; text-transform: uppercase; opacity: 0.8; margin-bottom: 3px; color: var(--text-secondary); }
    .telemetry-value { font-size: 1rem; font-weight: 700; line-height: 1.1; color: var(--text-primary); }
    @keyframes valueUpdateFlash { 0%, 100% { opacity: 1; transform: scale(1); } 50% { opacity: 0.7; transform: scale(1.05); } }
    .telemetry-item.value-updated .telemetry-value { animation: valueUpdateFlash 0.4s ease-in-out; }

    /* --- Camera Container (Center Panel - Expanded) --- */
    .camera-container.card {
      flex-grow: 1;
      overflow: hidden;
    }

    .video-feed-section {
      flex-grow: 1;
      display: flex;
      flex-direction: column;
      min-height: 0;
    }

    .video-feed {
      position: relative;
      width: 100%;
      background-color: #000;
      border-radius: 8px;
      overflow: hidden;
      aspect-ratio: auto;
      box-shadow: var(--shadow-sm);
      flex-grow: 1;
      display: flex;
      align-items: center;
      justify-content: center;
      margin-bottom: 15px;
    }

    #camera-stream {
      width: 100%;
      height: 100%;
      object-fit: cover;
      display: block;
    }

    .stream-overlay { position: absolute; top: 0; left: 0; width: 100%; height: 100%; background-color: rgba(0, 0, 0, 0.7); display: flex; flex-direction: column; align-items: center; justify-content: center; color: white; gap: 10px; transition: opacity 0.3s ease; text-align: center; z-index: 5; border-radius: 8px; }
    .stream-overlay i { font-size: 1.8rem; }
    .stream-overlay.hidden { opacity: 0; pointer-events: none; }

    .video-controls { display: flex; align-items: center; justify-content: space-between; margin-top: auto; padding-top: 10px; flex-wrap: wrap; gap: 10px; flex-shrink: 0; }
    .camera-btn { background-color: var(--accent-primary); color: white; border: none; border-radius: 50px; padding: 8px 18px; font-size: 0.9rem; font-weight: 600; cursor: pointer; display: flex; align-items: center; justify-content: center; gap: 8px; transition: var(--transition); box-shadow: var(--shadow-sm); flex-grow: 1; }
    .camera-btn:hover { background-color: var(--accent-primary-dark); transform: translateY(-2px); box-shadow: var(--shadow-md); }
    .camera-btn:disabled { background-color: var(--gray); cursor: not-allowed; transform: none; box-shadow: none; }
    .quality-selector { display: flex; align-items: center; gap: 8px; flex-grow: 1; justify-content: flex-end; }
    .quality-selector label { font-size: 0.9rem; white-space: nowrap; color: var(--text-secondary); }
    .quality-selector select { padding: 6px 12px; border-radius: 20px; border: 1px solid var(--border-color); background-color: white; font-size: 0.9rem; outline: none; transition: var(--transition); min-width: 90px; color: var(--text-primary); }
    .quality-selector select:focus { border-color: var(--accent-primary); box-shadow: 0 0 0 2px rgba(25, 55, 109, 0.2); }

    /* Full-page mode styles */
    .full-page-button { position: absolute; bottom: 10px; right: 10px; background: rgba(0,0,0,0.5); color: white; border: none; border-radius: 50%; width: 36px; height: 36px; display: flex; align-items: center; justify-content: center; cursor: pointer; z-index: 10; transition: background-color 0.2s; }
    .full-page-button:hover { background-color: rgba(0,0,0,0.8); }
    .full-page-mode-exit { position: absolute; top: 10px; left: 10px; z-index: 11; background: rgba(230, 57, 70, 0.8); color: white; border: none; border-radius: 50%; width: 36px; height: 36px; box-shadow: var(--shadow-md); cursor: pointer; transition: background-color 0.2s; display: none; }
    .full-page-mode-exit:hover { background-color: var(--danger); }

    body.full-page-mode .video-feed { position: fixed; top: 0; left: 0; width: 100vw; height: 100vh; z-index: 9999; border-radius: 0; object-fit: contain; background-color: #000; margin-bottom: 0; }
    body.full-page-mode .full-page-mode-exit { display: flex; position: fixed; top: 20px; left: 20px; z-index: 10000; }
    body.full-page-mode .top-navbar,
    body.full-page-mode .main-content,
    body.full-page-mode footer {
        display: none !important;
    }
    /* KEEP floating controls visible in fullscreen */
    body.full-page-mode #floating-control-button,
    body.full-page-mode #floating-control-panel {
        /* display: block !important; */ /* Not needed if we remove the hiding rule */
        z-index: 10001; /* Ensure they are above the fullscreen video */
    }

    /* --- Floating Control Button Styles --- */
    #floating-control-button {
      position: fixed;
      bottom: 30px;
      right: 30px;
      width: 60px;
      height: 60px;
      background-color: var(--accent-primary); /* Use theme blue */
      border-radius: 50%;
      display: flex;
      align-items: center;
      justify-content: center;
      cursor: grab;
      z-index: 1000; /* High z-index */
      box-shadow: var(--shadow-lg);
      transition: background-color 0.2s, transform 0.2s;
    }
    #floating-control-button:active {
        cursor: grabbing;
        transform: scale(0.95);
    }
    #floating-control-button i {
        color: white;
        font-size: 1.8rem;
    }

    /* --- Floating Control Panel Styles (Pop-up) --- */
    #floating-control-panel {
      position: fixed;
      width: 180px;
      height: 180px;
      background-color: rgba(25, 55, 109, 0.9); /* Dark blue translucent */
      backdrop-filter: blur(5px);
      -webkit-backdrop-filter: blur(5px);
      border-radius: var(--border-radius);
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      grid-template-rows: repeat(3, 1fr);
      gap: 8px;
      padding: 15px;
      z-index: 999; /* Below main button when dragging, but above content */
      box-shadow: var(--shadow-lg);
      transform: scale(0.8);
      opacity: 0;
      transition: transform 0.2s ease-out, opacity 0.2s ease-out, background-color 0.3s ease;
      transform-origin: bottom right;
    }

    #floating-control-panel.visible {
        transform: scale(1);
        opacity: 1;
    }

    /* Style buttons inside the floating panel */
    #floating-control-panel .control-button {
        background: #A5D7E8; /* Lighter theme blue */
        color: #0B2447;
        border: none;
        border-radius: 8px;
        display: flex;
        align-items: center;
        justify-content: center;
        cursor: pointer;
        font-size: 1.5rem;
        transition: background-color 0.2s, transform 0.1s;
        width: 100%;
        height: 100%;
        box-shadow: var(--shadow-sm);
        user-select: none;
        -webkit-user-select: none;
        touch-action: manipulation;
    }

    #floating-control-panel .control-button:hover {
        background-color: var(--accent-primary); /* Darker theme blue */
        color: white;
    }
    #floating-control-panel .control-button:active,
    #floating-control-panel .control-button.active {
        transform: scale(0.92);
        box-shadow: inset 0 2px 4px rgba(0,0,0,0.2);
    }

    /* Grid placement for floating controls */
    #floating-control-panel .control-button.up    { grid-row: 1; grid-column: 2; }
    #floating-control-panel .control-button.left  { grid-row: 2; grid-column: 1; }
    #floating-control-panel .control-button.stop  { grid-row: 2; grid-column: 2; background: var(--danger); color: white; } /* Stop button red */
    #floating-control-panel .control-button.right { grid-row: 2; grid-column: 3; }
    #floating-control-panel .control-button.down  { grid-row: 3; grid-column: 2; }

    #floating-control-panel .control-button.stop:hover { background: #c42c34; }

    /* Add to your CSS styles */
    #floating-control-panel .control-button.disabled {
      opacity: 0.5;
      cursor: not-allowed;
      background-color: #888888;
    }

    /* --- Footer --- */
    footer {
      text-align: center;
      color: var(--text-secondary);
      font-size: 0.85rem;
      padding: 10px 0;
      width: 100%;
      flex-shrink: 0;
      background-color: var(--bg-panel); /* Match panel background */
      border-top: 1px solid var(--border-color);
    }
    footer a { color: var(--accent-primary); text-decoration: none; font-weight: 600; transition: var(--transition); }
    footer a:hover { opacity: 0.8; text-decoration: underline; }
    .sponsor { margin-top: 5px; font-size: 0.8rem; }

    /* --- Toast Notifications --- */
    .toast-container { position: fixed; bottom: 20px; left: 20px; z-index: 1050; max-width: 350px; }
    .toast { background-color: var(--dark); /* Use dark color for contrast */ color: white; border-radius: 10px; box-shadow: var(--shadow-md); padding: 15px 20px; margin-top: 10px; display: flex; align-items: center; gap: 10px; transform: translateX(-120%); transition: transform 0.5s cubic-bezier(0.68, -0.55, 0.27, 1.55); opacity: 0.95; }
    .toast.show { transform: translateX(0); }
    .toast-icon { font-size: 20px; width: 24px; height: 24px; display: flex; align-items: center; justify-content: center; flex-shrink: 0; }
    .toast-success .toast-icon { color: var(--success); }
    .toast-error .toast-icon { color: var(--danger); }
    .toast-info .toast-icon { color: var(--accent-primary-light); } /* Use light blue */
    .toast-message { font-size: 0.9rem; }

    /* --- Dark Mode Styles --- */
    /* Palette: https://colorhunt.co/palette/000000282a3a735f32c69749 */
    body.dark-mode {
        /* Define dark mode variables using the new palette */
        --bg-main: #000000;                     /* Black background */
        --bg-card: #282A3A;                     /* Dark Purple/Blue for cards */
        --bg-navbar: rgba(40, 42, 58, 0.9);    /* Dark Purple/Blue translucent navbar */
        --bg-panel: #282A3A;                    /* Dark Purple/Blue for left menu */
        --text-primary: #E0E0E0;                /* Light Gray for primary text (contrast on dark) */
        --text-secondary: #A5A5A5;              /* Medium Gray for secondary text */
        --border-color: transparent;                /* Muted Olive/Brown for borders */
        --accent-primary: #C69749;              /* Gold/Mustard for main accents */
        --accent-primary-light: #E1B46B;        /* Lighter Gold for hover */
        --accent-primary-dark: #A9823C;         /* Darker Gold for active/focus */
        --accent-secondary: #735F32;            /* Muted Olive/Brown for secondary accents */
        --accent-secondary-light: #A0874E;      /* Lighter Olive */
        --gray: #A5A5A5;                        /* Remap gray to medium gray */
        --danger: #e63946;                      /* Keep danger color */
        --success: #2ecc71;                     /* Keep success color */
    }

    /* Apply the variables */
    body.dark-mode .top-navbar {
        color: var(--text-primary);
        background-color: var(--bg-navbar);
        box-shadow: 0 5px 15px rgba(0, 0, 0, 0.3); /* Slightly stronger shadow on black */
    }
    body.dark-mode .menu-button {
        color: var(--text-primary);
    }
    body.dark-mode .menu-button:hover {
        color: var(--accent-primary); /* Gold hover */
    }
    body.dark-mode .top-navbar h1 {
        color: var(--text-primary);
    }
    body.dark-mode .top-navbar .logo {
        background-color: var(--accent-primary); /* Gold logo bg */
        color: var(--bg-main); /* Black icon */
    }
    body.dark-mode .navbar-connection-status {
        background-color: rgba(40, 42, 58, 0.8); /* Panel color background */
        color: var(--text-secondary);
        box-shadow: var(--shadow-sm);
    }
    body.dark-mode .navbar-connection-status.connected { color: var(--success); }
    body.dark-mode .navbar-connection-status.error { color: var(--danger); }

    body.dark-mode .dark-mode-nav-toggle .option-label {
        color: var(--text-primary);
    }
    body.dark-mode .card {
        background-color: var(--bg-card);
        color: var(--text-primary);
        border-color: var(--border-color);
    }
    body.dark-mode .card-header {
        border-bottom-color: var(--border-color);
    }
    body.dark-mode .card-title {
        color: var(--accent-primary); /* Gold title */
    }
    body.dark-mode .card-title i {
        color: var(--accent-primary);
    }
    body.dark-mode .status-card,
    body.dark-mode .telemetry-item {
        background-color: rgba(0, 0, 0, 0.2); /* Slightly darker than card bg */
        color: var(--text-primary);
    }
    body.dark-mode .status-label { color: var(--text-primary); }
    body.dark-mode .status-details { color: var(--text-secondary); }
    body.dark-mode #auth_time { color: var(--text-secondary); }

    body.dark-mode .left-menu {
        background-color: var(--bg-panel);
        border-right-color: var(--border-color);
    }
    body.dark-mode .left-menu-header {
        border-bottom-color: var(--border-color);
    }
    body.dark-mode .left-menu-header h3 { color: var(--text-primary); }
    body.dark-mode .close-menu-button { color: var(--text-secondary); }
    body.dark-mode .close-menu-button:hover { color: var(--accent-primary); }
    body.dark-mode .left-menu .option-label { color: var(--text-primary); }
    body.dark-mode .left-menu .settings-title { color: var(--accent-primary); }

    body.dark-mode .toggle-slider {
        background-color: #444; /* Darker toggle off state */
    }
    body.dark-mode input:checked + .toggle-slider {
        background-color: var(--accent-primary); /* Gold toggle on */
    }
    body.dark-mode .option-label {
        color: var(--text-primary);
    }
    body.dark-mode .connect-btn {
        background-color: var(--accent-primary); /* Gold connect btn */
        color: var(--bg-main); /* Black text for contrast */
    }
    body.dark-mode .connect-btn:hover {
        background-color: var(--accent-primary-light); /* Lighter gold hover */
        color: var(--bg-main);
    }
    body.dark-mode .connect-btn:disabled {
        background-color: #555;
        color: #999;
        opacity: 0.7;
    }

    body.dark-mode .disconnect-btn {
        background-color: var(--danger); /* Keep danger for disconnect */
        color: white;
    }
    body.dark-mode .disconnect-btn:hover {
        background-color: #c42c34;
    }

    body.dark-mode .telemetry-label {
        opacity: 0.8;
        color: var(--text-secondary);
    }
    body.dark-mode .telemetry-value { color: var(--text-primary); }
    /* Telemetry Icons - Using Accent Palette */
    body.dark-mode .telemetry-icon { color: var(--accent-secondary); } /* Olive */
    body.dark-mode .telemetry-item:nth-child(2) .telemetry-icon { color: var(--accent-secondary-light); } /* Signal: Lighter Olive */
    body.dark-mode .telemetry-item:nth-child(3) .telemetry-icon { color: var(--accent-primary); } /* Latency: Gold */

    body.dark-mode .camera-btn {
        background-color: var(--accent-primary); /* Gold camera btn */
        color: var(--bg-main); /* Black text */
    }
    body.dark-mode .camera-btn:hover {
        background-color: var(--accent-primary-light);
    }
    body.dark-mode .camera-btn:disabled {
        background-color: #555;
        color: #999;
        opacity: 0.7;
    }

    body.dark-mode .quality-selector label {
        color: var(--text-secondary);
    }
    body.dark-mode .quality-selector select {
        background-color: var(--accent-secondary); /* Olive background */
        color: var(--text-primary);
        border-color: var(--border-color);
    }
    body.dark-mode .quality-selector select:focus {
        border-color: var(--accent-primary); /* Gold focus border */
        box-shadow: 0 0 0 2px rgba(198, 151, 73, 0.3); /* Gold focus shadow */
    }

    body.dark-mode .toast {
        background-color: #333; /* Keep dark toast background */
        color: #e0e0e0;
        box-shadow: var(--shadow-md);
    }
    body.dark-mode footer {
        background-color: var(--bg-main); /* Black footer */
        color: var(--text-secondary);
        border-top-color: var(--border-color);
    }
    body.dark-mode footer a {
        color: var(--accent-primary); /* Gold links */
    }

    /* Dark mode for floating controls */
    body.dark-mode #floating-control-button {
        background-color: var(--accent-primary); /* Gold floating button */
    }
    body.dark-mode #floating-control-button i {
        color: var(--bg-main); /* Black icon */
    }

    body.dark-mode #floating-control-panel {
        background-color: rgba(40, 42, 58, 0.9); /* Dark Purple/Blue panel */
        backdrop-filter: blur(5px);
        -webkit-backdrop-filter: blur(5px);
    }
    body.dark-mode #floating-control-panel .control-button {
        background: var(--accent-primary); /* Gold buttons */
        color: var(--bg-main); /* Black icons */
        box-shadow: inset 0 -2px 4px rgba(0,0,0,0.3); /* Add subtle depth */
    }
    body.dark-mode #floating-control-panel .control-button:hover {
        background-color: var(--accent-primary-light); /* Lighter gold hover */
        color: var(--bg-main);
    }
    body.dark-mode #floating-control-panel .control-button.stop {
        background: var(--danger); /* Keep Stop red */
        color: white;
        box-shadow: inset 0 -2px 4px rgba(0,0,0,0.3);
    }
    body.dark-mode #floating-control-panel .control-button.stop:hover {
        background: #c42c34;
    }
    body.dark-mode #floating-control-panel .control-button:active,
    body.dark-mode #floating-control-panel .control-button.active {
        transform: scale(0.92);
        box-shadow: inset 0 2px 4px rgba(0,0,0,0.4); /* Deeper inset shadow */
    }

    /* --- Responsive Design --- */
    @media (max-width: 1024px) {
      .main-content {
        flex-direction: column;
        height: auto;
        overflow-y: auto;
        padding: 15px;
        gap: 15px;
      }
      body {
        overflow-y: auto;
        height: auto;
      }
      .left-panel, .center-panel {
        flex-basis: auto;
        width: 100%;
        min-width: 0;
        overflow-y: visible;
      }
      .telemetry-bar { justify-content: space-between; }
      #floating-control-button { width: 55px; height: 55px; bottom: 20px; right: 20px; }
      #floating-control-panel { width: 160px; height: 160px; }
      .navbar-connection-status { font-size: 0.9rem; padding: 4px 12px; }
    }

    @media (max-width: 768px) {
        .top-navbar h1 { font-size: 1.2rem; }
        .top-navbar { padding: 0 15px; }
        .top-navbar .logo-container { gap: 8px; }
        .top-navbar .logo { width: 35px; height: 35px; font-size: 16px; }
        .menu-button { font-size: 1.3rem; }
        .card { padding: 15px; }
        .video-controls { flex-direction: column; align-items: stretch; gap: 12px; }
        .camera-btn, .quality-selector { width: 100%; justify-content: center; }
        .quality-selector { justify-content: space-between; }
        .toast-container { bottom: 10px; left: 10px; width: calc(100% - 20px); max-width: none; }
        #floating-control-button { width: 50px; height: 50px; bottom: 15px; right: 15px; }
        #floating-control-button i { font-size: 1.5rem; }
        #floating-control-panel { width: 150px; height: 150px; padding: 10px; gap: 6px;}
        #floating-control-panel .control-button { font-size: 1.3rem; }
        .left-menu { width: 250px; }
        .navbar-connection-status { display: none; } /* Hide status on smaller screens */
    }

    @media (max-width: 480px) {
        .top-navbar h1 { font-size: 1.1rem; }
        .top-navbar .logo { display: none; } /* Hide logo */
        .card-title { font-size: 1rem; }
        .telemetry-item { min-width: 60px; padding: 8px; }
        .telemetry-icon { font-size: 1.1rem; }
        .telemetry-label { font-size: 0.6rem; }
        .telemetry-value { font-size: 0.9rem; }
        #floating-control-panel { width: 130px; height: 130px; }
        #floating-control-panel .control-button { font-size: 1.2rem; }
        .left-menu { width: 100%; border-right: none; } /* Full width menu */
        .dark-mode-nav-toggle .option-label { display: none; } /* Hide label */
        .status-card { flex-direction: column; align-items: flex-start; gap: 5px; }
        .status-indicator { width: auto; height: auto; margin-bottom: 5px; }
    }



  </style>
</head>
<body>
  <div class="particles-container" id="particles"></div>

  <!-- Left Menu Popup -->
  <div id="left-menu-popup" class="left-menu">
    <div class="left-menu-header">
      <h3>Interface Settings</h3>
      <button id="close-menu-btn" class="close-menu-button" title="Close Menu">&times;</button>
    </div>
    <div class="settings-group">
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
      <div class="settings-option">
          <label class="toggle-switch">
            <input type="checkbox" id="sound_toggle" checked>
            <span class="toggle-slider"></span>
          </label>
          <div class="option-label">Sound Controls (Placeholder)</div>
      </div>
      <div class="navbar-right">
        <!-- Dark Mode Toggle -->
        <div class="settings-option dark-mode-nav-toggle">
            <label class="toggle-switch">
              <input type="checkbox" id="dark_mode_toggle">
              <span class="toggle-slider"></span>
            </label>
            <div class="option-label">Dark Mode</div>
        </div>
    </div>
    </div>
  </div>
  <div id="menu-overlay" class="menu-overlay"></div> <!-- Overlay for closing menu -->

  <!-- Top Navigation Bar -->
  <nav class="top-navbar">
    <div class="navbar-left">
      <button id="menu-toggle-btn" class="menu-button" title="Open Settings Menu">
        <i class="fas fa-bars"></i>
      </button>
      <div class="logo-container">
        <div class="logo">
          <i class="fas fa-car"></i>
        </div>
        <h1>RC Command Center</h1>
      </div>
    </div>

    <!-- Centered Connection Status -->
    <div id="navbar-connection-status" class="navbar-connection-status"></div>

  </nav>

  <!-- Main Content Area (Flex Container) -->
  <div class="main-content">

    <!-- Left Panel (Connection, Status, Telemetry) -->
    <aside class="left-panel">
      <!-- Connection Panel Card -->
      <div class="connection-panel card">
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
        <div class="status-card" id="authorizationPanel">
          <div class="status-indicator">
            <div class="status-dot auth-dot"></div>
          </div>
          <div class="status-info">
            <div class="status-label">RFID: <span id="auth_state">WAITING</span></div>
            <div class="status-details" id="auth_details">Present RFID card to authenticate</div>
            <div class="status-timestamp" id="auth_time"></div>
          </div>
        </div>

        <!-- Settings Panel Removed from here -->

        <button id="wc_conn" class="connect-btn">
          <i class="fas fa-plug"></i>
          <span>Connect</span>
        </button>
      </div> <!-- End Connection Panel Card -->

      <!-- Telemetry Section -->
      <div class="telemetry-section card">
         <div class="card-header">
             <div class="card-title">
                 <i class="fas fa-tachometer-alt"></i>
                 Telemetry
             </div>
         </div>
         <div class="telemetry-bar">
            <div class="telemetry-item">
               <i class="fas fa-signal telemetry-icon"></i>
              <span class="telemetry-label">Signal</span>
              <span class="telemetry-value" id="telemetry-signal">N/A</span>
            </div>
            <div class="telemetry-item">
              <i class="fas fa-stopwatch telemetry-icon"></i>
              <span class="telemetry-label">Latency</span>
              <span class="telemetry-value" id="telemetry-latency">--- ms</span>
            </div>
            <div class="telemetry-item">
              <i class="fas fa-ruler telemetry-icon"></i>
              <span class="telemetry-label">Distance</span>
              <span class="telemetry-value" id="telemetry-distance">--- cm</span>
            </div>
            <div class="telemetry-item" id="obstacle-status-item">
              <i class="fas fa-car-crash telemetry-icon"></i>
              <span class="telemetry-label">Obstacle</span>
              <span class="telemetry-value" id="obstacle-status">CLEAR</span>
            </div>
          </div>
      </div> <!-- End Telemetry Section -->
    </aside> <!-- End Left Panel -->

    <!-- Center Panel (Camera Feed - Expanded) -->
    <section class="center-panel">
      <div class="camera-container card">
        <div class="card-header">
          <div class="card-title">
            <i class="fas fa-video"></i>
            Camera Feed
          </div>
        </div>

        <div class="video-feed-section">
          <div class="video-feed">
            <img id="camera-stream" src="" alt="RC Car Camera Feed">
            <div class="stream-overlay" id="stream-overlay">
              <i class="fas fa-power-off"></i>
              <span>Camera feed inactive</span>
            </div>
            <button class="full-page-button" title="Toggle Fullscreen">
              <i class="fas fa-expand"></i>
            </button>
             <button class="full-page-mode-exit" title="Exit Fullscreen">
               <i class="fas fa-times"></i>
             </button>
          </div>
          <div class="video-controls">
            <button id="camera-toggle" class="camera-btn" disabled>
              <i class="fas fa-play"></i>
              <span>Start Feed</span>
            </button>
            <button id="refresh-feed" class="camera-btn">
              <i class="fas fa-sync"></i>
              <span>Refresh</span>
            </button>
            <div class="quality-selector">
              <label for="video-quality">Quality:</label>
              <select id="video-quality">
                <option value="high">High</option>
                <option value="medium" selected>Medium</option>
                <option value="low">Low</option>
              </select>
            </div>
          </div>
        </div>

      </div> <!-- End Camera Container Card -->
    </section> <!-- End Center Panel -->

  </div> <!-- End Main Content -->

  

  <div class="toast-container" id="toast-container"></div>

  <!-- Floating Control Button and Panel will be added by JS -->

  <script>
    // Constants
    const CMD_STOP     = 0;
    const CMD_FORWARD  = 1;
    const CMD_BACKWARD = 2;
    const CMD_LEFT     = 4;
    const CMD_RIGHT    = 8;
    const VALUE_UPDATE_ANIMATION_DURATION = 400; // ms, match CSS

    // --- State Variables ---
    let ws = null;
    let lastSentCommand = CMD_STOP;
    let keyboardEnabled = true;
    let keyPressActive = {};
    let latestPing = 0;
    let currentLatency = 0;
    let pingInterval = null;
    let reconnectAttempts = 0;
    const maxReconnectAttempts = 3;
    let rfidAuthorized = false;
    let lastAccessTime = null;
    let authorizedUser = null;
    let videoStreamActive = false;
    let streamUrl = null;
    let videoQuality = 'medium';
    let isFullScreen = false;
    let isDragging = false;
    let floatingControlPanelVisible = false;
    let floatingPanel = null;
    let isLeftMenuOpen = false; // State for left menu
    let soundEnabled = true; // Added from paste.txt
    let modalConfirmCallback = null; // Added for modal

    // --- DOM Element References (Cache for performance) ---
    const particlesContainer = document.getElementById('particles');
    const fullPageExitBtn = document.querySelector('.full-page-mode-exit');
    const statusPanel = document.getElementById('statusPanel');
    const wsStateEl = document.getElementById('ws_state');
    const connectionDetailsEl = document.getElementById('connection_details');
    const authorizationPanel = document.getElementById('authorizationPanel');
    const authStateEl = document.getElementById('auth_state');
    const authDetailsEl = document.getElementById('auth_details');
    const authTimeEl = document.getElementById('auth_time');
    const connectBtn = document.getElementById('wc_conn');
    const telemetrySignalEl = document.getElementById('telemetry-signal');
    const telemetryLatencyEl = document.getElementById('telemetry-latency');
    const particleToggle = document.getElementById('particle_toggle'); // In menu
    const keyboardToggle = document.getElementById('keyboard_toggle'); // In menu
    const soundToggle = document.getElementById('sound_toggle');     // In menu
    const darkModeToggle = document.getElementById('dark_mode_toggle'); // In nav
    const cameraStream = document.getElementById('camera-stream');
    const streamOverlay = document.getElementById('stream-overlay');
    const cameraToggleBtn = document.getElementById('camera-toggle');
    const qualitySelector = document.getElementById('video-quality');
    const fullPageBtn = document.querySelector('.full-page-button');
    const toastContainer = document.getElementById('toast-container');
    const menuToggleBtn = document.getElementById('menu-toggle-btn'); // New
    const leftMenuPopup = document.getElementById('left-menu-popup'); // New
    const closeMenuBtn = document.getElementById('close-menu-btn');   // New
    const menuOverlay = document.getElementById('menu-overlay');      // New
    const navbarConnectionStatus = document.getElementById('navbar-connection-status'); // New

    // Modal Elements (Added from paste.txt structure)
    const confirmationModal = document.getElementById('confirmation-modal'); // Assumes exists in HTML
    const modalTitle = document.getElementById('modal-title');           // Assumes exists in HTML
    const modalMessage = document.getElementById('modal-message');         // Assumes exists in HTML
    const modalConfirm = document.getElementById('modal-confirm');         // Assumes exists in HTML
    const modalCancel = document.getElementById('modal-cancel');           // Assumes exists in HTML
    const closeModalBtn = confirmationModal ? confirmationModal.querySelector('.close-modal') : null; // Assumes exists in HTML

    // --- Sound Effects (Added from paste.txt) ---
    const soundEffects = {
        // Placeholder: Actual Audio objects would be created here, e.g.,
        // click: new Audio('sounds/click.wav'),
        // connect: new Audio('sounds/connect.wav'),
        // disconnect: new Audio('sounds/disconnect.wav'),
        // alert: new Audio('sounds/alert.wav')
        // Using dummy objects to prevent errors if sounds aren't loaded
        click: { play: () => {}, currentTime: 0 },
        connect: { play: () => {}, currentTime: 0 },
        disconnect: { play: () => {}, currentTime: 0 },
        alert: { play: () => {}, currentTime: 0 }
    };

    function toggleSoundEffects(event) {
        soundEnabled = event.target.checked;
        showToast(
            soundEnabled ? 'Sound effects enabled' : 'Sound effects disabled',
            'info'
        );
        // Optional: Persist preference
        // localStorage.setItem('soundEnabled', soundEnabled);
    }

    function playSound(soundName) {
        if (soundEnabled && soundEffects[soundName]) {
            try {
                soundEffects[soundName].currentTime = 0; // Reset playback
                soundEffects[soundName].play().catch(err => console.warn('Sound play failed:', err)); // Use warn for failed play
            } catch (e) {
                console.error('Error playing sound:', soundName, e);
            }
        }
    }

    // --- Utility Functions ---
    function showToast(message, type = 'info', duration = 3000) {
      if (!toastContainer) return;
      const toast = document.createElement('div');
      toast.classList.add('toast', `toast-${type}`);
      let icon = 'info-circle';
      if (type === 'success') icon = 'check-circle';
      if (type === 'error') icon = 'exclamation-circle';
      if (type === 'warning') icon = 'exclamation-triangle'; // Added warning icon
      toast.innerHTML = `
        <div class="toast-icon"><i class="fas fa-${icon}"></i></div>
        <div class="toast-message">${message}</div>
      `;
      toastContainer.appendChild(toast);
      // Trigger reflow before adding 'show' class for animation
      toast.offsetHeight;
      toast.classList.add('show');
      setTimeout(() => {
        toast.classList.remove('show');
        toast.addEventListener('transitionend', () => toast.remove(), { once: true });
      }, duration);
    }

    // --- Initialization Functions ---
    function initParticles() {
      if (!particlesContainer || !particleToggle || !particleToggle.checked || particlesContainer.childElementCount > 0) return; // Added toggle check
      const particleCount = 20;
      const fragment = document.createDocumentFragment();
      for (let i = 0; i < particleCount; i++) {
        const particle = document.createElement('div');
        particle.classList.add('particle');
        const size = Math.floor(Math.random() * 30) + 10;
        particle.style.width = `${size}px`; particle.style.height = `${size}px`;
        particle.style.left = `${Math.random() * 100}%`;
        particle.style.top = `${Math.random() * 100 + 100}%`;
      // Adjust hue for light theme if needed, or keep original
      const hue = Math.floor(Math.random() * 60) + 200; // Original blue/purple range
      particle.style.backgroundColor = `hsla(${hue}, 70%, 70%, 0.3)`;
      particle.style.animationDelay = `${Math.random() * 30}s`;
      fragment.appendChild(particle);
        }
        particlesContainer.appendChild(fragment);
      }

      // --- Left Menu Logic ---
      function openLeftMenu() {
          if (!leftMenuPopup || !menuOverlay) return;
          leftMenuPopup.classList.add('visible');
          menuOverlay.classList.add('visible');
          isLeftMenuOpen = true;
      }

      function closeLeftMenu() {
          if (!leftMenuPopup || !menuOverlay) return;
          leftMenuPopup.classList.remove('visible');
          menuOverlay.classList.remove('visible');
          isLeftMenuOpen = false;
      }

      function toggleLeftMenu() {
          if (isLeftMenuOpen) {
              closeLeftMenu();
          } else {
              openLeftMenu();
          }
      }

      function initLeftMenu() {
          if (!menuToggleBtn || !closeMenuBtn || !menuOverlay) return;
          menuToggleBtn.addEventListener('click', toggleLeftMenu);
          closeMenuBtn.addEventListener('click', closeLeftMenu);
          menuOverlay.addEventListener('click', closeLeftMenu);
      }

      // --- Floating Controls ---
      function initFloatingControls() {
          // Check if button already exists to prevent duplicates on potential re-init
          let floatingButton = document.getElementById('floating-control-button');
          if (floatingButton) return; // Already initialized

          floatingButton = document.createElement('div');
          floatingButton.id = 'floating-control-button';
          floatingButton.innerHTML = '<i class="fas fa-gamepad"></i>';
          document.body.appendChild(floatingButton);

          let offsetX, offsetY, startX, startY;

          const onDragStart = (e) => {
              // Only allow drag with primary button (mouse) or single touch
              if (e.type === 'mousedown' && e.button !== 0) return;
              if (e.type === 'touchstart' && e.touches.length > 1) return;

              e.preventDefault(); // Prevent text selection/default drag behavior only if needed
              isDragging = false; // Reset drag flag initially
              floatingButton.style.cursor = 'grabbing';

              if (e.type === 'touchstart') {
                  startX = e.touches[0].clientX;
                  startY = e.touches[0].clientY;
                  offsetX = startX - floatingButton.offsetLeft;
                  offsetY = startY - floatingButton.offsetTop;
              } else {
                  startX = e.clientX;
                  startY = e.clientY;
                  offsetX = startX - floatingButton.offsetLeft;
                  offsetY = startY - floatingButton.offsetTop;
              }

              document.addEventListener('mousemove', onDragging);
              document.addEventListener('touchmove', onDragging, { passive: false });
              document.addEventListener('mouseup', onDragEnd);
              document.addEventListener('touchend', onDragEnd);
          };

          const onDragging = (e) => {
              // Allow touch scrolling if needed by checking e.cancelable, but default is to prevent
              if (e.cancelable) e.preventDefault();

              isDragging = true; // Set flag when movement occurs

              let currentX, currentY;
              if (e.type === 'touchmove') {
                  currentX = e.touches[0].clientX;
                  currentY = e.touches[0].clientY;
              } else {
                  currentX = e.clientX;
                  currentY = e.clientY;
              }

              let newLeft = currentX - offsetX;
              let newTop = currentY - offsetY;

              // Boundary checks (keep button within viewport)
              const rect = floatingButton.getBoundingClientRect();
              const bodyRect = document.body.getBoundingClientRect();
              newLeft = Math.max(0, Math.min(newLeft, bodyRect.width - rect.width));
              newTop = Math.max(0, Math.min(newTop, bodyRect.height - rect.height));

              floatingButton.style.left = `${newLeft}px`;
              floatingButton.style.top = `${newTop}px`;

              // Reposition panel if visible while dragging
              if (floatingControlPanelVisible && floatingPanel) {
                  positionControlPanel(floatingButton, floatingPanel);
              }
          };

          const onDragEnd = () => {
              floatingButton.style.cursor = 'grab';
              document.removeEventListener('mousemove', onDragging);
              document.removeEventListener('touchmove', onDragging);
              document.removeEventListener('mouseup', onDragEnd);
              document.removeEventListener('touchend', onDragEnd);
              // Set isDragging flag to false *after* a short delay
              // This prevents the click event from firing immediately after dragging
              setTimeout(() => {
                  isDragging = false;
              }, 50); // Reduced delay slightly
          };

          floatingButton.addEventListener('mousedown', onDragStart);
          floatingButton.addEventListener('touchstart', onDragStart, { passive: false });

          // Click handler (only triggers if not dragging)
          floatingButton.addEventListener('click', (e) => {
              // Ensure click doesn't propagate if dragging occurred
              if (isDragging) {
                  e.stopPropagation();
                  e.preventDefault();
                  return;
              }
              if (!isDragging) {
                  toggleFloatingControlPanel(floatingButton);
              }
          });
      }

      function toggleFloatingControlPanel(button) {
          if (floatingControlPanelVisible) {
              hideFloatingControlPanel();
          } else {
              showFloatingControlPanel(button);
          }
      }

      function showFloatingControlPanel(button) {
          if (floatingPanel) return; // Already shown or being created

          floatingPanel = document.createElement('div');
          floatingPanel.id = 'floating-control-panel';

          const buttons = [
              [CMD_FORWARD, 'fa-chevron-up', 'up'],
              [CMD_LEFT, 'fa-chevron-left', 'left'],
              [CMD_STOP, 'fa-stop', 'stop'], // Changed icon to STOP
              [CMD_RIGHT, 'fa-chevron-right', 'right'],
              [CMD_BACKWARD, 'fa-chevron-down', 'down']
          ];

          buttons.forEach(([cmd, icon, gridClass]) => {
              const btn = document.createElement('button');
              btn.classList.add('control-button', gridClass);
              btn.dataset.command = cmd;
              btn.innerHTML = `<i class="fas ${icon}"></i>`;

              btn.addEventListener('pointerdown', handleControlButtonPress);
              btn.addEventListener('pointerup', handleControlButtonRelease);
              btn.addEventListener('pointerleave', handleControlButtonRelease); // Use pointerleave for consistency
              btn.addEventListener('contextmenu', (e) => e.preventDefault());
              // Make buttons unfocusable to avoid interference with keyboard controls
              btn.setAttribute('tabindex', '-1');

              floatingPanel.appendChild(btn);
          });

          document.body.appendChild(floatingPanel);
          positionControlPanel(button, floatingPanel);

          requestAnimationFrame(() => {
              floatingPanel.classList.add('visible');
          });

          floatingControlPanelVisible = true;

          setTimeout(() => {
              document.addEventListener('click', handleClickOutsidePanel, { capture: true, once: true });
              document.addEventListener('touchstart', handleClickOutsidePanel, { capture: true, once: true });
          }, 0);
      }

      function hideFloatingControlPanel() {
          if (!floatingPanel) return;

          floatingPanel.classList.remove('visible');
          
          floatingPanel.addEventListener('transitionend', () => {
              if (floatingPanel && !floatingPanel.classList.contains('visible')) {
                  floatingPanel.remove();
                  floatingPanel = null;
              }
          }, { once: true });

          floatingControlPanelVisible = false;
          document.removeEventListener('click', handleClickOutsidePanel, { capture: true });
          document.removeEventListener('touchstart', handleClickOutsidePanel, { capture: true });
      }

      function handleControlButtonPress(e) {
          e.preventDefault();
          e.stopPropagation();
          const button = e.currentTarget;
          button.classList.add('active');
          const command = parseInt(button.dataset.command);
          sendCommand(command);
          playSound('click'); // Added sound
          // Attempt pointer capture for smoother control if finger slides off
          try { button.setPointerCapture(e.pointerId); } catch(err) {}
      }

      function handleControlButtonRelease(e) {
          e.preventDefault();
          const button = e.currentTarget;

          // Release pointer capture if held by this button
          try {
              if (button.hasPointerCapture(e.pointerId)) {
                  button.releasePointerCapture(e.pointerId);
              }
          } catch(err) {}

          if (button.classList.contains('active')) {
              button.classList.remove('active');
              const command = parseInt(button.dataset.command);
              if (command !== CMD_STOP) {
                  // Check if another control button is still active (e.g., multi-touch scenario)
                  // Find active button within the specific panel
                  const anotherActive = floatingPanel?.querySelector('.control-button.active');
                  if (!anotherActive) {
                      sendCommand(CMD_STOP);
                  }
              }
              // If the released button was STOP, we still send STOP explicitly
              // (sendCommand already handles redundant STOPs if needed)
              else {
                  sendCommand(CMD_STOP);
              }
          }
      }

      function positionControlPanel(button, panel) {
          if (!button || !panel) return;

          const btnRect = button.getBoundingClientRect();
          // Ensure panel dimensions are measured after content is potentially added
          const panelWidth = panel.offsetWidth;
          const panelHeight = panel.offsetHeight;
          const bodyRect = document.body.getBoundingClientRect();
          const margin = 10; // Spacing from button

          // Default: Above and to the left
          let panelTop = btnRect.top - panelHeight - margin;
          let panelLeft = btnRect.left - panelWidth - margin;

          // Adjust horizontal position
          if (panelLeft < margin) { // Off left edge
              panelLeft = btnRect.right + margin; // Try right
              if (panelLeft + panelWidth > bodyRect.width - margin) { // Still off right? Center horizontally relative to button
                  panelLeft = btnRect.left + (btnRect.width / 2) - (panelWidth / 2);
              }
          } else if (panelLeft + panelWidth > bodyRect.width - margin) { // Off right edge
              panelLeft = btnRect.left - panelWidth - margin; // Stick to left (already default)
          }

          // Adjust vertical position
          if (panelTop < margin) { // Off top edge
              panelTop = btnRect.bottom + margin; // Try below
              if (panelTop + panelHeight > bodyRect.height - margin) { // Still off bottom? Center vertically relative to button
                  panelTop = btnRect.top + (btnRect.height / 2) - (panelHeight / 2);
              }
          } else if (panelTop + panelHeight > bodyRect.height - margin) { // Off bottom edge
              panelTop = btnRect.top - panelHeight - margin; // Stick above (already default)
          }

          // Final boundary clamp
          panelLeft = Math.max(margin, Math.min(panelLeft, bodyRect.width - panelWidth - margin));
          panelTop = Math.max(margin, Math.min(panelTop, bodyRect.height - panelHeight - margin));

          panel.style.top = `${panelTop}px`;
          panel.style.left = `${panelLeft}px`;

          // Adjust transform origin based on final position relative to button center
          let originX = (panelLeft + panelWidth / 2) < (btnRect.left + btnRect.width / 2) ? 'right' : 'left';
          let originY = (panelTop + panelHeight / 2) < (btnRect.top + btnRect.height / 2) ? 'bottom' : 'top';
          panel.style.transformOrigin = `${originY} ${originX}`;
      }

      function handleClickOutsidePanel(e) {
          const floatingButton = document.getElementById('floating-control-button');
          // If click is outside panel AND outside the main button, hide panel
          if (floatingPanel && !floatingPanel.contains(e.target) && (!floatingButton || !floatingButton.contains(e.target))) {
              hideFloatingControlPanel();
          } else {
              // Click was inside panel or on the button, need to re-attach listener for the *next* click
              setTimeout(() => {
                  // Only re-attach if the panel is still visible
                  if (floatingControlPanelVisible) {
                      document.addEventListener('click', handleClickOutsidePanel, { capture: true, once: true });
                      document.addEventListener('touchstart', handleClickOutsidePanel, { capture: true, once: true });
                  }
              }, 0);
          }
      }

      // --- Video Stream ---
      function initVideoStreamControls() {
          if (!qualitySelector || !cameraToggleBtn || !streamOverlay) return;
          qualitySelector.addEventListener('change', function() {
              videoQuality = this.value;
              if (videoStreamActive) {
                updateStreamUrl(); // Quality might affect URL in some implementations
                refreshVideoStream(); // Reload with new quality if necessary
                showToast(`Video quality changed to ${videoQuality}`, 'info'); // Feedback
              }
          });
          cameraToggleBtn.addEventListener('click', () => {
              if (videoStreamActive) { pauseVideoStream(); }
              else { startVideoStream(); }
          });
          streamOverlay.classList.remove('hidden');
          streamOverlay.innerHTML = '<i class="fas fa-power-off"></i><span>Camera feed inactive</span>';
          cameraToggleBtn.disabled = true; // Disabled until connected
      }

      function initFullPageMode() {
          if (!fullPageBtn || !fullPageExitBtn || !cameraStream) {
              console.warn("Full page mode elements not found."); return;
          }
          fullPageBtn.addEventListener('click', () => {
              document.body.classList.add('full-page-mode');
              isFullScreen = true;
              // Request fullscreen on the body or a container, not just the video feed element
              // This allows overlay elements like controls to remain within the fullscreen context
              document.documentElement.requestFullscreen?.().catch(err => console.error("Fullscreen request failed:", err));
          });
          fullPageExitBtn.addEventListener('click', () => {
              document.body.classList.remove('full-page-mode');
              isFullScreen = false;
              document.exitFullscreen?.().catch(err => console.error("Exit fullscreen failed:", err));
          });
          document.addEventListener('fullscreenchange', handleFullscreenChange);
          // Add vendor prefixes if needed, though modern browsers handle standard 'fullscreenchange'
      }

      function handleFullscreenChange() {
          const isBrowserFullscreen = !!document.fullscreenElement;
          if (isBrowserFullscreen !== isFullScreen) {
              isFullScreen = isBrowserFullscreen; // Sync state
              if (isBrowserFullscreen) {
                  document.body.classList.add('full-page-mode');
              } else {
                  // Ensure class is removed if exited via ESC key etc.
                  document.body.classList.remove('full-page-mode');
              }
          }
      }

      // --- Keyboard Controls ---
      function setupKeyboardControls() {
        document.addEventListener('keydown', handleKeyDown);
        document.addEventListener('keyup', handleKeyUp);
      }

      // --- Dark Mode ---
      function initDarkMode() {
          if (!darkModeToggle) return;

          darkModeToggle.addEventListener('change', () => {
              setDarkMode(darkModeToggle.checked);
              showToast(darkModeToggle.checked ? 'Dark mode enabled' : 'Light mode enabled', 'info'); // Feedback
          });

          // Apply initial dark mode state from localStorage or system preference
          const prefersDark = window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches;
          const savedMode = localStorage.getItem('darkMode');

          if (savedMode === 'enabled' || (!savedMode && prefersDark)) {
              setDarkMode(true);
              if (darkModeToggle) darkModeToggle.checked = true;
          } else {
              setDarkMode(false);
              if (darkModeToggle) darkModeToggle.checked = false;
          }
      }

      function setDarkMode(enabled) {
          if (enabled) {
              document.body.classList.add('dark-mode'); // Use body for global styles
              localStorage.setItem('darkMode', 'enabled');
          } else {
              document.body.classList.remove('dark-mode');
              localStorage.setItem('darkMode', 'disabled');
          }
          // Add any JS logic needed when theme changes (e.g., chart colors)
      }

      // --- Event Handlers ---
      function handleKeyDown(event) {
        // Ignore if keyboard controls disabled, typing in input/select, or menu is open
        if (!keyboardEnabled || event.target.matches('input, select, textarea') || isLeftMenuOpen || event.repeat) return;
        
        let command = -1;
        const key = event.key.toLowerCase(); // Use toLowerCase for case-insensitivity

        switch(key) {
          case 'arrowup': case 'w': command = CMD_FORWARD; break;
          case 'arrowdown': case 's': command = CMD_BACKWARD; break;
          case 'arrowleft': case 'a': command = CMD_LEFT; break;
          case 'arrowright': case 'd': command = CMD_RIGHT; break;
          case ' ': case 'h': command = CMD_STOP; break; // Spacebar or 'h' for Halt/Horn/Stop
          default: return;
        }

        // Prevent default browser action for arrow keys and space
        if (['arrowup', 'arrowdown', 'arrowleft', 'arrowright', ' '].includes(key)) {
          event.preventDefault();
        }

        if (keyPressActive[key]) return; // Prevent re-sending if key already down
        keyPressActive[key] = true;

        // Determine the command to send based on currently pressed keys
        const currentCommand = calculateCommandFromKeys();
        sendCommand(currentCommand);
        if (command !== CMD_STOP && command !== -1) playSound('click'); // Play sound on key press (not STOP)
      }

      function handleKeyUp(event) {
          // Ignore if keyboard controls disabled, typing in input/select, or menu is open
          if (!keyboardEnabled || event.target.matches('input, select, textarea') || isLeftMenuOpen) return;

          const key = event.key.toLowerCase();
          if (!keyPressActive[key]) return; // Only act if the key was active

          // Prevent default browser action for arrow keys and space
          if (['arrowup', 'arrowdown', 'arrowleft', 'arrowright', ' '].includes(key)) {
              event.preventDefault();
          }

          keyPressActive[key] = false; // Reset flag for the released key

          // Calculate the new command based on remaining pressed keys
          const currentCommand = calculateCommandFromKeys();
          sendCommand(currentCommand);
      }

      function calculateCommandFromKeys() {
          let command = CMD_STOP; // Start with stop
          // Combine directional commands - allows diagonal movement if coded on Arduino
          if (keyPressActive['arrowup'] || keyPressActive['w']) {
              command |= CMD_FORWARD;
          }
          if (keyPressActive['arrowdown'] || keyPressActive['s']) {
              command |= CMD_BACKWARD;
          }
          // Note: Typically forward/backward are mutually exclusive. Prioritize forward.
          if ((command & CMD_FORWARD) && (command & CMD_BACKWARD)) {
              command &= ~CMD_BACKWARD; // Remove backward if forward is pressed
          }

          if (keyPressActive['arrowleft'] || keyPressActive['a']) {
              command |= CMD_LEFT;
          }
          if (keyPressActive['arrowright'] || keyPressActive['d']) {
              command |= CMD_RIGHT;
          }
          // Prioritize left over right if both pressed? (Optional)
          if ((command & CMD_LEFT) && (command & CMD_RIGHT)) {
              command &= ~CMD_RIGHT; // Example: prioritize left
          }

          // If space/'h' is pressed, force STOP, overriding movement keys
          if (keyPressActive[' '] || keyPressActive['h']) {
              command = CMD_STOP;
          }

          return command;
      }

      function toggleParticles(event) {
        if (!particlesContainer) return;
        const enabled = event.target.checked;
        particlesContainer.style.display = enabled ? 'block' : 'none';
        if (enabled && particlesContainer.childElementCount === 0) { initParticles(); }
        // Persist choice?
        // localStorage.setItem('particlesEnabled', enabled);
      }

      function toggleKeyboardControls(event) {
        keyboardEnabled = event.target.checked;
        keyPressActive = {}; // Reset active keys when toggling
        if (!keyboardEnabled) {
            sendCommand(CMD_STOP); // Ensure car stops if controls are disabled
        }
        showToast(`Keyboard controls ${keyboardEnabled ? 'enabled' : 'disabled'}`, 'info');
        // Persist choice?
        // localStorage.setItem('keyboardEnabled', keyboardEnabled);
      }

      function handleConnectionToggle() {
        if (!ws || ws.readyState === WebSocket.CLOSED) {
            connectWebSocket();
        } else if (ws.readyState === WebSocket.OPEN) {
            // Use modal confirmation - Added from paste.txt
            showConfirmationDialog(
                'Disconnect',
                'Are you sure you want to disconnect from the RC car?',
                disconnectWebSocket // Pass the disconnect function as the callback
            );
        }
        // Do nothing if CONNECTING or CLOSING
      }

      // --- WebSocket Logic ---
      function connectWebSocket() {
        if (ws && ws.readyState !== WebSocket.CLOSED && ws.readyState !== WebSocket.CLOSING) {
            console.warn("WebSocket is already open or connecting.");
            return;
        }
        try {
          const host = window.location.hostname || '192.168.4.1'; // Default IP if hostname fails
          const wsUrl = `ws://${host}:81`; // Standard port for ESP AsyncWebSocket
          ws = new WebSocket(wsUrl);
          console.log(`Attempting connect: ${wsUrl}`);
          updateConnectionStatus('CONNECTING'); // Update main status
          updateNavbarStatus('CONNECTING');   // Update navbar status

          if (connectBtn) { // Check if element exists
            connectBtn.innerHTML = '<i class="fas fa-spinner fa-spin"></i><span>Connecting</span>';
            connectBtn.disabled = true;
          }
          if (cameraToggleBtn) cameraToggleBtn.disabled = true; // Disable camera controls during connection attempt

          ws.onopen = handleWebSocketOpen;
          ws.onclose = handleWebSocketClose;
          ws.onmessage = handleWebSocketMessage;
          ws.onerror = handleWebSocketError;
        } catch (error) {
          console.error('WS Instantiate Error:', error);
          showToast('Failed connection initialization', 'error');
          updateConnectionStatus('ERROR');
          updateNavbarStatus('ERROR', 'Init Failed');
          if (connectBtn) { // Check if element exists
            connectBtn.innerHTML = '<i class="fas fa-plug"></i><span>Connect</span>';
            connectBtn.disabled = false;
          }
          resetTelemetryDisplay();
          ws = null; // Ensure ws is null after instantiation error
        }
      }

      function disconnectWebSocket() {
        if (ws && (ws.readyState === WebSocket.OPEN || ws.readyState === WebSocket.CONNECTING)) {
            playSound('disconnect'); // Play sound before closing
            ws.close(1000, "User disconnected"); // Normal closure code
            // Status updates will happen in handleWebSocketClose
            if (connectBtn) { // Check if element exists
              connectBtn.innerHTML = '<i class="fas fa-spinner fa-spin"></i><span>Disconnecting</span>';
              connectBtn.disabled = true;
            }
        } else {
            console.log("WebSocket not open or connecting, cannot disconnect.");
            // Ensure UI reflects disconnected state if it wasn't already
            handleWebSocketClose({ code: 1000, reason: "Already disconnected", wasClean: true });
        }
      }

      function handleWebSocketOpen() {
        console.log('WS Connected');
        updateConnectionStatus('CONNECTED');
        updateNavbarStatus('CONNECTED');
        statusPanel?.classList.add('connected'); // Use optional chaining
        if (connectBtn) { // Check if element exists
            connectBtn.innerHTML = '<i class="fas fa-unlink"></i><span>Disconnect</span>';
            connectBtn.classList.add('disconnect-btn');
            connectBtn.disabled = false;
        }
        if (connectionDetailsEl) connectionDetailsEl.textContent = `Connected to ${ws.url.replace(/^ws:\/\//, '')}`; // Show cleaner URL
        if (cameraToggleBtn) cameraToggleBtn.disabled = false; // Enable camera button
        showToast('Connected successfully', 'success');
        playSound('connect'); // Added sound
        reconnectAttempts = 0; // Reset on successful connection
        clearInterval(pingInterval); // Clear existing interval just in case
        pingInterval = setInterval(sendPing, 3000); // Start pinging
        sendPing(); // Send initial ping immediately
        resetAuthorizationStatus(); // Reset RFID state
        resetTelemetryDisplay(); // Clear old telemetry
        if (streamOverlay) {
            streamOverlay.innerHTML = '<i class="fas fa-play"></i><span>Press Start Feed</span>';
            streamOverlay.classList.remove('hidden');
        }
        if (cameraToggleBtn) cameraToggleBtn.innerHTML = '<i class="fas fa-play"></i><span>Start Feed</span>';
      }

      function handleWebSocketClose(event) {
        console.log(`WS Closed: Code=${event.code}, Reason='${event.reason}', Clean=${event.wasClean}`);
        updateConnectionStatus('DISCONNECTED');
        updateNavbarStatus('DISCONNECTED'); // Clear navbar status on close
        statusPanel?.classList.remove('connected', 'connecting'); // Also remove connecting class
        if (connectBtn) { // Check if element exists
            connectBtn.innerHTML = '<i class="fas fa-plug"></i><span>Connect</span>';
            connectBtn.classList.remove('disconnect-btn');
            connectBtn.disabled = false; // Allow reconnect attempt
        }
        if (connectionDetailsEl) connectionDetailsEl.textContent = 'Not connected to device';
        if (cameraToggleBtn) cameraToggleBtn.disabled = true; // Disable camera
        clearInterval(pingInterval); pingInterval = null;
        resetTelemetryDisplay();
        resetAuthorizationStatus();
        pauseVideoStream(); // Ensure video stops and overlay shows disconnected state
        if (event.wasClean && event.code === 1000) { // Only show disconnect toast if initiated by user
            showToast('Disconnected', 'info');
            // playSound('disconnect'); // Moved sound to disconnectWebSocket initiation
        } else if (!event.wasClean) {
            // Non-clean closure often means network error or server unavailable
            showToast(`Connection lost (Code: ${event.code})`, 'error');
            updateNavbarStatus('ERROR', 'Lost Connection'); // Show error in navbar briefly?
            // Optional: Attempt reconnect here if desired
        }
        ws = null; // Set to null to allow connectWebSocket to create a new instance
        lastSentCommand = CMD_STOP; // Reset command state
        keyPressActive = {}; // Reset keys
      }

      function handleWebSocketMessage(event) {
        const message = event.data;
        // console.log("WS Recv:", message); // Can be noisy, uncomment if debugging
        if (message.startsWith('PONG:')) {
          const now = Date.now();
          currentLatency = now - latestPing;
          updateTelemetryValue(telemetryLatencyEl, `${currentLatency} ms`, false); // Update frequently, no animation
        } else if (message.startsWith('TELEMETRY:')) {
          processTelemetry(message);
        } else if (message.startsWith('RFID:')) {
          handleRfidAuthorization(message);
        } 
          else if (message.startsWith('OBSTACLE:')) {
          try {
            const obstacleData = JSON.parse(message.substring('OBSTACLE:'.length));
            const obstacleStatusEl = document.getElementById('obstacle-status');
            const obstacleItem = obstacleStatusEl?.closest('.telemetry-item');
            
            if (obstacleData.active) {
              // Obstacle avoidance is active
              if (obstacleStatusEl) obstacleStatusEl.textContent = 'AVOIDING';
              if (obstacleItem) {
                obstacleItem.classList.remove('warning-color');
                obstacleItem.classList.add('danger-color');
              }
              
              // Show notification to user
              showToast(`Obstacle detected at ${obstacleData.distance}cm - Moving backward`, 'warning');
              
              // Disable forward controls temporarily
              disableForwardControls(true);
            } else {
              // Obstacle avoidance has cleared
              if (obstacleStatusEl) obstacleStatusEl.textContent = 'CLEAR';
              if (obstacleItem) obstacleItem.classList.remove('warning-color', 'danger-color');
              
              // Show notification
              showToast('Path clear - Obstacle avoidance deactivated', 'success');
              
              // Re-enable controls
              disableForwardControls(false);
            }
          } catch (error) {
            console.error('Obstacle data parse error:', error);
          }
        }
        else {
                console.log("Unknown WS msg:", message);
              }
      }

      function handleWebSocketError(error) {
        console.error('WS Error:', error);
        showToast('WebSocket connection error', 'error');
        updateConnectionStatus('ERROR'); // Update main status
        updateNavbarStatus('ERROR', 'Connection Error'); // Show error in navbar
        resetTelemetryDisplay();
        // The 'onclose' event usually follows 'onerror'.
        // We might not need to change button state here if onclose handles it.
          if (ws && ws.readyState !== WebSocket.OPEN) {
            if(connectBtn) {
                connectBtn.innerHTML = '<i class="fas fa-plug"></i><span>Connect</span>';
                connectBtn.classList.remove('disconnect-btn');
                connectBtn.disabled = false; // Allow retry
            }
            if(cameraToggleBtn) cameraToggleBtn.disabled = true;
        }
          // Don't explicitly call ws.close() here, let the browser handle error/close sequence
      }

      function sendPing() {
        if (ws && ws.readyState === WebSocket.OPEN) {
          latestPing = Date.now();
          ws.send('PING\r\n'); // Using CRLF as common ESP terminator
        } else {
          // Stop pinging if WS is not open
          clearInterval(pingInterval); pingInterval = null; currentLatency = 0;
          if (telemetryLatencyEl) updateTelemetryValue(telemetryLatencyEl, '--- ms', false);
        }
      }

      // Add this function to your JavaScript code
      function disableForwardControls(disabled) {
        // This is a helper function to disable forward movement controls
        // when obstacle avoidance is active
        
        // Update keyboard controls
        if (disabled) {
          keyPressActive['arrowup'] = false;
          keyPressActive['w'] = false;
        }
        
        // Update UI to reflect disabled state if needed
        const forwardButton = document.querySelector('#floating-control-panel .control-button.up');
        if (forwardButton) {
          forwardButton.disabled = disabled;
          if (disabled) {
            forwardButton.classList.add('disabled');
          } else {
            forwardButton.classList.remove('disabled');
          }
        }
      }

      // --- Command & Control ---
      function sendCommand(command) {
          // Check if RFID is authorized
          if (!rfidAuthorized) {
            showToast('Access denied: RFID not authenticated', 'error');
            console.warn('Command blocked: RFID authentication required');
            return; // Prevent sending commands if not authorized
          }
        
          // Only send if WebSocket is open and command is different from last sent,
          // OR always send STOP command immediately when requested.
          if (ws && ws.readyState === WebSocket.OPEN) {
            if (command !== lastSentCommand || command === CMD_STOP) {
              ws.send(command + "\r\n"); // Send command + CRLF
              lastSentCommand = command;
              console.log(`Sent command: ${command}`); // Debug log
            }
          } else if ((!ws || ws.readyState !== WebSocket.OPEN) && command !== CMD_STOP) {
            console.warn("WS not open. Command not sent:", command);
            showToast("Not connected", "warning", 1000);
          }
      }

      // --- Telemetry & Status Updates ---
      function updateConnectionStatus(status) {
        if (wsStateEl) wsStateEl.textContent = status;
        // Add/remove classes for styling if needed (e.g., connecting animation)
        statusPanel?.classList.remove('connecting', 'connected'); // Clear both
        if (status === 'CONNECTING') { statusPanel?.classList.add('connecting'); }
        else if (status === 'CONNECTED') { statusPanel?.classList.add('connected'); }
      }

      // Update Navbar Status Display
      function updateNavbarStatus(status, message = '') {
          if (!navbarConnectionStatus) return;

          navbarConnectionStatus.classList.remove('connected', 'error', 'connecting'); // Clear previous states
          navbarConnectionStatus.style.display = 'none'; // Default to hidden

          switch (status) {
              case 'CONNECTING':
                  navbarConnectionStatus.innerHTML = `<i class="fas fa-spinner fa-spin"></i> CONNECTING...`;
                  navbarConnectionStatus.classList.add('connecting');
                  navbarConnectionStatus.style.display = 'flex'; // Show (use flex for icon alignment)
                  navbarConnectionStatus.style.alignItems = 'center';
                  break;
              case 'CONNECTED':
                  navbarConnectionStatus.textContent = 'CONNECTED';
                  navbarConnectionStatus.classList.add('connected');
                  navbarConnectionStatus.style.display = 'block'; // Show
                  // Optional: Hide after a few seconds?
                  // setTimeout(() => { if (ws && ws.readyState === WebSocket.OPEN) navbarConnectionStatus.style.display = 'none'; }, 3000);
                  break;
              case 'ERROR':
                  navbarConnectionStatus.textContent = message || 'UNABLE TO CONNECT';
                  navbarConnectionStatus.classList.add('error');
                  navbarConnectionStatus.style.display = 'block'; // Show
                  // Optional: Hide error after a few seconds
                  setTimeout(() => { if (navbarConnectionStatus.classList.contains('error')) navbarConnectionStatus.style.display = 'none'; }, 5000);
                  break;
              case 'DISCONNECTED':
              default:
                  navbarConnectionStatus.textContent = '';
                  navbarConnectionStatus.style.display = 'none'; // Hide explicitly
                  break;
          }
      }

      function updateTelemetryValue(element, newValue, animate = true) {
        if (element && element.textContent !== newValue) {
          element.textContent = newValue;
          const item = element.closest('.telemetry-item');
          if (item && animate) {
            item.classList.add('value-updated');
            // Use transitionend or a timeout matching the CSS animation duration
            setTimeout(() => {
              item?.classList.remove('value-updated'); // Check item still exists
            }, VALUE_UPDATE_ANIMATION_DURATION);
          }
        }
      }

      function processTelemetry(data) {
        try {
              const jsonData = data.substring('TELEMETRY:'.length);
              const telemetryData = JSON.parse(jsonData);

              // Signal (RSSI)
              const rssiValue = telemetryData.rssi;
              let signalText = 'N/A';
              const signalItem = telemetrySignalEl?.closest('.telemetry-item');
              let signalIconClass = 'fas fa-signal telemetry-icon'; // Default icon class
              if (signalItem) signalItem.classList.remove('danger-color', 'warning-color'); // Clear status colors

              if (rssiValue !== undefined && rssiValue !== null) {
                  const rssi = parseInt(rssiValue, 10);
                  // Using standard fa-signal with level classes for better visuals
                  if (rssi >= -60) { signalText = 'Excellent'; signalIconClass = 'fas fa-signal telemetry-icon level-4'; }
                  else if (rssi >= -70) { signalText = 'Good'; signalIconClass = 'fas fa-signal telemetry-icon level-3'; }
                  else if (rssi >= -80) { signalText = 'Fair'; signalIconClass = 'fas fa-signal telemetry-icon level-2 warning-color'; if (signalItem) signalItem.classList.add('warning-color'); }
                  else if (rssi < -80) { signalText = 'Weak'; signalIconClass = 'fas fa-signal telemetry-icon level-1 danger-color'; if (signalItem) signalItem.classList.add('danger-color'); }
                  // Example CSS required: .fa-signal.level-1::before { opacity: 0.25; } .fa-signal.level-2::before { opacity: 0.5; } etc.
              }
              updateTelemetryValue(telemetrySignalEl, signalText);
              const signalIcon = signalItem?.querySelector('.telemetry-icon');
              if (signalIcon) signalIcon.className = signalIconClass; // Update entire class string

              // Distance
              const distanceValue = telemetryData.distance;
              if (distanceValue !== undefined && distanceValue !== null) {
                updateTelemetryValue(document.getElementById('telemetry-distance'), `${distanceValue} cm`);
                
                // Update obstacle status based on distance
                const obstacleStatusEl = document.getElementById('obstacle-status');
                const obstacleItem = obstacleStatusEl?.closest('.telemetry-item');
                
                if (distanceValue !== undefined && obstacleStatusEl) {
                  if (distanceValue <= 100) {
                    obstacleStatusEl.textContent = 'NEAR';
                    if (obstacleItem) obstacleItem.classList.add('warning-color');
                  } else {
                    obstacleStatusEl.textContent = 'CLEAR';
                    if (obstacleItem) obstacleItem.classList.remove('warning-color', 'danger-color');
                  }
                }
                
                // Update obstacle avoidance mode status
                const avoidanceActive = telemetryData.obstacleAvoidance;
                if (avoidanceActive && obstacleStatusEl) {
                  obstacleStatusEl.textContent = 'AVOIDING';
                  if (obstacleItem) {
                    obstacleItem.classList.remove('warning-color');
                    obstacleItem.classList.add('danger-color');
                  }
                }
              }
        } catch (error) {
            console.error('Telemetry Parse Error:', error, "Data:", data);
            showToast('Error processing telemetry', 'error', 2000);
            // Optionally reset only specific fields on error
            // resetTelemetryDisplay(false);
        }
      }

      function resetTelemetryDisplay(animate = false) {
        updateTelemetryValue(telemetrySignalEl, 'N/A', animate);
        updateTelemetryValue(telemetryLatencyEl, '--- ms', animate);
        currentLatency = 0;

        const signalItem = telemetrySignalEl?.closest('.telemetry-item');
        const signalIcon = signalItem?.querySelector('.telemetry-icon');
        if (signalIcon) signalIcon.className = 'fas fa-signal telemetry-icon'; // Default icon
        if (signalItem) signalItem.classList.remove('danger-color', 'warning-color');
      }

      function handleRfidAuthorization(data) {
        if (!authorizationPanel || !authStateEl || !authDetailsEl || !authTimeEl) return;
        try {
            const authData = JSON.parse(data.substring('RFID:'.length));
            rfidAuthorized = authData.authorized;
            authorizationPanel.classList.remove('authorized', 'unauthorized', 'waiting');
            authStateEl.classList.remove('authorized', 'unauthorized'); // Also clear status classes from text element

            if (rfidAuthorized) {
                authorizedUser = authData.user || 'Unknown';
                lastAccessTime = new Date();
                authorizationPanel.classList.add('authorized');
                authStateEl.textContent = 'AUTHORIZED';
                authStateEl.classList.add('authorized'); // Add class to text
                authDetailsEl.textContent = `User: ${authorizedUser}`;
                authTimeEl.textContent = `Access: ${lastAccessTime.toLocaleTimeString()}`;
                showToast(`RFID Authorized: ${authorizedUser}`, 'success');
                playSound('connect'); // Use 'connect' sound for success
            } else {
                authorizedUser = null; lastAccessTime = null;
                authorizationPanel.classList.add('unauthorized');
                authStateEl.textContent = 'UNAUTHORIZED';
                authStateEl.classList.add('unauthorized'); // Add class to text
                authDetailsEl.textContent = authData.message || 'Access Denied';
                authTimeEl.textContent = '';
                showToast(`RFID Denied: ${authData.message || 'Invalid Card'}`, 'error');
                playSound('alert'); // Use 'alert' sound for failure
            }
        } catch (error) {
            console.error('RFID Parse Error:', error, "Data:", data);
            showToast('Invalid RFID data received', 'error');
            resetAuthorizationStatus();
        }
      }

      function resetAuthorizationStatus() {
        rfidAuthorized = false; authorizedUser = null; lastAccessTime = null;
        if (authorizationPanel) {
            authorizationPanel.classList.remove('authorized', 'unauthorized');
            authorizationPanel.classList.add('waiting'); // Default state
        }
        if(authStateEl) {
            authStateEl.textContent = 'WAITING';
            authStateEl.classList.remove('authorized', 'unauthorized');
        }
        if(authDetailsEl) authDetailsEl.textContent = 'Present RFID card';
        if(authTimeEl) authTimeEl.textContent = '';
      }

      // --- Video Stream Logic ---
      function updateStreamUrl() {
          const host = window.location.hostname || '192.168.4.1';
          const streamPort = 81; // ESP32-CAM default stream port is often 81
          streamUrl = `http://${host}:${streamPort}/stream`; // Standard endpoint for camera stream
          console.log("Stream URL set to:", streamUrl);
      }

      function startVideoStream() {
          if (!ws || ws.readyState !== WebSocket.OPEN) {
              showToast('Connect to RC Car first', 'error');
              return;
          }
          if (videoStreamActive) {
              console.log("Video stream already active.");
              return;
          }
          if (!cameraStream || !streamOverlay || !cameraToggleBtn) return;

          updateStreamUrl(); // Ensure URL is current
          streamOverlay.classList.remove('hidden');
          streamOverlay.innerHTML = '<i class="fas fa-spinner fa-spin"></i><span>Connecting camera...</span>';
          cameraToggleBtn.innerHTML = '<i class="fas fa-spinner fa-spin"></i><span>Starting...</span>';
          cameraToggleBtn.disabled = true;

          const cacheBustedUrl = `${streamUrl}?_t=${Date.now()}`; // Prevent browser caching
          cameraStream.src = cacheBustedUrl;
          console.log("Attempting to load camera stream:", cameraStream.src);

          // Clear previous listeners to prevent multiple triggers
          cameraStream.onload = null;
          cameraStream.onerror = null;

          cameraStream.onload = () => {
              // Check if src is still the one we set (prevents race condition if paused quickly)
              if (cameraStream.src.startsWith(streamUrl)) {
                  console.log("Camera stream loaded successfully.");
                  streamOverlay.classList.add('hidden'); // Hide overlay on success
                  videoStreamActive = true;
                  cameraToggleBtn.innerHTML = '<i class="fas fa-pause"></i><span>Pause Feed</span>';
                  cameraToggleBtn.disabled = false;
                  showToast('Camera feed started', 'success');
              } else {
                  console.log("Stream loaded, but src was changed before onload fired. Ignoring.");
              }
          };
          cameraStream.onerror = () => {
              console.error("Failed to load camera stream:", cameraStream.src);
              // Only show error if we weren't intentionally pausing/disconnecting
              if (cameraStream.src !== '' && ws && ws.readyState === WebSocket.OPEN) {
                  streamOverlay.classList.remove('hidden');
                  streamOverlay.innerHTML = '<i class="fas fa-exclamation-circle"></i><span>Camera connection failed</span>';
                  videoStreamActive = false;
                  cameraToggleBtn.innerHTML = '<i class="fas fa-play"></i><span>Start Feed</span>';
                  cameraToggleBtn.disabled = false; // Allow retry
                  showToast('Camera connection failed', 'error');
                  cameraStream.src = ''; // Clear broken src to stop browser retries
              } else {
                  console.log("Stream error occurred after src was cleared or WS disconnected. Ignored.");
              }
          };
      }

      function refreshVideoStream() {
          if (!videoStreamActive || !streamUrl || !cameraStream) return;
          console.log("Refreshing video stream source");
          // Re-apply src with cache buster - might cause flicker but ensures fresh stream
          const oldSrc = cameraStream.src;
          cameraStream.src = ''; // Clear src first to force stop
          setTimeout(() => { // Short delay before setting new src
            if(videoStreamActive) { // Check again if still active
                cameraStream.src = `${streamUrl}?_t=${Date.now()}`;
            }
          }, 50);
      }

      function pauseVideoStream() {
          if (!cameraStream || !streamOverlay || !cameraToggleBtn) return;
          // Check if it's truly active or already paused/stopped
          if (!videoStreamActive && cameraStream.src === '') return;

          console.log("Pausing video stream.");
          cameraStream.src = ''; // Main way to stop the stream
          videoStreamActive = false;

          streamOverlay.classList.remove('hidden'); // Show overlay
          // Set overlay and button text based on WebSocket connection status
          if (ws && ws.readyState === WebSocket.OPEN) {
              streamOverlay.innerHTML = '<i class="fas fa-play"></i><span>Feed Paused</span>';
              cameraToggleBtn.innerHTML = '<i class="fas fa-play"></i><span>Resume Feed</span>';
              cameraToggleBtn.disabled = false; // Can resume if connected
          } else {
              streamOverlay.innerHTML = '<i class="fas fa-power-off"></i><span>Disconnected</span>';
              cameraToggleBtn.innerHTML = '<i class="fas fa-play"></i><span>Start Feed</span>';
              cameraToggleBtn.disabled = true; // Cannot start if disconnected
          }
      }

      // --- Modal Dialog Logic ---
      function showConfirmationDialog(title, message, callback) {
        if (!confirmationModal || !modalTitle || !modalMessage || !modalConfirm) {
            console.error("Modal elements not found!");
            // Fallback to browser confirm if modal is missing
            if (confirm(`${title}\n\n${message}`)) {
                if (typeof callback === 'function') {
                    callback();
                }
            }
            return;
        }

        modalTitle.textContent = title;
        modalMessage.textContent = message;
        modalConfirmCallback = callback; // Store the callback

        confirmationModal.classList.add('active');
      }

      function hideModal() {
        if (confirmationModal) {
          confirmationModal.classList.remove('active');
        }
        modalConfirmCallback = null; // Clear callback when hiding
      }

      function handleModalConfirm() {
        // This function is now just the event handler for the button
        if (typeof modalConfirmCallback === 'function') {
            modalConfirmCallback(); // Execute the stored callback
        }
        hideModal(); // Hide modal after confirm action
      }

      function initModalControls() {
          if (!modalConfirm || !modalCancel || !closeModalBtn) {
              // Only log warning if the modal itself exists but buttons are missing
              if (confirmationModal) console.warn("Modal control buttons not found.");
              return;
          }
          modalConfirm.addEventListener('click', handleModalConfirm);
          modalCancel.addEventListener('click', hideModal);
          closeModalBtn.addEventListener('click', hideModal);

          // Close modal if clicking outside of it
          confirmationModal.addEventListener('click', (event) => {
              if (event.target === confirmationModal) {
                  hideModal();
              }
          });
      }

      // --- Initial Page Load ---
      function initializeApp() {
        // Core UI element check
        if (!particleToggle || !keyboardToggle || !soundToggle || !connectBtn || !darkModeToggle || !menuToggleBtn || !navbarConnectionStatus) {
            console.error("Core UI elements missing. Initialization incomplete.");
            // Consider a user-friendly message instead of alert
            showToast("Interface Error: Reload required.", 'error', 10000);
            // Optionally disable critical controls
            if(connectBtn) connectBtn.disabled = true;
            return;
        }

        // Setup Event Listeners
        particleToggle.addEventListener('change', toggleParticles);
        keyboardToggle.addEventListener('change', toggleKeyboardControls);
        soundToggle.addEventListener('change', toggleSoundEffects); // Added listener
        connectBtn.addEventListener('click', handleConnectionToggle);
        // Dark mode toggle listener is setup in initDarkMode()
        // Left menu listeners are setup in initLeftMenu()
        // Video control listeners are setup in initVideoStreamControls()
        // Fullscreen listeners setup in initFullPageMode()
        // Modal listeners are setup in initModalControls()

        // Initialize Modules/Features
        if (particleToggle.checked) initParticles();
        initLeftMenu(); // Setup menu toggle functionality
        initFloatingControls();
        initVideoStreamControls();
        initFullPageMode();
        setupKeyboardControls();
        initDarkMode(); // Apply theme and setup toggle
        initModalControls(); // Setup modal buttons

        // Initial UI State
        updateConnectionStatus('DISCONNECTED');
        updateNavbarStatus('DISCONNECTED'); // Ensure navbar status is hidden initially
        resetTelemetryDisplay(false);
        resetAuthorizationStatus();
        if (cameraToggleBtn) cameraToggleBtn.disabled = true; // Disabled until connected
        if (streamOverlay) {
            streamOverlay.classList.remove('hidden');
            streamOverlay.innerHTML = '<i class="fas fa-power-off"></i><span>Connect to RC Car</span>'; // Initial message
        }

        // Restore settings from localStorage? (Example for sound)
        const savedSound = localStorage.getItem('soundEnabled');
        if (savedSound !== null) {
            soundEnabled = savedSound === 'true';
            soundToggle.checked = soundEnabled;
        } else {
            soundToggle.checked = soundEnabled; // Default is true
        }
        // Apply other saved settings similarly if needed

        console.log("RC Command Center Initialized.");
        showToast('Welcome to RC Command Center!', 'info', 4000);
      }

      // Wait for the DOM to be fully loaded before running initialization
    document.addEventListener('DOMContentLoaded', initializeApp);

    function updateNavbarStatus(status) {
        if (!navbarConnectionStatus) return;

        // Clear previous states
        navbarConnectionStatus.classList.remove('connected', 'error', 'connecting');

        // Set new status
        switch (status) {
            case 'CONNECTING':
                navbarConnectionStatus.innerHTML = `
                    <i class="fas fa-spinner fa-spin"></i>
                    CONNECTING...
                `;
                navbarConnectionStatus.classList.add('connecting');
                break;

            case 'CONNECTED':
                navbarConnectionStatus.textContent = 'CONNECTED';
                navbarConnectionStatus.classList.add('connected');
                break;

            case 'ERROR':
                navbarConnectionStatus.textContent = 'ERROR';
                navbarConnectionStatus.classList.add('error');
                break;

            case 'DISCONNECTED':
            default:
                navbarConnectionStatus.textContent = 'DISCONNECTED';
                navbarConnectionStatus.classList.add('error'); // Use error style for disconnected
                break;
        }

        // Ensure it's always visible
        navbarConnectionStatus.style.display = 'block';
    }
  </script>
</body>
</html>
)=====";