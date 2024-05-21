const express = require('express');
const bodyParser = require('body-parser');
const sqlite3 = require('sqlite3').verbose();
const path = require('path');

// Create Express app
const app = express();
const port = 8990;

// Middleware
app.use(bodyParser.urlencoded({ extended: true }));
app.set('view engine', 'ejs');

// Database setup
const dbPath = path.resolve(__dirname, 'ctf_database.db');
const db = new sqlite3.Database(dbPath);

// Ensure the database and tables are created
db.serialize(() => {
    db.run(`CREATE TABLE IF NOT EXISTS teams (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL UNIQUE,
        score INTEGER DEFAULT 0
    )`);

    db.run(`CREATE TABLE IF NOT EXISTS flags (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        value TEXT NOT NULL UNIQUE,
        points INTEGER NOT NULL
    )`);

    db.run(`CREATE TABLE IF NOT EXISTS submissions (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        team_id INTEGER NOT NULL,
        flag_id INTEGER NOT NULL,
        timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
        FOREIGN KEY(team_id) REFERENCES teams(id),
        FOREIGN KEY(flag_id) REFERENCES flags(id)
    )`);
});

// Route to handle team registration
app.post('/register', (req, res) => {
    const { teamName } = req.body;
    if (!teamName) {
        return res.status(400).json({ error: 'Team name is required' });
    }

    const query = `INSERT INTO teams (name) VALUES (?)`;
    db.run(query, [teamName], function (err) {
        if (err) {
            return res.status(500).json({ error: err.message });
        }
        res.status(201).json({ message: 'Team registered', id: this.lastID });
    });
});

// Route to handle flag submission
app.post('/submitFlag', (req, res) => {
    const { teamName, flagValue } = req.body;
    if (!teamName || !flagValue) {
        return res.status(400).json({ error: 'Team name and flag value are required' });
    }

    db.get('SELECT id FROM teams WHERE name = ?', [teamName], (err, team) => {
        if (err) return res.status(500).json({ error: err.message });
        if (!team) return res.status(400).json({ error: 'Team not found' });

        db.get('SELECT id, points FROM flags WHERE value = ?', [flagValue], (err, flag) => {
            if (err) return res.status(500).json({ error: err.message });
            if (!flag) return res.status(400).json({ error: 'Invalid flag' });

            db.run(`INSERT INTO submissions (team_id, flag_id) VALUES (?, ?)`, [team.id, flag.id], function (err) {
                if (err) return res.status(500).json({ error: err.message });

                db.run(`UPDATE teams SET score = score + ? WHERE id = ?`, [flag.points, team.id], function (err) {
                    if (err) return res.status(500).json({ error: err.message });

                    res.status(200).json({ message: 'Flag submitted successfully', points: flag.points });
                });
            });
        });
    });
});

// Route to retrieve the scoreboard
app.get('/scoreboard', (req, res) => {
    const query = `SELECT name, score FROM teams ORDER BY score DESC`;
    db.all(query, [], (err, rows) => {
        if (err) {
            return res.status(500).json({ error: err.message });
        }
        res.render('scoreboard', { teams: rows });
    });
});

// Start the server
app.listen(port, () => {
    console.log(`Server is running on http://localhost:${port}`);
});
