const express = require('express');
const cors = require('cors');
const { execFile } = require('child_process');
const path = require('path');

const app = express();
app.use(cors());
app.use(express.json());

app.post('/run', (req, res) => {
    const { source, output, mode } = req.body;

    // --- THE SDE PATH FIX ---
    // Check if the app is packaged inside the Electron .asar archive
    const isPackaged = __dirname.includes('app.asar');
    
    // Dynamically route the path based on the environment
    const exePath = isPackaged 
        ? path.join(__dirname, '..', '..', 'engine', 'metasort.exe') // Production Path
        : path.join(__dirname, 'engine', 'metasort.exe');            // Development Path

    console.log(`Executing: ${exePath} "${source}" "${output}" ${mode}`);

    // Fire the executable with the UI arguments
    execFile(exePath, [source, output, mode], (error, stdout, stderr) => {
        if (error) {
            console.error(`Execution Error: ${error.message}`);
            return res.status(500).send(`Engine Error:\n${stderr}`);
        }
        
        // Send the terminal output straight back to the HTML window
        res.send(stdout);
    });
});

app.listen(3000, () => {
    console.log('MetaSort Bridge Server running on http://localhost:3000');
});