import React, { useState } from "react";
import { View, Text, TextInput, StyleSheet } from "react-native";

export default function SudokuGrid() {
  const [board, setBoard] = useState(
    Array(9).fill(null).map(() => Array(9).fill(0))
  );

  const handleChange = (text, rowIndex, colIndex) => {
    let value = parseInt(text, 10);

    // allow only digits 1–9 (or empty)
    if (isNaN(value) || value < 1 || value > 9) {
      value = 0;
    }

    const newBoard = [...board];
    newBoard[rowIndex] = [...newBoard[rowIndex]];
    newBoard[rowIndex][colIndex] = value;
    setBoard(newBoard);
  };

  return (
    <View style={styles.container}>
        <Text style={styles.title}>
            sudoku
        </Text>
      {board.map((row, rowIndex) => (
        <View key={rowIndex} style={styles.row}>
          {row.map((cell, colIndex) => (
            <View
              key={colIndex}
              style={[
                styles.cell,
                (colIndex + 1) % 3 === 0 && colIndex !== 8 ? styles.verticalBorder : {},
                (rowIndex + 1) % 3 === 0 && rowIndex !== 8 ? styles.horizontalBorder : {}
              ]}
            >
              <TextInput 
                style={styles.cellText}
                keyboardType="numeric"
                maxLength={1} //one digit at a cell
                value={cell === 0 ? "": String(cell)}
                onChangeText={(text) => handleChange(text,rowIndex, colIndex)}
                textAlign="center"
              />
            </View>
          ))}
        </View>
      ))}
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    marginTop: 40,
    alignItems: "center",
  },
  title: {
    fontSize : 20,
    fontWeight : 'bold',
  },
  row: {
    flexDirection: "row",
  },
  cell: {
    width: 35,
    height: 35,
    borderWidth: 1,
    borderColor: "black",
    justifyContent: "center",
    alignItems: "center",
  },
  cellText: {
    fontSize: 18,
    fontWeight: "bold",
    padding: 0,
  },
  verticalBorder: {
    borderRightWidth: 3, // thick border every 3 cols
  },
  horizontalBorder: {
    borderBottomWidth: 3, // thick border every 3 rows
  },
});
