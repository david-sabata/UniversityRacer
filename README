
Jak to funguje
--------------

`main.cpp` zajišuje inicializaci SDL, spuštìní hlavní smyèky a pøeposílání eventù aplikaci.

Samotnou aplikaci pak pøedstavuje potomek tøídy `BaseApp`, kterı podle potøeby pøekrıvá defaultní handlery událostí. (Dùleité jsou zejména metody `onInit` a `onWindowRedraw` - pojmenování je stejné jako v souborech ze cvièení.) Jeden potomek je tedy samotná hra, další mùe bıt menu a podobnì. 
Dokonce by mìlo bıt moné pouívat více potomkù souèasnì - napøíklad vykreslit snímek hry, event pøedat objektu menu, kterı do snímku ještì vloí menu. Alespoò teoreticky by to takhle fungovat mohlo, vyzkoušené to nemám :-)

Git
----

Jestli nevíte jak na Git, je to jednoduché, snad je na to i nìjaké GUI. Bohatì ale staèí pøíkazovı øádek, terminál nebo tak nìco. Velice pìknì popsané pøíkazy jsou tady: http://gitref.org/remotes/ . 
Pro bìné verzování mi prozatím vdycky staèil následující postup: 
 - `pull` (stáhne data ze vzdáleného repozitáøe)
 - (`log` - vypíše poslední commity a jejich zprávy)
 - upravení souborù, pøidání, smazání
 - (`status` - vypíše co se zmìnilo, co u je pøidané do commitu,...)
 - `add` (pøidá soubory do commitu)
 - `commit` (uloí commit, je moné a vhodné vloit zprávu)
 - `push` (nahraje commit do vzdáleného repozitáøe)

 Verzování
 ---------

 Bylo by urèitì dobré pouívat branche, alespoò ze zaèátku, ne to slepíme dohromady. Vidìl bych to tak, e si mùete udìlat vlastního potomka `BaseApp`, kde by vám mohlo staèit upravovat asi jen `onWindowRedraw` a `onInit`. Ikdyby toho ale bylo víc, tak to nevadí, bude to vaše tøída, mùete si tam dìlat co chcete :-)
 Vıhoda je v tom, e staèí upravit jedinı øádek v `main.cpp` a tím se všechny eventy a jejich obsluha pøesune na vaši tøídu. Díky tomu budeme moct vyvíjet nezávisle na sobì, ale souèasnì sdílet nìkteré funkce - kdy napøíklad pøidám nìjaké naèítání modelù, budete ho moct pouít taky, protoe bude buï v oddìlené tøídì anebo v `BaseApp`.

 Take ideálnì kadı svoji branch + vlastní implementace `BaseApp`.

 Taky do Gitu nenahrávejte soubory vašeho IDE, includy knihoven a jiné vìci, co se dají stáhnout, nainstalovat,... Aby vám Git pøi kadém `add` nepøidával konfiguraèní soubory IDE a jiné, mùete masky pøidat do `.gitignore`.