Hace algún tiempo lei un artículo bastante interesante de mi compñaero Kub0x en EHN, el cual explicaba [como tener un certicado SSL de una CA de forma gratuita](https://foro.elhacker.net/seguridad/tutorial_certificado_https_iexclgratis-t445507.0.html). Si queréis saber de que estoy hablando, os recomiendo que os leáis el articulo para saber más detalles técnicos de lo que hablo, o en su defecto, leer un poco de la Wikipedia.

Una vez leí el artículo entendí que Let's Encrypt es una página que te da los certificados, pero lo que no es gratuito es tener una página corriendo con dicho certificado, que a fin de cuentas, es una chuminá, básicamente, si lo quieres hacer por tu propia cuenta es coger dicho certificado y bajo Apache, ngingx, IIS o cualquier otro propio servidor HTTP (estoy redactando un articulo de como poder hacerte uno propio o al menos usar ciertas librerías apra hacer uno propio modificando su source code) e isntalarlo para que se ejecute bajo el puerto 443.

Entonces, pensé según lo que ya había leído sobre [el futuro decadente y obsoleto del protocolo HTTP](https://elpais.com/tecnologia/2016/03/14/actualidad/1457954626_707257.html) (el cual veo una buena oportunidad de negocio), que ya habría hostings que tuviesen disponibles esta tecnología de forma gratuita, por la competencia, y así fue, buscando encontre esta maravilla: https://freehosting.host/

No es una cosa del otro mundo, pero cumple, tiene un buen porcentaje de HA en lo que a uptime se refiere (alta disponibilidad), y algunas características interesantes.

Entre sus ventajas:

* Dispone de instalación de protocolos SSL de Let's Encrypt, lo que te asegurará que tengas HTTPS en tu web.
* Puedes tener hasta 5 bases de datos en MySQL.
* No hay limites de usuarios MySQL
* Puedes tener hasta 5 subdominios.
* Monitor personalizado, bastante interesante para los amantes de estadísticas ([1](https://i.gyazo.com/9ba8e2319008f9e9041d31a2a5af5d0d.png) & [2](https://i.imgur.com/xSU4aXt.png))
* Interfaz en español (si lo deseamos)
* Según veo no hay restricciones de actividad, es decir, si dejas abandonado el cPanel por un tiempo no te borran nada
* 100GB de ancho de banda, no sé si es mucho, pero bueno, x10Hosting tenía un BW ilimitado pero luego el uptime era desastroso, más que tenías que estar activo siempre en el cPnael para que no te borrasen la cuenta
* 512MB de RAM, que para un wordpress está muy bien, pero el tamaño del disco es poco

Obviamente, por algo gratuito no vamos a tener la bomba de recursos... Y por eso nos encontramos con estas...

... desventajas:

* Solo una cuenta de FTP
* 200 MB de Disco y de BBDD
* 256 IOPS
* 2 Entry Processes
* 20 Procesos en total
* Y entre lo más importante, no te deja tener un nombre de subdominio personalizado por lo que veo, de ahí a que el uso que le de sea meramente para llamadas a una API que estoy diseñando en PHP para hacer pruebas tontas y simples.

Estas 3 últimas desventajas (sin contar la penúltima) no tengo muy claro como afectarían a una página con un CMS demandante, como puede ser Wordpress o SMF, pero entiendo que por como se plantea es una cantidad baja.

Y no, no me han pagado para que los promocione, pero por ser gratuito ya era digno de mencionar.

Hay alternativas de pago muchisimo mejor y por un precio módico (15€ + IVA/año con un dominio (12€ + IVA si no quieres dominio) + 20GB de espacio + 1GB de RAM + Linux + 2TB de tráfico/mes o un server dedicado con Intel i5 (Benchmark: 3,700) + 2TB HDD + 16GB RAM (DDR3 - 1333Mhz) por 21€/mes + IVA = 305€/año) que ya iré explicando, pero bueno, por el momento, me quedo con esto, para ir probando.