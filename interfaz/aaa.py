import subprocess

# Ejecuta el programa C++
proceso = subprocess.run(["./prueba"], capture_output=True, text=True)

# Captura la salida
output = proceso.stdout

# Divide la salida en líneas
lineas = output.strip().split("\n")

# Asegúrate de que hay al menos dos líneas
if len(lineas) >= 2:
    # Captura la segunda línea
    segunda_linea = lineas[1]
    
    # Convierte la segunda línea en una lista de enteros
    numeros = list(map(int, segunda_linea.split()))
    
    print(f"Los números capturados son: {numeros}")
else:
    print("No se recibió suficiente salida del programa.")
