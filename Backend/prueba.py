import requests

# URL del servidor Flask
url = 'http://127.0.0.1:5000/run_query'

# El query que queremos enviar
query_data = {
    'query': 'SELECT MeetID,Federation,MeetCountry FROM CustomerSparse WHERE MeetID BETWEEN 160 AND 170'
}

# Hacer la petición POST
response = requests.post(url, json=query_data)

# Mostrar el resultado de la ejecución
if response.status_code == 200:
    print("Output from C++ program:")
    print(response.json()['output'])
else:
    print(f"Error: {response.status_code}")
    print(response.json().get('error'))
