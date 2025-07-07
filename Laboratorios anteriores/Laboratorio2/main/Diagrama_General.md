graph TD
    A[INICIO app_main] --> B(Establecer nivel de logs para módulos específicos)
    B --> C(Inicializar TouchPad)
    C --> D(Inicializar LED RGB)
    D --> E(Inicializar WiFi AP/STA)
    E --> F(Inicializar Web Service)
    F --> G(Inicializar Watchdog para la tarea actual)
    G --> H{BUCLE PRINCIPAL (while true)}

    H --> I(Ejecutar bucle de polling del TouchPad)
    I --> J(Ejecutar bucle del Web Service)
    J --> K(Ejecutar bucle del LED RGB)
    K --> L(Esperar 10 ms)
    L --> M(Resetear Watchdog)
    M --> H