# Sonar con microcontrolador STM32

Sistema embebido que imita el funcionamiento básico de un sonar: un servomotor orienta un sensor de ultrasonidos a distintos ángulos, mide la distancia al obstáculo en cada posición y envía el resultado al PC por puerto serie.

Proyecto de la asignatura **Microprocesadores y Microcontroladores** del Grado en Ingeniería Electrónica Industrial y Automática (Universidad Carlos III de Madrid, curso 2024-25).

## Funcionamiento

El sistema tiene dos modos, que se alternan con el botón **USER (B1)** de la placa:

- **Modo manual**: cada uno de los cuatro botones lleva el servo a una posición (−90°, −30°, +30°, +90°) y muestra en los LEDs qué botón se ha pulsado. Al llegar a la posición, se mide la distancia y se envía por UART.
- **Modo automático**: el servo recorre las posiciones de forma secuencial, midiendo y enviando la distancia tras cada movimiento.

Ejemplo de salida por el puerto serie:

```
La distancia es: 342 mm
```

## Hardware

- Placa de desarrollo **STM32 NUCLEO-L152RE**
- Servomotor **MG90S**
- Sensor de ultrasonidos **HC-SR04**
- Módulo con 4 pulsadores y 2 LEDs

## Aspectos técnicos

| Función | Periférico | Implementación |
|---|---|---|
| Control del servo | TIM2 CH1 (PWM) | El ancho de pulso (≈1–2 ms) fija el ángulo |
| Disparo del sensor (Trigger) | TIM3 + PC4 | Pulso de 10 µs generado por interrupción de temporizador, sin bloquear |
| Medida del eco (Echo) | TIM4 CH1 (Input Capture) | Captura de flancos de subida y bajada, con gestión del desbordamiento |
| Botones | PB1–PB4, PC13 (EXTI) | Configurados **a nivel de registros** (SYSCFG, FTSR, IMR, NVIC), por flanco de bajada y con antirrebotes |
| LEDs | PC0, PC1 | Escritura atómica mediante el registro BSRR |
| Comunicación | USART2 | 8N1, envío de la distancia en milímetros |

La distancia se calcula con **aritmética entera**, sin usar `float`, y el envío por UART se hace en el bucle principal, nunca dentro de las rutinas de interrupción.

## Estructura del repositorio

```
Core/
├── Inc/      # Cabeceras (.h)
└── Src/      # Código fuente (.c), incluido main.c
*.ioc         # Configuración del proyecto en STM32CubeMX
```

## Cómo usarlo

1. Abre el proyecto en **STM32CubeIDE**.
2. Compila y carga el programa en la placa NUCLEO-L152RE.
3. Abre un terminal serie (PuTTY, Tera Term o el de CubeIDE) en el puerto COM de la placa para ver las medidas.

## Autores

- Rafael Torres Olmedo
- Gonzalo Martínez Cominero
- Manuel Jesús Ruiz Valdepeñas
