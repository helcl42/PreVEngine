# import requests, zipfile, io, sys

# def download_file_from_google_drive(id):
#     print("Downloading dependencies, plaese wait...", end='\n', file=sys.stdout, flush=True)
#     URL = "https://docs.google.com/uc?export=download"

#     session = requests.Session()

#     response = session.get(URL, params = { 'id' : id }, stream = True)
#     token = get_confirm_token(response)

#     if token:
#         params = { 'id' : id, 'confirm' : token }
#         response = session.get(URL, params = params, stream = True)

#     save_and_extract_response_content(response)    

# def get_confirm_token(response):
#     for key, value in response.cookies.items():
#         if key.startswith('download_warning'):
#             return value

#     return None

# def save_and_extract_response_content(response):
#     CHUNK_SIZE = 32768
#     buffer = io.BytesIO()
#     for chunk in response.iter_content(CHUNK_SIZE):
#         if chunk: # filter out keep-alive new chunks
#             buffer.write(chunk)

#     z = zipfile.ZipFile(buffer)
#     z.extractall()

# file_id = '1_9gK0mHCFIuXkZlF7yv9HmXcr4KabK0c'
# download_file_from_google_drive(file_id)

import requests
import zipfile
import os


def download_file_from_google_drive(file_id, destination, chunk_size=32768):
    url = "https://docs.google.com/uc?export=download"

    session = requests.Session()
    params = {'id': file_id, 'confirm': 1}
    response = session.get(url, params=params, stream=True)

    file_size = response.headers.get('content-length')

    for i, chunk_size_ in save_response_content(response, destination, chunk_size):
        yield i, chunk_size_, int(file_size)


def get_confirm_token(response):
    for key, value in response.cookies.items():
        if key.startswith('download_warning'):
            return value

    return None


def save_response_content(response, destination, chunk_size):
    with open(destination, "wb") as f:
        for i, chunk in enumerate(response.iter_content(chunk_size)):
            if chunk:  # filter out keep-alive new chunks
                f.write(chunk)
                yield i, chunk_size


def download_file(file_id, destination):
    bytes_downloaded = 0
    for i, chunk_size, file_size in download_file_from_google_drive(file_id, destination):
        bytes_downloaded += chunk_size
        downloaded_percentage = (bytes_downloaded / file_size) * 100.0
        print(f'Downloaded {bytes_downloaded} from {file_size} - {downloaded_percentage:.1f}%', end = '\r')
    print('')


def unzip_file(zip_file_path, output_path):
    print(f'Unzipping {zip_file_path} ...')
    with zipfile.ZipFile(zip_file_path, 'r') as zip_ref:
        zip_ref.extractall(output_path)


if __name__ == '__main__':
    output_folder = './'
    file_id = '1_9gK0mHCFIuXkZlF7yv9HmXcr4KabK0c'
    archive_path = os.path.join(output_folder, 'Dependencies.zip')
    download_file(file_id, archive_path)
    unzip_file(archive_path, output_folder)
    os.remove(archive_path)
