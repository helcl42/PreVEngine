import requests, zipfile, io, sys

def download_file_from_google_drive(id):
    print("Downloading dependencies, plaese wait...", end='\n', file=sys.stdout, flush=True)
    URL = "https://docs.google.com/uc?export=download"

    session = requests.Session()

    response = session.get(URL, params = { 'id' : id }, stream = True)
    token = get_confirm_token(response)

    if token:
        params = { 'id' : id, 'confirm' : token }
        response = session.get(URL, params = params, stream = True)

    save_and_extract_response_content(response)    

def get_confirm_token(response):
    for key, value in response.cookies.items():
        if key.startswith('download_warning'):
            return value

    return None

def save_and_extract_response_content(response):
    CHUNK_SIZE = 32768
    buffer = io.BytesIO()
    for chunk in response.iter_content(CHUNK_SIZE):
        if chunk: # filter out keep-alive new chunks
            buffer.write(chunk)

    z = zipfile.ZipFile(buffer)
    z.extractall()

file_id = '1_9gK0mHCFIuXkZlF7yv9HmXcr4KabK0c'
download_file_from_google_drive(file_id)