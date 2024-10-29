# Robot_Futbolista
Robot que juega futbol, y va a competir en Torneo de robótica USCOTRON
## Requisitos
- Control: Radiofrecuencia (RF) o Bluetooth.
- Distancia mínima: 3 metros.
- Frecuencias múltiples: Evitar interferencias; disponer de una alternativa.
- Conflictos de frecuencia: Organizadores asignan nuevas frecuencias.
- Prohibido interferir: Sanciones y descalificación.
## Características de Hardware
En proceso de definición 
## Características de Software
En proceso de definición, se debe definir el hardware a utiliar en primer lugar.
## Control del Robot 
- **Control Manual**
  - Aplicación android
  - joystick (Investigar).
  - Aplicación web.
  - RF control a distancia
- **Control Semiautomático** **Hard**
  Implementación avanzada que pueda ayudar al robot a reaccionar parcialmente de forma autónoma, asistiendo al operador en el juego.
- **Control Automático**
  Uso de **Machine Learning** (propuesta en evaluación, es una meta de alta complejidad) para desarrollar un control completamente autónomo del robot.

### Etapa de Alimentación
Uso de baterías, ya sea recargables o de un solo uso:
- Baterias de litio 18650
- Baterias alcalinas
- Baterias de 9V


### Etapa de Control de Motores
El control de los motores es esencial para la precisión en los movimientos del robot. En esta etapa, se están evaluando diferentes controladores para optimizar la potencia, eficiencia y precisión.

Aquí tienes una tabla comparativa de los controladores de motor que mencionamos, con sus principales características:

| Controlador   | Voltaje Operativo | Corriente Máxima (por canal) | Caída de Voltaje | Modo de Control     | Características Destacadas                                  |
|---------------|-------------------|------------------------------|-------------------|----------------------|------------------------------------------------------------|
| **L298N**     | 5 - 46 V          | 2 A                          | Alta              | PWM y digital        | Comúnmente utilizado; diseño antiguo, menor eficiencia     |
| **DRV8833**   | 2.7 - 10.8 V      | 1.5 A                        | Baja              | PWM y digital        | Compacto, eficiente, menor generación de calor             |
| **DRV8825**   | 8.2 - 45 V        | 2.5 A                        | Baja              | PWM (con microstepping)| Compatible con motores paso a paso; ideal para alta precisión|
| **TB6612FNG** | 2.5 - 13.5 V      | 1.2 A                        | Baja              | PWM y digital        | Alta eficiencia; menos calor, adecuado para robótica       |
| **A4950**     | 4.5 - 40 V        | 3.5 A                        | Muy baja          | PWM y digital        | MOSFET, menor disipación de energía, excelente para alta potencia |
| **L6205**     | 8 - 52 V          | 2.8 A                        | Moderada          | PWM y digital        | Alta eficiencia, adecuado para motores de potencia media-alta |

### Análisis de Opciones
- **Para proyectos de baja potencia**, el **DRV8833** y **TB6612FNG** son ideales debido a su eficiencia y bajo consumo de energía.
- **Para motores de alta precisión o paso a paso**, el **DRV8825** es excelente ya que soporta microstepping, lo que permite un control detallado del motor.
- **Para proyectos que requieren alta corriente**, el **A4950** y el **L6205** pueden manejar mayor carga, ideales para robots con requerimientos de potencia elevados.