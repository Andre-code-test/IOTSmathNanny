
-- Create database
CREATE DATABASE smartnanny_db;

-- Use the database
USE smartnanny_db;

-- Create table for storing baby cry data
CREATE TABLE baby_cry_data (
    id INT AUTO_INCREMENT PRIMARY KEY,
    date DATE NOT NULL,
    time TIME NOT NULL,
    decibels VARCHAR(10),
    duration_seconds INT,
    parent_note TEXT,
    professional_note TEXT
);

-- Insert dummy data (optional)
INSERT INTO baby_cry_data (date, time, decibels, duration_seconds, parent_note, professional_note)
VALUES
('2024-07-18', '01:00:00', '75 dB', 10, 'Excelente, voltou a dormir sozinho!', 'O padrão de choro parece consistente com períodos de desconforto. Monitorar a frequência e intensidade.'),
('2024-07-18', '02:30:00', '80 dB', 20, 'O bebê chora frequentemente durante a madrugada.', 'Verificar possíveis causas de irritação ou desconforto físico.');
