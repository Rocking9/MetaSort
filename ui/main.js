const { app, BrowserWindow } = require('electron');

// 1. Instantly ignite your Node.js backend server!
require('./server.js'); 

function createWindow () {
    // 2. Build the physical Desktop Window
    const win = new BrowserWindow({
        width: 600,
        height: 750,
        autoHideMenuBar: true, // Hides the ugly "File Edit View" menu
        resizable: false,      // Locks the size so your Glass UI stays perfect
        title: "MetaSort Architect Edition",
        webPreferences: {
            nodeIntegration: true
        }
    });

    // 3. Load your Aero Glass UI into the window
    win.loadFile('index.html');
}

// 4. Start the app when Electron is ready
app.whenReady().then(() => {
    createWindow();

    app.on('activate', () => {
        if (BrowserWindow.getAllWindows().length === 0) createWindow();
    });
});

// 5. Kill the server and exit when the user clicks the 'X' button
app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        app.quit();
    }
});