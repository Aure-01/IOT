from http.server import BaseHTTPRequestHandler, HTTPServer
import json

contador = 11

class MyHTTPRequestHandler(BaseHTTPRequestHandler):
    def _set_response(self, content_type="text/plain"):
        self.send_response(200)
        self.send_header("Content-type", content_type)
        self.end_headers()

    def throw_custom_error(self, message):
        self._set_response("application/json")
        self.wfile.write(json.dumps({"message": "Invalid JSON"}).encode())

    def do_GET(self):
        self._set_response()
        respuesta = "El valor es: " + str(contador)
        self.wfile.write(respuesta.encode())

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
            self._set_response("application/json")
            self.throw_custom_error("Missing action or quantity")
            return
        if (body_json['action'] != 'asc' and body_json['action'] != 'desc'):
            self.throw_custom_error("Invalid action")
            return
        
        try: 
            int(body_json['quantity'])
        except:
            self.throw_custom_error("Invalidad quantity")
            return
        
        # Check if action is valid
        if(body_json['action'] == 'asc'):
            contador += int(body_json['quantity'])
        elif(body_json['action'] == 'desc'):
            contador -= int(body_json['quantity'])

        # Print the complete HTTP request
        print("\n----- Incoming POST Request -----")
        print(f"Requestline: {self.requestline}")
        print(f"Headers:\n{self.headers}")
        print(f"Body:\n{post_data.decode()}")
        print("-------------------------------")

        # Respond to the client
        response_data = json.dumps({"message": "Received POST data", "data": post_data.decode(), "status": "OK"})
        self._set_response("application/json")
        self.wfile.write(response_data.encode())

def run_server(server_class=HTTPServer, handler_class=MyHTTPRequestHandler, port=7800):
    server_address = ("", port)
    httpd = server_class(server_address, handler_class)
    print(f"Starting server on port {port}...")
    httpd.serve_forever()

if __name__ == "__main__":
    run_server()
