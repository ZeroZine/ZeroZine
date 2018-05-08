En este artículo os explicaré como usar nuestro propio sistema de captchas (el cual está desfasado porque Google tiene uno más avanzado el cual no dispone de texto si no de una casilla la cual verifica si somos un humano según los movimientos que hagamos con nuestro cursor) de texto para integrarlas a WinForms o a una consola por tal de hacer pruebas.

Lo primero que necesitamos es un hosting ([si tiene HTTPS mucho mejor](https://github.com/z3nth10n/ZeroZine/blob/master/edicion1/zenth10n/1-gangas/Tener%20nuestro%20propia%20API%20bajo%20HTTPS%20gratuitamente.md)) capaz de ejecutar PHP y SQL.

Lo segundo tener el [código fuente de un sistema que dibuje captchas en PHP](https://github.com/Gregwar/Captcha) (no vamos a reeinventar la rueda y más si esta ya está obsoleta).

Lo tercero es tener tiempo para configurar todo, pero una vez tengamos ya todo montado, solo será cuestión de crear un nuevo proyecto en PHP Storm / Sublime Text o la IDE que nos de la gana. En este proyecto importaremos dicho source code y empezaremos a crear.

Lo cuarto que necesitaremos será hacer las llamadas apropiadas al source code, tenemos un tutorial en inglés más abajo en el repositorio.

Básicamente lo que nos sugiere que hagamos es lo siguiente:

	<?php

	use Gregwar\Captcha\CaptchaBuilder;

	$builder = new CaptchaBuilder;
	$builder->build();

	header('Content-type: image/jpeg');
	$builder->output();

¿Y que estamos haciendo con esto?, primero estamos importando el namespace de la captcha para poder usarlo, despues estamos llamando al constructor de la clase, después llamamos al metodo construir (build) y luego al output, sin antes no olvidarnos del header, y todo esto para generar una imagen en PHP.

Bueno, pues empezemos a rizar el rizo, teniendo esto claro podemos hacer cosas interesantes, como por ejemplo, hacer que todo esto pase a ser JSON para luego ser leído por un cliente. ¿Y como logramos tal hazaña?

Pues primero debemos cambiar el header image/jpeg (`header('Content-type: image/jpeg');`) a JSON (`header('Content-Type: application/json');`).

Lo segundo es conviertir una imagen a texto o al menos a bytes, y como se consigue esto, [pues aquí tenemos la solución](https://stackoverflow.com/a/11446101/3286975) y si queremos darle más flow al asunto convertiremos dicha string en una string base64 para que quede mejor, dejando que quede así.

	ob_start();
    $builder->output();
    $imageString = ob_get_clean();

    $output = array();
    $output["data"] = base64_encode($imageString);

Y por último, volcando los datos en una string propia de JSON:

	json_encode($output, true);

Los que estéis manejando PHP procedimental ya habréis notado que os falta algo, y es el `autoload` el cual asegurara que todo cargue.

	require("PhraseBuilderInterface.php");
	require("PhraseBuilder.php");
	require("CaptchaBuilderInterface.php");
	require("CaptchaBuilder.php");

Su importación allá donde lo necesitéis:

	include("libs/captcha/autoload.php");

Y evitar el uso de `use` en este tipo de código cambiando de:

	use Gregwar\Captcha\CaptchaBuilder;

	$builder = new CaptchaBuilder;

a:

	$builder = new Gregwar\Captcha\CaptchaBuilder;

Con esto, ya nos habremos asegurado de tener un output de captcha adecuado, pero nos falta algo, y es ponerlo a prueba. Ya que estoy usando tecnologías obsoletas y rudimentarias no me corto un pelo en seguir dicha inercia, y en lo primero que había pensado es en md5.

¿Y como implementariamos esto?

Dentro del repositorio, nos dicen que con:

	$builder->getPhrase();

Obtenemos la solución. El autor ha usado `$_SESSION` el cual es un poco más engorroso de llevar a [un cliente fuera de la web (como por ejemplo: WinForms en VB.NET)](https://stackoverflow.com/a/2616917/3286975) (cubriremos todos los casos), pero empecemos con md5.

La solución es simple, cogemos y convertimos la solución a md5, se la decimos al cliente para luego volver a enviarsela al servidor:

	$output["md5"] = md5($builder->getPhrase());

Y luego en un `get` volvemos a comparar:

    $secret = @$_GET["secret"];
    $input = @$_GET["input"];

    $coreData["valid"] = md5($input) === $secret;

Pero esto es super cutre, y veréis porqué:

![...](https://i.gyazo.com/b27ab22c9a5fa90e6326dcb2cfcb5bf5.png)

**PD:** Y sí, lo que estáis viendo es una imagen en una consola. En la [segunda parte](https://github.com/z3nth10n/ZeroZine/blob/master/edicion1/zenth10n/2-captchas/2-Integrando%20una%20captcha%20a%20WinForms%20o%20a%20una%20Consola.md) explicaré como lo he hecho.

Sí, en ciertos casos, incluso en diccionarios online tenemos super extensas listas de resumenes md5 ya a nuestra disposición e incluso con una combinación de 37 caracteres con 5 posiciones de longitud nos da unos 37^5 (69.343.957) posibles soluciones por fuerza bruta, lo cual con la velocidad de computación de los ordenadores que tenemos hoy se resolvería en un abrir y cerrar de ojos. Hagamos cuentas:

	$time_start = microtime(true);

	$num = 100000;
	$i = 0;
	$addalphabeth = array_merge(range('a', 'z'), range(0,9));
	$txt = "";

	$setcharacter = [];
	foreach ($addalphabeth as $setcharacter[0]) {
	    foreach ($addalphabeth as $setcharacter[1]) {
	        foreach ($addalphabeth as $setcharacter[2]) {
	            foreach ($addalphabeth as $setcharacter[3]) {
	                foreach ($addalphabeth as $setcharacter[4]) {
	                    $str = vsprintf('%s%s%s%s%s', $setcharacter);

	                    $txt .= $str." - ".md5($str).PHP_EOL;

	                    ++$i;
	                    if($i == $num)
	                    {
	                        break 5;
	                    }
	                }
	            }
	        }
	    }
	}

	file_put_contents('results/md5.txt', $txt);

	echo (int)((microtime(true) - $time_start) * 1000);

El código lo hemos sacado de aquí: http://www.onlinecode.org/generate-string-aaaa-9999-using-php-code/

Ocupa un espacio de 4102 KB (lógico, ya que son 5 caracteres de la captcha, 3 para separar, y 32 del md5, más el salto de linea, en total 41 caracteres * 100.000, nos dan unos 4,1M bytes a KB = 4.100.000 b / 1024 = 4003KB).

De media el script tarda unos 170 ms en generar 100.000 keys, lo que nos da una tasa de 588235 hashes/sec.

Comparandolo con las bitcoins, tenemos que [según dice la wiki](https://en.bitcoin.it/wiki/Non-specialized_hardware_comparison#Intel):

![...](https://i.gyazo.com/9d3d41af35a7664afa2a9b912b8e5eaa.png)

![...](https://i.gyazo.com/7012aa805353ecfd1fe895b3fddf11b9.png)

![...](https://i.gyazo.com/2b49c400798339591e10a331a9125873.png)

Según mis cuentas, el rate de mi CPU sería 61,6Mhash/s, sin embargo aquí obtenemos, un resultado 104 veces (10470,2%) más lento. ¿Quizás sea porque estoy usando PHP y quizás en C# sea algo más rápido y en C++ algo más?

Aún así con ese hashrate, las 37^5 posibilidades se calcularían en unos 117 segundos (es decir, 2 minutos) ocupando unos 2.844.489 KB o lo que es lo mismo 2,71 GBs.

¿Quizás sea más rápido almacenar en una tabla MySQL? No lo sé.

Vuelta al meollo, como vemos, md5 es nada a la hora de la seguridad. También podríamos probar a generar certificados en PHP con esta [maravillosa librería](https://github.com/phpseclib/phpseclib) y enciptar la clave en vez de con md5 con una clave privada.

http://phpseclib.sourceforge.net/rsa/2.0/examples.html

Con una clave de 4096 bits cubririamos en salud, aún así, lo estariamos haciendo mal.

Por eso, aquí el camino se nos divide en 2, o enviarle la clave al cliente (lo cual hemos visto que es inseguro, ya que cualquier información que se le envie al cliente es crackeable) o mantener la información dentro del servidor y acceder a ella o bien con una sesión  o con una id + mysql, lo cual viene a ser lo mismo, ya que la sesión de PHP nos da una id para poder consultar.

No voy a implementar todo eso ahora mismo, pero se puede hacer de forma fácil. Lo más recomendable es una sesión, como nos sugiere @Gregwar:

	$_SESSION['phrase'] = $builder->getPhrase();

Y ya, pues según donde tengamos cliente hacer una u otra cosa.

Proseguiremos la segunda parte en [una consola la cual es capaz de volcar la imagen la captcha directamente en ella](https://github.com/z3nth10n/ZeroZine/blob/master/edicion1/zenth10n/2-captchas/2-Integrando%20una%20captcha%20a%20WinForms%20o%20a%20una%20Consola.md), y haremos otros cuantos experimentos.