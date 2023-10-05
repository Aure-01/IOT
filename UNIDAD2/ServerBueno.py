from http.server import BaseHTTPRequestHandler, HTTPServer
import json
import os

# Inicializa variables
contador = 0  # Inicializa un contador en 0
led = False  # Inicializa el estado del LED como apagado
temperature = 0  # Inicializa la temperatura como un valor de punto flotante
humidity = 0 #Inicializa la temperatura de la humedad

# Clase que maneja las solicitudes HTTP
class MyHTTPRequestHandler(BaseHTTPRequestHandler):

    def _set_response(self, content_type="text/plain", status_code=200):
        # Método para establecer las respuestas HTTP
        self.send_response(status_code)
        self.send_header("Content-type", content_type)
        self.end_headers()

    def throw_custom_error(self, message, status_code=400):
        # Método para enviar respuestas de error personalizadas
        self._set_response("application/json", status_code)
        self.wfile.write(json.dumps({"message": message}).encode())

    def do_GET(self):
        # Manejo de solicitudes GET
        if self.path == "/":
            # Si la URL es la raíz ("/"), responde con un archivo HTML
            try:
                self._set_response(content_type="text/html")
                html_file_path = os.path.abspath("APPS_IOT/UNIDAD2/index.html")
                with open(html_file_path, "r", encoding="utf-8") as file_to_open:
                    self.wfile.write(file_to_open.read().encode())
            except Exception as e:
                print(f"Error: {e}")
                self.wfile.write(f"Error: {e}".encode())
        elif self.path == "/counter":
            # Si la URL es "/counter", responde con el valor del contador
            self._set_response()
            self.wfile.write(json.dumps({"contador": contador}).encode())
        elif self.path == "/led":
            # Si la URL es "/led", responde con el estado del LED
            global led
            self._set_response()
            self.wfile.write(json.dumps({"status": led}).encode())
        elif self.path == "/led/on":
            # Si la URL es "/led/on", enciende el LED
            led = True
            self._set_response()
            self.wfile.write(json.dumps({"status": led}).encode())
        elif self.path == "/led/off":
            # Si la URL es "/led/off", apaga el LED
            led = False
            self._set_response()
            self.wfile.write(json.dumps({"status": led}).encode())
        elif self.path == "/temperature":
            # Si la URL es "/temperature", responde con el valor de temperatura
            self._set_response()
            self.wfile.write(json.dumps({"temperature": temperature, "humidity": humidity}).encode())
        else:
            # Responde con un error si la URL no coincide con ninguna de las anteriores
            self.throw_custom_error("Invalid path")

    def do_POST(self):
        # Manejo de solicitudes POST
        global contador  # Use the global variable contador
        content_length = int(self.headers["Content-Length"])
        post_data = self.rfile.read(content_length)
        print(f"Received POST data: {post_data.decode()}")  # Agregar este mensaje de depuración

        try:
            body_json = json.loads(post_data.decode())
            if 'temperature' in body_json:
                global temperature  # Asegúrate de que estás modificando la variable global
                global humidity 
                temperature = float(body_json['temperature'])
                humidity = float(body_json['humidity'])
                print(f"Received temperature: {temperature}")  # Agregar este mensaje de depuración
                print(f"Received humidity: {humidity}")  # Agregar este mensaje de depuración
                self._set_response()
                self.wfile.write(json.dumps({"message": "Received temperature and humidity data", "temperature": temperature, "humidity": humidity, "status": "OK"}).encode())
            elif 'action' in body_json and 'quantity' in body_json:
                if body_json['action'] == 'asc':
                    contador += int(body_json['quantity'])
                elif body_json['action'] == 'desc':
                    contador -= int(body_json['quantity'])
                response_data = json.dumps({"message": "Received POST data, new value: " + str(contador), "status": "OK"})
                self._set_response("application/json")
                self.wfile.write(response_data.encode())
            else:
                self.throw_custom_error("Invalid data")
        except ValueError as e:
            print(f"Error in processing POST data: {e}")  # Agregar este mensaje de depuración
            self.throw_custom_error("Invalid JSON: " + str(e))

def run_server(server_class=HTTPServer, handler_class=MyHTTPRequestHandler, port=7800):
    # Función para ejecutar el servidor
    server_address = ("", port)
    httpd = server_class(server_address, handler_class)
    print(f"Starting server on port {port}...")
    httpd.serve_forever()

if __name__ == "__main__":
    run_server()  # Ejecuta el servidor si este archivo es el punto de entrada
