graph TD
    A[INICIO] --> B(Inicializar biblioteca de sensores táctiles)
    B --> C(Configurar canales táctiles)
    C --> D(Establecer modo de medición FSM por timer)
    D --> E(Iniciar escaneo de pads)
    E --> F[BUCLE PRINCIPAL (ITERACIÓN)]

    F --> G{PARA CADA BOTÓN TÁCTIL}
    G --> H(Leer valor crudo de capacitancia)
    H --> I{¿Valor > Umbral Y != Último Botón?}
    I -- No --> F
    I -- Sí --> J(Actualizar evento de LED asociado)
    J --> K(Guardar índice del último botón presionado)
    K --> G
    G --> L(FIN PARA CADA BOTÓN TÁCTIL)
    L --> F