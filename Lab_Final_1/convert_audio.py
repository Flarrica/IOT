import subprocess
import os

def convert_wav_to_pcm(input_file, output_file, sample_rate=16000):
    command = [
        "ffmpeg",
        "-y",
        "-i", input_file,
        "-f", "s16le",
        "-acodec", "pcm_s16le",
        "-ac", "2",
        "-ar", str(sample_rate),
        output_file
    ]
    try:
        print(f" Convirtiendo: {input_file}")
        result = subprocess.run(command, check=True, capture_output=True, text=True)
        print(f" Convertido correctamente a: {output_file}")
        os.remove(input_file)
        print(f" Eliminado original: {input_file}")
    except subprocess.CalledProcessError as e:
        print(f" Error al convertir {input_file}")
        print(" STDOUT:", e.stdout)
        print(" STDERR:", e.stderr)

# Carpeta de entrada
spiffs_dir = "spiffs"

# Ejecutar conversión en todos los .wav dentro de ./spiffs/
for file in os.listdir(spiffs_dir):
    if file.lower().endswith(".wav"):
        input_path = os.path.join(spiffs_dir, file)
        output_path = os.path.join(spiffs_dir, file.replace(".wav", ".pcm"))
        convert_wav_to_pcm(input_path, output_path)