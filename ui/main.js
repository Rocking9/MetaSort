const { app, BrowserWindow, dialog, ipcMain } = require('electron');

// 1. Instantly ignite your Node.js backend server!
require('./server.js'); 

function createWindow () {
    // 2. Build the physical Desktop Window
    const win = new BrowserWindow({
        width: 600,
        height: 750,
        autoHideMenuBar: true, 
        resizable: false,      
        title: "MetaSort Architect Edition",
        webPreferences: {
            nodeIntegration: true,
            contextIsolation: false // CRITICAL: Allows index.html to use 'require'
        }
    });

    // 3. Load your Aero Glass UI
    win.loadFile('index.html');
}

// --- NEW SDE FEATURE: Native Folder Dialog Bridge ---
ipcMain.handle('dialog:openDirectory', async () => {
    const { canceled, filePaths } = await dialog.showOpenDialog({
        properties: ['openDirectory'] // Forces the OS to only select folders
    });
    if (canceled) {
        return null;
    } else {
        return filePaths[0]; // Returns the selected folder path string
    }
});

// 4. Start the app
app.whenReady().then(() => {
    createWindow();
    app.on('activate', () => {
        if (BrowserWindow.getAllWindows().length === 0) createWindow();
    });
});

// 5. Kill the server on exit
app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        app.quit();
    }
});