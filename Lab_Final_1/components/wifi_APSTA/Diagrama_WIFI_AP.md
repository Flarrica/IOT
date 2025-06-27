graph TD
    A[INICIO] --> B(Inicializar módulos para red y sistema de eventos)
    B --> C(Crear interfaz de red en modo Punto de Acceso (AP))
    C --> D(Configurar parámetros de la red a crear:)
    D --> D1{Nombre del SSID}
    D1 --> D2{Contraseña}
    D2 --> D3{Canal}
    D3 --> D4{Número máximo de conexiones}
    D4 --> D5{Tipo de autenticación}
    D5 --> E(Inicializar controlador WiFi con configuración por defecto)
    E --> F(Registrar manejadores de eventos para:)
    F --> F1{Detección de dispositivos conectados}
    F1 --> F2{Desconexión de dispositivos}
    F2 --> G(Establecer modo de operación WiFi como Punto de Acceso (AP))
    G --> H(Aplicar la configuración a la interfaz WiFi)
    H --> I(Iniciar el módulo WiFi)
    I --> J[BUCLE PRINCIPAL O ESPERA DE EVENTOS]

    J --> K{EN LOS MANEJADORES DE EVENTOS:}
    K --> L{Si un dispositivo se conecta}
    L --> M(Mostrar su dirección MAC)
    M --> N(Confirmar la conexión)
    N --> K
    K --> O{Si un dispositivo se desconecta}
    O --> P(Registrar el evento)
    P --> Q(Mostrar su dirección MAC)
    Q --> K