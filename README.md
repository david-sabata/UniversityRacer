
Jak to funguje
--------------

`main.cpp` zajišťuje inicializaci SDL, spuštění hlavní smyčky a přeposílání eventů aplikaci.

Samotnou aplikaci pak představuje potomek třídy `BaseApp`, který podle potřeby překrývá defaultní handlery událostí. (Důležité jsou zejména metody `onInit` a `onWindowRedraw` - pojmenování je stejné jako v souborech ze cvičení.) Jeden potomek je tedy samotná hra, další může být menu a podobně. 
Dokonce by mělo být možné používat více potomků současně - například vykreslit snímek hry, event předat objektu menu, který do snímku ještě vloží menu. Alespoň teoreticky by to takhle fungovat mohlo, vyzkoušené to nemám :-)

Git
----

Jestli nevíte jak na Git, je to jednoduché, snad je na to i nějaké GUI. Bohatě ale stačí příkazový řádek, terminál nebo tak něco. Velice pěkně popsané příkazy jsou tady: http://gitref.org/remotes/ . 
Pro běžné verzování mi prozatím vždycky stačil následující postup: 
 - `pull` (stáhne data ze vzdáleného repozitáře)
 - (`log` - vypíše poslední commity a jejich zprávy)
 - upravení souborů, přidání, smazání
 - (`status` - vypíše co se změnilo, co už je přidané do commitu,...)
 - `add` (přidá soubory do commitu)
 - `commit` (uloží commit, je možné a vhodné vložit zprávu)
 - `push` (nahraje commit do vzdáleného repozitáře)

 Verzování
 ---------

 Bylo by určitě dobré používat branche, alespoň ze začátku, než to slepíme dohromady. Viděl bych to tak, že si můžete udělat vlastního potomka `BaseApp`, kde by vám mohlo stačit upravovat asi jen `onWindowRedraw` a `onInit`. Ikdyby toho ale bylo víc, tak to nevadí, bude to vaše třída, můžete si tam dělat co chcete :-)
 Výhoda je v tom, že stačí upravit jediný řádek v `main.cpp` a tím se všechny eventy a jejich obsluha přesune na vaši třídu. Díky tomu budeme moct vyvíjet nezávisle na sobě, ale současně sdílet některé funkce - když například přidám nějaké načítání modelů, budete ho moct použít taky, protože bude buď v oddělené třídě anebo v `BaseApp`.

 Takže ideálně každý svoji branch + vlastní implementace `BaseApp`.

 Taky do Gitu nenahrávejte soubory vašeho IDE, includy knihoven a jiné věci, co se dají stáhnout, nainstalovat,... Aby vám Git při každém `add` nepřidával konfigurační soubory IDE a jiné, můžete masky přidat do `.gitignore`.