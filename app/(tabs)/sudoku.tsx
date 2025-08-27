
import React, { useState, useRef } from "react";
import { View, Text,  TextInput, TouchableOpacity, Image, StyleSheet, Button, Animated, Pressable} from "react-native";
import * as ImagePicker from "expo-image-picker";
//import AsyncStorage from "@react-native-async-storage/async-storage";

const API_URL = "http://192.168.1.155:8000";

// Types
type Cell = {
  value: number;
  original: boolean;
};

  type historyItem = 
    | { type: "cell"; row_idx: number; col_idx: number; value: number; origin: boolean;}
    | {type: "full"; prev: Cell[][]}

type ActiveCell = { row: number; col: number } | null;

export default function SudokuGrid() {
  const [board, setBoard] = useState<Cell[][]>(
    Array.from({ length: 9 }, () =>
      Array.from({ length: 9 }, () => ({ value: 0, original: false }))
    )
  );
  //board[2][2] = {value:2, original: true,};

  const [activeCell, setActiveCell] = useState<ActiveCell>(null);
  const [history, setHistory] = useState<historyItem[]>([]);
  const [imageUri, setImageUri] = useState<string | null>(null);
  const [showAlert, setShowAlert] = useState(false);

  const lastTap = useRef<number>(0)
  const inputRef = useRef<TextInput>(null);

  function allowedMove(board: Cell[][], row: number, col: number, value: number ) {
    for(let c = 0; c < 9; c++){
      if(board[row][c].value === value){return [row,c];}
    }
    for(let r = 0; r < 9; r++){
      if(board[r][col].value === value){return [r,col];}
    }
    const startRow = Math.floor(row / 3) * 3;
    const startCol = Math.floor(col / 3) * 3;
    for (let r = startRow; r < startRow + 3; r++) {
      for (let c = startCol; c < startCol + 3; c++) {
        if (board[r][c].value === value) {return [r,c];}
      }
    }

    return null;
  }

  const handleChange = (value: number, row: number, col: number) => {
    const cell = board[row][col];
    if(value !== 0){
      const blockedCell = allowedMove(board,row,col,value);
      if(blockedCell){
        console.log(blockedCell);
        alert("blocked digit");
        return;
      }
    }

    setHistory((h) => [...h, {type: "cell", row_idx: row, col_idx: col, value: cell.value, origin: false}]);

    const newBoard = board.map((r, rIdx) =>
      r.map((c, cIdx) => {
        if (rIdx === row && cIdx === col) {
          return { ...c, value }; // apply new value
        }
        return c;
      })
    );
    setBoard(newBoard);
  };

  const handelDoubleTap = (row: number, col: number) => {
      console.log("double tap occured!");
      setHistory((h) => [...h, {type: "cell", row_idx:row, col_idx: col, value: board[row][col].value, origin: false}]);
      const newBoard = board.map((r, rIdx) =>
        r.map((c, cIdx) => {
          if (rIdx === row && cIdx === col) {
            return { ...c, value: 0 }; // apply new value
          }
          return c;
        })
      );

      setBoard(newBoard);
      setTimeout(() => inputRef.current?.focus(),50);
  };

  const sendAlert = () => {
    if(!showAlert){
      alert("black digit are unchangable");
      setShowAlert(true);
    }
  }

  const undo = () => {
    setHistory((prev) => {
      if (prev.length === 0) return prev;
      const last = prev[prev.length - 1];
      
      if(last.type === "cell"){
        setBoard((prev) => {
          board[last.row_idx][last.col_idx] = {
            value: last.value,
            original : last.origin,
          };
          return board;
        });
      } else {
        setBoard(last.prev);
      }

      return prev.slice(0, -1);
    });
  };

  function base64ToBlob(base64: string, type = "image/png"){
    const byteCharacter = atob(base64);
    const byteNumbers = new Array(byteCharacter.length);
    for(let i = 0; i < byteCharacter.length; i++){
      byteNumbers[i] = byteCharacter.charCodeAt(i);
    }
    const byteArray = new Uint8Array(byteNumbers);
    return new Blob([byteArray], { type });
  }

  const image = async () => {
    let result = await ImagePicker.launchImageLibraryAsync({
      allowsEditing: true,
      quality: 1,
    });

    if (result.canceled || !result.assets) {
      alert("You did not select any image");
      return;
    }
    const tempUri = result.assets[0].uri;
    setImageUri(tempUri);
    console.log(tempUri)
    if(!tempUri){
      return { error: "no image selected"};
    }
    const new_url = API_URL + "/predict";
    console.log(new_url);

    console.log("image URI:", tempUri);
    //const base64Data = imageUri.split(",")[1];//remove("image/png;base64,")
    //const blob = base64ToBlob(base64Data, "image/png")
    
    console.log("blob", tempUri);
    const formImage = new FormData();
    formImage.append("file", {
      uri: tempUri,
      name: "upload.png",
      type: "image/png",
    } as any);
    try {
      const response = await fetch(new_url, { 
        method: "POST",
        body: formImage,
      });

      if (!response.ok) {
        const errorText = await response.text();
        throw new Error(`Request failed: ${response.status} - ${errorText}`);
      }
      const data = await response.json();
      console.log(data.board_list);
      const convertBoard = data.board_list.map((row: number[]) => 
        row.map((val): Cell => ({
          value: val,
          original: val !== 0,
        }))
      );
      setBoard((prev) => {
        const prevBoard = prev.map((r) => [...r]);
        setHistory((h) => [...h, { type: "full", prev: prevBoard }]);
        return convertBoard;
      });
      //console.log("board:", convertBoard);

    } catch (err) {
      console.error(err);
    }
  };
  
  const solve = async () => {
    const boardStr = board.flat().map(cell => cell.value.toString()).join("");
    console.log(boardStr)
    const new_url = API_URL + "/solve"
    try {
      const response = await fetch(new_url, {
        method: "POST",
        headers: {
          "Content-Type" : "text/plain",
        },
        body: boardStr,
      });

      const data = await response.json();
      console.log(data);
      const convertBoard = data.solved.map((row: number[], row_idx: number) =>
        row.map((val, col_idx): Cell => ({
          value: val,
          original: board[row_idx][col_idx].original,
        }))
      );
      console.log(convertBoard)
      setBoard((prev) => {
        const prevBoard = prev.map((r) => [...r]);
        setHistory((h) => [...h, { type: "full", prev: prevBoard }]);
        return convertBoard;
      });
    } catch (error){
      console.error("Error", error);
    }
  };

  const isBoardEmpty = board.every((row) => row.every((cell) => cell.value === 0));
  const isBoardOrigin = board.every((row => row.every((cell => cell.value === 0 || cell.original))))

  const clearBoard = () => {
    setBoard((prev) => {
      const prevBoard = prev.map((r) => [...r]);
      console.log("is empty:", isBoardEmpty);
      if(isBoardOrigin){
        const emptyBoard: Cell[][] = Array.from({ length: 9}, () =>
          Array.from({ length: 9}, () => ({value: 0, original: false}))
        )
      } else {
        const emptyBoard: Cell[][] = Array.from({ length:9}, () =>
          Array.from({ length: 9}, () => ({value: 0, original: false}))
        )
      }

      setHistory((h) => [...h, { type: "full", prev: prevBoard }]);
      if(isBoardOrigin){
        const emptyBoard: Cell[][] = Array.from({ length: 9}, () =>
        Array.from({ length:9}, () => ({value: 0, original: false}))
        )
        return emptyBoard;
      }
      const originBoard: Cell[][] = board.map(row =>
        row.map(cell => ({
          ...cell,
          value: cell.original? cell.value: 0,
        }))
      )
      return originBoard;
    });
  };

  return (
    <View style={styles.container}>
      <Text style={styles.title}>Sudoku</Text>

      {board.map((row, rowIndex) => (
        <View key={rowIndex} style={styles.row}>
          {row.map((cell, colIndex) => {
            const isActive =
              activeCell?.row === rowIndex && activeCell?.col === colIndex;

            return (
              <View
                key={colIndex}
                style={[
                  styles.cell,
                  (colIndex + 1) % 3 === 0 && colIndex !== 8
                    ? styles.verticalBorder
                    : {},
                  (rowIndex + 1) % 3 === 0 && rowIndex !== 8
                    ? styles.horizontalBorder
                    : {},
                  isActive ? styles.activeCell : {},
                ]}
              >
                {cell.original ? (
                  <TouchableOpacity onPress={sendAlert} activeOpacity={0.7}>
                    <Text style={styles.cellOrigin}>{cell.value}</Text>
                  </TouchableOpacity>
                ) : (
                  <TextInput
                    style={styles.cellText} // blue for user input
                    keyboardType="numeric"
                    maxLength={1}
                    returnKeyType="done"
                    value={cell.value === 0 ? "" : String(cell.value)}
                    onChangeText={(text) =>
                      handleChange(Number(text), rowIndex, colIndex)
                    }
                    textAlign="center"
                    onFocus={() => setActiveCell({ row: rowIndex, col: colIndex })}
                    onBlur={() => setActiveCell(null)}
                    onTouchEnd={() => {
                        const now = Date.now();
                        if(now - lastTap.current < 500){
                          handelDoubleTap(rowIndex,colIndex);
                        }
                        lastTap.current = now;
                    }}
                    />
                )}
              </View>
            );
          })}
        </View>
      ))}

      <View style={styles.buttonRow}>
        <TouchableOpacity
          style={[styles.actionButton, history.length === 0 && { opacity: 0.5 }]}
          onPress={undo}
          disabled={history.length === 0}
        >
          <Text style={styles.buttonText}>Undo</Text>
        </TouchableOpacity>

        <TouchableOpacity
          style={[styles.actionButton, isBoardEmpty && { opacity: 0.5 }]}
          onPress={clearBoard}
          disabled={isBoardEmpty}
        >
          <Text style={styles.buttonText}>Clear</Text>
        </TouchableOpacity>

        <TouchableOpacity
          style={styles.actionButton}
          onPress={solve}
        >
          <Text style={styles.buttonText}>Solve</Text>
        </TouchableOpacity>
      </View>

      <View>
        <Button title="Press me" onPress={image} />
        {imageUri && (
          <Image source={{ uri: imageUri }} style={{ width: 200, height: 200 }} />
        )}
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    marginTop: 70,
    alignItems: "center",
  },
  title: {
    fontSize: 20,
    fontWeight: "bold",
    color: "orange",
  },
  row: {
    flexDirection: "row",
  },
  cell: {
    width: 40,
    height: 40,
    borderWidth: 1,
    borderColor: "green",
    justifyContent: "center",
    alignItems: "center",
  },
  cellText: {
    fontSize: 32,
    padding: 0,
    color: "blue",
  },
  cellOrigin: {
    fontSize: 32,
    fontWeight: "bold",
    padding: 0,
    color: "black",
  },
  verticalBorder: {
    borderRightWidth: 3,
  },
  horizontalBorder: {
    borderBottomWidth: 3,
  },
  activeCell: {
    backgroundColor: "#ddd",
  },
  buttonRow: {
    flexDirection: "row",
    marginTop: 20,
    gap: 15,
  },
  actionButton: {
    paddingHorizontal: 20,
    paddingVertical: 10,
    backgroundColor: "#444",
    borderRadius: 5,
  },
  buttonText: {
    color: "white",
    fontWeight: "bold",
  },
  imagebutton: {
    flexDirection: "row",
    justifyContent: "space-between",
  },
});
