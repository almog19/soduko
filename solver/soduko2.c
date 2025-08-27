#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "stack.h"

#define sudoku 9

typedef struct Node Node;
typedef struct Header Header;

typedef struct num{
    int r,c,n;
}Num;

struct Node{
    int row,col;
    struct Node *up,*down,*right,*left;
};

struct Header{
    int size,col;
    Header *right,*left;
    Node* nodeP;
};


int* sudokuToMatrix(Num* n){
/*
פעולה שלוקחת מספר בלוח סודוקו ומחזירה מערך עם ערכים לפי הסדר:
מס שורה, מס עמודה בחלק 1, מס עמודה בחלק 2, מס עמודה בחלק 3, מס עמודה בחלק 4
*/
    int sqr = sqrt(sudoku);
    int* arr = (int*) malloc(sizeof(int) * 5);
    arr[0] = (n->r-1)*sudoku*sudoku + (n->c -1)*sudoku + n->n;
    int box = sqr*((n->r-1)/sqr) + (n->c-1)/sqr + 1;
    arr[1] = (n->r -1)*sudoku + (n->c-1) + 1;
    arr[2] = (n->r-1)*sudoku + n->n + sudoku*sudoku;
    arr[3] = (n->c-1)*sudoku + n->n + 2*sudoku*sudoku;
    arr[4] = (box-1)*sudoku + n->n + 3*sudoku*sudoku;
    return arr;
}
Num* matrixToSudoku(int row){
/*
פעולה שמקבלת שורה במטריצה ומחזירה מצביע לעצם של מספר בלוח סודוקו
*/
    Num* num = (Num*)malloc(sizeof(Num));
    num->n = row%sudoku + (row%sudoku==0)*sudoku;
    row -= num->n;
    num->r = (row/(sudoku*sudoku))+1;
    row -= ((num->r-1)*sudoku*sudoku);
    num->c = (row/sudoku) + 1;
    return num;
}


static Header* root;//מצביע שישמש כמערך לראשים במטריצה,
static int root_len = 4*sudoku*sudoku +1;
static int root_depth = sudoku*sudoku*sudoku;


void init_root(){
/*
פעולה שמאתחלת את הראשיים
col = c, גדל באחד מ - 0 עד 325
size = soduko(9)
.nodeP = NULL
*/
    printf("\ninit_root:\n");
    int c;
    for(c = 0; c < root_len; c++){
        root[c].size=sudoku;
        root[c].col=c;
        if(c!=0){
            root[c].left = &root[c-1];
            root[c-1].right = &root[c];
        }
        root[c].nodeP = NULL;
    }
    root[0].left = &root[root_len-1];
    root[root_len-1].right=&root[0];
}
void print_roots(){
/*
פעולה שמדפיסה את כל העמודות שלא מכוסות, עם מספר החוליות בעמודה
*/
    printf("\nroot:\n %d-->",0);
    Header* temp = root[0].right;
    while(temp != &root[0]){
        printf("%d,%d-->", temp->col,temp->size);
        temp=temp->right;
    }
    printf("finish printing_root\n");
}

void init_nodes(){
/*
פעולה שיוצרת את כל החוליות במטריצה, מקנה לכל חוליה את מס עמודה ושורה, ומקשרת בין החוליות באותה עמודה ושורה
תהליך:
נעבור על כל השורות במטריצה(729)
לכל שורה ניצור מספר לפי השורה, נעביר את המספר למערך עם מס השורה ו4 מספרי העמודות,
לכל חוליה נביא את מספר השורה והעמודה הנכונים,
נקשר עם העמודה, כאשר העמודה מצביע על כלום, החוליה בהתחלה
אם העמודה מצביע על משהו ניקח את החוליה האחרונה שנוצרה בעמודה ונקשר ביניהם

אחרי יצירת 4 החוליות נקשר ביניהם (ימין שמאל)
בסוף נחבר את החוליה הראשונה עם החוליה הראשונה
*/
    printf("\ninit_nodes:\n");
    Node* arr[root_len-1];//מערך שיכיל את החוליה האחרונה שנוצר לכל עמודה
    int r,c,i,counter = 0;
    for(r = 1; r < root_depth+1; r++){//לולאה לכל השורות
//        printf("\nrow(%d)\t",r);
        Num* numTemp = matrixToSudoku(r);//יצירת מספר
        int* arrTemp = sudokuToMatrix(numTemp);//קבלת המערך עם מס השורה והעמודות
        for(i = 1; i <= 4; i++){//לולאה לכל חוליה
            Node *nodeTemp = (Node*) malloc(sizeof(Node));
            nodeTemp->col = arrTemp[i];
            nodeTemp->row = r;
//            printf("%d\t", arrTemp[i]);
            if(root[arrTemp[i]].nodeP == NULL){//חוליה ראשונה
                counter++;
                root[arrTemp[i]].nodeP = nodeTemp;
//                printf("nodeP(%d,%d)\t", nodeTemp->col,nodeTemp->row);
            }else{//חוליה לא ראשונה
                arr[arrTemp[i]-1]->down = nodeTemp;
                nodeTemp->up = arr[arrTemp[i]-1];
            }
            arr[arrTemp[i]-1] = nodeTemp;//עדכון מערך של החוליות האחרונות
        }
//        printf("linking:");
        for(i = 1; i <= 4; i++){//לחבר את כל החוליות שנוצרו
            arr[arrTemp[i]-1]->right = (Node*) arr[arrTemp[((i)%4)+1] -1];
            arr[arrTemp[i]-1]->left = (Node*) arr[arrTemp[((i+(4-2))%4)+1] -1];
        }
        free(numTemp);
        free(arrTemp);
    }
    for(c=1;c < root_len; c++){//קישור החוליה הראשונה עם האחרונה
        root[c].nodeP->up = (Node*) arr[c-1];
        arr[c-1]->down = (Node*) root[c].nodeP;
    }
    printf("\nfinish init_nodes\n");
}

void free_nodes(){
/*
פעולה שמשחררת את כל החוליות,
עוברת על כל העמודות, לוקחת כל חוליה בעמודה, ומפנה את המקום השמור שלה מה HEAP
*/
    printf("\nfree_nodes\n");
    Header* tempH = root[0].right;
    while(tempH != &root[0]){
        Node* tempN = tempH->nodeP;
//        printf("free col:%d", tempN->col);
//            printf("start(%d,%d)", tempN->col,tempN->row);
        tempN = (Node*) tempN->down;
        while(tempN != tempH->nodeP){
            Node* freeN = tempN;
//            printf("\t%d", tempN->row);
            tempN = (Node*) tempN->down;
            free(freeN);
        }
//      printf("\t%d,%d\tfinish col %d\n", tempN->col,tempN->row, tempH->col);
        free(tempN);
        tempH = tempH->right;
    }
    printf("finish free_nodes");
}


void remove_rowNodes(Node* rowN){
/*
פעולה שמקבלת חוליה:
מקטינה את ראש החוליה באחד,
כאשר היא החוליה הראשונה מורידה את המצביע של הראש אחד למטה, אם אין אחד למטה שיצביע על נאל,
*/
    int c = rowN->col;
//    printf("freeing:");
    do{
        if(root[rowN->col].nodeP == rowN){//מצביע עליה
            if(root[rowN->col].size == 1){root[rowN->col].nodeP=NULL;//החוליה היחידה בעמודה
            }else{//יש עוד חוליות
                root[rowN->col].nodeP = (Node*) rowN->down;
            }
        }
        if(root[rowN->col].size != 1){
            Node* upN = (Node*) rowN->up;
            Node* downN = (Node*) rowN->down;
            upN->down = (Node*) downN;
            downN->up = (Node*) upN;
        }
        root[rowN->col].size = root[rowN->col].size-1;
        Node* freeN = rowN;
        rowN = (Node*) rowN->right;
        if(freeN->col != c){//לא לשחרר כי יהיה בעיה בפינוי בסוף
            free(freeN);
        }
    }
    while(rowN->col != c);
//    printf("%d,%d\t",rowN->col,rowN->row);
    free(rowN);
}
int pre_num(int** board){
/*
הפעולה מקבלת לוח סודוקו(מערך דו מימדי),
עוברת על כל מספר בלוח כאשר המספר שונה מ - 0 עושה את הדברים הבאים:
עוברת על כל החוליות בעמודה עד שנמצא החוליה באותה שורה של המספר
**במקרה שחוליה לפי מספר הלוח סודוקו לא תמצא, לא יהיה פתרון בסודוקו ויוחזר ערך  שונה מ - 0
מוציאה את כל החוליות באותה עמודה,
כך שב 4 חלקים שונים במטריצה(שורה) החוליות השכנות ידלגו על החוליה,
מספר גודל החוליות בעמודה יקטן באחד,
בסוף התהליך נכסה את העמודה, בשביל לאפשר לפעולת הפתרון לפתור מבלי לבזבז זמן על המספרים הידועים.
*/
//    printf("\npre_num\n");
    Num* tempN = malloc(sizeof(Num));
    for(int i = 0; i < sudoku; i++){
        tempN->r = i+1;
        for(int j = 0; j < sudoku; j++){
            if(board[i][j] != 0){//כאשר מספר שונה מ 0, כלומר יש מספר כלשהו
                tempN->c = j+1;
                tempN->n = board[i][j];
//                printf("\nnum: %d,%d,%d\n", tempN->c,tempN->r,tempN->n);
                int counter = (sudoku+1);//מקרה שבו אין מספר, אין פתרון, נבדוק האם בדקנו יותר מ 9 חוליות
                int* arr = sudokuToMatrix(tempN);//קבלת כל הפרטים במטריצה לגבי מהספר
                Node* temp = root[arr[1]].nodeP;//קבלת החוליה הראשונה בעמודה החלק הראשון
                while(temp->row != arr[0] && counter > 0){
                    temp = (Node*) temp->down;
                    counter--;
                }
                if(counter == 0){//לא מצא את המספר, ולכן אין פתרון לסודוקו
//                    printf("\t\t\t\t\t\tno solution to the sudoku");
                    return -1;;
                }
//                else{printf("found at:(%d,%d)", temp->col,temp->row);}
                for(int i = 1; i <= 4; i++){
//                    printf("\nnextloop col:%d\t", arr[i]);
                    if(temp == NULL || temp->col != arr[i]){//כאשר חסר חוליה, כלומר הסודוקו לא פתיר, באחת מהעמודות האחרות
//                        printf("\t\t\t\t\t\tno solution to the sudoku");
                        return -1;
                    }
                    temp = (Node*) temp->down;
                    while(temp->row != arr[0]){//לולאה על כל החוליות באותה עמודה
//                        printf("%d,\t", temp->row);
                        Node* upN = (Node*) temp->up;
                        Node* downN = (Node*) temp->down;
                        upN->down = (Node*) downN;
                        downN->up = (Node*) upN;
                        Node* freeN = temp;
                        temp = (Node*) temp->down;
                        remove_rowNodes(freeN);//הוצאת כל החוליות באותה שורה של החוליה שמוציאים
                    }
        //        printf("temp at: (%d,%d)\t", temp->col,temp->row);
                    temp = (Node*) temp->right;
                    free(temp->left);//שחרור החוליה בעמודה הקודמת
        //        printf("new temp: (%d,%d)\n", temp->col,temp->row);
                    //כיסוי העמודה
                    root[arr[i]].right->left = root[arr[i]].left;
                    root[arr[i]].left->right = root[arr[i]].right;

                }
                free(arr);
            }
        }
    }
    free(tempN);
    return 0;
}


// לאגוריתם x

//מערך חיפוש:
static int* cover_order;//מכיל את סדר הכיסוי העכשוי, 
static int* cover_laste_col;//מכיל את מספר החיפוש שכל העמודה כוסתה בה אחרון
static int cover_size;//גודל סדר הכיסוי העכשיו


int least_nodes(int depth){
/*
פועל על פי מערכת נקודות, ככל שיש פחות נקודות עדיפה העמודה,
תחזיר את העמודה שהכי עדיף לנסות לכסות עכשיו
*/
    int min_score = INT_MAX;
    int idx = -1;
    Header* temp = root[0].right;
    while(temp != &root[0]){
        if(temp->size <= 0){//כאשר החיפוש לא טוב, יש עמודה עם 0 חוליות
//            printf("\n\nat index(%d)\n\t\t\tno soulution found\n\n", temp->col);
            return -1;
        }
        int score = temp->size * 1000;//עונש של מספר חוליות
        if(cover_laste_col[temp->col-1] != -1){score += (depth - cover_laste_col[temp->col-1]) * 500;}//עונש לפי הזמן האחרון שנעשה בו שימוש
        else{score -= 100;}//לא נעשה שימוש משהו טוב
        if(score < min_score){
            min_score = score;
            idx = temp->col;
        }
        temp = temp->right;
    }
    return idx;
}

int is_empty(){
/*
פעולה שבודקת האם המטריצה ריקה
*/
return (root[0].right == &root[0]);
}
int is_colver_c(Node* n){
    return (root[n->col].right->left != &root[n->col]);//כאשר העמודה מכוסה מחזיר אמת(1)
}
int is_colver_r(Node* n){
    Node* upP = (Node*) n->up;
    if(root[n->col].size == 1 || upP == n){//לא בהכרח רק שיש אחד, או שהוא שווה לעצמו
        if(root[n->col].nodeP != n){return 1;}//יש חוליה אחת וראש לא מצביע עליה
    }
    return (upP->down != (Node*) n);//כאשר השורה מכוסה מחזיר אמת(1)
}

int cover_row(Node* n){
/*
פעולה שמקבלת חוליה, ומכסה את השורה שלה
בכל מצב שבו ידוע שהשורה מכוסה יוצאים מהפעולה,
כאשר העמודה מכוסה נעבור לחוליה ימינה,
נדלג על החוליה(למעלה למטה),
נקטין את גודל החוליות בראש באחד,
כאשר העמודה מצביע על החוליה, מורידים אחד למטה, כאשר אין אחד למטה מצביע על נאל
*/
    if(is_colver_r(n)){//בדיקה האם השורה מכוסה
//        printf("\talready covered1\t");
        return 0;
    }
    int c = n->col;
    n = (Node*) n->right;
    while(n->col != c){
//בדיקה האם העמודה לא מכוסה, כאשר היא מכוסה לעבור
        if(!is_colver_c(n)){
                if(is_colver_r(n) || root[n->col].nodeP == NULL){//בדיקה האם השורה מכוסה משפיע רק כאשר העמודה לא מכוסה(כאשר המצביע הוא נאל העמודה מכוסה)
//                printf("\talready covered\t");
                return 0;
            }
//            printf("(%d,%d),",n->col,n->row);
            Node* upP = (Node*) n->up;
            Node* downP = (Node*) n->down;
            upP->down = (Node*) downP;
            downP->up = (Node*) upP;
            root[n->col].size = root[n->col].size-1;
            if(root[n->col].nodeP==n){//הראש מצביע על החוליה
                if(root[n->col].size==0){//אין עוד חוליות שיצביע על נאל
//                    printf("setting to NULL(%d)", n->col);
                    root[n->col].nodeP=NULL;
                }else{//יש עוד חוליות
                    root[n->col].nodeP=downP;
                }
            }
        }
//        else{printf("[%d,%d],",n->col,n->row);}//העמודה מכוסה
        n = (Node*) n->right;
    }
    return 0;
}

int uncover_row(Node* n){
/*
פעולה שמקבלת חוליה, ומגלה את השורה שלה,
בכל מצב שיש שהשורה לא מכוסה הפעולה מפסיקה,
כאשר העמודה מכוסה, נעבור ימינה,
נגדיל את מספר החוליות בראש באחד,
כאשר העמודה מצביע על נאל, שיצביע על החוליה, והחוליה תצביע על עצמה,
מציאת מיקום החוליה:
    מוצאים את החוליה שנמצאת מתחת לחוליה שמוסיפים,
    כאשר היא ראשונה נעדכן את הראש,
    מתי שמכניסים מפסיקים את החיפוש,
    כאשר החוליה מצביע לעצמה, או שכבר שהיא אחרונה(נחזור אחורה בחיפוש) נכניס את החוליה אחרונה,
*/
    if(is_colver_r(n)){
//        printf("\talready expose1\t");
        return 0;
    }//השורה כבר גלוייה
    int c = n->col;
    n = (Node*) n->right;
    while(n->col != c){
        if(!is_colver_c(n)){//העמודה גלוייה
           if(!is_colver_r(n) && root[n->col].nodeP != NULL){//כאשר השורה לא מכוסה, פועל רק שהמצביע לא על נאל
//            printf("\talready expose\t");
            return 0;
            }
//           printf("(%d,%d),",n->col,n->row);
            root[n->col].size = root[n->col].size+1;
            if(root[n->col].nodeP == NULL){//העמודה לא מצביע על כלום
//                printf("header to NULL(%d)", n->col);
                root[n->col].nodeP = n;
                n->down = n;
                n->up = n;
            }else{
                int row = n->row;
                Node* tempN = root[n->col].nodeP;
                do{//לולאה למציעת החוליה מתחת
                    if(tempN->row > row){//נמצא חוליה להכניס
                        if(root[n->col].nodeP->row > row){//החוליה ראשונה, לעדכן את המציע בראש
                            root[n->col].nodeP=n;
                            }
    //                      printf("entering(%d,%d)", tempN->col,tempN->row);
                        Node* upP = tempN->up;
                        Node* downP = tempN;
                        upP->down = n;
                        downP->up = n;
                        n->down = downP;
                        n->up = upP;
                        goto next_column;//יציאה מהלולאה, לעבור חוליה ימינה
                    }
                    if(tempN == tempN->down || tempN->row > tempN->down->row){//למקרה שבו החוליה מצביע לעצמה ואף פעם לא תיהיה מתחת או שהוא מצביע להתחלה כבר
//                        printf("breaking:");
                        break;}
                    tempN = tempN->down;
                }while(tempN->row != row);
                //לא הוכנסה החוליה, טימפ אחורנה בעמודה, ניקח אחד מעליו לטיפק החדש
                Node* upP = tempN->up;//החוליה האחרונה(הכי קרובה לחוליה)
                Node* downP = tempN;
                upP->down = n;
                downP->up = n;
                n->down = downP;
                n->up = upP;
            }
        }
//        else{printf("[%d,%d],",n->col,n->row);}
        next_column:
        n = n->right;
    }
    return 0;
}


int cover(Node* n, Stack* s){
/*
פעולת כיסוי, המקבלת חוליה שנבחרה, ומחסנית שתכיל את הפתרון הסופי,
כאשר המחסנית מלאה, להחזיר מינוס 1,
    - כיסוי העמודות, נעבור על כל העמודות בעלי חוליה בשורה של החוליה ונכסה אותן

    -  נעבור עוד פעם על כל העמודות עם חוליה בחוליה שנבחרה
    נעבור על כל החוליות בעמודה:
        נכסה את השורה של החוליה
*/
    if(!stack_push(s,n)){printf("\n\nstack is full(%d)!\n\n",stack_size(s));return -1;}
//    printf("\ncover node:%d,%d\n", n->col,n->row);
    int c = n->col;
//    printf("covering cols:");
    do{//עוברים על כל העמודות ורק מכסים
//        printf("%d\t", n->col);
        root[n->col].left->right = root[n->col].right;
        root[n->col].right->left = root[n->col].left;
        n = (Node*) n->right;
    }
    while(n->col != c);

    do{//נעבור פעם נוספת על כל העמודות של החוליה
//        printf("\ncol(%d,%d) : ", n->col,n->row);
        int r = n->row;
        do{//נעבור על כל החוליות בעמודה
//            printf("row(%d,%d)-",n->col,n->row);
            cover_row(n);//נכסה את החוליות בשורה
            n = (Node*) n->down;
        }while(n->row != r);
        n = (Node*) n->right;
    }while(n->col != c);
//    printf("\nfinish cover\n");
    return 0;
}

Node* uncover(Stack* s){
/*
פעולה שמקבלת מחסנית של הפתרון, מוציא מהמחסנית את החוליה האחרונה שכוסתה, ומגלה אותה,  ומחזירה מצביע לחוליה
כאשר המחסנית ריקה נחזיר נאל
    - נעבור על כל העמודות בחוליה שנבחרה
    בכל עמודה נעבור על כל החוליות בעמודה,
    נגלה את כל השורה של החוליה הנמצאת בשורה

    - נעבור פעם נוספת על כל העמודות בחוליה שנבחרה
    נגלה את כל העמודות
*/
    Node* n = stack_pop(s);
    if(n==NULL){printf("\n\nstack is empty\n\n"); return NULL;}
//    printf("\nuncover:\n");
    int c = n->col;
    do{//עוברים שוב עלכל העמודות הגלויות, אבל בשביל לעבור על כל החוליות בעמודה
//        printf("\ncol(%d,%d) : ", n->col,n->row);
        int r = n->row;
        do{//נעבור על כל החוליות בעמודה
//            printf("row(%d,%d)-",n->col,n->row);
            uncover_row(n);//נגלה את נחוליה, ואת כל החוליות בשורה
            n = (Node*) n->down;
        }while(n->row != r);
        n = n->right;
    }while(n->col != c);
//    printf("\nuncovering col:");
    do{//מגלים את כל העמודות
//        printf("%d\t",n->col);
        root[n->col].left->right = &root[n->col];
        root[n->col].right->left = &root[n->col];
        n = n->right;
    }while(n->col != c);
//    printf("\nfinish uncover\n");
    return n;
}


int solve_recur(Stack* stack, int depth,FILE* file){
/*
פעולה רקורסיבית, שמקבלת מחסנית, ומספר שמייצג את עומק החיפוש,
הפעולה פעולת באלגוריתם כך שתעצר במקרה שנמצא פתרון(המטריצה ריקה),
כאשר המטריצה נמצא במצב של שיש עמודה עם 0 חוליות, יוחזר סימון מינוס 1, שמסמן לחזור לאחור לפי סדר הכיסוי

כאשר המטריצה נמצאת במצב תקין, נכסה את העמודה:
    נבחר את חוליה העליונה ביותר:
    סיבות לבחירה חוליה לפי הסדר:
        בחירה שורה שהסיכוי שלה להיות נכון יותר קשה לחישוב, ולוקח יותר זמן למודל,
        בחירת השורה האופטימציה לרוב לא ישפיע יותר מידי על המודל, והחישוב הכבד לא שווה את זה.
    נכסה את החוליה בשורה,
    **נקרא רקורסיבי לפעולה עוד פעם, הקריאה תסתיים בשתי מקריים, הפתרון נמצא ותחזיר 0, הגענו למצב לא אפשר למטריצה ותחזיר מינוס 1
    במקרה שהוחזר 0 הפעולה תסתיים
    כאשר הגיענו למצב לא טוב, צריך לשנות את החוליה שכוסתה:
        נגלה את החוליה האחרונה שכוסתה(החוליה שנמצאת באותה הלולאה)
        נירד חוליה למטה ונכסה אותה
    במקרה שסיימנו את לעדכן את עמוק החיפוש, ולהחזיר -1
    **תאוריתי אין מצב שבו אין חוליה לא נכונה ולא נגיע למצב שבוא העומק שווה ל - 0.
*/
    if(is_empty()){printf("\n\nfound solution!\n\n");return 0;}
    int c = least_nodes(depth);
//    if(c == -1){print_roots();printf("\nbacktracking\n");return -1;} //לחזור אחורה
    if(c == -1){return -1;}
//    printf("column chose(%d)", c);
    //לגרום להשפעה בבחירת העמודה בפעם הבאה
    if (depth < cover_size) {//מקרה שהחיפוש מתקדם קטן מהחיפוש האחרון
        cover_order[depth] = c;//נעדכן המערך חיפוש, במיקום האחרון שיהיה שווה למספר העמודה
    } else {//כאשר החיפוש יותר מתקדם מהחיפוש הקודם
        cover_order[cover_size++] = c;//נשתמש בגודל החיפוש הקוד, נשוואה למס העמודה ונגדיל את ערך גודל החיפוש הקודם
    }
    cover_laste_col[c] = depth;//נעדכן במערך של החוליות מתי היה שלב החיפוש האחרון

    Node* tempC = root[c].nodeP;//נתחיל בשורה הראשונה בעמודה
    do{//נעבור על כל החוליות בשורה, נבדוק כל חוליה
//        printf("cover(%d,%d)",tempC->col,tempC->row);
        cover(tempC,stack);//כיסוי החוליה
        Num* coverN = matrixToSudoku(tempC->row);
        fprintf(file, "%d %d %d\n", coverN->c, coverN->r, coverN->n);
        fflush(file);
        if(solve_recur(stack,depth+1,file) == 0){return 0;}//הגיע לתשובה
//לא נמצא תשובה לבטל את פעולת הכיסוי, ולעבור חוליה למטה
        uncover(stack);
        fprintf(file, "%d %d -c\n", coverN->c, coverN->r);
        fflush(file);
        tempC = tempC->down;
    }while(tempC->row != root[c].nodeP->row);
//    printf("\nnfinish collumn(%d)\n",c);
    cover_size = depth;
    /*if(depth == 0){
        return(solve_recur(stack,depth));
    }*/
    return -1;
}
Stack* solution_way(){
/*
פעולה שיוצרת מחסנית,
קוראת לפעולה הראקורסיבית למציאת פתרון, כך שהמחסנית תיהיה עם החוליות שנבחרו בשביל הפתרון,
כאשר מוחזר מספר ששונה מ - 0, אין פתרון ונחזיר נאל
*/
    FILE* progress;
        progress = fopen("progress_soduko.txt", "w");
    if (progress == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }
    Stack* stack = stack_create(323);
    if(solve_recur(stack,0,progress) != 0){//אין פתרון
        printf("\n\n\t\t\\ttno solution!\n\n");
        fprintf(progress, "no solution!!");
        fflush(progress);
        fclose(progress);
        return NULL;
    }
//    printf("\n\nstack size = (%d)\ndepth = (%d)\n\n", stack_size(stack), cover_size);
    fprintf(progress, "found solution!");
    fflush(progress);
    fclose(progress);
    return stack;
}

int** init_board(){
/*
פעולה שפותחת קובץ טקסט, וממירה את המערך הדו מימידי שם למערך פתוכנה, ומחזירה מצביע עליו
*/
    FILE* board_file = fopen("sudoku_board.txt", "r");
        if (board_file == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    int **board = (int**)malloc(sudoku * sizeof(int *));

    for(int i = 0; i < sudoku; i++){
        board[i] = (int*)malloc(sudoku * sizeof(int));
        for(int j = 0; j < sudoku; j++){
            fscanf(board_file, "%d", &board[i][j]);
        }
    }
    return board;
}

void solve_board(Stack* s,int** pre_board){
/*
פעולה המקבלת את המחסנית  אם הפתרון, ואת הלוח שהיה מקודם
שתעבור על המחסנית, תגלה את המחסנית(בשביל פינוי החוליות),
לנעביר את החוליה למספר,
נוסיף אותו ללוח.

נוסיף את המספרים שהיו כבר בלוח הקודם,
ונדפיס את המערך הדו מימידי
*/
    printf("\nboard_create:\n");
    int** board = malloc(sizeof(int*)*sudoku);
    for(int i = 0; i < sudoku; i++){
        board[i] = malloc(sizeof(int)*sudoku);
    }
    
    int row = 0;
    int col = 0;
    int num = 0;
    int size = stack_size(s);
    for(int i = 0; i < size; i++){
        Node* poped = uncover(s);
        if(poped == NULL){printf("NULL");}
        Num* temp = matrixToSudoku(poped->row);
        col = temp->c;
        row = temp->r;
        num = temp->n;
        board[row-1][col-1] = num;
        free(temp);
    }
    for(int i = 0; i < sudoku; i++){
        for(int j = 0; j < sudoku; j++){
            if(pre_board[i][j] != 0){board[i][j] = pre_board[i][j];}
        }
    }

    for(int i = 0; i < sudoku; i++){
        printf("\n");
        for(int j = 0; j < sudoku; j++){
            printf("%d,", board[i][j]);
        }
    }

    for(int i = 0; i < sudoku; i++){
        free(board[i]);
    }
    free(board);
    printf("\nfinish board create\n");
}



int main(){
    root = (Header*) malloc(sizeof(Header)*root_len);//הקצאת מקום לערך של העמודות
    init_root();//אתחול העמודות
    init_nodes();//אתחול החוליות

    /*int board[sudoku][sudoku] = {
        {0,0,8,0,4,0,0,0,0},
        {0,0,2,0,0,1,4,0,3},
        {0,0,0,0,0,0,8,0,0},
        {0,0,0,9,0,0,5,6,0},
        {5,0,0,0,8,7,0,0,0},
        {0,0,0,2,0,0,0,9,0},
        {0,3,1,0,0,0,0,0,0},
        {0,9,0,0,0,2,7,0,0},
        {0,0,7,5,0,0,0,0,9}
    };*/
    
    /*int board[sudoku][sudoku] = {
        {0,0,9,0,8,5,0,0,0},
        {0,6,0,7,0,0,0,0,0},
        {5,3,8,0,1,9,0,0,4},
        {9,0,0,0,6,8,0,0,0},
        {0,0,0,0,4,0,1,0,5},
        {0,5,4,0,0,0,0,6,0},
        {0,0,0,9,2,0,7,0,1},
        {0,0,6,0,0,1,4,3,0},
        {0,0,0,0,0,0,0,0,0}
    };*/

    /*int board[sudoku][sudoku] = {
        {0,0,9,8,0,0,0,0,0},
        {0,0,8,9,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,6,5,0,0,0,0,0},
        {0,0,5,6,0,0,0,0,0},
        {0,0,4,3,0,0,0,0,0},
        {0,0,3,4,0,0,0,0,0},
        {0,0,2,1,0,0,0,0,0},
        {0,0,1,2,0,0,0,0,0}
    };*/

    /*int** pre_board = malloc(sudoku * sizeof(int*));
    for(int i = 0; i < sudoku; i++){
        pre_board[i] = malloc(sudoku * sizeof(int));
    }
    for(int i = 0; i < sudoku; i++){
        for(int j = 0; j < sudoku; j++){
        pre_board[i][j] = board[i][j];
        }
    }*/
    int** pre_board = init_board();
    if(pre_board == NULL){goto no_solution;}
    int error = pre_num(pre_board);//נעדכן את המטריצה לפי המספרים הידועים
    if(error == -1){goto no_solution;}//כאשר מספר כלשהו חוסם זה את זה במספרים הידועים ואין פתרון

    cover_order = malloc((root_len-1)*sizeof(int));//הקצאת מקום לסדר כיסוי העמודות
    cover_laste_col = malloc((root_len-1)*sizeof(int));//הקצאת מקום למערך של מתי העמודה נבחרו לאחרונה
    for(int i = 0; i < (root_len-1); i++){cover_laste_col[i] = -1;}

    Stack* stack = solution_way();
        printf("test3");

    if(stack==NULL){goto no_solution;}//כאשר אין פתרון והמחסנית שווה לנאל
    solve_board(stack,pre_board);//הדפסה של הלוח
    
    //שחרור כל הזיכרון ב HEAP שהוקצה
    stack_free(stack);
    free_label:
    for(int i = 0; i < sudoku; i++){
        free(pre_board[i]);
    }
    free(pre_board);
    free(cover_order);
    free(cover_laste_col);
    free_nodes();
    free(root);
    return 0;
//למקרה שיש בעיה נעדכן את הקובץ שאין פתרון
    no_solution:
    printf("\n\nno solution!\n\n");
    FILE* noSOl;
        noSOl = fopen("progress_soduko.txt", "w");
    if (noSOl == NULL) {printf("Error opening file!\n");}
    fprintf(noSOl, "no solution!");
    fflush(noSOl);
    fclose(noSOl);
    fclose(noSOl);
    goto free_label;//לשחרר זיכרון
}
