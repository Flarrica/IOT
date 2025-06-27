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
    except subprocess.CalledProcessError as e:
        print(f" Error al convertir {input_file}")
        print(" STDOUT:", e.stdout)
        print(" STDERR:", e.stderr)

# Ejecutar conversión en todos los .wav dentro de ./main/
for file in os.listdir("main"):
    if file.lower().endswith(".wav"):
        input_path = os.path.join("main", file)
        output_path = os.path.join("main", file.replace(".wav", ".pcm"))
        convert_wav_to_pcm(input_path, output_path)