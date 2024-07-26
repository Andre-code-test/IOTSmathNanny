
from flask import Flask, request, jsonify
import os

app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = 'uploads'

# Ensure the upload directory exists
if not os.path.exists(app.config['UPLOAD_FOLDER']):
    os.makedirs(app.config['UPLOAD_FOLDER'])

@app.route('/upload', methods=['POST'])
def upload_file():
    if 'file' not in request.files:
        return jsonify({"error": "No file part"}), 400

    file = request.files['file']
    if file.filename == '':
        return jsonify({"error": "No selected file"}), 400

    if file:
        filepath = os.path.join(app.config['UPLOAD_FOLDER'], file.filename)
        file.save(filepath)
        # Placeholder for processing file (like storing metadata in database)
        return jsonify({"message": "File uploaded successfully", "filename": file.filename}), 200

@app.route('/data', methods=['GET'])
def get_data():
    # Placeholder for returning data, e.g., from a database or processed results
    # Return dummy data for now
    data = [
        {"date": "2024-07-18", "time": "01:00", "decibels": "75 dB", "seconds": 10, "parent_note": "Excellent, went back to sleep alone!", "professional_note": "The crying pattern seems consistent with periods of discomfort. Monitor frequency and intensity."},
        {"date": "2024-07-18", "time": "02:30", "decibels": "80 dB", "seconds": 20, "parent_note": "The baby cries frequently during the night.", "professional_note": "Check for possible causes of irritation or physical discomfort."}
    ]
    return jsonify(data), 200

if __name__ == '__main__':
    app.run(debug=True)
