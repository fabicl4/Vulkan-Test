# Vulkan-Renderer

El objetivo de este proyecto aprender

Para ello, siguiendo el tutorial de vulkan de [https://vulkan-tutorial.com/](https://vulkan-tutorial.com/), se busca realizar una simple abstracción que me permita hacer pruebas.

El proyecto se divide en una librería estática que contiene todo el código core del proyecto y una carpeta sandbox que incluye y define ejemplos y pruebas.

La idea es crear una base que me permita aprender y experimentar con Vulkan

## Architectural Patterns

> [!abstract] Filosofía de diseño
> Basado en **Data-Oriented Design** y **Layered Architecture** para minimizar el overhead de la CPU y facilitar la portabilidad.

### Layered Architecture

#### Core

- logging
- memory
- utils
- containers
- asserts

#### Application

Clase central del proyecto. Se encarga de:

- manejar el _life cycle_ de cada uno de los sistemas de la aplicación.
- inicializa, actualiza y destruye los sistemas
- inyecta dependecias
- contiene el main loop

#### Render

Organiza el frame.

RenderPass, como puedo crear pass que rendericen tipos de objeto en especifico, por ejemplo sprites o simples mallas de trianglos?

#### Resource

Clase que gestion la creación, la carga y la destrucción de recursos de programa

#### Platform

- **Window**
Representa una abstracción de la ventana de GLFW. Maneja la creación de la ventana así como su lifecycle. Gestiona los eventos de la ventana. Es necesaria para crear la Swapchain

- **Vulkan**
Abstracciones de tipos básicos de vulkan y helpers. Destacan:

- Abstraccion de Buffers
- Pipeline estructura
- Contexto gráfico
- Vertice Layout

---

## Contexto gráfico

### Instance

Representa la instancia global de Vulkan, entre sus responsabilidades incluye:

- Crea la instancia de Vulkan
- Activa (si habilitado) las _validation layers_
- Activa las extensiones globales
- Destruye la instancia

### Device

Entre sus responsabilidades se incluye:

- Selecciona la GPU que se utilizará.
- Comprueba las capacidades de la GPU
- Crea el _logical device_
- Las _queues_ necesarias (_Graphics Queue_ y _Present Queue_)
- Crea la _command pool_.
- Asigna la memoria de los _command buffers_. El número de _command buffers_ depende de si nuestro sistema soporta double o triple buffering

Depende de: **Surface**, **Instance**.

### Swapchain

Maneja los Backbuffers y la presentación de las imagenes. La SwapChain se encarga de:

- Crea swapchain
- Crea image views
- Recrea la swapchain cuando la ventana cambia de tamaño
- Adquiere image
- Presenta image

**Instance** y **Device** están ligadas al contexto global, mientras que **Swapchain** se encuentra ligado a la Surface de la ventana.

---

## Librerías

- GLFW
- STB
- GLM
- SPDLOG

---

## Resources

- [Vulkan Tutorial](https://vulkan-tutorial.com/)
- [Vulkan Documentation: Building a Simple Engine](https://docs.vulkan.org/tutorial/latest/Building_a_Simple_Engine/introduction.html)