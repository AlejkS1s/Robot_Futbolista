# Robot_Futbolista
Robot que juega futbol, y va a competir en Torneo de robótica USCOTRON
## Requisitos
- Control: Radiofrecuencia (RF) o Bluetooth.
- Distancia mínima: 3 metros.
- Frecuencias múltiples: Evitar interferencias; disponer de una alternativa.
- Conflictos de frecuencia: Organizadores asignan nuevas frecuencias.
- Prohibido interferir: Sanciones y descalificación.
## Características de Hardware
  - ESP32
  - Driver 4 Motores DC (3v3 o 5v)
  - Baterias

## Características de Software
  - ESP32 SDK Arduino (PlatformIO)
  - Android Studio (Java)

## Control del Robot 
- **Control Manual**
  - Aplicación android (Bluetooth).
  - joystick (Investigar).
  - Aplicación web.
  - RF control a distancia.

- **Control Semiautomático** **Hard**
  Implementación avanzada que pueda ayudar al robot a reaccionar parcialmente de forma autónoma, asistiendo al operador en el juego.
- **Control Automático**
  Uso de **Machine Learning** (propuesta en evaluación, es una meta de alta complejidad) para desarrollar un control completamente autónomo del robot.

### Etapa de Alimentación
Uso de baterías, ya sea recargables o de un solo uso:
- Baterias de litio 18650
- Baterias alcalinas
- Baterias de 9V
- regulador para 3.3v (esp32)

### Etapa de Control de Motores
El control de los motores es esencial para la precisión en los movimientos del robot. En esta etapa, se están evaluando diferentes controladores para optimizar la potencia, eficiencia y precisión.

**Controladores para 2 Motores DC**

| Controlador   | Voltaje Operativo | Corriente Máxima (por canal) | Caída de Voltaje | Modo de Control     | Características Destacadas                                  |
|---------------|-------------------|------------------------------|-------------------|----------------------|------------------------------------------------------------|
| **L298N**     | 5 - 46 V          | 2 A                          | Alta              | PWM y digital        | Comúnmente utilizado; diseño antiguo, menor eficiencia     |
| **DRV8833**   | 2.7 - 10.8 V      | 1.5 A                        | Baja              | PWM y digital        | Compacto, eficiente, menor generación de calor             |
| **DRV8825**   | 8.2 - 45 V        | 2.5 A                        | Baja              | PWM (con microstepping)| Compatible con motores paso a paso; ideal para alta precisión|
| **TB6612FNG** | 2.5 - 13.5 V      | 1.2 A                        | Baja              | PWM y digital        | Alta eficiencia; menos calor, adecuado para robótica       |
| **A4950**     | 4.5 - 40 V        | 3.5 A                        | Muy baja          | PWM y digital        | MOSFET, menor disipación de energía, excelente para alta potencia |
| **L6205**     | 8 - 52 V          | 2.8 A                        | Moderada          | PWM y digital        | Alta eficiencia, adecuado para motores de potencia media-alta |

**Controladorespara 4 Motores DC**
Aquí tienes una tabla comparativa de los controladores de motor que pueden manejar hasta cuatro motores:

| Controlador                        | Voltaje Operativo | Corriente Máxima (por canal) | Canales de Motor | Modo de Control     | Características Destacadas                                       |
|------------------------------------|-------------------|------------------------------|-------------------|----------------------|------------------------------------------------------------------|
| **DRV8434S**                       | 4.5 - 48 V       | 1.5 A                        | 4                 | PWM y digital        | Alta eficiencia, baja generación de calor, buen rendimiento en aplicaciones de robótica móvil. |
| **MDD10A (Dual Channel H-Bridge)** | 7 - 30 V         | 10 A                         | 2 (paralelo para 4) | PWM y entradas lógicas | Robusto y adecuado para motores de alta potencia, permite hasta 10A por canal.   |
| **PCA9685 + MOSFETs**              | Depende del MOSFET | Depende del MOSFET          | 16 (PWM)          | I2C                  | Control de hasta 16 canales PWM; necesita MOSFETs para manejar corriente alta. Ideal para proyectos que requieren muchas salidas. |
| **Pololu Dual G2 18v25 (Stackable)** | Hasta 30 V     | 25 A                         | 2 (stackable para 4) | PWM y digital       | Controlador de alta potencia, stackable para manejar hasta cuatro motores con gran capacidad de corriente. |
