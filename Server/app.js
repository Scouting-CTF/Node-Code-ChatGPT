const express = require('express');
const bodyParser = require('body-parser');
const sqlite3 = require('sqlite3').verbose();
const path = require('path');

// Create Express app
const app = express();
const port = 8990; // Set the port to 8990

// Middleware
app.use(bodyParser.urlencoded({ extended: true }));
app.set('view engine', 'ejs');

// Database setup
const dbPath = path.resolve(__dirname, 'ctf_database.db');
const db = new sqlite3.Database(dbPath);

// Ensure the database and tables are created
db.serialize(() => {
    db.run(`CREATE TABLE IF NOT EXISTS nodes (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        SystemID TEXT NOT NULL,
        NodeID TEXT NOT NULL,
        UUID TEXT NOT NULL,
        timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
    )`);
});

// Route to handle node submission
app.post('/ctf', (req, res) => {
    const { SystemID, NodeID, UUID } = req.body;
    if (!SystemID || !NodeID || !UUID) {
        return res.status(400).json({ error: 'SystemID, NodeID, and UUID are required' });
    }

    const query = `INSERT INTO nodes (SystemID, NodeID, UUID) VALUES (?, ?, ?)`;
    db.run(query, [SystemID, NodeID, UUID], function (err) {
        if (err) {
            return res.status(500).json({ error: err.message });
        }
        res.status(201).json({ message: 'Node data inserted', id: this.lastID });
    });
});

// Route to retrieve the latest UUID for each node
// Route to retrieve unique node IDs
app.get('/scoreboard', (req, res) => {
    const query = `
        SELECT NodeID, UUID, timestamp
        FROM nodes
        WHERE (NodeID, timestamp) IN (
            SELECT NodeID, MAX(timestamp)
            FROM nodes
            GROUP BY NodeID
        )
        ORDER BY NodeID;
    `;
    db.all(query, [], (err, rows) => {
        if (err) {
            return res.status(500).json({ error: err.message });
        }
        res.render('scoreboard', { nodes: rows });
    });
});


// Start the server
app.listen(port, () => {
    console.log(`Server is running on http://localhost:${port}`);
});
