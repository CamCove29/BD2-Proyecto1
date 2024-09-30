from flask import Flask, request, jsonify
from flask_cors import CORS
import subprocess

app = Flask(__name__)

# Habilita CORS para todas las rutas y orígenes
CORS(app)

# Ruta para procesar la query
@app.route('/run_query', methods=['POST'])
def run_query():
    data = request.get_json()  # Recibe el query en JSON
    query = data['query']

    # Llama al parser C++ pasando el query como archivo
    with open("input_query.sql", "w") as f:
        f.write(query)

    try:
        # Ejecuta el programa de C++ que procesará el archivo
        result = subprocess.run(['./main.exe', 'input_query.sql'], capture_output=True, text=True)
        
        # Captura la salida del cout
        output = result.stdout

        return jsonify({'output': output})
    
    except Exception as e:
        print(e)
        return jsonify({'error': str(e)}), 500

# Nueva ruta para obtener el contenido de metadata.csv
@app.route('/metadata', methods=['GET'])
def get_metadata():
    metadata = {}
    
    try:
        # Lee el archivo CSV
        with open('metadata.csv', mode='r') as file:
            #headers
            headers = file.readline().strip().split(',')

            metadata['headers'] = headers
            metadata['tables'] = []

            lista = file.readlines()
            for line in lista:
                line = line.strip().split(',')
                metadata['tables'].append(line)
            
        return jsonify(metadata)  # Devuelve el contenido como JSON

    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == "__main__":
    app.run(debug=True)
