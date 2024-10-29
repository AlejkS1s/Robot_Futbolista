# Robot_Futbolista
Robot que juega futbol, y va a competir en Torneo de robótica USCOTRON
## Requisitos
- Tracción: Libre.
- Dimensiones máximas: 15 x 15 x 15 cm.
- Materiales y diseño: Libre.
- Indicador de encendido: Obligatorio.
- Peso máximo: 750 gramos. Sin tolerancia.
- Alimentación: Debe ser por baterías;
- Resistencia: Las partes en contacto con otros robots deben ser resistentes para evitar roturas.
- Restricciones: Prohibido el uso de sistemas de disparo, agarre, pateo, grippers, materiales pegajosos, solenoides, o cualquier sistema que empuje o agarre la pelota.

- Control: Radiofrecuencia (RF) o Bluetooth.
- Distancia mínima: 3 metros.
- Frecuencias múltiples: Evitar
- interferencias; disponer de una alternativa.

## Características de Hardware
### ESP32

### MOTORES DC

### Driver 4 Motores DC (3v3 o 5v)

### Baterias

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

**Controladores para 4 Motores DC**
Motor Shield Driver L293d Puente H Pap Dc Servo Arduino L293
