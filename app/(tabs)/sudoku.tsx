import React, { useState } from "react";
import { View, Text,  TextInput, TouchableOpacity, Image, StyleSheet, Button,} from "react-native";
import * as ImagePicker from "expo-image-picker";

const API_URL = "http://192.168.1.101:8000";

// Types
type CellChangeAction = {
  type: "cell";
  row: number;
  col: number;
  prev: number;
  next: number;
};

type ClearAction = {
  type: "clear";
  prevBoard: number[][];
};

type Action = CellChangeAction | ClearAction;

type ActiveCell = { row: number; col: number } | null;

export default function SudokuGrid() {
  const [board, setBoard] = useState<number[][]>(
    Array(9)
      .fill(null)
      .map(() => Array(9).fill(0))
  );

  const [activeCell, setActiveCell] = useState<ActiveCell>(null);
  const [history, setHistory] = useState<Action[]>([]);
  const [imageUri, setImageUri] = useState<string | null>(null);

  const handleChange = (text: string, rowIndex: number, colIndex: number) => {
    let value = parseInt(text, 10);

    if (isNaN(value) || value < 1 || value > 9) {
      value = 0;
    }

    setBoard((prev) => {
      const newBoard = prev.map((r) => [...r]);
      const prevVal = newBoard[rowIndex][colIndex];

      if (prevVal !== value) {
        setHistory((h) => [
          ...h,
          { type: "cell", row: rowIndex, col: colIndex, prev: prevVal, next: value },
        ]);
      }

      newBoard[rowIndex][colIndex] = value;
      return newBoard;
    });
  };

  const undo = () => {
    setHistory((prev) => {
      if (prev.length === 0) return prev;
      const last = prev[prev.length - 1];

      setBoard((b) => {
        const newBoard = b.map((r) => [...r]);

        if (last.type === "cell") {
          newBoard[last.row][last.col] = last.prev;
        } else if (last.type === "clear") {
          return last.prevBoard;
        }

        return newBoard;
      });

      return prev.slice(0, -1);
    });
  };

  const image = async () => {
    let result = await ImagePicker.launchImageLibraryAsync({
      allowsEditing: true,
      quality: 1,
    });

    if (!result.canceled) {
      setImageUri(result.assets[0].uri);
    } else {
      alert("You did not select any image");
    }

    const new_url = API_URL + "/hello";
    console.log(new_url);
    try {
      const response = await fetch(new_url, { method: "POST" });

      if (!response.ok) {
        const errorText = await response.text();
        throw new Error(`Request failed: ${response.status} - ${errorText}`);
      }

      const data = await response.json();
      console.log(data.message);
    } catch (err) {
      console.error(err);
    }
  };

  const isBoardEmpty = board.every((row) => row.every((cell) => cell === 0));

  const clearBoard = () => {
    setBoard((prev) => {
      const prevBoard = prev.map((r) => [...r]);
      const emptyBoard = Array(9)
        .fill(null)
        .map(() => Array(9).fill(0));

      setHistory((h) => [...h, { type: "clear", prevBoard }]);

      return emptyBoard;
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
                <TextInput
                  style={styles.cellText}
                  keyboardType="numeric"
                  maxLength={1}
                  returnKeyType="done"
                  value={cell === 0 ? "" : String(cell)}
                  onChangeText={(text) => handleChange(text, rowIndex, colIndex)}
                  textAlign="center"
                  onFocus={() => setActiveCell({ row: rowIndex, col: colIndex })}
                  onBlur={() => setActiveCell(null)}
                />
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
    marginTop: 120,
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
    fontWeight: "bold",
    padding: 0,
    color: "blue",
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
