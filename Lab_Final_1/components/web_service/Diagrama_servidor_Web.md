graph TD
    A[INICIO] --> B(Inicializar módulos necesarios para red y eventos del sistema)
    B --> C(Crear una interfaz de red (AP o STA según el caso))
    C --> D(Configurar y activar el módulo WiFi con los parámetros correspondientes)
    D --> E(Crear una instancia del servidor HTTP)
    E --> F(Definir un manejador para solicitudes HTTP)
    F --> F1[El manejador debe generar una respuesta con el contenido: "Hola mundo"]
    F1 --> G(Registrar el manejador en el servidor para el endpoint deseado ("/"))
    G --> H(Iniciar el servidor web)
    H --> I[Esperar conexiones entrantes desde clientes (navegador u otros)]

    I --> J{Cuando se recibe una solicitud HTTP}
    J --> K(Responder con el mensaje "Hola mundo")
    K --> I