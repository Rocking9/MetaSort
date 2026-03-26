const express = require('express');
const cors = require('cors');
const { execFile } = require('child_process');
const path = require('path');

const app = express();
app.use(cors());
app.use(express.json());

app.post('/run', (req, res) => {
    const { source, output, mode } = req.body;

    // Point to your compiled C++ executable
    const exePath = path.resolve(__dirname, '../bin/metasort.exe');

    console.log(`Executing: metasort.exe "${source}" "${output}" ${mode}`);

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