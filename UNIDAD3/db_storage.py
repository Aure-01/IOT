import sqlite3
import datetime

class DBStorage:
    def __init__(self, db_name = "data.db"):
        self.db_name = db_name
        self.db = None
        self.cursor = None

    def connect(self):
        self.db = sqlite3.connect(self.db_name)
        self.cursor = self.db.cursor()

    def disconnect(self):
        self.db.close()

    def create_table(self):
        self.cursor.execute(
            "CREATE TABLE IF NOT EXISTS mediciones (id INTEGER PRIMARY KEY AUTOINCREMENT, valor_temperatura FLOAT, valor_humedad FLOAT, fecha TIMESTAMP DEFAULT CURRENT_TIMESTAMP)")

    def insert(self, humedad, temperatura):
        fecha = datetime.datetime.now()
        self.cursor.execute(
            "INSERT INTO mediciones (valor_humedad, valor_temperatura, fecha) VALUES (?, ?, ?)", 
            (humedad, temperatura, fecha)
        )
        self.db.commit()

    def get_measurements(self, start_date, end_date):
        query = "SELECT fecha, valor_temperatura, valor_humedad FROM mediciones WHERE fecha BETWEEN ? AND ?"
        self.cursor.execute(query, (start_date, end_date))
        rows = self.cursor.fetchall()

        measurements = {
            "labels": [],
            "temperatures": [],
            "humidities": []
        }

        for row in rows:
            date, temperature, humidity = row
            measurements["labels"].append(date)
            measurements["temperatures"].append(temperature)
            measurements["humidities"].append(humidity)
        return measurements


if __name__ == "__main__":
    db = DBStorage("data.db")
    db.connect()
    db.create_table()
    db.insert(10, 20)
    print(db.get_measurements(0,0))
    db.disconnect()