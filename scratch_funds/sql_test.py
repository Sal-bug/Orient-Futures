import mysql.connector

dataBase = mysql.connector.connect(
  host ="localhost",
  user ="marcus",
  passwd ="200718",
  database = "funds"
)

cursorObject = dataBase.cursor()

# Create table 
studentRecord = """CREATE TABLE IF NOT EXISTS STUDENT (
                   NAME  VARCHAR(20) NOT NULL,
                   BRANCH VARCHAR(50),
                   ROLL INT NOT NULL,
                   SECTION VARCHAR(5),
                   AGE INT
                  )"""
cursorObject.execute(studentRecord)

# Insert data
insert = "INSERT INTO STUDENT (NAME, BRANCH, ROLL, SECTION, AGE)\
VALUES (%s, %s, %s, %s, %s)"
val = ("Ram", "CSE", "85", "B", "19")
cursorObject.execute(insert, val)

insert = "INSERT INTO STUDENT (NAME, BRANCH, ROLL, SECTION, AGE)\
VALUES (%s, %s, %s, %s, %s)"
val = ("Sam", "ECE", "90", "A", "20")
cursorObject.execute(insert, val)

# Retrieve data
query = "SELECT * FROM STUDENT ORDER BY NAME DESC"
cursorObject.execute(query)

myresult = cursorObject.fetchall()
   
for each in myresult:
    print(each)
  
# disconnecting from server
cursorObject.close()
dataBase.close()