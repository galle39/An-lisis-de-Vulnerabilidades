# Análisis de Binarios y Binary Diffing: Mitigación de Vulnerabilidades

Práctica correspondiente al **Módulo 6: Vulnerabilidades** del Máster en Ciberseguridad. Este repositorio documenta el proceso de ingeniería inversa, análisis estático comparativo (*binary diffing*) y validación dinámica de fallos por desbordamiento de búfer (*buffer overflow*) empleando **Ghidra**.

---

## Tabla de Contenidos
1. [Introducción y Objetivos](#-introducción-y-objetivos)
2. [Entorno de Trabajo](#-entorno-de-trabajo)
3. [Análisis Estático (Ghidra Version Tracking)](#-análisis-estático-ghidra-version-tracking)
4. [Análisis Dinámico y Evidencias de Ejecución](#-análisis-dinámico-y-evidencias-de-ejecución)
5. [Contexto Teórico: Casos Reales y Vulnerabilidades 0-Day](#-contexto-teórico-casos-reales-y-vulnerabilidades-0-day)
6. [Conclusiones](#-conclusiones)

---

## 1. Introducción y Objetivos
El objetivo principal de esta práctica es realizar un análisis comparativo (*binary diffing*) entre dos binarios de prueba (`seguro` y `vulnerable`). Mediante técnicas de ingeniería inversa, se busca:
* Identificar las diferencias críticas en el código fuente compilado.
* Localizar las malas prácticas de programación frente a las mitigaciones de seguridad.
* Comprobar de forma práctica el impacto de un desbordamiento de búfer.

---

## 2. Entorno de Trabajo
* **Sistema Operativo:** Windows 11 con subsistema WSL (Linux) para pruebas dinámicas.
* **Herramienta de Análisis Estático:** Ghidra (v11.4.3) con su utilidad nativa **Version Tracking**.
* **Binarios analizados:** `seguro` y `vulnerable`.

---

## 3. Análisis Estático (Ghidra Version Tracking)
Utilizando la función nativa de *Version Tracking* en Ghidra y ejecutando los correlacionadores automáticos, se ha contrastado la correspondencia de funciones:

* **Función `main`:** Es **idéntica** en ambos ejecutables. Su única función es validar los argumentos de entrada por consola y derivar la ejecución.
* **Función `vulnerable_function`:** Se detecta la divergencia clave de seguridad:
  * **Versión Segura:** Emplea **`strncpy(local_3f8, param_1, 999);`**, limitando estrictamente el número de bytes copiados. Además, incorpora protecciones de pila (*stack canary* mediante `in_GS_OFFSET` y llamadas a `__stack_chk_fail_local()`).
  * **Versión Vulnerable:** Emplea la función insegura **`strcpy(local_3f4, param_1);`**, que carece de control de límites sobre el tamaño de la entrada, permitiendo corromper los registros de la pila.

> 📸 *[Insertar aquí la captura de la comparativa de `vulnerable_function` en Ghidra]*

---

## 4. Análisis Dinámico y Evidencias de Ejecución
Se han ejecutado ambos binarios en entorno Linux (WSL) mediante terminal para comprobar su comportamiento ante entradas normales y desbordamientos masivos ($1050$ caracteres 'A' generados con Python).

### 4.1. Binario `seguro`
* **Entrada normal:** Procesa el argumento correctamente y finaliza de forma controlada.
* **Entrada masiva (`./seguro $(python3 -c "print('A' * 1050)")`):** Gestiona el límite de tamaño de manera segura gracias al uso de `strncpy` y la protección de la pila.

> 📸 *[Insertar aquí las capturas de pantalla de la ejecución del binario `seguro`]*

### 4.2. Binario `vulnerable`
* **Entrada normal:** Muestra el texto introducido sin incidencias aparentes.
* **Entrada masiva (`./vulnerable $(python3 -c "print('A' * 1050)")`):** Al sobrepasar la capacidad del búfer estático debido a la ausencia de límites de `strcpy`, el programa colapsa generando un **Segmentation fault** (volcado de memoria).

> 📸 *[Insertar aquí las capturas de pantalla del fallo de segmentación en el binario `vulnerable`]*

---

## 5. Contexto Teórico: Casos Reales y Vulnerabilidades 0-Day

### 5.1. Caso Real: El impacto del Buffer Overflow (MS03-026 / Gusano Blaster)
Un ejemplo histórico y devastador de este fallo a nivel de sistema operativo ocurrió con la vulnerabilidad **MS03-026** en el servicio RPC de Microsoft Windows.
* **El fallo:** Una función interna procesaba datos de red copiándolos a un búfer estático sin comprobar límites (un error conceptualmente idéntico al uso de `strcpy`).
* **El impacto:** Permitía a un atacante remoto ejecutar código arbitrario con privilegios de `SYSTEM`. Fue explotado masivamente en 2003 por el **gusano Blaster**, infectando cientos de miles de equipos y obligándolos a reiniciarse en bucle. Demuestra empíricamente por qué la validación de tamaños (`strncpy`) es vital en ciberseguridad.

### 5.2. ¿Qué es una vulnerabilidad 0-Day (Zero-Day)?
Un **0-day** es una brecha de seguridad desconocida para el fabricante o los desarrolladores, lo que significa que **no existe un parche ni contramedida oficial** en el momento de su explotación.
* **Dinámica:** Al no haber firmas de detección, los atacantes operan con total impunidad hasta que se descubre el vector y se emite un parche de emergencia.
* **Relación con el Binary Diffing:** Herramientas como el *Version Tracking* de Ghidra son precisamente las armas principales que utilizan los analistas de seguridad cuando se libera un parche: comparan el binario actualizado con la versión previa (*binary diff*) para aislar exactamente qué función se modificó y comprender la vulnerabilidad original que el fabricante silenció.

---

## 6. Conclusiones
El análisis comparativo (*binary diffing*) demuestra de forma empírica cómo el uso de funciones inseguras (`strcpy`) frente a alternativas robustas (`strncpy`), unido a la ausencia de mitigaciones de compilación (*stack canaries*), compromete la integridad del software. La ingeniería inversa y las herramientas de diffing son claves tanto para la auditoría defensiva como para la comprensión de fallos críticos en entornos reales.
