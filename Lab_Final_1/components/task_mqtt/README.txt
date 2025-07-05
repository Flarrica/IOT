COMO MANDAR COMANDOS POR MOSQUITTO

PS C:\Users\gasto> & "C:\Program Files\mosquitto\mosquitto_pub.exe" -h broker.hivemq.com -t "/control/reproduccion" -m "play" -V mqttv5        # Inicia la reproducción
PS C:\Users\gasto> & "C:\Program Files\mosquitto\mosquitto_pub.exe" -h broker.hivemq.com -t "/control/reproduccion" -m "pause" -V mqttv5       # Pausa la reproducción
PS C:\Users\gasto> & "C:\Program Files\mosquitto\mosquitto_pub.exe" -h broker.hivemq.com -t "/control/reproduccion" -m "stop" -V mqttv5        # Detiene (alias de pause)
PS C:\Users\gasto> & "C:\Program Files\mosquitto\mosquitto_pub.exe" -h broker.hivemq.com -t "/control/reproduccion" -m "next" -V mqttv5        # Pasa al siguiente track
PS C:\Users\gasto> & "C:\Program Files\mosquitto\mosquitto_pub.exe" -h broker.hivemq.com -t "/control/reproduccion" -m "previous" -V mqttv5    # Vuelve al track anterior
PS C:\Users\gasto> & "C:\Program Files\mosquitto\mosquitto_pub.exe" -h broker.hivemq.com -t "/control/reproduccion" -m "volume_up" -V mqttv5   # Aumenta el volumen
PS C:\Users\gasto> & "C:\Program Files\mosquitto\mosquitto_pub.exe" -h broker.hivemq.com -t "/control/reproduccion" -m "volume_down" -V mqttv5 # Disminuye el volumen