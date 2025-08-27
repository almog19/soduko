# Sudoku Solver Project

This project is a full-stack Sudoku solver system, combining C, Python, machine learning, and a mobile front-end built with Expo. It allows users to solve Sudoku boards in multiple ways, including manual input, image upload, and automated solving using a compiled C program.

---

## Project Components

### 1. C Program
- **Purpose:** Solve a Sudoku board efficiently.
- **Functionality:** Takes a 9x9 board as input and returns the solved board.
- **Integration:** The compiled C program is called by the Python backend to solve boards requested from the Expo app.

### 2. Expo App (React Native)
- **Purpose:** Front-end for users to interact with Sudoku boards.
- **Features:**
  - Create and fill a Sudoku board manually.
  - Upload images of Sudoku boards to be converted into a virtual model using a machine learning model.
  - Send the board to the backend for solving by pressing a "Solve" button.
  - Display the solved board to the user.
- **Dependencies:** Node.js, Expo CLI, React Native, TensorFlow.js (optional if using ML model on-device).

### 3. Python Backend (FastAPI)
- **Purpose:** Serve as the API between the Expo app and the compiled C solver.
- **Functionality:**
  - Accept board data or images from the Expo app.
  - Convert images into a virtual Sudoku board using a trained ML model.
  - Call the compiled C program to solve the board.
  - Return the solved board to the app in JSON format.
- **Dependencies:** Python 3.10+, FastAPI, Uvicorn, NumPy, ML model dependencies.

### 4. ML Model
- **Purpose:** Convert an uploaded Sudoku image into a digital 9x9 board representation.
- **Integration:** The Expo app uploads images to the FastAPI backend, which uses the ML model to predict the digits.

---

## How It Works

1. **User Interaction**
   - The user opens the Expo app.
   - They can either fill a board manually or upload an image.

2. **Data Processing**
   - If uploading an image, the backend ML model converts the image to a 9x9 board.
   - The backend calls the compiled C solver with the board data.

3. **Solution Delivery**
   - The C program solves the board and returns the solution to the backend.
   - FastAPI sends the solved board back to the Expo app.
   - The solved board is displayed to the user.

---

## Installation & Setup

### Backend
```bash
# Create a virtual environment
python -m venv venv
source venv/bin/activate  # Linux/Mac
venv\Scripts\activate     # Windows

# Install dependencies
pip install fastapi uvicorn numpy
