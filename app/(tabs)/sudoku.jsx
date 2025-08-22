
import React, { useState } from "react";
import { View, Text, TextInput, TouchableOpacity, StyleSheet } from "react-native";
export default function SudokuGrid() {
  const [board, setBoard] = useState(
    Array(9).fill(null).map(() => Array(9).fill(0))
  );
  const [activeCell, setActiveCell] = useState(null); // {row, col} or null
  const [history, setHistory] = useState([]); // undo stack

  const handleChange = (text, rowIndex, colIndex) => {
    let value = parseInt(text, 10);

    // allow only digits 1–9 (or empty)
    if (isNaN(value) || value < 1 || value > 9) {
      value = 0;
    }

    setBoard(prev => {
      const newBoard = prev.map(r => [...r]); // deep copy
      const prevVal = newBoard[rowIndex][colIndex];

      if (prevVal !== value) {
        // push change to history stack
        setHistory(h => [...h, { type: "cell", row: rowIndex, col: colIndex, prev: prevVal, next: value }]);
      }

      newBoard[rowIndex][colIndex] = value;
      return newBoard;
    });
  };

  const undo = () => {
    setHistory(prev => {
      if (prev.length === 0) return prev; // nothing to undo
      const last = prev[prev.length - 1];

      setBoard(b => {
        const newBoard = b.map(r => [...r]);

        if (last.type === "cell") {
          // revert a single cell
          newBoard[last.row][last.col] = last.prev;
        } else if (last.type === "clear") {
          // revert a clear → restore full board
          return last.prevBoard;
        }

        return newBoard;
      });

      return prev.slice(0, -1); // remove last action
    });
  };

  const isBoardEmpty = board.every(row => row.every(cell => cell === 0));

  const clearBoard = () => {
    setBoard(prev => {
      const prevBoard = prev.map(r => [...r]); // save current board
      const emptyBoard = Array(9).fill(null).map(() => Array(9).fill(0));

      // push clear action to history stack
      setHistory(h => [...h, { type: "clear", prevBoard }]);

      return emptyBoard;
    });
  };

  return (
    <View style={styles.container}>
      <Text style={styles.title}>Sudoku</Text>

      {board.map((row, rowIndex) => (
        <View key={rowIndex} style={styles.row}>
          {row.map((cell, colIndex) => {
            const isActive = activeCell?.row === rowIndex && activeCell?.col === colIndex;

            return (
              <View
                key={colIndex}
                style={[
                  styles.cell,
                  (colIndex + 1) % 3 === 0 && colIndex !== 8 ? styles.verticalBorder : {},
                  (rowIndex + 1) % 3 === 0 && rowIndex !== 8 ? styles.horizontalBorder : {},
                  isActive ? styles.activeCell : {}
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
          style={[styles.actionButton, isBoardEmpty && { opacity: 0.5}]} 
          onPress={clearBoard}
          disabled={isBoardEmpty}  
        >
          
          <Text style={styles.buttonText}>Clear</Text>
        </TouchableOpacity>
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    marginTop: "120",
    alignItems: "center",
  },
  title: {
    fontSize: 20,
    fontWeight: "bold",
  },
  row: {
    flexDirection: "row",
  },
  cell: {
    width: 40,
    height: 40,
    borderWidth: 1,
    borderColor: "black",
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
    backgroundColor: "#ddd", // gray highlight when active
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
});
