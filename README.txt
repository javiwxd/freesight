
-- Freesight para Windows --

La intención de este programa es bloquear la conexión a la red local del programa "Insight" de Faronics, desbloquear el administrador de tareas y las páginas web restringidas.
La principal vulnerabilidad que explota este programa es la libertad que tiene el usuario para ejecutar cualquier programa en el ordenador con permisos de administrador, y que
la versión de Insight utilizada es antigua, utilizando el industry-standard disassembler se puede ver que a travels de un exploit en una versión antigua de una de sus librerías
es posible hacer inyección de memoria en el proceso de Insight, pero inyectar memoria haría que el antivirus detectara este programa como un virus, por lo que se ha optado por
bloquear la conexión a la red local del programa Insight a través del Firewall de Windows.

-- Fases de desbloqueo

    - Inhibición de las funciones del programa "Insight"

    Insight es una herramienta de control remoto de ordenadores creada por la empresa Faronics,
    freesight borra todas las reglas del Firewall de Windows y crea una regla que deniega la conexión
    de esta herramienta a la red.

    Más información sobre Insight: http://www.faronics.com/products/insight/

    - Desactivación del bloqueo del administrador de tareas

    El administrador de tareas es una herramienta de Windows que permite ver los procesos que se
    están ejecutando en el ordenador, terminarlos, etc. Este está bloqueado desde el registro
    de Windows, freesight lo desbloquea cambiando el valor de una clave.

    - Desbloqueo de las páginas web restringidas

    Este bloqueo está implementado a nivel de red y no se puede desbloquear desde el ordenador,
    freesight lo desbloquea instalando una VPN gratuita en el ordenador proporcionada por
    Cloudflare

    Más información sobre Cloudflare Warp: https://1.1.1.1

-- Desactivar ciertas fases

    Este programa cuenta con un archivo de configuración que permite desactivar ciertas fases
    llamado "config.yaml" que se encuentra en la misma carpeta que el ejecutable.

-- Licencias

    Este programa está licenciado bajo la licencia Apache 2.0 (http://www.apache.org/licenses/LICENSE-2.0)

    Librerías utilizadas:

        - yaml-cpp: Licencia MIT (https://github.com/jbeder/yaml-cpp/blob/master/LICENSE)
        - curl: (https://github.com/curl/curl/blob/master/COPYING)
        - libzippp (https://github.com/ctabin/libzippp/blob/master/LICENCE)
        - libzip (https://github.com/nih-at/libzip/blob/main/LICENSE)

-- Garantía

    Este programa no tiene ninguna garantía, no se hace responsable de ningún daño que pueda
    causar en el ordenador.

    Este programa no está asociado de ninguna manera con Faronics o Cloudflare.

    Este programa tiene fines educativos y no se recomienda su uso en ordenadores de centros educativos.