from http.server import BaseHTTPRequestHandler, HTTPServer
import json
# Inicializacion del contador
contador = 0
# Configuracion de la url 
class MyHTTPRequestHandler(BaseHTTPRequestHandler):
    def _set_response(self, content_type="text/plain"):
        self.send_response(200)
        self.send_header("Content-type", content_type)
        self.end_headers()
# Cuando usemos GET se muestra el valor de contador 
    def do_GET(self):
        self._set_response()
        respuesta = "El valor es: " + str(contador)
        self.wfile.write(respuesta.encode())
# Cuando usemos POST se haran 2 condicones una con action y otra con quantity
    def do_POST(self):
        content_length = int(self.headers["Content-Length"])
        post_data = self.rfile.read(content_length)
        # Almacenamos la informacion del JASON en una variable llamada body_json
        body_json = json.loads(post_data.decode())
        # Obtenemos el valor de quantity y lo almacenamos en una variable 
        quantity = body_json.get('quantity')
        
        global contador
        # Creamos la condicion con la informacion mandada por el JASON
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

"""
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
    self.wfile.write(json.dumps({"message": message}).encode())

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

    # Check if action and quantity are present
    if (body_json.get('action') is None or body_json.get('quantity') is None):
      self.throw_custom_error("Missing action or quantity")
      return

    # Check if action is valid
    if (body_json['action'] != 'asc' and body_json['action'] != 'desc'):
      self.throw_custom_error("Invalid action")
      return

    # Check if quantity is valid, integer
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
    response_data = json.dumps({"message": "Received POST data, new value:" + 
    str(contador), "status": "OK"})
    self._set_response("application/json")
    self.wfile.write(response_data.encode())


def run_server(server_class=HTTPServer, handler_class=MyHTTPRequestHandler, port=7800):
  server_address = ("", port)
  httpd = server_class(server_address, handler_class)
  print(f"Starting server on port {port}...")
  httpd.serve_forever()


if __name__ == "__main__":
  run_server()
"""