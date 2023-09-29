from http.server import BaseHTTPRequestHandler, HTTPServer
import json
import os

contador = 11
led = False
temperature = 0

class MyHTTPRequestHandler(BaseHTTPRequestHandler):
    def _set_response(self, content_type="text/plain", status_code=200):
        self.send_response(status_code)
        self.send_header("Content-type", content_type)
        self.end_headers()

    def throw_custom_error(self, message, status_code=400):
        self._set_response("application/json", status_code)
        self.wfile.write(json.dumps({"message": message}).encode())

    def do_GET(self):
        if self.path == "/":
            try:
                self._set_response(content_type="text/html")
                html_file_path = os.path.abspath("APPS_IOT/UNIDAD2/index.html")
                with open(html_file_path, "r", encoding="utf-8") as file_to_open:
                    self.wfile.write(file_to_open.read().encode())
            except Exception as e:
                print(f"Error: {e}")
                self.wfile.write(f"Error: {e}".encode())
        elif self.path == "/counter":
            self._set_response()
            self.wfile.write(json.dumps({"contador": contador}).encode())
        elif self.path == "/temperature":
            self._set_response()
            # Simplemente devuelve un valor de temperatura fijo para demostración
            self.wfile.write(json.dumps({"temperature": temperature}).encode())
        elif self.path == "/led":
            self._set_response()
            self.wfile.write(json.dumps({"status": led}).encode())
        else:
            self.throw_custom_error("Invalid path")

    def do_POST(self):
        content_length = int(self.headers["Content-Length"])
        post_data = self.rfile.read(content_length)

        try:
            body_json = json.loads(post_data.decode())
        except:
            self.throw_custom_error("Invalid JSON")
            return

        global contador

        if (body_json.get('action') is None or body_json.get('quantity') is None):
            self.throw_custom_error("Missing action or quantity")
            return

        if (body_json['action'] != 'asc' and body_json['action'] != 'desc'):
            self.throw_custom_error("Invalid action")
            return

        try:
            int(body_json['quantity'])
        except:
            self.throw_custom_error("Invalid quantity")
            return

        if (body_json['action'] == 'asc'):
            contador += int(body_json['quantity'])
        elif (body_json['action'] == 'desc'):
            contador -= int(body_json['quantity'])

        # Respond to the client
        response_data = json.dumps({"message": "Received POST data, new value: " + str(contador), "status": "OK"})
        self._set_response("application/json")
        self.wfile.write(response_data.encode())

def run_server(server_class=HTTPServer, handler_class=MyHTTPRequestHandler, port=7800):
    server_address = ("", port)
    httpd = server_class(server_address, handler_class)
    print(f"Starting server on port {port}...")
    httpd.serve_forever()

if __name__ == "__main__":
    run_server()
