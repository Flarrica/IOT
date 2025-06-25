graph TD
    A[INICIO] --> B(Inicializar módulos para red y sistema de eventos)
    B --> C(Crear interfaz de red en modo estación)
    C --> D(Configurar parámetros de la red WiFi: SSID, contraseña, etc.)
    D --> E(Inicializar controlador WiFi con configuración por defecto)
    E --> F(Registrar manejadores de eventos para:)
    F --> F1{Eventos de WiFi (conexión, desconexión, etc.)}
    F1 --> F2{Eventos de IP (IP obtenida, IP perdida, etc.)}
    F2 --> G(Establecer modo de operación WiFi como estación (STA))
    G --> H(Aplicar la configuración de red a la interfaz)
    H --> I(Iniciar el módulo WiFi)
    I --> J(Intentar conectar a la red configurada)
    J --> K[BUCLE PRINCIPAL O ESPERA DE EVENTOS]

    K --> L{EN LOS MANEJADORES DE EVENTOS:}
    L --> M{Si se obtiene una IP}
    M --> N(Mostrar mensaje de conexión exitosa)
    N --> K
    L --> O{Si se pierde la conexión}
    O --> P(Intentar reconectar)
    P --> Q(O mostrar mensaje de error)
    Q --> K