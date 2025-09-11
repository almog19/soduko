import React, { useState } from "react";
import { View, Button, Text, StyleSheet } from "react-native";

//const API_URL = "http://192.168.1.155:8000";
const API_URL = "https://soduko-onta.onrender.com";


const HelloButton: React.FC = () => {
  const [message, setMessage] = useState<string | null>(null);

  const handleClick = async () => {
    const hello_URL = API_URL + "/hello";
    console.log(hello_URL)
    try {
      const res = await fetch(hello_URL, { method: "POST" });
  
      if (!res.ok) {
        // throw with more detail (status + text)
        const errorText = await res.text();
        throw new Error(`Request failed: ${res.status} - ${errorText}`);
      }
  
      const data = await res.json();
      setMessage(data.message);
    } catch (err: any) {
      console.error(err);
      setMessage(err.message || "Unknown error");
    }
  };



  return (
    <View style={styles.container}>
      <Text style={styles.ip}>server IP:{"\n"}{API_URL}</Text>
      <Button title="get hello" onPress={handleClick} />

      {message && <Text style={styles.message}>{message}</Text>}
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: "center",
    alignItems: "center",
    padding: 20,
  },
  ip: {
    fontSize: 14,
    fontWeight: "bold",
  },
  message: {
    marginTop: 20,
    fontSize: 18,
    fontWeight: "600",
    color: "green",
  },
});

export default HelloButton;

