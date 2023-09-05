from http.server import BaseHTTPRequestHandler, HTTPServer
import json

contador = 0

class MyHTTPRequestHandler(BaseHTTPRequestHandler):
    def _set_response(self, content_type="text/plain"):
        self.send_response(200)
        self.send_header("Content-type", content_type)
        self.end_headers()

    def do_GET(self):
        self._set_response()
        respuesta = "El valor es: " + str(contador)
        self.wfile.write(respuesta.encode())

    def do_POST(self):
        content_length = int(self.headers["Content-Length"])
        post_data = self.rfile.read(content_length)
        
        body_json = json.loads(post_data.decode())
        quantity = body_json.get('quantity')
        
        global contador
        
        if(body_json['action'] == 'asc'):
            contador += quantity
        elif(body_json['action'] == 'desc'):
            contador -= quantity

        # Crear una respuesta JSON con el valor actualizado de contador
        response_data = json.dumps({"message": "Received POST data", "contador": contador, "status": "OK"})

        # Configurar la respuesta HTTP y enviarla al cliente
        self._set_response("application/json")
        self.wfile.write(response_data.encode())

        # Print the complete HTTP request
        print("\n----- Incoming POST Request -----")
        print(f"Requestline: {self.requestline}")
        print(f"Headers:\n{self.headers}")
        print(f"Body:\n{post_data.decode()}")
        print("-------------------------------")

def run_server(server_class=HTTPServer, handler_class=MyHTTPRequestHandler, port=7800):
    server_address = ("", port)
    httpd = server_class(server_address, handler_class)
    print(f"Starting server on port {port}...")
    httpd.serve_forever()

if __name__ == "__main__":
    run_server()