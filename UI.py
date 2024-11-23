import tkinter as tk
from tkinter import messagebox
import subprocess
import time
import threading

#מחלקה, שמקבלת הפניה לחלון(ROOT),
#תיצור חלון עם 9 על 9 כניסות, כאשר יהיה בעל אינטרקציה עם המשתמש, לפי לחיצה
#וכאשר ילחצו enter ישנה את החלון של המערך לפי הפתרון של הלוח סודוקו
class SudokuApp:
    def __init__(self, root):
        self.root = root#דרך ליצור ממשק לחלון של TK
        self.root.title("Sudoku Board")
        self.position = 0
#יצירת מערך של לוח סודוקו
#כניסה זה מקום של טקסט בחלון, בשביל שהמשתמש יוכל לתת פלט
        self.entries = []
        for i in range(9):
            row_entries = []
            for j in range(9):
                entry = tk.Entry(root, width=3, font=('Arial', 18), justify='center')
                entry.grid(row=i, column=j)
                entry.bind("<KeyRelease>", self.on_key_release)  #קבלת מספר לפי לחיצה על המשבצת
                row_entries.append(entry)
            self.entries.append(row_entries)
        self.root.bind("<Return>", self.solving_soduko)



    def on_key_release(self, event):
        entry = event.widget
        input_value = entry.get()
# תחום הגדרה לקלט לא תקין, נמחוק את ערך של המשבצת
        if not (input_value.isdigit() and 0 <= int(input_value) <= 9):#לא בין טווח של 0 ל - 9
            entry.delete(0, tk.END)#כאשר המשבצת ריקה או עם 0 נחליף ב 0,
            if input_value != '':#כאשר הוכנס מספר תווים, דבר לא תקין
                messagebox.showwarning("Invalid Input", "Please enter a number between 1 and 9.")


    def compile_run_c(self):
#פעולה שמקמפלת את הקובץ של הסודוקו ומחסנית, ומריצה את התוכנה המקומפלת
        compile_c = ['gcc', "soduko2.c", "stack.c", '-o', "soduko2.exe"]
        subprocess.run(compile_c, check=True)
        run_c = ['./'+"soduko2.exe"]
        try:
        # Run the command and wait for it to complete
            subprocess.run(run_c, check=True)
            print("c program finished running")
        except subprocess.CalledProcessError as e:
            print(f"Error: {e.returncode}. Output: {e.output}")

    def new_line_reader(self):
#פעולה קוראה מקובץ, ומשנה את המשתנה הפנימי של המיקום בקובץ לשורה מתחת
        with open("progress_soduko.txt", 'r') as file:
            if self.position == -1:
                messagebox.showinfo("no solution","no solution!")
                return
            file.seek(self.position)
            line = file.readline()#קורא שורה מוריד את המצביע לקובץ אחד למטה
            if line:
                current_position = file.tell()
                file.seek(current_position)
                if(line.strip() == "no solution!"):
                    messagebox.showinfo("no solution",line.strip())
                    print("no solution")
                    self.position = -1
                    return
                elif(line.strip() == "found solution!"):
                    print("found solution")
                    messagebox.showinfo("solution",line.strip())
                    self.position = -1
                    return
                if("-c" in line.strip()):
                        line = line.replace(" -c", "")
                        numbers = line.strip().split()#מחלק את השורה כל לרשימה של מספרים, לפי רווח ביניהם
                        col,row = map(int, numbers)
                        print("uncover:", col,row)
                        self.entries[row-1][col-1].delete(0, tk.END)
                        self.position = file.tell()
                        return
                numbers = line.strip().split()#מחלק את השורה כל לרשימה של מספרים, לפי רווח ביניהם
                col,row,num = map(int, numbers)
                print(col,row,num)
                self.entries[row-1][col-1].insert(0, str(num))  # Insert new number
                self.entries[row-1][col-1].config(fg='blue')
                self.position = file.tell()
                return
            else:
                self.position = -1#הקובץ ריק, או הגיעה לסוף


    def solving_soduko(self, event):
#פעולה שממירה את הכניסות שבחלון למערך דו מימדי
#רושמת את המערך בקובץ טקסט, בשביל שהתוכנה לפתרון הסודוקו, תוכל להשתמש בה
        board = []
#אוסף את המערך הדו מימידי של הסודוקו
        for i in range(9):
            row = []
            for j in range(9):
                value = self.entries[i][j].get()
                if value == '':#ערך משבצת ריקה, נשווה אותה ל 0
                    row.append(0)
                else:
                    row.append(int(value))
            board.append(row)

#רשימה בקובץ טקסט את הלוח הסודוקו ההתחלי
#ירשום שורה שורה, כמחרוזות, וירד שורה
        with open('sudoku_board.txt', 'w') as file:
            for row in board:
                file.write(' '.join(map(str, row)) + '\n')

        self.compile_run_c()#לקמפל את קוד, לפתרון ולהריץ אותו, מעקב אחרי התוכנה יודפס ב terminal

#קריאת שורה שורה לפי קובץ שעוקבת אחרי הפתרון, ויעדכן את הלוח סודוקו
        while True:
            self.new_line_reader()
            #self.root.after(100)
            if self.position == -1:
                break#נגמר הקובץ, או נמצא הסודוקו, או אין פתרון לסודוקו


#יצירת חלון, מאתחל את ה framework של ה TK, 
#ומקנה למשתנה ROOT הפנייה לחליון
root = tk.Tk()

#אתחול של החלון של TK, לפי המחלקה של הסודוקו
app = SudokuApp(root)

#מתחיל את האפליקציה, ומשאיר אותה בלולאה בשביל לאפשר אינטרקיות של המשתמש
root.mainloop()
