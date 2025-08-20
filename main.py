#uvicorn main:app --reload
#import tensorflow as tf
#import cv2
#import numpy as np
from fastapi import FastAPI, File, UploadFile

app = FastAPI()
"""
class Sudoku_board:
  def __init__(self) -> None:
      self.model = tf.keras.models.load_model('/content/drive/MyDrive/synthetic_sudoku/model_underfit_3.keras')

  def label_board(self, img_path: str):
      img, thresh = self.preprocess_image(img_path)
      contour = self.find_largest_contour(thresh)
      corners = self.get_approx_corners(contour)
      if corners is None:
        raise ValueError("could not find sudoku board")
      self.wraped_sudoku = self.warp_perspective(img,corners)
      #plt.imshow(self.wraped_sudoku)
      #plt.show()
      cells = self.split_cells(self.wraped_sudoku)
      self.arr = [0] * 81
      for i in range(9):
            #plt.figure(figsize=(8, 2))
            for j in range(9):
            #    plt.subplot(1, 9, j + 1)
            #    plt.imshow(cells[i*9+j], cmap='gray')
                  digit,conf = self.predict_digit(cells[i*9+j])
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

  def preprocess_image(self,img_path):
      img = cv2.imread(img_path)
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

  def preprocess_cell(self,cell):
      cell = cv2.cvtColor(cell, cv2.COLOR_BGR2RGB)
      cell = cv2.resize(cell, (64, 64))                      # Resize to match input shape
      cell = cell.astype(np.float32) / 255.0
      cell = np.expand_dims(cell, axis=0)                    # Add batch dimension
      return cell

  def predict_digit(self,cell):
      img = self.preprocess_cell(cell)
      preds = self.model.predict(img,verbose=0)
      predicted_class = np.argmax(preds)
      confidence = np.max(preds)
      return predicted_class, confidence
"""
@app.post("/predict")
async def upload_file(file: UploadFile = File(...)):
    contents = await file.read()
    return {"filename": file.filename, "size": len(contents)}
