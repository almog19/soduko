

#venv\Scripts\activate.bat
#loop back:
# uvicorn main:app --reload/ python -m uvicorn main:app --reload

#pc IP:
#uvicorn main:app --reload --host 0.0.0.0 --port 8000

import torch
import torch.nn as nn
import torch.nn.functional as F
import cv2
import numpy as np
import subprocess

from fastapi import FastAPI, File, UploadFile, Request
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
import socket

app = FastAPI()

#CORS - cross origin resource sharing
#2 origins(servers)
#react dev - localhost(:3000)
#fastAPI backend - loopback(:8000)
#without CORS - block request from one origin for security

local_ip = socket.gethostbyname(socket.gethostname())
print("local_ip", local_ip)

origins = [
    f"http://{local_ip}:19006",   # Expo web default
    "http://127.0.0.1:19006",   # Alternative Expo web
    f"http://{local_ip}:3000",    # React web dev
    "http://127.0.0.1:3000",    # React web dev
    "*"                         # Mobile apps (requests not CORS-checked)
]
app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,  # or ["http://localhost:3000"] for safety
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# ---------------- pytorch model ----------------

class CNNModel(nn.Module):
    def __init__(self):
        super(CNNModel, self).__init__()

        self.conv1 = nn.Conv2d(in_channels=3, out_channels=64, kernel_size=3, padding=1)
        self.pool1 = nn.MaxPool2d(kernel_size=2, stride=2)

        self.conv2 = nn.Conv2d(64, 128, kernel_size=3, padding=1)
        self.pool2 = nn.MaxPool2d(kernel_size=2, stride=2)

        self.conv3 = nn.Conv2d(128, 256, kernel_size=3, padding=1)
        self.pool3 = nn.MaxPool2d(kernel_size=2, stride=2)

        self.fc1 = nn.Linear(8*8*256, 256)
        self.fc2 = nn.Linear(256, 128)
        self.fc3 = nn.Linear(128, 64)
        self.dropout = nn.Dropout(0.5)
        self.fc4 = nn.Linear(64, 10)

    def forward(self, x):
        x = F.relu(self.conv1(x)); x = self.pool1(x)
        x = F.relu(self.conv2(x)); x = self.pool2(x)
        x = F.relu(self.conv3(x)); x = self.pool3(x)
        x = x.view(x.size(0), -1)
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = F.relu(self.fc3(x))
        x = self.dropout(x)
        return self.fc4(x)

# ---------------- sudoku Setup ----------------

class Sudoku_board:
  def __init__(self) -> None:
      self.device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
      self.model = CNNModel().to(self.device)
      self.model.load_state_dict(torch.load("./synthetic_sudokumodel_weights.pth", map_location=self.device))
      self.model.eval()

  def label_board(self, img_path: str):
      img, thresh = self.preprocess_image(img_path)
      contour = self.find_largest_contour(thresh)
      corners = self.get_approx_corners(contour)
      if corners is None:
        raise ValueError("could not find sudoku board")
      print("found sudoku board")
      self.wraped_sudoku = self.warp_perspective(img,corners)
      #plt.imshow(self.wraped_sudoku)
      #plt.show()
      cells = self.split_cells(self.wraped_sudoku)
      self.arr = [[0] * 9 for _ in range(9)]
      for i in range(9):
            #plt.figure(figsize=(8, 2))
            for j in range(9):
            #    plt.subplot(1, 9, j + 1)
            #    plt.imshow(cells[i*9+j], cmap='gray')
                  digit,conf = self.predict_digit(cells[i*9+j])
                  self.arr[i][j] = digit
            #    plt.title(f"{digit}\n{conf:.3f}")
            #    plt.axis('off')
            #plt.tight_layout()
            #plt.show()
      return self.arr

  def split_cells(self, board_img, size=640):
      cells = []
      step = size // 9
      for y in range(9):
          for x in range(9):
              cell = board_img[y*step:(y+1)*step, x*step:(x+1)*step]
              cells.append(cell)
      return cells

  def preprocess_image(self,img):
      #img = cv2.imread(img_path)
      gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
      blur = cv2.GaussianBlur(gray, (7, 7), 0)
      thresh = cv2.adaptiveThreshold(blur, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
                                    cv2.THRESH_BINARY_INV, 11, 2)
      return img, thresh

  def find_largest_contour(self,thresh_img):
      contours, _ = cv2.findContours(thresh_img, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
      largest = max(contours, key=cv2.contourArea)
      return largest

  def get_approx_corners(self,contour):
      epsilon = 0.02 * cv2.arcLength(contour, True)
      approx = cv2.approxPolyDP(contour, epsilon, True)
      if len(approx) == 4:
          return np.float32([pt[0] for pt in approx])
      return None

  def order_points(self,pts):
      # Order corners: [top-left, top-right, bottom-right, bottom-left]
      s = pts.sum(axis=1)
      diff = np.diff(pts, axis=1)
      return np.array([
          pts[np.argmin(s)],
          pts[np.argmin(diff)],
          pts[np.argmax(s)],
          pts[np.argmax(diff)]
      ], dtype='float32')

  def warp_perspective(self,orig_img, corners, size=640):
      ordered = self.order_points(corners)
      target = np.array([[0, 0], [size-1, 0], [size-1, size-1], [0, size-1]], dtype='float32')
      matrix = cv2.getPerspectiveTransform(ordered, target)
      return cv2.warpPerspective(orig_img, matrix, (size, size))

  def is_blank(self,cell, threshold=10):
      std_dev = np.std(cell)
      return std_dev < threshold

  def preprocess_cell(self, cell):
        cell = cv2.cvtColor(cell, cv2.COLOR_BGR2RGB)
        cell = cv2.resize(cell, (64, 64))
        cell = cell.astype(np.float32) / 255.0
        cell = np.transpose(cell, (2, 0, 1))     # HWC -> CHW
        tensor = torch.from_numpy(cell).unsqueeze(0).to(self.device)  # [1,3,64,64]
        return tensor

  def predict_digit(self,cell):
        img_tensor = self.preprocess_cell(cell)
        with torch.no_grad():
            outputs = self.model(img_tensor)
            probs = F.softmax(outputs, dim=1).cpu().numpy()[0]
        predicted_class = np.argmax(probs)
        confidence = float(np.max(probs))
        return predicted_class, confidence
      
board = Sudoku_board()
@app.get("/")
def read_root():
    return {"message": "Hello from FastAPI"}

#curl -X POST "http://127.0.0.1:8000/predict" -F "file=@C:/Users/almog/Downloads/CAVEMAN.png"

#image is encoded with base64

@app.post("/predict")
async def upload_file(file: UploadFile = File(...)):
    contents = await file.read()

    nparr = np.frombuffer(contents, np.uint8)
    img = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
    if img is None:
        print("error")
        return {"error": "Could not decode image"}
    print("got image")
    h, w, c = img.shape
    arr_board = board.label_board(img)
    for i in range(9):
        print("[", end='')
        for j in range(9):
            print(arr_board[i][j], end=', ')
        print("]")
    board_list = np.array(arr_board, dtype=np.int64)
    print("sending back the file")
    return {"filename": file.filename, "shape": [h, w, c], "board_list": board_list.tolist()}

class BoardString(BaseModel):#validate(JSON has board key), ensure board is a string, later use(difficulty level and user ID)
    board:str

@app.post("/solve")
#def solve_sudoku(data: BoardString):
async def solve_sudoku(request: Request):
    body_bytes = await request.body()           # raw bytes
    body_text = body_bytes.decode()             # convert to string
    print("Raw body:", body_text)
    board_str = body_text.strip()
    print(board_str)
    if len(board_str) != 81 or not board_str.isdigit():
        return {"error": "Board must be 81 digits long."}
    try:
        print("trying")
        result = subprocess.run(
            ["./solver"],               # your compiled C solver
            input=board_str,   # pass board as bytes
            capture_output=True,
            text=True
        )
        if result.returncode != 0:
            return {"error": result.stderr.strip()}

        output = result.stdout.strip()
        print(output)
        solved_board = [[0]*9 for _ in range(9)]
        solved_board = [[(output[i*9 + j]) for j in range(9)] for i in range(9)]
        return {"solved": solved_board}

    except Exception as e:
        return {"error": str(e)}

@app.post("/hello")
def hello():
    arr = [[0]*9 for _ in range(9)]
    arr[1][2]=3
    #print(arr)
    return {"message" : "hello world", "arr": arr}
                    






